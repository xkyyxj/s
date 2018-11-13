//
// Created by wangqchf on 2018/10/19.
//

#ifndef _PREPROCESS_H
#define _PREPROCESS_H

#include <string>
#include <map>
#include <boost/algorithm/string.hpp>
#include <boost/xpressive/xpressive_static.hpp>
#include "../stock_base_info/stock_info.h"
#include <list>

class PreProcess {
    std::map<std::string,stock_info> info_map;  //key为日期字符串，value为stock_info
public:
    void pre_process_base_info(const std::string&, std::list<stock_info>&);
    stock_info pre_process_item(const std::string&);
    void pro_weight(std::list<stock_info>&, float);
    void pro_weight_front(std::list<stock_info>&, float);    //前复权
    void pro_weight_back(std::list<stock_info>&);     //后复权

    float set_stoinfo_weifactor(std::list<stock_info>&, std::list<std::string>&);

    //从新浪财经网页当中取出股票的权重因子
    void process_weight_info(const std::string&, std::list<std::string>&);

    std::string process_weight_item(const std::string&);

    //从磁盘当中获取上次处理后写入到磁盘的结果
    void process_existed_info(const std::string&, std::list<stock_info>&);

    void process_all_stock_list(const std::string&, std::list<std::string>&);
};


#endif //_PREPROCESS_H
