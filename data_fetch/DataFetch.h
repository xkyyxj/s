//
// Created by wangqchf on 2018/10/19.
//

#ifndef INC_123_DATAFETCH_H
#define INC_123_DATAFETCH_H

#include <string>
#include <boost/asio.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>
#include "HttpHeader.h"

using boost::asio::ip::tcp;

class DataFetch {
    static const std::string FETCH_URL;
    static const std::string FETCH_PARAM;
    static const std::string WEIGHT_URL;
    static const std::string WEIGHT_PARAM;
    static const std::string STOCK_LIST_URL;
    static const std::string STOCK_LIST_PARAM;
    std::string curr_host;
    std::string stock_code;
    boost::asio::io_context context;
    tcp::socket socket;
    HttpHeader header;
    size_t chunk_size = 0;

    void init_socket();
    size_t process_http_header(std::string&);
    std::string process_data_chunk(const std::string& data);
    size_t strtosize(const std::string&, int);
public:
    DataFetch(std::string stock_code);

    //http相关处理函数
    void get_http_data(std::function<void (std::string&)> const& f);

    //用boost的beast库请求HTTP
    void get_http_by_beast(const std::string&,const std::string&,std::function<void (const std::string&)> const&);

    //爬虫获取复权信息
    void get_weight_info(std::string, int year, int region, std::function<void (const std::string&)> const&);

    //通过新浪接口获取股票的历史交易信息
    void get_stock_base_info(std::string, std::function<void (const std::string&)> const&);

    void get_all_stock_list(std::function<void (const std::string&)> const& callback);
};


#endif //INC_123_DATAFETCH_H
