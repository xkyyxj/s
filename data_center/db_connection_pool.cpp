#include "db_connection_pool.h"

std::string db_connection_pool::IP = "127.0.0.1";
std::string db_connection_pool::PORT = "3306";
std::string db_connection_pool::USER_NAME = "root";
std::string db_connection_pool::PASSWORD = "123";

int db_connection_pool::INIT_SIZE = 10;
int db_connection_pool::MAX_SIZE = 40;

boost::mutex db_connection_pool::_mutex;

// --智能指针引用计数组件--
db_connection::db_connection_ref_count::db_connection_ref_count(long init_count) {
	this->ref_count = init_count;
}

// 增加引用计数
void db_connection::db_connection_ref_count::add_ref_count() {
	++this->ref_count;
}

// 减少引用计数
bool db_connection::db_connection_ref_count::del_ref_count() {
	// 当引用计数大于1的时候方可减少，否则返回false
	if (this->ref_count > 1) {
		--this->ref_count;
		return true;
	}
	return false;
}

// 返回引用计数
long db_connection::db_connection_ref_count::get_ref_count() {
	return this->ref_count;
}

// 数据库连接对象，实则为一个“智能指针”，只能够用于单线程当中
db_connection::db_connection(MYSQL* _con, db_connection_pool* _pool) {
	this->conn = _con;
	this->pool = _pool;
	this->ref_count = new db_connection_ref_count(1);
}

// 拷贝构造函数
db_connection::db_connection(const db_connection& ori_conn) {
	this->conn = ori_conn.conn;
	this->pool = ori_conn.pool;
	ori_conn.ref_count->add_ref_count();
	this->ref_count = ori_conn.ref_count;
}

// 析构函数
db_connection::~db_connection() {
	// 如果引用计数为1的话，那么就销毁引用计数，并将数据库连接返回到连接池当中
	if (this->ref_count->get_ref_count() == 1) {
		this->pool->ret_connection(this->conn);
		delete this->ref_count;
	}
	// 否则引用计数减一
	else {
		this->ref_count->del_ref_count();
	}
}

// 数据库连接池相关的函数
// 构造函数
db_connection_pool::db_connection_pool(std::string ip, std::string port, 
	std::string user_name, std::string password): pool_conn(), in_use_conn(), ip(ip), port(port),
	user_name(user_name), password(password), defaultdb("stock") {
	
}

// 析构函数，释放所有的连接
db_connection_pool::~db_connection_pool() {
	auto list_begin = pool_conn.begin(), list_end = pool_conn.end();
	while (list_begin != list_end) {
		MYSQL* temp = *list_begin;
		mysql_close(temp);
		++list_begin;
	}

	list_begin = in_use_conn.begin(), list_end = in_use_conn.end();
	while (list_begin != list_end) {
		MYSQL* temp = *list_begin;
		mysql_close(temp);
		++list_begin;
	}
}

// 返回单例对象
volatile db_connection_pool* db_connection_pool::get_instance() {
	if(only_instance == nullptr) {
		_mutex.lock();
		if (only_instance == nullptr) {
			only_instance = new db_connection_pool(db_connection_pool::IP, db_connection_pool::PORT, db_connection_pool::USER_NAME, db_connection_pool::PASSWORD);
		}
		_mutex.unlock();
	}
	return only_instance;
}

// 创建一个真正的数据库连接
MYSQL* db_connection_pool::new_real_connection() {
	MYSQL* temp_con = NULL;
	temp_con = mysql_init((MYSQL*)0);
	if (mysql_real_connect(temp_con, this->ip.data(), this->user_name.data(), this->password.data(), this->defaultdb.data(), 3306, NULL, 0)) {
		return temp_con;
	}
	else {
		return nullptr;
	}
}

// 返回连接
db_connection db_connection_pool::get_connection() {
	_mutex.lock();
	if (pool_conn.size() == 0) {
		MYSQL* temp_ptr = new_real_connection();
		if (temp_ptr != nullptr) {
			in_use_conn.push_back(temp_ptr);
			db_connection conn(temp_ptr, this);
			return conn;
		}
	}
	else {
		MYSQL* temp_ptr = pool_conn.back();
		pool_conn.pop_back();
		db_connection conn(temp_ptr, this);
		return conn;
	}
	return db_connection(nullptr, this);
	_mutex.unlock();
}

// 将连接归还到连接池当中，会在db_connection当中调用
bool db_connection_pool::ret_connection(MYSQL* con) {
	_mutex.lock();
	if (con != nullptr) {
		pool_conn.push_back(con);
		in_use_conn.remove(con);
	}
	_mutex.unlock();
	return true;
}