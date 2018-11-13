//
// Created by wangqchf on 2018/10/29.
//

#ifndef INC_123_DATACENTER_H
#define INC_123_DATACENTER_H

#include "../stock_base_info/stock_info.h"
#include "PreProcess.h"
#include "DataFetch.h"
#include <map>
#include <list>
#include <fstream>
#include <boost/filesystem.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

/**
 * 存储在map当中，key值为股票代码，value是list,list当中存储着改股票各天的详细信息
 * 列表当中的数据按照日期排列降序排列
 *
 * 对于一支股票某一特定天的信息的查询，用二分法查询
 *
 * TODO -- 后期可能用redis实现缓存以及缓存文件的写入
 */
class DataCenter {
    std::map<std::string,std::list<stock_info>> data_center;
    std::list<std::string> all_stock_code;
    DataFetch data_fetch;
    PreProcess pre_processor;

    std::string read_str_from_f(std::string& full_path);
    bool find_last_fname(const std::string& dir_path,std::string&);

    boost::gregorian::date get_file_stamp(std::string&);
    bool is_curr_day_update(std::string&);
public:
    DataCenter(): data_center(),data_fetch("601978"),pre_processor() {};
    ~DataCenter();

    std::list<stock_info>& get_stock_info(std::string stock_code, bool = false);
    stock_info* get_stock_info(std::string stock_code, std::string begin_date, std::string end_date);

    std::list<std::string>& get_all_stock_list(bool = false);
};


#endif //INC_123_DATACENTER_H
