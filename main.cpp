#include <iostream>
#include "data_fetch/DataFetch.h"
#include "data_fetch/PreProcess.h"
#include "data_fetch/DataCenter.h"
#include "./algorithm/Calculator.h"
#include <fstream>
#include <boost/filesystem.hpp>
std::map<std::string,std::string> res_map;
#ifdef WINDOWS_PLATFORM
#define DllExport __declspec( dllexport )//宏定义
#define ROOT_DIR "D:\\stock_workspace\\"
#else
#define DllExport
#define ROOT_DIR "~/stock_workspace"
#endif

#ifndef EXECUTION

DllExport std::string get_str() {
    return std::string("12213");
}

#else

int main(int argc, char* argv[]) {

	namespace rst_info = cassiopeia::stock_analyze::rst_info;
    using rst_info::operator<<;

	const std::string root_dir(ROOT_DIR);

	//读取资源文件，放置到map当中
	std::string res_file(root_dir);
	res_file.append("res_chinese");
	std::ifstream resource_file(res_file);
	while(!resource_file.eof()) {
		std::string temp_value;
		std::getline(resource_file, temp_value);
		const size_t index = temp_value.find('=');
		if(index < 0) {
			continue;
		}
		std::string key = temp_value.substr(0, index);
		const std::string value = temp_value.substr(index + 1, std::string::npos);
		res_map[key] = value;
	}
	resource_file.close();

	//测试斜率分析的函数
	DataCenter data_center;
	std::list<stock_info>& stock_list = data_center.get_stock_info("600072");

	std::string output_file_name(root_dir);
	output_file_name.append("processed_info\\600072\\ana_result\\").append("600072.csv");
	std::ofstream output_file(output_file_name);
	Calculator calculator;
	std::list<rst_info::turn_point> rst_list;
	calculator.slope_ana(stock_list, rst_list);
	auto rst_begin = rst_list.begin();
	const auto rst_end = rst_list.end();
	output_file << rst_info::turn_point::to_csv_head();
	while(rst_begin != rst_end) {
		output_file << *rst_begin;
		++rst_begin;
	}

	return 0;
}

#endif