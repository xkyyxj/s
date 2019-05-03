//
// Created by wangqchf on 2018/10/29.
//

#include "DataCenter.h"

DataCenter::~DataCenter() {
    using namespace std;
    auto begin = data_center.begin(), end = data_center.end();
    for(;begin != end;begin++) {
        begin->second.clear();
    }
    data_center.clear();
}

/**
 * 返回一个方法的时间戳
 *
 * @param file_path 文件路径
 * @return boost::gregorian::date类型的文件时间戳
 */
boost::gregorian::date DataCenter::get_file_stamp(std::string& file_path) {
    namespace fs = boost::filesystem;
    fs::path path(file_path);
    if(fs::exists(path)) {
        std::time_t last_time = fs::last_write_time(path);
        boost::posix_time::ptime posix_time = boost::posix_time::from_time_t(last_time);
        return posix_time.date();
    }
    else {
        return boost::gregorian::date(boost::gregorian::not_a_date_time);
    }
}

/**
 * 判定一个文件是否在调用这个方法的那一天更新过
 * 如果更新过返回true，否则返回false
 *
 * @param file_name 文件路径
 * @return 调用方法当天更新过，返回true，否则返回false
 */
bool DataCenter::is_curr_day_update(std::string& file_name) {
    using namespace boost::gregorian;
    namespace fs = boost::filesystem;
    fs::path path(file_name);
    date file_time = get_file_stamp(file_name);
    if(file_time.is_not_a_date()) {
        return false;
    }
    date now = day_clock::local_day();
    file_time.year_month_day();
    return file_time.year() == now.year() && file_time.month() == now.month() && file_time.day() == now.day();
}

/**
 * 从文件当中读取内容到一个字符串当中
 *
 * @param full_path 文件的全路径名
 * @return std::string，文件内容
 */
std::string DataCenter::read_str_from_f(std::string& full_path) {
    std::string ret_value, temp_str;
    std::ifstream in_file(full_path);
    while(!in_file.eof()) {
        std::getline(in_file,temp_str);
        ret_value.append(temp_str).append("\n");
    }

    return ret_value;
}

/**
 * 按照文件名字的字母序，从@param dir_path当中取出排序最高前的文件的文件名
 *
 * @param dir_path 搜索文件的路径，全路径名
 * @param last_file_name 返回的文件名，通过引用写入到这个变量当中
 * @return 如果找到了排名最靠前的文件，返回true，否则返回false
 */
bool DataCenter::find_last_fname(const std::string& dir_path, std::string& last_file_name) {
    namespace fs = boost::filesystem;
    std::string _last_file_name("0000-00-00");
    fs::path path(dir_path);
    bool ret_result = false;
    if(fs::exists(path)) {
        std::list<std::string> all_file_names;
        if(fs::is_directory(path)) {
            fs::directory_iterator end_file_ite;
            fs::directory_iterator start_file_ite(path);
            while(start_file_ite != end_file_ite) {
                if(fs::is_regular_file(start_file_ite->status())) {
                    //如果下面一行是引用的话总是会出问题，不知道是不是因为curr_file_name被清空了？
                    const std::string curr_file_name = start_file_ite->path().filename().string();
                    if(curr_file_name.compare(_last_file_name) > 0) {
                        _last_file_name = curr_file_name;
                        ret_result = true;
                    }
                }
                start_file_ite++;
            }
        }
    }

    last_file_name = _last_file_name;

    return ret_result;
}

/**
 * 根据编码获取相应基本信息，同时将复权因子赋值上去
 * 对于已经获取到的网页，会在本地进行一份缓存，以后如果是重新计算的时候可以通过缓存获取
 * 同时对于处理过的内容，也会写入到磁盘上，如果@param update_last为false，那么从磁盘读处理后结果
 * 如果磁盘上没有结果或者@param update_last为true，那么重新获取信息
 * 复权因子网页尽可能从磁盘上取
 *
 * @param stock_code 编码
 * @param update_last 是否更新到最新
 */
std::list<stock_info>& DataCenter::get_stock_info(std::string stock_code, bool update_last) {
    std::list<stock_info>& info_list = data_center[stock_code];
    if(info_list.empty()) {
        if(!update_last) {
            std::string file_name(R"(D:\stock_workspace\processed_info\)");
            file_name.append(stock_code).append("\\").append(stock_code);
            update_last = !is_curr_day_update(file_name);
            if(!update_last) {
                std::string&& input = read_str_from_f(file_name);
                pre_processor.process_existed_info(input, info_list);
            }
        }
        if(update_last) {
            //重新获取一下股票基本信息
            data_fetch.get_stock_base_info(stock_code, [this,&info_list, &stock_code](const std::string& stock_info) -> void {
                this->pre_processor.pre_process_base_info(stock_info, info_list);

                boost::gregorian::date date = boost::gregorian::day_clock::local_day();
                std::string date_str = boost::gregorian::to_iso_extended_string(date);
                std::string file_name(R"(D:\stock_workspace\origin_info\base_info\)");
                file_name.append(stock_code).append("\\");
                boost::filesystem::create_directory(file_name);
                file_name.append(stock_code);
                std::ofstream output_stream(file_name);
                output_stream << stock_info;
            });
        }
    }

    //加一个特殊判定，上证成指不需要有复权信息
    if(stock_code != "000001") {
        auto l_begin = info_list.begin();
        auto l_end = info_list.rbegin();
        int begin_year = (*l_begin).get_date_info().year();
        int begin_month = (*l_begin).get_date_info().month();
        int begin_region = begin_month % 3 > 0 ? begin_month / 3 + 1 : begin_month / 3;

        int end_year = (*l_end).get_date_info().year();
        int end_month = (*l_end).get_date_info().month();
        int end_region = end_month % 3 > 0 ? end_month / 3 + 1 : end_month / 3;

        //获取一下股票的复权因子信息
        std::list<std::string> weight_info;
        //为了减少不必要的流量浪费，如果该季度的文件已经存在，那么不需要重新下载
        std::string stock_weight_info_path(R"(D:\stock_workspace\origin_info\weight_factor\)");
        stock_weight_info_path.append(stock_code).append("\\");
        std::string last_file_name;
        find_last_fname(stock_weight_info_path, last_file_name);

        namespace fs = boost::filesystem;
        std::string temp_weight_fname;
        for(;begin_year > end_year || (begin_year == end_year && begin_region >= end_region);) {
            temp_weight_fname = "_";
            temp_weight_fname.append(std::to_string(begin_year)).append("_").append(std::to_string(begin_region));

            /**
             * 做个推定：系统获取的复权因子文件都是按照如下格式命名文件的：_${四位年份}_${一位季度}
             * 所以如果需要获取的文件的文件名大于等于现存的文件名，那么需要获取，否则就用现存的
             * 前提是没有修改磁盘当中的文件
             */
            if(temp_weight_fname.compare(last_file_name) >= 0) {
                data_fetch.get_weight_info(stock_code, begin_year,begin_region,[this,&weight_info,&stock_code,begin_year,begin_region](const std::string& input) -> void {
                    this->pre_processor.process_weight_info(input,weight_info);
                    std::string file_name(R"(D:\stock_workspace\origin_info\weight_factor\)");
                    file_name.append(stock_code).append("\\");
                    fs::create_directory(fs::path(file_name));
                    file_name.append("_").append(std::to_string(begin_year)).append("_").append(std::to_string(begin_region));
                    std::ofstream output_file(file_name);
                    output_file << input;
                });
            }
            else {
                std::string curr_weight_fname(stock_weight_info_path);
                curr_weight_fname.append(temp_weight_fname);
                std::string&& ret_value = read_str_from_f(curr_weight_fname);
                pre_processor.process_weight_info(ret_value,weight_info);
            }


            if(begin_region == 1) {
                begin_year--;
                begin_region = 4;
            }
            else {
                begin_region--;
            }
        }

        //将股票的复权因子信息融入到股票的基本信息结构stock_info当中
        float max_weight_factor = pre_processor.set_stoinfo_weifactor(info_list, weight_info);

        //处理一下复权前后的价格信息，写入到stock_info当中
        pre_processor.pro_weight(info_list, max_weight_factor);

        //将数据缓存到一个map当中
        data_center.insert(std::pair<std::string,std::list<stock_info>>(stock_code,info_list));
    }

    //增加一个步骤，顺便把处理后的信息写入到文件当中
    std::string processed_file_name(R"(D:\stock_workspace\processed_info\)");
    processed_file_name.append(stock_code).append("\\").append(stock_code);
    std::ofstream stock_final_file(processed_file_name);

    auto stock_begin_ite = info_list.begin(), stock_end_ite = info_list.end();
    while(stock_begin_ite != stock_end_ite) {
        stock_final_file << (*stock_begin_ite).to_string();
        stock_begin_ite++;
    }
    stock_final_file.flush();

    return info_list;
}

/**
 * 返回所有的沪深两市的股票的代码，并且将股票代码写入到文件当中
 * 后面再读取的话，可以从文件当中读取，如果是写入的文件是当天修改过的话
 */
std::list<std::string>& DataCenter::get_all_stock_list(bool is_update) {
    std::string result;
    std::list<std::string>& all_stock_list = all_stock_code;
    if(!is_update) {
        std::string file_name(R"(D:\stock_workspace\processed_info\all_stock_code)");
        is_update = !is_curr_day_update(file_name);
        if(!is_update) {
            std::string&& input = read_str_from_f(file_name);
            size_t begin_ind = 0;
            size_t target_ind = input.find('(', begin_ind);
            while(target_ind < input.size() - 1) {
                all_stock_list.push_back(input.substr(++target_ind, 6));
                target_ind = input.find('(', begin_ind);
            }
        }
    }
    data_fetch.get_all_stock_list([this, &all_stock_list](const std::string& info) -> void {
        std::string all_stock_list_file(R"(D:\stock_workspace\origin_info\)");
        all_stock_list_file.append("all_stock_list");
        std::ofstream output(all_stock_list_file);
        output << info;
        output.flush();

        //处理一下获取的网页，从中得到所有的股票代码
        this->pre_processor.process_all_stock_list(info, all_stock_list);
    });

    //将处理完的所有的股票信息写入到文件当中
    auto rst_begin = all_stock_list.begin(), rst_end = all_stock_list.end();
    std::ofstream all_stock_out(R"(D:\stock_workspace\processed_info\all_stock_code)");
    while(rst_begin != rst_end) {
        all_stock_out << (*rst_begin) << "\n";
        rst_begin++;
    }

    return all_stock_list;
}