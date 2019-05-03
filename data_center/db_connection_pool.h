#ifndef DB_CONNECTION_POOL_H_
#define DB_CONNECTION_POOL_H_

#include <vector>
#include <list>
#include <string>
#include <boost/thread.hpp>

#include <mysql.h>

class db_connection_pool;

class db_connection {
	db_connection_pool* pool;
	MYSQL* conn;

	class db_connection_ref_count {
		long ref_count;
	public:
		db_connection_ref_count(long);
		void add_ref_count();
		bool del_ref_count();
		long get_ref_count();
	};

	db_connection_ref_count* ref_count;
public:
	db_connection(MYSQL* _con, db_connection_pool* _pool);
	db_connection(const db_connection&);
	~db_connection();
};

class db_connection_pool {
	std::string ip, port, user_name, password, defaultdb;
	std::list<MYSQL*> pool_conn;
	std::list<MYSQL*> in_use_conn;
	explicit db_connection_pool(std::string, std::string, std::string, std::string);

	static std::string IP, PORT, USER_NAME, PASSWORD, DEFAULT_DB;
	static int INIT_SIZE;
	static int MAX_SIZE;
	volatile static db_connection_pool* only_instance;
	static boost::mutex _mutex;
	friend db_connection;

	bool ret_connection(MYSQL* con);

	MYSQL* new_real_connection();
public:
	volatile static db_connection_pool* get_instance();

	db_connection get_connection();
	~db_connection_pool();
};

#endif