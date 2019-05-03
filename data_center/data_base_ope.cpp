#include "data_base_ope.h"
#include <iostream>

data_base_ope::data_base_ope(std::string ip, std::string port, 
	std::string user_name, std::string password): con_pool(), ip(ip), port(port), 
	user_name(user_name), password(password), defaultdb("stock"){

	MYSQL* temp_con = NULL;
	temp_con = mysql_init((MYSQL*)0);
	if (mysql_real_connect(temp_con, this->ip.data(), this->user_name.data(),
		this->password.data(), this->defaultdb.data(), 3306, NULL, 0)) {
		con_pool.push_back(temp_con);
	}
}

data_base_ope::~data_base_ope() {
	MYSQL* temp_con = nullptr;
	while (con_pool.size() > 0) {
		temp_con = con_pool.back();
		mysql_close(temp_con);
		con_pool.pop_back();
	}

}

void data_base_ope::execute_query(std::string query_sql) {
	MYSQL* con = con_pool.back();
	int rt = mysql_real_query(con, query_sql.data(), query_sql.size());
	if (rt) {
		std::cout << mysql_error(con) << std::endl;
		return;
	}
	MYSQL_RES* res = mysql_store_result(con);
	MYSQL_ROW row;
	while (row = mysql_fetch_row(res)) {
		MYSQL_FIELD* field_t = mysql_fetch_fields(res);
		for (int t = 0; t < mysql_num_fields(res); t++) {
			printf("%s ", row[t]);
		}
	}
}