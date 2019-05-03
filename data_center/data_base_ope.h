#pragma once
#ifndef DATA_BASE_OPE_H_
#define DATA_BASE_OPE_H_

#include <string>
#include <mysql.h>
#include <vector>

class data_base_ope {
private:
	std::string ip, port, user_name, password, defaultdb;
	std::vector<MYSQL*> con_pool;
public:
	explicit data_base_ope(std::string, std::string, std::string, std::string);
	~data_base_ope();

	// ≤È—Ø∑Ω∑®
	void execute_query(std::string);
};

#endif