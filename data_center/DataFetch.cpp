//
// Created by wangqchf on 2018/10/19.
//

#include "DataFetch.h"

#include <iostream>
#include <boost/container/vector.hpp>
#include <boost/asio/basic_streambuf.hpp>
#include <cstdio>

using boost::asio::ip::tcp;

const std::string DataFetch::FETCH_URL = "quotes.money.163.com";
const std::string DataFetch::FETCH_PARAM = "/service/chddata.html?code=";
const std::string DataFetch::WEIGHT_URL = "vip.stock.finance.sina.com.cn";
const std::string DataFetch::WEIGHT_PARAM = "/corp/go.php/vMS_FuQuanMarketHistory/stockid/";
const std::string DataFetch::STOCK_LIST_URL = "quote.eastmoney.com";
const std::string DataFetch::STOCK_LIST_PARAM = "/stocklist.html";
const std::string DataFetch::STOCK_INDEX_URL = "hq.sinajs.cn";

void DataFetch::init_socket() {
    socket.close();
    tcp::resolver resolver(context);
    tcp::resolver::query q(curr_host, "80");
    tcp::resolver::iterator endpoint_iterator,end_it;
    try {
        endpoint_iterator = resolver.resolve(q);
    }catch (std::exception& e) {
        std::cout << e.what() << std::endl;
    }
    boost::system::error_code err_code = boost::asio::error::host_not_found;
    boost::asio::ip::tcp::endpoint end_point;
    while (err_code && endpoint_iterator != end_it) {
        end_point = *endpoint_iterator;
        socket.close();
        socket.connect(end_point, err_code);
        std::cout << end_point.address().to_string() << ":" << end_point.port() << std::endl;
        endpoint_iterator++;
    }
}

DataFetch::DataFetch(std::string stock_code) : curr_host(FETCH_URL), stock_code(stock_code),context(),socket(context),header() {
    init_socket();
}

size_t DataFetch::process_http_header(std::string& header_chunk) {
    header.clear();
    size_t start_posi = 0, curr_posi = 0, divi_posi = 0;
    start_posi = header_chunk.find("\r\n",start_posi);
    start_posi += 2;
    while((curr_posi = header_chunk.find("\r\n", start_posi)) != start_posi){
        curr_posi += 2;
        std::string item = header_chunk.substr(start_posi, curr_posi - start_posi);
        divi_posi = item.find(": ");
        std::string key = item.substr(0,divi_posi);
        divi_posi += 2;
        std::string value = item.substr(divi_posi, item.length() - divi_posi - 2);
        header.put(key,value);
        start_posi = curr_posi;
    }

    return curr_posi + 2;
}

size_t DataFetch::strtosize(const std::string& str,int base) {
    size_t ret_size = 0;
    for(char cha : str) {
        ret_size *= base;
        if(cha >= 97 && cha <= 102) {
            ret_size += 10 + cha - 97;
        }
        else if(cha >= 48 && cha <= 57) {
            ret_size += cha - 48;
        }
    }
    return ret_size;
}

/**
 * 有可能一个data当中包含了多个chunk
 */
std::string DataFetch::process_data_chunk(const std::string& data) {
    std::string ret_str;
    if(data.length() <= 0) {
        return ret_str;
    }
    if(data.length() <= chunk_size) {
        chunk_size -= data.length();
    }
    else {
        size_t chunk_l_index = 0, chunk_l_end_ind = 0;
        if(chunk_size > 0) {
            chunk_l_index = data.find("\r\n", chunk_size) + 2;//上一段内报文由HTTP协议添加的\r\n加上
        }
        chunk_l_end_ind = data.find("\r\n", chunk_l_index);
        chunk_size = strtosize(data.substr(chunk_l_index,chunk_l_end_ind - chunk_l_index), 16);
        chunk_size += 2;    //将每个分块后面的\r\n两个字符数添加到上一个chunk的size当中
        ret_str.append(data.substr(0,chunk_l_index));
        ret_str.append(data.substr(chunk_l_end_ind + 2,data.length() - chunk_l_end_ind - 2));
    }

    return ret_str;
}

void DataFetch::get_http_data(std::function<void (std::string&)> const& callback) {
    std::string ret_result;
    boost::asio::streambuf response;
    std::ostringstream packetStream;
    boost::system::error_code err_code;
    boost::container::vector<char> vector;

    boost::asio::streambuf request;
    std::ostream request_stream(&request);
    request_stream << "GET /" << FETCH_PARAM << "0" << stock_code << " HTTP/1.1\r\n";
    request_stream << "Host: " << FETCH_URL << "\r\n";
    request_stream << "Accept: */*\r\n";
    request_stream << "Cache-Control: no-cache\r\n";
    request_stream << "Connection: close\r\n";
    request_stream << "\r\n";
    boost::asio::write(socket, request);

    /*size_t rec_size = socket.receive(response);
    response.commit(rec_size);
    while(rec_size) {
        rec_size = socket.receive(response);
        response.commit(rec_size);
    }

    auto bufs = response.data();
    auto i = bufs.begin();
    size_t final_size = 0;
    while (i != bufs.end())
    {
        boost::asio::const_buffer buf(*i++);
        final_size += buf.size();
    }

    char* final_data = new char[final_size];
    char* curr_point = final_data;
    i = bufs.begin();
    size_t temp_size = 0;
    while (i != bufs.end())
    {
        boost::asio::const_buffer buf(*i++);
        temp_size = buf.size();
        const void* temp_data = buf.data();
        memcpy(static_cast<void*>(curr_point),temp_data,temp_size);
        curr_point += temp_size;
    }*/
    std::vector<unsigned char> data;
    while (boost::asio::read(socket, response, boost::asio::transfer_at_least(1), err_code));
    auto data_ite = response.data();
    auto ite = data_ite.begin();
    bool is_head_end = false;
    while(ite != data_ite.end()) {
        size_t size = ite->size();
        std::string chunk(static_cast<const char*>(ite->data()));
        for(int i = 0;i < size;i++) {
            if(is_head_end) {
                ret_result.append(process_data_chunk(chunk));
            }
            else {
                size_t data_start_index = process_http_header(chunk);
                ret_result.append(process_data_chunk(chunk.substr(data_start_index)));
                is_head_end = true;
            }
        }
        ite++;
    }

//    回调，炫技用，lambda表达式
//    此外，外部程序负责相关内容的清理
    callback(ret_result);
}

void DataFetch::get_http_by_beast(const std::string& host, const std::string& target, std::function<void (const std::string&)> const& callback) {
    namespace http = boost::beast::http;
    //HTTP虽然可实现长连接，但是此处先使用短连接
    curr_host = host;
    init_socket();
    http::request<http::string_body> req{http::verb::get, target, 11};
    req.set(http::field::host, curr_host);
    req.set(http::field::user_agent, BOOST_BEAST_VERSION_STRING);

    // Send the HTTP request to the remote host
    http::write(socket, req);

    // This buffer is used for reading and must be persisted
    boost::beast::flat_buffer buffer;

    // Declare a container to hold the response
    http::response<http::dynamic_body> res;

    // Receive the HTTP response
    http::read(socket, buffer, res);

    //<std::allocator<char>>
    std::string ret_result;
    size_t ret_data_size = 0;
    boost::beast::multi_buffer::const_buffers_type data = res.body().data();
    boost::beast::multi_buffer::const_buffers_type::const_iterator ite = data.begin();
    while(ite != data.end()) {
        auto mu_buffer = *ite;
        ret_data_size += mu_buffer.size();
        std::string data_str(static_cast<const char*>(mu_buffer.data()),mu_buffer.size());
        ret_result.append(data_str);
        ite++;
    }

    callback(ret_result);

    /*char* ret_data = new char[ret_data_size];
    char* pointer = ret_data;
    ite = data.begin();
    while(ite != data.end()) {
        boost::asio::mutable_buffer mu_buffer = *ite;
        void* temp_data = mu_buffer.data();
        size_t temp_size = mu_buffer.size();
        memcpy(pointer, temp_data, temp_size);
        pointer += temp_size;
        ite++;
    }


    //下面的只能够返回结尾部分的数据，只是用来中间操作过程的缓冲
    //不明白为什么不在上述的read当中构建一个buffer
    *//*const char* real_data = static_cast<const char*>(buffer.data().data());
    std::string data_str(real_data);*//*
    callback(ret_data, ret_data_size);

    delete[] ret_data;*/
}

void DataFetch::get_weight_info(std::string _stock_code, int year, int region, std::function<void (const std::string&)> const& callback) {
    std::string target(WEIGHT_PARAM);
    //target.append(stock_code).append(".phtml?year=2014&jidu=4");
    target.append(_stock_code).append(".phtml?year=").append(std::to_string(year)).append("&jidu=").append(std::to_string(region));
    get_http_by_beast(WEIGHT_URL,target,callback);
}

void DataFetch::get_stock_base_info(std::string _stock_code,std::function<void (const std::string&)> const& callback) {
    std::string target(FETCH_PARAM);
    target.append("0").append(_stock_code);
    get_http_by_beast(FETCH_URL,target,callback);
}

void DataFetch::get_all_stock_list(std::function<void (const std::string&)> const& callback) {
    std::string target(STOCK_LIST_PARAM);
    get_http_by_beast(STOCK_LIST_URL, target, callback);
}