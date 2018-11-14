#include <iostream>
#include "data_fetch/DataFetch.h"
#include "data_fetch/PreProcess.h"
#include "data_fetch/DataCenter.h"
#include "./algorithm/Calculator.h"
#include <fstream>
#include <boost/filesystem.hpp>

class One {
public:
    One() {std::cout << "构造函数" << std::endl;};
    One(const One&) {std::cout << "拷贝构造函数" << std::endl;};
    ~One() {std::cout << "析构函数" << std::endl;};
    //One& operator=(const One&) {std::cout << "等号赋值操作" << std::endl;}
};

std::map<std::string,std::string> res_map;

void batch_process() {
    DataCenter data_center;
    std::list<std::string>& ret_list = data_center.get_all_stock_list();
    std::list<stock_info> info_list = data_center.get_stock_info("600588");

    Calculator calculator;
    std::string&& output = calculator.limit_up_ana(info_list,7);

    std::string output_file_name(R"(D:\stock_workspace\processed_info\)");
    output_file_name.append("600588\\ana_result");
    output_file_name.append("\\limit_up_ana.csv");

    std::ofstream output_file(output_file_name);
    output_file << output;
    output_file.flush();
    output_file.close();
}

int main() {
     /*DataFetch data_fetch("601398");
     std::cout << "what" << std::endl;
     std::ofstream examplefile("D:\\example1s.txt");
     data_fetch.get_weight_info(2014,12,[&examplefile](const std::string& str) -> void {
        examplefile << str;
        PreProcess pre;
        std::list<std::string> ret_result;
        pre.process_weight_info(str,ret_result);
        for(std::string str: ret_result) {
            std::cout << str <<std:: endl;
        }
    });*/

    /*DataFetch data_fetch("600312");
    std::cout << "what" << std::endl;
    std::ofstream examplefile("D:\\example123.txt");
    data_fetch.get_stock_base_info("600312",[&examplefile](const std::string& str) -> void {
        examplefile << str;
        PreProcess pre;
        std::list<std::string> ret_result;
        pre.process_weight_info(str,ret_result);
        for(std::string str: ret_result) {
            std::cout << str <<std:: endl;
        }
    });*/

    //资源文件读取，因为文件编码格式为UTF-8，输出内容为GBK2312
    //所以输出文件当中会有乱码，此处通过读取文件解决
    std::string res_file_path(R"(D:\stock_workspace\res_chinese)");
    std::ifstream in_file(res_file_path);
    std::string temp_str, key, value;
    while(!in_file.eof()) {
        in_file >> temp_str;
        unsigned long index = temp_str.find("=");
        key = temp_str.substr(0,index);
        value = temp_str.substr(index + 1, std::string::npos);
        res_map[key] = value;
    }

    DataCenter data_center;
    std::list<std::string>& ret_list = data_center.get_all_stock_list();
    std::list<stock_info> info_list = data_center.get_stock_info("600588");

    Calculator calculator;
    /*std::string&& output = calculator.win_x_percent_days_ana(info_list,10);

    std::string output_file_name(R"(D:\stock_workspace\processed_info\)");
    output_file_name.append("600312\\ana_result");
    output_file_name.append("\\percent.csv");

    std::ofstream output_file(output_file_name);
    output_file << output;
    output_file.flush();
    output_file.close();*/

    /*std::string&& output = calculator.buy_interest_ana(info_list, 7, "0000-00-00", Calculator::PriceFlag::MIN_PRICE);

    std::string output_file_name(R"(D:\stock_workspace\processed_info\)");
    output_file_name.append("600312\\ana_result");
    output_file_name.append("\\with_min_buy_7_days.csv");

    std::ofstream output_file(output_file_name);
    output_file << output;
    output_file.flush();
    output_file.close();*/

    //std::string&& output = calculator.patter_buy(info_list, 1, 7, Calculator::PriceFlag::MIN_PRICE);
    //std::string&& output = calculator.up_buy(info_list);
    /*std::string&& output = calculator.limit_up_ana(info_list,7);*/

    /*std::string output_file_name(R"(D:\stock_workspace\processed_info\)");
    output_file_name.append("600588\\ana_result");
    output_file_name.append("\\up_10per_7_buy_info.csv");

    std::ofstream output_file(output_file_name);
    output_file << output;
    output_file.flush();
    output_file.close();*/

    //斜率分析的相关内容
    std::list<Calculator::turn_point> turn_list;
    calculator.slope_ana(info_list,turn_list);

    auto turn_begin = turn_list.begin(), turn_end = turn_list.end();

    using std::cout;
    using std::endl;
    using std::to_string;
    Calculator::turn_point* temp_turn;
    std::string output;
    while(turn_begin != turn_end) {
        temp_turn = &(*turn_begin);
        output.append(temp_turn->start_point->get_date_info_str()).append(",");
        output.append(to_string(temp_turn->percent)).append(",");
        output.append(to_string(temp_turn->wave_days)).append(",");
        output.append(to_string(temp_turn->slope)).append(",");
        output.append(temp_turn->origin_up ? "Y" : "N").append(",");
        output.append("\n");
        //cout << << " " << temp_turn
        turn_begin++;
    }

    std::string file_name(R"(D:\stock_workspace\one.csv)");
    std::ofstream output_file(file_name);
    output_file << output;
    output_file.flush();
    output_file.close();

    /*std::list<stock_info> temp_list;
    auto start = temp_list.rbegin(), end = temp_list.rend();
    if(start != end) {
        std::cout << "123" << std::endl;
    }*/

    /*stock_info info1;
    info1.set_max_price(123123);

    stock_info& info1_ref = info1;

    stock_info info2;
    info2.set_max_price(66666);
    std::string stock_name("heheheheheheheheeh");
    info2.set_stock_name(stock_name);

    info1_ref = info2;

    std::cout << info1_ref.get_max_price() << std::endl;
    std::cout << info1.get_stock_name() << std::endl;

    info1.set_max_price(777777);
    std::cout << info1_ref.get_max_price() << std::endl;
    std::cout << info2.get_max_price() << std::endl;

    One one_1;
    One& one_1_ref = one_1;

    One one_2;
    one_1_ref = one_2;

    std::cout << "temp one" << std::endl;

    std::cout << &one_1 << std::endl;
    std::cout << &one_1_ref << std::endl;*/

    /*std::string str("12.345");
    float f2 = str2float(str);
    float f = std::strtof(str.data(), NULL);
    std::cout << f << std::endl;*/

    /*std::string input;
    std::ifstream in_file(R"(D:\stock_workspace\origin_info\weight_factor\600312\_2006_2)");
    char temp_char[4096];
    while(!in_file.eof()) {
        in_file.getline(temp_char, 4096);
        input.append(temp_char).append("\n");
    }
    PreProcess process;
    std::list<std::string> res_list;
    process.process_weight_info(input,res_list);

    auto begin_ite = res_list.begin(), end_ite = res_list.end();
    while(begin_ite != end_ite) {
        std::cout << (*begin_ite) << std::endl;
        begin_ite++;
    }*/

    /*std::string input, temp_str;
    std::ifstream in_file(R"(D:\stock_workspace\origin_info\weight_factor\600312\_2006_2)");
    while(!in_file.eof()) {
        std::getline(in_file,temp_str);
        input.append(temp_str).append("\n");
    }
    std::cout << input << std::endl;*/

    /*namespace fs = boost::filesystem;
    fs::path path(R"(D:\stock_workspace\processed_info\600312\)");
    if(!fs::exists(path)) {
        return - 1;
    }

    std::list<std::string> all_file_names;
    if(fs::is_directory(path)) {
        fs::directory_iterator end_file_ite;
        fs::directory_iterator start_file_ite(path);
        while(start_file_ite != end_file_ite) {
            if(fs::is_regular_file(start_file_ite->status())) {
                all_file_names.push_back(start_file_ite->path().filename().string());
            }
            start_file_ite++;
        }
    }

    auto begin_fname_ite = all_file_names.begin(), end_fname_ite = all_file_names.end();
    while(begin_fname_ite != end_fname_ite) {
        std::cout << (*begin_fname_ite) << std::endl;
        begin_fname_ite++;

    }*/

    /*stock_info info;
    info.set_af_end_price(1234);

    stock_info& info_ref = info;

    stock_info* info_p = &info_ref;

    info_ref.set_af_end_price(5555);

    std::cout << info_p->get_af_end_price() << std::endl;*/
}