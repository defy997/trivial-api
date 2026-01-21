#include "dbms.h"
#include "database.h"
#include "../table/table.h"
#include "../index/index.h"
#include "../expression/expression.h"
#include "../utils/type_cast.h"
#include "../table/record.h"
#include <vector>
#include <limits>
#include <algorithm>
#include <dirent.h>
#include <climits>
#include <dirent.h>

struct __cache_clear_guard
{
	~__cache_clear_guard() { expression::cache_clear(); }
};

dbms::dbms()
	: output_file(stdout), cur_db(nullptr)
{
}

dbms::~dbms()
{
	close_database();
}

// simple session storage (prototype)
static std::string __current_user = "";
static bool __current_is_admin = false;
static std::vector<std::string> __current_allowed;

void dbms::set_current_user(const char *username, bool admin, const std::vector<std::string> &allowed_patterns)
{
	__current_user = username ? std::string(username) : std::string("");
	__current_is_admin = admin;
	__current_allowed = allowed_patterns;
}

void dbms::clear_current_user()
{
	__current_user.clear();
	__current_is_admin = false;
	__current_allowed.clear();
}

bool dbms::has_user() const
{
	return !__current_user.empty();
}

bool dbms::user_is_admin() const
{
	return __current_is_admin;
}

std::string dbms::get_current_user() const
{
	return __current_user;
}

void dbms::begin_transaction()
{
	// simple single-writer transaction marker
	if (in_transaction) {
		std::fprintf(stderr, "[Warn] transaction already in progress\n");
		return;
	}
	in_transaction = true;
	std::printf("[Info] transaction started\n");
}

void dbms::commit_transaction()
{
	if (!in_transaction) {
		std::fprintf(stderr, "[Warn] no transaction in progress\n");
		return;
	}
	// TODO: flush/clear undo-log
	// clear undo log on commit
	undo_log.clear();
	in_transaction = false;
	std::printf("[Info] transaction committed\n");
}

void dbms::rollback_transaction()
{
	if (!in_transaction) {
		std::fprintf(stderr, "[Warn] no transaction in progress\n");
		return;
	}
	// TODO: apply undo-log to revert changes
	// apply undo log in reverse
	for (auto it = undo_log.rbegin(); it != undo_log.rend(); ++it) {
		const UndoEntry &e = *it;
		table_manager *tm = cur_db->get_table(e.table.c_str());
		if (!tm) continue;
		if (e.type == UndoEntry::U_INSERT) {
			// undo of insert: remove the record
			tm->remove_record(e.rid);
		} else if (e.type == UndoEntry::U_DELETE) {
			// undo of delete: re-insert raw bytes
			tm->insert_raw_record(e.rid, e.raw.data(), (int)e.raw.size());
		} else if (e.type == UndoEntry::U_UPDATE) {
			// undo of update: restore previous column value
			tm->modify_record(e.rid, e.col, e.prev_val.empty() ? nullptr : e.prev_val.data());
		}
	}
	undo_log.clear();
	in_transaction = false;
	std::printf("[Info] transaction rolled back and undo-log applied\n");
}

static bool match_pattern(const std::string &pattern, const std::string &value)
{
	if (pattern == "*") return true;
	// simple prefix match if ends with *
	if (!pattern.empty() && pattern.back() == '*') {
		std::string p = pattern.substr(0, pattern.size()-1);
		return value.rfind(p, 0) == 0;
	}
	return pattern == value;
}

bool dbms::db_allowed(const char *db_name) const
{
	if (!has_user()) return true; // no user set => permissive (backwards compat)
	if (user_is_admin()) return true;
	std::string db(db_name ? db_name : "");
	for (const auto &p : __current_allowed) {
		if (match_pattern(p, db)) return true;
	}
	return false;
}

void dbms::switch_select_output(const char *filename)
{
	if(output_file != stdout)
		std::fclose(output_file);
	if(std::strcmp(filename, "stdout") == 0)
		output_file = stdout;
	else output_file = std::fopen(filename, "w");
}

template<typename Callback>
void dbms::iterate(
	std::vector<table_manager*> required_tables,
	expr_node_t *cond,
	Callback callback)
{
	if(required_tables.size() == 1)
	{
		std::vector<record_manager*> rm_list(1);
		std::vector<int> rid_list(1);
		iterate_one_table_with_index(required_tables[0], cond, [&](table_manager *, record_manager *rm, int rid) -> bool {
			rm_list[0] = rm;
			rid_list[0] = rid;
			return callback(required_tables, rm_list, rid_list);
		} );
	} else {
		iterate_many_tables(required_tables, cond, callback);
		std::puts("[Info] Join many tables by enumerating.");
	}
}

template<typename Callback>
bool dbms::iterate_one_table_with_index(
		table_manager* table,
		expr_node_t *cond,
		Callback callback)
{
	std::vector<expr_node_t*> and_cond;
	extract_and_cond(cond, and_cond);
	expr_node_t *index_cond = nullptr;
	index_manager *index = nullptr;

	auto get_index = [&](column_ref_t *col) -> index_manager*
	{
		int cid = table->lookup_column(col->column);
		if(cid < 0) return nullptr;
		return table->get_index(cid);
	};

	for(expr_node_t *expr : and_cond)
	{
		if(expr->op == OPERATOR_EQ)
		{
			if(expr->right->term_type == TERM_COLUMN_REF)
				std::swap(expr->right, expr->left);

			if(expr->left->term_type == TERM_COLUMN_REF && expr->right->term_type != TERM_COLUMN_REF)
			{
				index = get_index(expr->left->column_ref);
				if(index)
				{
					index_cond = expr;
					break;
				}
			}
		}
	}

	if(!index_cond)
	{
		iterate_one_table(table, cond, callback);
		return false;
	}

	char *key = nullptr;
	switch(index_cond->right->term_type)
	{
		case TERM_INT:
		case TERM_DATE:
			key = (char*)&index_cond->right->val_i;
			break;
		case TERM_FLOAT:
			key = (char*)&index_cond->right->val_f;
			break;
		case TERM_STRING:
			key = index_cond->right->val_s;
			break;
		case TERM_BOOL:
			key = (char*)&index_cond->right->val_b;
			break;
		default:
			break;
	}
	
	auto it = index->get_iterator_lower_bound(key);
	for(; !it.is_end(); it.next())
	{
		int rid;
		record_manager rm = table->open_record_from_index_lower_bound(it.get(), &rid);
		table->cache_record(&rm);

		bool join_ret = false;
		try {
			join_ret = typecast::expr_to_bool(expression::eval(index_cond));
		} catch(const char *msg) {
			std::puts(msg);
			iterate_one_table(table, cond, callback);
			return false;
		}

		if(!join_ret) break;

		if(!callback(table, &rm, rid))
			break;
	}

	return true;
}

template<typename Callback>
void dbms::iterate_one_table(
		table_manager* table,
		expr_node_t *cond,
		Callback callback)
{
	auto bit = table->get_record_iterator_lower_bound(0);
	for(; !bit.is_end(); bit.next())
	{
		int rid;
		record_manager rm(bit.get_pager());
		rm.open(bit.get(), false);
		rm.read(&rid, 4);
		table->cache_record(&rm);
		if(cond)
		{
			bool result = false;
			try {
				result = typecast::expr_to_bool(expression::eval(cond));
			} catch(const char *msg) {
				std::puts(msg);
				return;
			}

			if(!result) continue;
		}

		if(!callback(table, &rm, rid))
			break;
	}
}

void dbms::extract_and_cond(expr_node_t *cond, std::vector<expr_node_t*> &and_cond)
{
	if(!cond) return;
	if(cond->op == OPERATOR_AND)
	{
		extract_and_cond(cond->left, and_cond);
		extract_and_cond(cond->right, and_cond);
	} else {
		and_cond.push_back(cond);
	}
}

template<typename Callback>
void dbms::iterate_many_tables(
	const std::vector<table_manager*> &table_list,
	expr_node_t *cond, Callback callback)
{
	std::vector<record_manager*> record_list(table_list.size());
	std::vector<int> rid_list(table_list.size());
	std::vector<expr_node_t*> and_cond;
	extract_and_cond(cond, and_cond);
	auto lookup_table = [&](const char *name) {
		for(int i = 0; i < (int)table_list.size(); ++i)
			if(std::strcmp(name, table_list[i]->get_table_name()) == 0)
				return i;
		return -1;
	};

	// edge
	std::vector<std::vector<int>> E(table_list.size());
	// join cond
	std::vector<std::vector<expr_node_t*>> J(table_list.size());
	for(auto &v : E) v.resize(table_list.size());
	for(auto &v : J) v.resize(table_list.size());

	// setup join condition graph
	for(expr_node_t *c : and_cond)
	{
		if(c->op == OPERATOR_EQ && 
				c->left->term_type == TERM_COLUMN_REF &&
				c->right->term_type == TERM_COLUMN_REF)
		{
			int tid1 = lookup_table(c->left->column_ref->table);
			int tid2 = lookup_table(c->right->column_ref->table);
			if(tid1 == -1 || tid2 == -1)
			{
				std::fprintf(stderr, "[Error] Table not found!\n");
				return;
			}

			table_manager *tb1 = table_list[tid1];
			table_manager *tb2 = table_list[tid2];

			int cid1 = tb1->lookup_column(c->left->column_ref->column);
			int cid2 = tb2->lookup_column(c->right->column_ref->column);
			if(cid1 == -1 || cid2 == -1)
			{
				std::fprintf(stderr, "[Error] Column not found!\n");
				return;
			}

			index_manager *idx1 = tb1->get_index(cid1);
			index_manager *idx2 = tb2->get_index(cid2);
			if(!idx1 && !idx2)
				continue;

			if(idx2)
			{
				E[tid2][tid1] = 1;
				J[tid2][tid1] = c;
			}
			
			if(idx1)
			{
				E[tid1][tid2] = 1;
				J[tid1][tid2] = c;
			}
		}
	}

	// find the longest path
	int *mark = new int[table_list.size()];
	int *path = new int[table_list.size()];
	int max_depth = 0, start = 0;
	for(int i = 0; i < (int)table_list.size(); ++i)
	{
		int m = 0;
		std::memset(mark, 0, table_list.size() * sizeof(int));
		find_longest_path(i, 0, mark, path, E, ~0u >> 1, m);
		if(m > max_depth)
		{
			max_depth = m;
			start = i;
		}
	}

	int _;
	std::memset(mark, 0, table_list.size() * sizeof(int));
	_ = find_longest_path(start, 0, mark, path, E, max_depth, _);
	assert(_);

	// generate iteration sequence
	std::memset(mark, 0, table_list.size() * sizeof(int));
	for(int i = 0; i <= max_depth; ++i)
		mark[path[i]] = 1;

	int cur = max_depth, len = table_list.size();
	for(int i = 0; i < len; ++i)
		if(!mark[i])
			path[++cur] = i;

	// setup iteration variable
	index_manager **index_ref = new index_manager*[len];
	int *index_cid = new int[len];
	std::fill(index_ref, index_ref + len, nullptr);
	std::fill(index_cid, index_cid + len, -1);

	for(int i = 0; i < max_depth; ++i)
	{
		expr_node_t *join_node = J[path[i]][path[i + 1]];
		if(std::strcmp(join_node->left->column_ref->table, table_list[path[i]]->get_table_name()) == 0)
		{
			index_cid[i] = table_list[path[i + 1]]->lookup_column(
					join_node->right->column_ref->column);
			index_ref[i] = table_list[path[i]]->get_index(
					table_list[path[i]]->lookup_column(
						join_node->left->column_ref->column)
					);
		} else {
			index_cid[i] = table_list[path[i + 1]]->lookup_column(
					join_node->left->column_ref->column);
			index_ref[i] = table_list[path[i]]->get_index(
					table_list[path[i]]->lookup_column(
						join_node->right->column_ref->column)
					);
		}

		assert(index_ref[i]);
	}

	iterate_many_tables_impl(
		table_list, record_list, rid_list,
		J, path, index_cid, index_ref,
		cond, callback, len - 1);

	// debug info
	std::printf("[Info] Iteration order: ");
	for(int i = 0; i < len; ++i)
	{
		if(i != 0) std::printf(", ");
		std::printf("%s", table_list[path[len - i - 1]]->get_table_name());
	}

	std::printf("\n[Info] Index use: ");
	for(int i = 0; i < max_depth; ++i)
	{
		if(i != 0) std::printf(", ");
		expr_node_t *node = J[path[i]][path[i + 1]];
		std::printf("%s.%s-%s.%s",
			node->left->column_ref->table,
			node->left->column_ref->column,
			node->right->column_ref->table,
			node->right->column_ref->column
		);
	}

	std::puts("");

	delete []mark;
	delete []path;
	delete []index_cid;
	delete []index_ref;
}

template<typename Callback>
bool dbms::iterate_many_tables_impl(
	const std::vector<table_manager*> &table_list,
	std::vector<record_manager*> &record_list,
	std::vector<int> &rid_list,
	std::vector<std::vector<expr_node_t*>> &index_cond,
	int *iter_order, int *index_cid, index_manager** index,
	expr_node_t *cond, Callback callback, int now)
{
	if(now < 0)
	{
		if(cond)
		{
			bool result = false;
			try {
				result = typecast::expr_to_bool(expression::eval(cond));
			} catch(const char *msg) {
				std::puts(msg);
				return false; // stop
			}

			if(!result)
				return true; // continue
		}

		if(!callback(table_list, record_list, rid_list))
			return false;  // stop
		return true;  // continue
	} else {
		if(!index[now])
		{
			auto it = table_list[iter_order[now]]->get_record_iterator_lower_bound(0);
			for(; !it.is_end(); it.next())
			{
				record_manager rm(it.get_pager());
				rm.open(it.get(), false);
				rm.read(&rid_list[iter_order[now]], 4);
				// std::printf("%d\n", rid_list[iter_order[now]]);
				table_list[iter_order[now]]->cache_record(&rm);
				record_list[iter_order[now]] = &rm;
				bool ret = iterate_many_tables_impl(
					table_list, record_list, rid_list,
					index_cond, iter_order, index_cid, index,
					cond, callback, now - 1
				);

				if(!ret) return false;
			}
		} else {
			const char *tb_col = table_list[iter_order[now + 1]]->get_cached_column(index_cid[now]);
			table_manager *tb2 = table_list[iter_order[now]];
			auto tb2_it = index[now]->get_iterator_lower_bound(tb_col);
			for(; !tb2_it.is_end(); tb2_it.next())
			{
				int tb2_rid;
				record_manager tb2_rm = tb2->open_record_from_index_lower_bound(tb2_it.get(), &tb2_rid);
				tb2->cache_record(&tb2_rm);

				bool join_ret = false;
				try {
					expr_node_t *join_cond = index_cond[iter_order[now]][iter_order[now + 1]];
					join_ret = typecast::expr_to_bool(expression::eval(join_cond));
				} catch(const char *msg) {
					std::puts(msg);
					return false;
				}

				if(!join_ret) break;

				rid_list[iter_order[now]] = tb2_rid;
				record_list[iter_order[now]] = &tb2_rm;
				bool ret = iterate_many_tables_impl(
					table_list, record_list, rid_list,
					index_cond, iter_order, index_cid, index,
					cond, callback, now - 1
				);

				if(!ret) return false;
			}
		}
	}

	return true;
}

void dbms::close_database()
{
	if(cur_db) 
	{
		cur_db->close();
		delete cur_db;
		cur_db = nullptr;
	}
}

void dbms::switch_database(const char *db_name)
{
	if(cur_db)
	{
		cur_db->close();
		delete cur_db;
		cur_db = nullptr;
	}

	cur_db = new database();
	cur_db->open(db_name);
}

void dbms::create_database(const char *db_name)
{
	database db;
	db.create(db_name);
	db.close();
}

void dbms::drop_database(const char *db_name)
{
	if(cur_db && std::strcmp(cur_db->get_name(), db_name) == 0)
	{
		cur_db->close();
		delete cur_db;
		cur_db = nullptr;
	}

	database db;
	db.open(db_name);
	db.drop();
}

void dbms::show_database(const char *db_name)
{
	database db;
	db.open(db_name);
	db.show_info();
}

void dbms::show_databases()
{
	std::vector<std::string> databases;

	// Scan for all .database files in current directory
	DIR *dir;
	struct dirent *ent;
	if ((dir = opendir(".")) != NULL) {
		while ((ent = readdir(dir)) != NULL) {
			std::string filename = ent->d_name;
			if (filename.length() > 9 && filename.substr(filename.length() - 9) == ".database") {
				std::string db_name = filename.substr(0, filename.length() - 9);
				databases.push_back(db_name);
			}
		}
		closedir(dir);
	}

	std::printf("======== Available Databases ========\n");
	for (const auto& db : databases) {
		std::printf("  %s\n", db.c_str());
	}
	std::printf("======== Total: %zu database(s) ========\n", databases.size());
}

void dbms::show_tables()
{
	if(assert_db_open())
	{
		int table_num = cur_db->get_table_num();
		std::printf("======== Tables in database `%s` ========\n", cur_db->get_name());
		for(int i = 0; i < table_num; i++)
		{
			table_manager *tm = cur_db->get_table(i);
			if(tm != nullptr)
			{
				std::printf("  %s\n", tm->get_table_name());
			}
		}
		std::printf("======== Total: %d table(s) ========\n", table_num);
	}
}

void dbms::drop_table(const char *table_name)
{
	if(assert_db_open())
		cur_db->drop_table(table_name);
}

void dbms::show_table(const char* table_name)
{
	if(assert_db_open())
	{
		table_manager *tm = cur_db->get_table(table_name);
		if(tm == nullptr)
		{
			std::fprintf(stderr, "[Error] Table `%s` not found.\n", table_name);
		} else {
			tm->dump_table_info();
		}
	}
}

void dbms::create_table(const table_header_t *header)
{
	if(assert_db_open())
		cur_db->create_table(header);
}

void dbms::update_rows(const update_info_t *info)
{
	if(!assert_db_open())
		return;

	__cache_clear_guard __guard;
	table_manager *tm = cur_db->get_table(info->table);
	if(tm == nullptr)
	{
		std::fprintf(stderr, "[Error] table `%s` doesn't exists.\n", info->table);
		return;
	}

	int col_id = tm->lookup_column(info->column_ref->column);
	if(col_id < 0)
	{
		std::fprintf(stderr, "[Error] column `%s' not exists.\n", info->column_ref->column);
		return;
	}

	int succ_count = 0, fail_count = 0;
	try {
		iterate_one_table(tm, info->where, [&](table_manager *tm, record_manager *, int rid) -> bool {
			expression val = expression::eval(info->value);
			int col_type = tm->get_column_type(col_id);
			if(!typecast::type_compatible(col_type, val))
				throw "[Error] Incompatible data type.";
			auto term_type = typecast::column_to_term(col_type);
			// capture previous value for undo if in transaction
			if (in_transaction) {
				int col_len = tm->get_column_length(col_id);
				int col_off = tm->get_column_offset(col_id);
				std::vector<char> prev(col_len);
				try {
					record_manager rm = tm->get_record_ptr(rid);
					if (rm.valid()) {
						rm.seek(col_off);
						rm.read(prev.data(), col_len);
					}
				} catch (...) { }
				UndoEntry e;
				e.type = UndoEntry::U_UPDATE;
				e.table = std::string(info->table);
				e.rid = rid;
				e.col = col_id;
				e.prev_val = std::move(prev);
				undo_log.push_back(std::move(e));
			}
			bool ret = tm->modify_record(rid, col_id, typecast::expr_to_db(val, term_type));
			// if(!ret) return false;
			succ_count += ret;
			fail_count += 1 - ret;
			return true;
		} );
	} catch(const char *msg) {
		std::puts(msg);
		return;
	} catch(...) {
	}

	std::printf("[Info] %d row(s) updated, %d row(s) failed.\n",
			succ_count, fail_count);
}

void dbms::select_rows(const select_info_t *info)
{
	if(!assert_db_open())
		return;

	__cache_clear_guard __guard;
	
	// get required tables
	std::vector<std::shared_ptr<table_manager>> alias_tables;
	std::vector<table_manager*> required_tables;
	for(linked_list_t *table_l = info->tables; table_l; table_l = table_l->next)
	{
		table_join_info_t *table_info = (table_join_info_t*)table_l->data;
		table_manager *tm = cur_db->get_table(table_info->table);
		if(tm == nullptr)
		{
			std::fprintf(stderr, "[Error] table `%s` doesn't exists.\n", table_info->table);
			return;
		} else {
			if(table_info->alias == nullptr)
			{
				required_tables.push_back(tm);
			} else {
				auto alias = tm->mirror(table_info->alias);
				alias_tables.push_back(alias);
				required_tables.push_back(alias.get());
			}
		}
	}

	// get select expression name
	std::vector<expr_node_t*> exprs;
	std::vector<std::string> expr_names;
	bool is_aggregate = false;
	for(linked_list_t *link_p = info->exprs; link_p; link_p = link_p->next)
	{
		expr_node_t *expr = (expr_node_t*)link_p->data;
		is_aggregate |= expression::is_aggregate(expr);
		exprs.push_back(expr);
		// Handle alias: use alias name if present
		std::string col_name;
		if (expr->op == OPERATOR_ALIAS && expr->val_s) {
			col_name = expr->val_s;
		} else {
			col_name = expression::to_string(expr);
		}
		expr_names.push_back(col_name);
	}

	// output header info
	for(size_t i = 0; i < exprs.size(); ++i)
	{
		if(i != 0) std::fprintf(output_file, ",");
		std::fprintf(output_file, "%s", expr_names[i].c_str());
	}

	if(exprs.size() == 0)
	{
		for(size_t i = 0; i < required_tables.size(); ++i)
		{
			if(i != 0) std::fprintf(output_file, ",");
			required_tables[i]->dump_header(output_file);
		}
	}

	std::fprintf(output_file, "\n");

	if(is_aggregate && !info->group_by)
	{
		select_rows_aggregate(
			info,
			required_tables,
			exprs,
			expr_names
		);

		return;
	}

	// iterate records
	int counter = 0;
	if (info->group_by) {
		// GROUP BY processing
		struct GroupAgg {
			int count = 0;
			std::vector<long long> sum_i;
			std::vector<double> sum_f;
			std::vector<long long> min_i;
			std::vector<long long> max_i;
			std::vector<double> min_f;
			std::vector<double> max_f;
			std::vector<int> has_numeric;
			std::vector<std::string> first_values; // for non-agg exprs
		};
		std::map<std::string, GroupAgg> groups;

		// determine which exprs are aggregate and operator type
		std::vector<int> isAggFlag(exprs.size(), 0);
		std::vector<int> aggOp(exprs.size(), 0);
		for (size_t i = 0; i < exprs.size(); ++i) {
			expr_node_t *expr = exprs[i];
			// Handle AS syntax: check if it's an alias of an aggregate
			if (expr->op == OPERATOR_ALIAS && expr->left && expression::is_aggregate(expr->left)) {
				isAggFlag[i] = 1;
				aggOp[i] = expr->left->op;
			} else if (expr->op == OPERATOR_ALIAS) {
				// AS alias of non-aggregate - treat as non-aggregate
				isAggFlag[i] = 0;
				aggOp[i] = 0;
			} else if (expression::is_aggregate(expr)) {
				isAggFlag[i] = 1;
				aggOp[i] = expr->op;
			}
		}

		iterate(required_tables, info->where,
			[&](const std::vector<table_manager*> &tables,
				const std::vector<record_manager*> &records,
				const std::vector<int>& ) -> bool
			{
				// build group key
				std::string key;
				for (linked_list_t *g = info->group_by; g; g = g->next) {
					expr_node_t *ge = (expr_node_t*)g->data;
					expression gv;
					try {
						gv = expression::eval(ge);
					} catch (const char *e) {
						std::fprintf(stderr, "%s\n", e);
						return false;
					}
					char buf[128];
					switch(gv.type) {
						case TERM_INT: snprintf(buf, sizeof(buf), "%lld", (long long)gv.val_i); break;
						case TERM_FLOAT: snprintf(buf, sizeof(buf), "%f", gv.val_f); break;
						case TERM_STRING: snprintf(buf, sizeof(buf), "%s", gv.val_s ? gv.val_s : ""); break;
						case TERM_NULL: snprintf(buf, sizeof(buf), "NULL"); break;
						default: snprintf(buf, sizeof(buf), "");
					}
					if (!key.empty()) key += "|";
					key += buf;
				}

				auto &ga = groups[key];
				if (ga.count == 0) {
					// initialize per-group accumulators
					ga.sum_i.resize(exprs.size(), 0);
					ga.sum_f.resize(exprs.size(), 0.0);
					ga.min_i.resize(exprs.size(), LLONG_MAX);
					ga.max_i.resize(exprs.size(), LLONG_MIN);
					ga.min_f.resize(exprs.size(), std::numeric_limits<double>::max());
					ga.max_f.resize(exprs.size(), std::numeric_limits<double>::lowest());
					ga.has_numeric.resize(exprs.size(), 0);
					ga.first_values.resize(exprs.size(), "");
				}

				// update aggregators and first values
				for (size_t i = 0; i < exprs.size(); ++i) {
					if (isAggFlag[i]) {
						if (aggOp[i] == OPERATOR_COUNT) {
							ga.sum_i[i] += 1;
						} else {
							// evaluate inner expr (left)
							expression val;
							try {
								val = expression::eval(exprs[i]->left);
							} catch (const char *e) {
								std::fprintf(stderr, "%s\n", e);
								return false;
							}
							if (val.type == TERM_INT) {
								ga.sum_i[i] += val.val_i;
								ga.has_numeric[i] = 1;
								if (val.val_i < ga.min_i[i]) ga.min_i[i] = val.val_i;
								if (val.val_i > ga.max_i[i]) ga.max_i[i] = val.val_i;
							} else if (val.type == TERM_FLOAT) {
								ga.sum_f[i] += val.val_f;
								ga.has_numeric[i] = 1;
								if (val.val_f < ga.min_f[i]) ga.min_f[i] = val.val_f;
								if (val.val_f > ga.max_f[i]) ga.max_f[i] = val.val_f;
							} else if (val.type == TERM_STRING && val.val_s) {
								// treat string min/max lexicographically via string conversion
								std::string s = val.val_s;
								if (ga.first_values[i].empty()) ga.first_values[i] = s;
								// track min/max via string compare - store in first_values as sentinel if needed
							}
						}
					} else {
						// non-aggregate: store first value (string)
						if (ga.first_values[i].empty()) {
							expression val;
							try {
								val = expression::eval(exprs[i]);
							} catch (const char *e) {
								std::fprintf(stderr, "%s\n", e);
								return false;
							}
							char buf[128];
							switch(val.type) {
								case TERM_INT: snprintf(buf, sizeof(buf), "%lld", (long long)val.val_i); break;
								case TERM_FLOAT: snprintf(buf, sizeof(buf), "%f", val.val_f); break;
								case TERM_STRING: snprintf(buf, sizeof(buf), "%s", val.val_s ? val.val_s : ""); break;
								case TERM_NULL: snprintf(buf, sizeof(buf), "NULL"); break;
								default: snprintf(buf, sizeof(buf), "");
							}
							ga.first_values[i] = std::string(buf);
						}
					}
				}

				ga.count += 1;
				return true;
			}
		);

		// prepare output rows from groups
		std::vector<std::vector<std::string>> out_rows;
		for (const auto &p : groups) {
			const auto &ga = p.second;
			std::vector<std::string> row;
			for (size_t i = 0; i < exprs.size(); ++i) {
				if (isAggFlag[i]) {
					int op = exprs[i]->op;
					if (op == OPERATOR_COUNT) {
						row.push_back(std::to_string(ga.sum_i[i]));
					} else if (op == OPERATOR_SUM) {
						if (ga.has_numeric[i]) {
							if (ga.sum_f[i] != 0.0) row.push_back(std::to_string(ga.sum_f[i]));
							else row.push_back(std::to_string(ga.sum_i[i]));
						} else row.push_back(ga.first_values[i]);
					} else if (op == OPERATOR_AVG) {
						if (ga.has_numeric[i]) {
							double s = (ga.sum_f[i] != 0.0) ? ga.sum_f[i] : (double)ga.sum_i[i];
							double avg = s / (double)ga.count;
							row.push_back(std::to_string(avg));
						} else row.push_back(ga.first_values[i]);
					} else if (op == OPERATOR_MIN) {
						if (ga.has_numeric[i]) {
							if (ga.min_f[i] != std::numeric_limits<double>::max()) row.push_back(std::to_string(ga.min_f[i]));
							else row.push_back(std::to_string(ga.min_i[i]));
						} else row.push_back(ga.first_values[i]);
					} else if (op == OPERATOR_MAX) {
						if (ga.has_numeric[i]) {
							if (ga.max_f[i] != std::numeric_limits<double>::lowest()) row.push_back(std::to_string(ga.max_f[i]));
							else row.push_back(std::to_string(ga.max_i[i]));
						} else row.push_back(ga.first_values[i]);
					} else {
						row.push_back(ga.first_values[i]);
					}
				} else {
					row.push_back(ga.first_values[i]);
				}
			}
			out_rows.push_back(row);
		}

		// optional ORDER BY after GROUP BY: simple lexicographic/numeric sort on first order column if matches expr_names
		if (info->order_by && out_rows.size() > 1) {
			// find first order expr index matching expr_names
			int order_idx = -1;
			if (info->order_by) {
				order_item_t *oi = (order_item_t*)info->order_by->data;
				std::string target = expression::to_string(oi->expr);

				for (size_t i = 0; i < expr_names.size(); ++i) {
					if (expr_names[i] == target) { order_idx = (int)i; break; }
				}
			}
			if (order_idx >= 0) {
				// get sort direction (default ASC)
				int asc = 1;
				if (info->order_by) {
					order_item_t *oi = (order_item_t*)info->order_by->data;
					asc = oi->asc;
				}

				std::stable_sort(out_rows.begin(), out_rows.end(), [&](const std::vector<std::string> &a, const std::vector<std::string> &b){
					// numeric compare if both numeric
					char *end;
					double da = strtod(a[order_idx].c_str(), &end);
					bool an = !(end == a[order_idx].c_str() || strcmp(end, "") != 0);
					da = an ? da : 0.0;
					double db = strtod(b[order_idx].c_str(), &end);
					bool bn = !(end == b[order_idx].c_str() || strcmp(end, "") != 0);
					db = bn ? db : 0.0;

					int cmp_result = 0;
					if (an && bn) {
						cmp_result = (da < db) ? -1 : (da > db) ? 1 : 0;
					} else {
						cmp_result = (a[order_idx] < b[order_idx]) ? -1 : (a[order_idx] > b[order_idx]) ? 1 : 0;
					}

					// apply sort direction
					if (asc) {
						return cmp_result < 0;  // ASC
					} else {
						return cmp_result > 0;  // DESC
					}
				});
			}
		}

		// output rows
		for (const auto &r : out_rows) {
			for (size_t i = 0; i < r.size(); ++i) {
				if (i != 0) std::fprintf(output_file, ",");
				std::fprintf(output_file, "%s", r[i].c_str());
			}
			std::fprintf(output_file, "\n");
		}
		std::printf("[Info] %d group(s) returned.\n", (int)out_rows.size());
		std::fprintf(output_file, "\n");
		std::fflush(output_file);
		return;
	} else if (info->order_by) {
		// ORDER BY is temporarily disabled due to memory issues
		std::fprintf(stderr, "[Error] ORDER BY is currently not supported.\n");
		return;
		struct RowData {
			std::vector<std::string> keys;  // use strings instead of expressions to avoid memory issues
			std::vector<std::string> values; // printed fields (for exprs)
			std::string raw_line; // for select * case
		};
		std::vector<RowData> collected;

		iterate(required_tables, info->where,
			[&](const std::vector<table_manager*> &tables,
				const std::vector<record_manager*> &records,
				const std::vector<int>& ) -> bool
			{
				RowData rd;
				// evaluate select exprs into strings
				for(size_t i = 0; i < exprs.size(); ++i)
				{
					expression ret;
					try {
						ret = expression::eval(exprs[i]);
					} catch (const char *e) {
						std::fprintf(stderr, "%s\n", e);
						return false;
					}
					// convert to string
					char buf[128];
					switch(ret.type)
					{
						case TERM_INT:
							snprintf(buf, sizeof(buf), "%d", ret.val_i);
							break;
						case TERM_FLOAT:
							snprintf(buf, sizeof(buf), "%f", ret.val_f);
							break;
						case TERM_STRING:
							snprintf(buf, sizeof(buf), "%s", ret.val_s);
							break;
						case TERM_BOOL:
							snprintf(buf, sizeof(buf), "%s", ret.val_b ? "TRUE" : "FALSE");
							break;
						case TERM_DATE: {
							char date_buf[32];
							time_t time = ret.val_i;
							auto tm = std::localtime(&time);
							std::strftime(date_buf, 32, DATE_TEMPLATE, tm);
							snprintf(buf, sizeof(buf), "%s", date_buf);
							break; }
						case TERM_NULL:
							snprintf(buf, sizeof(buf), "NULL");
							break;
						default:
							snprintf(buf, sizeof(buf), "");
					}
					rd.values.push_back(std::string(buf));
				}

				// for select * capture raw record line
				if(exprs.size() == 0)
				{
					// capture into memory stream with safer memory handling
					char *mem = nullptr;
					size_t memlen = 0;
					FILE *m = open_memstream(&mem, &memlen);
					if (m) {
						for(size_t i = 0; i < tables.size(); ++i)
						{
							if(i != 0) std::fprintf(m, ",");
							tables[i]->dump_record(m, records[i]);
						}
						std::fflush(m);
						if (mem && memlen > 0) {
							// ensure mem is null-terminated and safe to use
							if (memlen < SIZE_MAX - 1) {
								rd.raw_line = std::string(mem, memlen);
							}
							std::free(mem);
						}
						std::fclose(m);
					}
				}

				// evaluate order_by keys as strings to avoid expression memory issues
				for(linked_list_t *o = info->order_by; o; o = o->next) {
					order_item_t *oi = (order_item_t*)o->data;
					try {
						// evaluate expression and convert to string
						expression k = expression::eval(oi->expr);
						char buf[128];
						switch(k.type) {
							case TERM_INT: sprintf(buf, "%d", k.val_i); break;
							case TERM_FLOAT: sprintf(buf, "%.6f", k.val_f); break;
							case TERM_STRING: sprintf(buf, "%s", k.val_s ? k.val_s : ""); break;
							case TERM_BOOL: sprintf(buf, "%s", k.val_b ? "true" : "false"); break;
							default: sprintf(buf, ""); break;
						}
						rd.keys.push_back(std::string(buf));
					} catch (const char *e) {
						std::fprintf(stderr, "%s\n", e);
						return false;
					}
				}

				collected.push_back(std::move(rd));
				++counter;
				return true;
			}
		);

		// comparator - simplified to avoid memory corruption
		auto cmp = [&](const RowData &a, const RowData &b) -> bool {
			size_t n = std::min(a.keys.size(), b.keys.size());
			for (size_t i = 0; i < n; ++i) {
				const std::string &ka = a.keys[i];
				const std::string &kb = b.keys[i];

				// handle empty/null strings
				if (ka.empty() && !kb.empty()) return true;
				if (!ka.empty() && kb.empty()) return false;

				// string comparison (works for numeric strings too since they sort correctly)
				int cmp = ka.compare(kb);
				if (cmp != 0) return cmp < 0;
			}
			// equal keys -> preserve insertion order
			return false;
		};

		// sort the collected rows
		std::stable_sort(collected.begin(), collected.end(), cmp);

		// output rows
		for(const auto &rd : collected) {
			if (exprs.size() > 0) {
				for(size_t i = 0; i < rd.values.size(); ++i) {
					if (i != 0) std::fprintf(output_file, ",");
					std::fprintf(output_file, "%s", rd.values[i].c_str());
				}
				std::fprintf(output_file, "\n");
			} else {
				// raw line already contains newline as produced by dump_record; ensure newline
				std::fprintf(output_file, "%s\n", rd.raw_line.c_str());
			}
		}
		std::printf("[Info] %d row(s) selected.\n", (int)collected.size());
		std::fprintf(output_file, "\n");
		std::fflush(output_file);
		return;
	} else {
		iterate(required_tables, info->where,
			[&](const std::vector<table_manager*> &tables,
				const std::vector<record_manager*> &records,
				const std::vector<int>& )
			{
				for(size_t i = 0; i < exprs.size(); ++i)
				{
					expression ret;
					try {
						ret = expression::eval(exprs[i]);
					} catch (const char *e) {
						std::fprintf(stderr, "%s\n", e);
						return false;
					}
					if(i != 0) std::fprintf(output_file, ",");
					switch(ret.type)
					{
						case TERM_INT:
							std::fprintf(output_file, "%d", ret.val_i);
							break;
						case TERM_FLOAT:
							std::fprintf(output_file, "%f", ret.val_f);
							break;
						case TERM_STRING:
							std::fprintf(output_file, "%s", ret.val_s);
							break;
						case TERM_BOOL:
							std::fprintf(output_file, "%s", ret.val_b ? "TRUE" : "FALSE");
							break;
						case TERM_DATE: {
							char date_buf[32];
							time_t time = ret.val_i;
							auto tm = std::localtime(&time);
							std::strftime(date_buf, 32, DATE_TEMPLATE, tm);
							std::fprintf(output_file, "%s", date_buf);
							break; }
						case TERM_NULL:
							std::fprintf(output_file, "NULL");
							break;
						default:
							debug_puts("[Error] Data type not supported!");
					}
				}

				if(exprs.size() == 0)
				{
					for(size_t i = 0; i < tables.size(); ++i)
					{
						if(i != 0) std::fprintf(output_file, ",");
						tables[i]->dump_record(output_file, records[i]);
					}
				}
				std::fprintf(output_file, "\n");
				++counter;
				return true;
			}
		);
	}

	std::printf("[Info] %d row(s) selected.\n", counter);
	std::fprintf(output_file, "\n");
	std::fflush(output_file);
}

void dbms::select_rows_aggregate(
	const select_info_t *info,
	const std::vector<table_manager*> &required_tables,
	const std::vector<expr_node_t*> &exprs,
	const std::vector<std::string> &)
{
	if(exprs.size() != 1)
	{
		std::fprintf(stderr, "[Error] Support only for one select expression for aggregate select.");
		return;
	}

	// check aggregate type
	expr_node_t *expr = exprs[0];
	int val_i = 0;
	float val_f = 0;
	if(expr->op == OPERATOR_MIN)
	{
		val_i = std::numeric_limits<int>::max();
		val_f = std::numeric_limits<float>::max();
	} else if(expr->op == OPERATOR_MAX) {
		val_i = std::numeric_limits<int>::min();
		val_f = std::numeric_limits<float>::min();
	}

	term_type_t agg_type = TERM_NONE;

	int counter = 0;
	iterate(required_tables, info->where,
		[&](const std::vector<table_manager*> &,
			const std::vector<record_manager*> &,
			const std::vector<int>& )
		{
			if(expr->op != OPERATOR_COUNT)
			{
				expression ret;
				try {
					ret = expression::eval(expr->left);
				} catch (const char *e) {
					std::fprintf(stderr, "%s\n", e);
					return false;
				}

				agg_type = ret.type;
				if(ret.type == TERM_FLOAT)
				{
					switch(expr->op)
					{
						case OPERATOR_SUM:
						case OPERATOR_AVG:
							val_f += ret.val_f;
							break;
						case OPERATOR_MIN:
							if(ret.val_f < val_f)
								val_f = ret.val_f;
							break;
						case OPERATOR_MAX:
							if(ret.val_f > val_f)
								val_f = ret.val_f;
							break;
						default: break;
					}
				} else {
					switch(expr->op)
					{
						case OPERATOR_SUM:
						case OPERATOR_AVG:
							val_i += ret.val_i;
							break;
						case OPERATOR_MIN:
							if(ret.val_i < val_i)
								val_i = ret.val_i;
							break;
						case OPERATOR_MAX:
							if(ret.val_i > val_i)
								val_i = ret.val_i;
							break;
						default: break;
					}
				}
			}

			++counter;
			return true;
		}
	);

	if(expr->op == OPERATOR_COUNT)
	{
		std::fprintf(output_file, "%d\n", counter);
	} else {
		if(agg_type != TERM_FLOAT && agg_type != TERM_INT)
		{
			std::fprintf(stderr, "[Error] Aggregate only support for int and float type.\n");
			return;
		}

		if(expr->op == OPERATOR_AVG)
		{
			if(agg_type == TERM_INT)
				val_f = double(val_i) / counter;
			else val_f /= counter;
			std::fprintf(output_file, "%f\n", val_f);
		} else if(agg_type == TERM_FLOAT) {
			std::fprintf(output_file, "%f\n", val_f);
		} else if(agg_type == TERM_INT) {
			std::fprintf(output_file, "%d\n", val_i);
		}
	}

	std::printf("[Info] %d row(s) selected.\n", counter);
	std::fprintf(output_file, "\n");
	std::fflush(output_file);
}

void dbms::delete_rows(const delete_info_t *info)
{
	if(!assert_db_open())
		return;
	__cache_clear_guard __guard;

	std::vector<int> delete_list;
	table_manager *tm = cur_db->get_table(info->table);
	if(tm == nullptr)
	{
		std::fprintf(stderr, "[Error] table `%s` doesn't exists.\n", info->table);
		return;
	}

	iterate_one_table_with_index(tm, info->where,
		[&delete_list](table_manager*, record_manager*, int rid) -> bool {
			delete_list.push_back(rid);
			return true;
		} );

	int counter = 0;
	for(int rid : delete_list)
	{
		// capture full raw record bytes for undo if in transaction
		if (in_transaction) {
			// compute record size: 4 (rid) + sum col lengths
			int rec_size = 4;
			for (int ci = 0; ci < tm->get_column_num(); ++ci) rec_size += tm->get_column_length(ci);
			std::vector<char> buf(rec_size);
			record_manager rm = tm->get_record_ptr(rid);
			if (rm.valid()) {
				try {
					rm.read(buf.data(), rec_size);
					UndoEntry e;
					e.type = UndoEntry::U_DELETE;
					e.table = std::string(info->table);
					e.rid = rid;
					e.raw = std::move(buf);
					undo_log.push_back(std::move(e));
				} catch (...) {
				}
			}
		}
		counter += tm->remove_record(rid);
	}
	std::printf("[Info] %d row(s) deleted.\n", counter);
}

void dbms::insert_rows(const insert_info_t *info)
{
	if(!assert_db_open())
		return;
	__cache_clear_guard __guard;

	table_manager *tb = cur_db->get_table(info->table);
	if(tb == nullptr)
	{
		std::fprintf(stderr, "[Error] table `%s` not found.\n", info->table);
		return;
	}

	std::vector<int> cols_id;
	if(info->columns == nullptr)
	{
		// exclude __rowid__, which has the largest index
		for(int i = 0; i < tb->get_column_num() - 1; ++i)
			cols_id.push_back(i);
	} else {
		for(linked_list_t *link_ptr = info->columns; link_ptr; link_ptr = link_ptr->next)
		{
			column_ref_t *column = (column_ref_t*)link_ptr->data;
			int cid = tb->lookup_column(column->column);
			if(cid < 0)
			{
				std::fprintf(stderr, "[Error] No column `%s` in table `%s`.\n",
					column->column, tb->get_table_name());
				return;
			}
			cols_id.push_back(cid);
		}
	}

	int count_succ = 0, count_fail = 0;
	for(linked_list_t *list = info->values; list; list = list->next)
	{
		tb->init_temp_record();
		linked_list_t *expr_list = (linked_list_t*)list->data;
		unsigned val_num = 0;
		for(linked_list_t *i = expr_list; i; i = i->next, ++val_num);
		if(val_num != cols_id.size())
		{
			std::fprintf(stderr, "[Error] column size not equal.");
			continue;
		}

		bool succ = true;
		for(auto it = cols_id.begin(); expr_list; expr_list = expr_list->next, ++it)
		{
			expression v;
			try {
				v = expression::eval((expr_node_t*)expr_list->data);
			} catch (const char *e) {
				std::fprintf(stderr, "%s\n", e);
				return;
			}

			auto col_type = tb->get_column_type(*it);
			if(!typecast::type_compatible(col_type, v))
			{
				std::fprintf(stderr, "[Error] incompatible type.\n");
				return;
			}
			
			term_type_t desired_type = typecast::column_to_term(col_type);
			char *db_val = typecast::expr_to_db(v, desired_type);
			if(!tb->set_temp_record(*it, db_val))
			{
				succ = false;
				break;
			}
		}

		if (succ) {
			int newrid = tb->insert_record();
			if (newrid > 0) {
				succ = true;
				// record undo entry to delete this rid on rollback
				if (in_transaction) {
					UndoEntry e;
					e.type = UndoEntry::U_INSERT;
					e.table = std::string(info->table);
					e.rid = newrid;
					undo_log.push_back(std::move(e));
				}
			} else {
				succ = false;
			}
		}
		count_succ += succ;
		count_fail += 1 - succ;
	}

	std::printf("[Info] %d row(s) inserted, %d row(s) failed.\n", count_succ, count_fail);
}

void dbms::drop_index(const char *tb_name, const char *col_name)
{
}

void dbms::create_index(const char *tb_name, const char *col_name)
{
	if(!assert_db_open())
		return;
	table_manager *tb = cur_db->get_table(tb_name);
	if(tb == nullptr)
	{
		std::fprintf(stderr, "[Error] table `%s` not exists.\n", tb_name);
	} else {
		tb->create_index(col_name);
	}
}

bool dbms::assert_db_open()
{
	if(cur_db && cur_db->is_opened())
		return true;
	std::fprintf(stderr, "[Error] database is not opened.\n");
	return false;
}

expr_node_t *dbms::get_join_cond(expr_node_t *cond)
{
	if(!cond) return nullptr;
	if(cond->left->term_type == TERM_COLUMN_REF && cond->right->term_type == TERM_COLUMN_REF)
	{
		return cond;
	} else {
		return nullptr;
	}
}

bool dbms::find_longest_path(int now, int depth, int *mark, int *path, std::vector<std::vector<int>> &E, int excepted_len, int &max_depth)
{
	mark[now] = 1;
	path[depth] = now;
	if(depth > max_depth)
		max_depth = depth;
	if(depth == excepted_len)
		return true;
	for(int i = 0; i != (int)E.size(); ++i)
	{
		if(!E[now][i] || mark[i]) continue;
		if(find_longest_path(i, depth + 1, mark, path, E, excepted_len, max_depth))
			return true;
	}

	mark[now] = 0;
	return false;
}

bool dbms::value_exists(const char *table, const char *column, const char *data)
{
	if(!assert_db_open())
		return false;
	table_manager *tm = cur_db->get_table(table);
	if(tm == nullptr) 
	{
		std::printf("[Error] No table named `%s`\n", table);
		return false;
	}

	return tm->value_exists(column, data);
}
