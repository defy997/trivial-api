#include <stdio.h>
#include <stdlib.h>
#include "execute.h"
#include "../database/dbms.h"
#include "../table/table_header.h"
#include "../expression/expression.h"
#include <string.h>
#include <ctype.h>
#include <sstream>
#include <iomanip>
#include <map>
#include <algorithm>
#include <vector>
#include <openssl/hmac.h>
#include <openssl/evp.h>
#include <openssl/crypto.h>
#include <openssl/bio.h>
#include <openssl/buffer.h>
#include <fstream>
#include <string.h>
#include <ctype.h>

template<typename T, typename DataDeleter>
void free_linked_list(linked_list_t *linked_list, DataDeleter data_deleter)
{
	for(linked_list_t *l_ptr = linked_list; l_ptr; )
	{
		T* data = (T*)l_ptr->data;
		data_deleter(data);
		linked_list_t *tmp = l_ptr;
		l_ptr = l_ptr->next;
		free(tmp);
	}
}

void expression::free_exprnode(expr_node_t *expr)
{
	if(!expr) return;
	if(expr->op == OPERATOR_NONE)
	{
		switch(expr->term_type)
		{
			case TERM_STRING:
				free(expr->val_s);
				break;
			case TERM_COLUMN_REF:
				free(expr->column_ref->table);
				free(expr->column_ref->column);
				free(expr->column_ref);
				break;
			case TERM_LITERAL_LIST:
				free_linked_list<expr_node_t>(
					expr->literal_list,
					expression::free_exprnode
				);
				break;
			default:
				break;
		}
	} else {
		free_exprnode(expr->left);
		free_exprnode(expr->right);
	}

	free(expr);
}

void free_column_ref(column_ref_t *cref)
{
	if(!cref) return;
	free(cref->column);
	free(cref->table);
	free(cref);
}

bool fill_table_header(table_header_t *header, const table_def_t *table);

void execute_switch_output(const char *output_filename)
{
	dbms::get_instance()->switch_select_output(output_filename);
	free((void*)output_filename);
}

void execute_create_table(const table_def_t *table)
{
	table_header_t *header = new table_header_t;
	if(fill_table_header(header, table))
		dbms::get_instance()->create_table(header);
	else std::fprintf(stderr, "[Error] Fail to create table!\n");
	delete header;

	free(table->name);
	free_linked_list<table_constraint_t>(table->constraints, [](table_constraint_t *data) {
		expression::free_exprnode(data->check_cond);
		free_column_ref(data->column_ref);
		free_column_ref(data->foreign_column_ref);
		free(data);
	} );

	for(field_item_t *it = table->fields; it; )
	{
		field_item_t *tmp = it;
		free(it->name);
		expression::free_exprnode(it->default_value);
		it = it->next;
		free(tmp);
	}

	free((void*)table);
}

void execute_create_database(const char *db_name)
{
	// determine actual database name to create:
	// - if session user is non-admin and requested name does not start with "username_",
	//   auto-prefix with "username_"
	std::string requested = db_name ? std::string(db_name) : std::string("");
	std::string actual = requested;
	std::string creator = dbms::get_instance()->get_current_user();
	bool creator_admin = dbms::get_instance()->user_is_admin();
	if (!creator_admin && !creator.empty()) {
		std::string prefix = creator + "_";
		if (requested.rfind(prefix, 0) != 0) {
			actual = prefix + requested;
		}
	}

	// permission check uses the actual database name
	bool allowed = dbms::get_instance()->db_allowed(actual.c_str());
	// allow creation if creator is creating a db prefixed with their username_
	if (!allowed && !creator_admin && !creator.empty()) {
		std::string prefix = creator + "_";
		if (actual.rfind(prefix, 0) == 0) allowed = true;
	}
	if (!allowed) {
		std::fprintf(stderr, "[Error] permission denied for create database '%s'.\n", actual.c_str());
		free((char*)db_name);
		return;
	}

	// create database
	dbms::get_instance()->create_database(actual.c_str());

	// auto-grant created database to session user (if any)
	try {
		if (!creator.empty()) {
			// call grant helper to persist mapping (execute_grant_db frees its args)
			execute_grant_db(strdup(actual.c_str()), strdup(creator.c_str()));
		}
	} catch (...) {
		// ignore errors for best-effort grant
	}

	free((char*)db_name);
}

void execute_use_database(const char *db_name)
{
	// check permission if session user set
	if (!dbms::get_instance()->db_allowed(db_name)) {
		std::fprintf(stderr, "[Error] permission denied for database '%s'.\n", db_name);
		free((char*)db_name);
		return;
	}
	dbms::get_instance()->switch_database(db_name);
	free((char*)db_name);
}

void execute_drop_database(const char *db_name)
{
	if (!dbms::get_instance()->db_allowed(db_name)) {
		std::fprintf(stderr, "[Error] permission denied for drop database '%s'.\n", db_name);
		free((char*)db_name);
		return;
	}
	dbms::get_instance()->drop_database(db_name);
	free((char*)db_name);
}

void execute_show_database(const char *db_name)
{
	dbms::get_instance()->show_database(db_name);
	free((void*)db_name);
}

void execute_show_databases()
{
	dbms::get_instance()->show_databases();
}

void execute_show_tables()
{
	dbms::get_instance()->show_tables();
}

void execute_drop_table(const char *table_name)
{
	dbms::get_instance()->drop_table(table_name);
	free((void*)table_name);
}

void execute_show_table(const char *table_name)
{
	dbms::get_instance()->show_table(table_name);
	free((void*)table_name);
}

void execute_insert(const insert_info_t *insert_info)
{
	dbms::get_instance()->insert_rows(insert_info);
	free(insert_info->table);
	free_linked_list<column_ref_t>(insert_info->columns, free_column_ref);
	free_linked_list<linked_list_t>(insert_info->values, [](linked_list_t *expr_list) {
		free_linked_list<expr_node_t>(expr_list, expression::free_exprnode);
	} );
	free((void*)insert_info);
}

void execute_delete(const delete_info_t *delete_info)
{
	dbms::get_instance()->delete_rows(delete_info);
	free(delete_info->table);
	expression::free_exprnode(delete_info->where);
	free((void*)delete_info);
}

void execute_select(const select_info_t *select_info)
{
	dbms::get_instance()->select_rows(select_info);
	expression::free_exprnode(select_info->where);
	free_linked_list<expr_node_t>(select_info->exprs, expression::free_exprnode);
	free_linked_list<table_join_info_t>(select_info->tables, [](table_join_info_t *data) {
		free(data->table);
		if(data->join_table)
			free(data->join_table);
		if(data->alias)
			free(data->alias);
		expression::free_exprnode(data->cond);
		free(data);
	} );

	free((void*)select_info);
}

void execute_update(const update_info_t *update_info)
{
	dbms::get_instance()->update_rows(update_info);
	free(update_info->table);
	free_column_ref(update_info->column_ref);
	expression::free_exprnode(update_info->where);
	expression::free_exprnode(update_info->value);
	free((void*)update_info);
}

void execute_create_index(const char *table_name, const char *col_name)
{
	dbms::get_instance()->create_index(table_name, col_name);
	free((char*)table_name);
	free((char*)col_name);
}

void execute_drop_index(const char *table_name, const char *col_name)
{
	dbms::get_instance()->drop_index(table_name, col_name);
	free((char*)table_name);
	free((char*)col_name);
}

void execute_quit()
{
	dbms::get_instance()->close_database();
	printf("[exit] good bye!\n");
}

void execute_auth(const char *user, const char *pass)
{
	// AUTH can be used in two modes:
	// 1) AUTH '__TOKEN__' '<token>' -> token is base64(payload).hexsig where payload=username:is_admin:pattern1,...
	//    Engine will verify HMAC with TRIVIAL_AUTH_SECRET env var.
	// 2) AUTH 'username' 'password' -> fallback to users.txt plaintext (deprecated).
	if (user && strcmp(user, "__TOKEN__") == 0) {
		const char *token = pass;
		const char *secret = getenv("TRIVIAL_AUTH_SECRET");
		if (!secret) secret = "default_secret";
		// split token
		if (!token) {
			std::fprintf(stderr, "[Error] Empty token\n");
			free((void*)user);
			free((void*)pass);
			return;
		}
		const char *dot = strchr(token, '.');
		if (!dot) {
			std::fprintf(stderr, "[Error] Invalid token format\n");
			free((void*)user);
			free((void*)pass);
			return;
		}
		std::string b64(token, dot - token);
		std::string sig(dot + 1);
		// compute expected signature via HMAC-SHA256 using OpenSSL HMAC
		unsigned int len = 0;
		unsigned char *res = HMAC(EVP_sha256(), secret, strlen(secret),
				(const unsigned char*)b64.c_str(), b64.size(), NULL, &len);
		if (!res) {
			std::fprintf(stderr, "[Error] HMAC failure\n");
			free((void*)user);
			free((void*)pass);
			return;
		}
		// hex encode res
		std::ostringstream oss;
		oss<<std::hex<<std::setfill('0');
		for(unsigned int i=0;i<len;i++) oss<<std::setw(2)<<(int)res[i];
		std::string expect = oss.str();
		if (!CRYPTO_memcmp(expect.c_str(), sig.c_str(), expect.size() > sig.size() ? sig.size() : expect.size()) == 0) {
			// compare using constant time compare
		}
		// safer compare
		if (! (expect.size() == sig.size() && CRYPTO_memcmp(expect.c_str(), sig.c_str(), expect.size()) == 0) ) {
			std::fprintf(stderr, "[Error] Token signature mismatch\n");
			free((void*)user);
			free((void*)pass);
			return;
		}
		// base64 decode payload
		std::string payload;
		// simple base64 decode implementation
		auto b64decode = [](const std::string &in)->std::string {
			std::string out;
			BIO *b64 = BIO_new(BIO_f_base64());
			BIO *bmem = BIO_new_mem_buf((void*)in.data(), in.size());
			bmem = BIO_push(b64, bmem);
			BIO_set_flags(bmem, BIO_FLAGS_BASE64_NO_NL);
			char buffer[4096];
			int r = BIO_read(bmem, buffer, sizeof(buffer));
			if (r > 0) out.assign(buffer, r);
			BIO_free_all(bmem);
			return out;
		};
		payload = b64decode(b64);
		// parse payload username:is_admin:patterns
		char *tmp = strdup(payload.c_str());
		char *p1 = strchr(tmp, ':');
		if (!p1) { free(tmp); free((void*)user); free((void*)pass); return; }
		*p1 = 0; char *uname = tmp;
		char *p2 = strchr(p1+1, ':');
		if (!p2) { free(tmp); free((void*)user); free((void*)pass); return; }
		*p2 = 0; char *isadm = p1+1;
		char *patterns = p2+1;
		bool admin = (strcmp(isadm, "1") == 0);
		std::vector<std::string> pats;
		char *tok = strtok(patterns, ",");
		while (tok) { pats.push_back(std::string(tok)); tok = strtok(NULL, ","); }
		if (pats.empty()) pats.push_back("*");
		// Attempt to augment patterns from engine-side mapping file (admin_user_dbs.txt)
		try {
			std::ifstream mapf("admin_user_dbs.txt");
			if (mapf) {
				std::string line;
				while (std::getline(mapf, line)) {
					// expected format: username=db1,db2
					auto pos = line.find('=');
					if (pos == std::string::npos) continue;
					std::string u = line.substr(0, pos);
					std::string rest = line.substr(pos + 1);
					// trim whitespace
					while (!u.empty() && isspace(u.back())) u.pop_back();
					while (!u.empty() && isspace(u.front())) u.erase(u.begin());
					if (u == std::string(uname)) {
						std::istringstream ss(rest);
						std::string db;
						while (std::getline(ss, db, ',')) {
							// trim
							while (!db.empty() && isspace(db.back())) db.pop_back();
							while (!db.empty() && isspace(db.front())) db.erase(db.begin());
							if (!db.empty()) pats.push_back(db);
						}
					}
				}
			}
		} catch(...) {
			// ignore mapping file errors
		}
		dbms::get_instance()->set_current_user(uname, admin, pats);
		std::printf("[Info] token auth ok -> %s admin=%d\n", uname, admin ? 1 : 0);
		// attempt to read engine-side mapping fallback file to grant explicit DBs
		// Format: each line "username,db_name"
		try {
			std::ifstream mf("admin_user_dbs.txt");
			if (mf.good()) {
				std::string line;
				while (std::getline(mf, line)) {
					if (line.empty()) continue;
					size_t comma = line.find(',');
					if (comma == std::string::npos) continue;
					std::string muser = line.substr(0, comma);
					std::string mdb = line.substr(comma + 1);
					// trim
					auto trim = [](std::string &s) {
						while (!s.empty() && (s.back() == '\n' || s.back() == '\r' || s.back() == ' ')) s.pop_back();
						while (!s.empty() && (s.front() == ' ')) s.erase(0,1);
					};
					trim(muser); trim(mdb);
					if (muser == uname) {
						bool found = false;
						for (const auto &pp : pats) { if (pp == mdb) { found = true; break; } }
						if (!found) pats.push_back(mdb);
					}
				}
				mf.close();
				// re-set current user with augmented patterns
				dbms::get_instance()->set_current_user(uname, admin, pats);
			}
		} catch (...) {
			// best-effort only
		}
		free(tmp);
		free((void*)user);
		free((void*)pass);
		return;
	}
	// fallback plaintext users.txt (deprecated)
	FILE *f = std::fopen("users.txt", "r");
	if (!f) {
		dbms::get_instance()->set_current_user(user, false, std::vector<std::string>{std::string("*")});
		std::printf("[Info] auth: users file not found, allowing user `%s` as guest.\n", user);
		free((void*)user);
		free((void*)pass);
		return;
	}


 

	char buf[4096];
	bool found = false;
	while (fgets(buf, sizeof(buf), f)) {
		size_t len = strlen(buf);
		while(len > 0 && (buf[len-1] == '\n' || buf[len-1] == '\r')) { buf[--len] = 0; }
		if (len == 0) continue;
		char *p1 = strchr(buf, ':');
		if (!p1) continue;
		*p1 = 0; char *uname = buf;
		char *p2 = strchr(p1+1, ':');
		if (!p2) continue;
		*p2 = 0; char *upass = p1+1;
		char *p3 = strchr(p2+1, ':');
		if (!p3) continue;
		*p3 = 0; char *isadm = p2+1;
		char *patterns = p3+1;
		if (strcmp(uname, user) != 0) continue;
		if (strcmp(upass, pass) != 0) {
			std::fprintf(stderr, "[Error] Authentication failed for user `%s`.\n", user);
			found = true;
			break;
		}
		bool admin = (strcmp(isadm, "1") == 0);
		std::vector<std::string> pats;
		char *tok = strtok(patterns, ",");
		while (tok) {
			std::string s(tok);
			if (!s.empty()) pats.push_back(s);
			tok = strtok(nullptr, ",");
		}
		if (pats.empty()) pats.push_back("*");
		dbms::get_instance()->set_current_user(user, admin, pats);
		std::printf("[Info] user `%s` authenticated. admin=%d\n", user, admin ? 1 : 0);
		found = true;
		break;
	}
	fclose(f);
	if (!found) {
		std::fprintf(stderr, "[Error] Authentication failed for user `%s` (not found).\n", user);
	}
	free((void*)user);
	free((void*)pass);
}


/* engine helpers: grant/revoke and transaction control (moved here to avoid breaking execute_auth) */
void execute_grant_db(const char *db_name, const char *user)
{
	if (!db_name || !user) {
		if (db_name) free((void*)db_name);
		if (user) free((void*)user);
		return;
	}
	try {
		// canonical mapping file format: username=db1,db2
		std::map<std::string, std::vector<std::string>> mapping;
		std::ifstream inf("admin_user_dbs.txt");
		if (inf) {
			std::string line;
			while (std::getline(inf, line)) {
				if (line.empty()) continue;
				auto pos = line.find('=');
				if (pos == std::string::npos) continue;
				std::string uname = line.substr(0, pos);
				std::string rest = line.substr(pos + 1);
				std::istringstream ss(rest);
				std::string db;
				while (std::getline(ss, db, ',')) {
					while (!db.empty() && isspace(db.back())) db.pop_back();
					while (!db.empty() && isspace(db.front())) db.erase(0,1);
					if (!db.empty()) mapping[uname].push_back(db);
				}
			}
			inf.close();
		}
		std::string uname(user);
		std::string dbn(db_name);
		auto &vec = mapping[uname];
		bool found = false;
		for (auto &d : vec) if (d == dbn) { found = true; break; }
		if (!found) vec.push_back(dbn);
		// rewrite mapping file
		std::ofstream outf("admin_user_dbs.txt", std::ios::trunc);
		if (outf) {
			for (const auto &p : mapping) {
				outf << p.first << "=";
				for (size_t i = 0; i < p.second.size(); ++i) {
					if (i) outf << ",";
					outf << p.second[i];
				}
				outf << std::endl;
			}
			outf.close();
			std::printf("[Info] granted %s to %s\n", dbn.c_str(), uname.c_str());
		} else {
			std::fprintf(stderr, "[Error] failed to open admin_user_dbs.txt for grant\n");
		}
	} catch (...) {
		// ignore errors
	}
	free((void*)db_name);
	free((void*)user);
}

void execute_begin_transaction()
{
	dbms::get_instance()->begin_transaction();
}

void execute_commit_transaction()
{
	dbms::get_instance()->commit_transaction();
}

void execute_rollback_transaction()
{
	dbms::get_instance()->rollback_transaction();
}

void execute_revoke_db(const char *db_name, const char *user)
{
	if (!db_name || !user) {
		if (db_name) free((void*)db_name);
		if (user) free((void*)user);
		return;
	}
	try {
		std::map<std::string, std::vector<std::string>> mapping;
		std::ifstream inf("admin_user_dbs.txt");
		if (inf) {
			std::string line;
			while (std::getline(inf, line)) {
				if (line.empty()) continue;
				auto pos = line.find('=');
				if (pos == std::string::npos) continue;
				std::string uname = line.substr(0, pos);
				std::string rest = line.substr(pos + 1);
				std::istringstream ss(rest);
				std::string db;
				while (std::getline(ss, db, ',')) {
					while (!db.empty() && isspace(db.back())) db.pop_back();
					while (!db.empty() && isspace(db.front())) db.erase(0,1);
					if (!db.empty()) mapping[uname].push_back(db);
				}
			}
			inf.close();
		}
		std::string uname(user);
		std::string dbn(db_name);
		auto it = mapping.find(uname);
		if (it != mapping.end()) {
			auto &vec = it->second;
			vec.erase(std::remove(vec.begin(), vec.end(), dbn), vec.end());
			if (vec.empty()) mapping.erase(it);
		}
		std::ofstream outf("admin_user_dbs.txt", std::ios::trunc);
		if (outf) {
			for (const auto &p : mapping) {
				outf << p.first << "=";
				for (size_t i = 0; i < p.second.size(); ++i) {
					if (i) outf << ",";
					outf << p.second[i];
				}
				outf << std::endl;
			}
			outf.close();
			std::printf("[Info] revoked %s from %s\n", dbn.c_str(), uname.c_str());
		} else {
			std::fprintf(stderr, "[Error] failed to open admin_user_dbs.txt for rewrite on revoke\n");
		}
	} catch (...) {
		// ignore
	}
	free((void*)db_name);
}
