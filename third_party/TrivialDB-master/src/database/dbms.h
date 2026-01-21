#ifndef __TRIVIALDB_DBMS__
#define __TRIVIALDB_DBMS__
#include "database.h"
#include "../table/table.h"
#include "../parser/defs.h"
#include "../expression/expression.h"
#include <cstdio>
#include <string>
#include <vector>

class dbms
{
	FILE *output_file;
	database *cur_db;
	bool in_transaction;
	// undo-log structure for simple transactions
	struct UndoEntry {
		enum Type { U_INSERT, U_DELETE, U_UPDATE } type;
		std::string table;
		int rid;
		std::vector<char> raw; // for DELETE full record
		int col; // for UPDATE target column
		std::vector<char> prev_val; // previous value bytes for UPDATE
	};
	std::vector<UndoEntry> undo_log;
	bool writer_locked;
private:
	dbms();

public:
	~dbms();

	void close_database();
	void show_database(const char *db_name);
	void show_databases();
	void show_tables();
	void switch_database(const char *db_name);
	void drop_database(const char *db_name);
	void create_database(const char *db_name);

	void create_table(const table_header_t *header);
	void show_table(const char *table_name);
	void drop_table(const char *table_name);

	void create_index(const char *tb_name, const char *col_name);
	void drop_index(const char *tb_name, const char *col_name);

	void insert_rows(const insert_info_t *info);
	void delete_rows(const delete_info_t *info);
	void select_rows(const select_info_t *info);
	void update_rows(const update_info_t *info);

	void switch_select_output(const char *filename);

	void select_rows_aggregate(
		const select_info_t *info,
		const std::vector<table_manager*> &required_tables,
		const std::vector<expr_node_t*> &exprs,
		const std::vector<std::string> &expr_names);

	bool value_exists(const char *table, const char *column, const char *data);

public:
	bool assert_db_open();
	void cache_record(table_manager *tm, record_manager *rm);

	template<typename Callback>
	void iterate(std::vector<table_manager*> required_tables, expr_node_t *cond, Callback callback);

	template<typename Callback>
	void iterate_one_table(table_manager* table,
			expr_node_t *cond, Callback callback);
	template<typename Callback>
	bool iterate_one_table_with_index(table_manager* table,
			expr_node_t *cond, Callback callback);
	template<typename Callback>
	bool iterate_many_tables_impl(
		const std::vector<table_manager*> &table_list,
		std::vector<record_manager*> &record_list,
		std::vector<int> &rid_list,
		std::vector<std::vector<expr_node_t*>> &index_cond,
		int *iter_order, int *index_cid, index_manager** index,
		expr_node_t *cond, Callback callback, int now);
	template<typename Callback>
	void iterate_many_tables(
		const std::vector<table_manager*> &table_list,
		expr_node_t *cond, Callback callback);

	static expr_node_t *get_join_cond(expr_node_t *cond);
	static void extract_and_cond(expr_node_t *cond, std::vector<expr_node_t*> &and_cond);
	static bool find_longest_path(int now, int depth, int *mark, int *path, std::vector<std::vector<int>> &E, int excepted_len, int &max_depth);

public:
	static dbms* get_instance()
	{
		static dbms ms;
		return &ms;
	}
	/* --- auth/session API --- */
	void set_current_user(const char *username, bool admin, const std::vector<std::string> &allowed_patterns);
	void clear_current_user();
	bool has_user() const;
	bool user_is_admin() const;
	std::string get_current_user() const;
	bool db_allowed(const char *db_name) const;
	/* transaction control */
	void begin_transaction();
	void commit_transaction();
	void rollback_transaction();
};

#endif
