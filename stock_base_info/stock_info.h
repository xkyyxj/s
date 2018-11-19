//
// Created by wangqchf on 2018/10/19.
//

#ifndef INC_123_STOCK_INFO_H
#define INC_123_STOCK_INFO_H

#include <string>
#include <cstdlib>
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/algorithm/string.hpp>

class stock_info {
    boost::gregorian::date date_info;
    std::string stock_code,stock_name;
    float weight_factor;    //复权因子
    float begin_price, end_price, max_price, min_price;
    float be_begin_price, be_end_price, be_max_price, be_min_price; //前复权价格
    float af_begin_price, af_end_price, af_max_price, af_min_price; //后复权价格

    float* ma = nullptr;
public:
    stock_info() {
        begin_price = end_price = max_price = end_price = 0;
        be_begin_price = be_end_price = be_max_price = be_min_price = 0;
        af_begin_price = af_end_price = af_max_price = af_min_price = 0;
        ma = nullptr;
    }

    stock_info(const stock_info& info) {
        date_info = info.date_info;
        stock_code = info.stock_code;
        stock_name = info.stock_name;

        begin_price = info.begin_price;
        end_price = info.end_price;
        max_price = info.max_price;
        min_price = info.min_price;

        be_begin_price = info.be_begin_price;
        be_end_price = info.be_end_price;
        be_max_price = info.be_max_price;
        be_min_price = info.be_min_price;

        af_begin_price = info.af_begin_price;
        af_end_price = info.af_end_price;
        af_max_price = info.af_max_price;
        af_min_price = info.af_min_price;

        if(info.ma) {
            ma = new float[60];
            memcpy(ma, info.ma, sizeof(float) * 60);
        }
    };

    ~stock_info() {
        if(ma) {
            delete[] ma;
            ma = nullptr;
        }
    }

    void set_weight_factor(float _factor) {weight_factor = _factor;}

    float get_weight_factor() {return weight_factor;}

    void set_date_info(std::string& _date) {
        this->date_info = boost::gregorian::from_string(_date);
    }

    boost::gregorian::date get_date_info() {
        return date_info;
    }

    std::string get_date_info_str() {
        //返回数据格式: yyyy-MM-dd
        return boost::gregorian::to_iso_extended_string(date_info);
    }

    void set_stock_code(std::string& code) {
        stock_code = code;
    }

    std::string get_stock_code() {
        return stock_code;
    }

    void set_stock_name(std::string& name) {
        stock_name = name;
    }

    std::string get_stock_name() {
        return stock_name;
    }

    float* get_ma() {return ma;}

    void set_ma(float* _ma) {ma = _ma;}

    float get_begin_price() { return begin_price; }

    float get_end_price() { return end_price; }

    float get_min_price() { return min_price; }

    float get_max_price() { return max_price; }

    float get_af_begin_price() { return af_begin_price; }

    float get_af_end_price() { return af_end_price; }

    float get_af_min_price() { return af_min_price; }

    float get_af_max_price() { return af_max_price; }

    /**
     * 此处接收std::string作为参数是为了方便构造
     * stock_info的成员参数列表
     */
    void set_begin_price(std::string& _price) {
        begin_price = std::strtof(_price.data(), NULL);
    }

    void set_end_price(std::string& _price) {
        end_price = std::strtof(_price.data(), NULL);
    }

    //设置的float类型精度上有出入，浮点数的固有特性
    void set_max_price(std::string& _price) {
        max_price = std::strtof(_price.data(), NULL);
    }

    void set_min_price(std::string& _price) {
        min_price = std::strtof(_price.data(), NULL);
    }

    void set_begin_price(float _price) {
        begin_price = _price;
    }

    void set_end_price(float _price) {
        end_price = _price;
    }

    void set_max_price(float _price) {
        max_price = _price;
    }

    void set_min_price(float _price) {
        min_price = _price;
    }

    void set_be_begin_price(float _price) {
        be_begin_price = _price;
    }

    void set_be_end_price(float _price) {
        be_end_price = _price;
    }

    void set_be_max_price(float _price) {
        be_max_price = _price;
    }

    void set_be_min_price(float _price) {
        be_min_price = _price;
    }

    void set_af_begin_price(float _price) {
        af_begin_price = _price;
    }

    void set_af_end_price(float _price) {
        af_end_price = _price;
    }

    void set_af_max_price(float _price) {
        af_max_price = _price;
    }

    void set_af_min_price(float _price) {
        af_min_price = _price;
    }

    /**
     * 重載判等操作符，方便用std::find查找列表
     * 只要编码和日期相符就是同一个
     */
    bool operator==(const stock_info& param) {
        bool is_equals = true;
        is_equals = is_equals && (date_info == param.date_info);
        is_equals = is_equals && (stock_code == param.stock_code);
        /*is_equals = is_equals && (min_price == param.min_price);
        is_equals = is_equals && (max_price == param.max_price);
        is_equals = is_equals && (begin_price == param.begin_price);
        is_equals = is_equals && (end_price == param.end_price);
        is_equals = is_equals && (weight_factor == param.weight_factor);*/
        return is_equals;
    }

    std::string to_string(bool is_brief = true) {
        std::string ret_value;
        ret_value.append(is_brief ? "" : "日期:").append(boost::gregorian::to_iso_extended_string(date_info)).append(",");
        ret_value.append(is_brief ? "" : "代码:").append(stock_code).append(",");
        ret_value.append(is_brief ? "" : "名称:").append(stock_name).append(",");

        //除权价格
        ret_value.append(is_brief ? "" : "开盘价:").append(std::to_string(begin_price)).append(",");
        ret_value.append(is_brief ? "" : "收盘价:").append(std::to_string(end_price)).append(",");
        ret_value.append(is_brief ? "" : "最高价:").append(std::to_string(max_price)).append(",");
        ret_value.append(is_brief ? "" : "最低价:").append(std::to_string(min_price)).append(",");

        //前复权价格
        ret_value.append(is_brief ? "" : "be开盘价:").append(std::to_string(be_begin_price)).append(",");
        ret_value.append(is_brief ? "" : "be收盘价:").append(std::to_string(be_end_price)).append(",");
        ret_value.append(is_brief ? "" : "be最高价:").append(std::to_string(be_max_price)).append(",");
        ret_value.append(is_brief ? "" : "be最低价:").append(std::to_string(be_min_price)).append(",");

        //后复权价格
        ret_value.append(is_brief ? "" : "af开盘价:").append(std::to_string(af_begin_price)).append(",");
        ret_value.append(is_brief ? "" : "af收盘价:").append(std::to_string(af_end_price)).append(",");
        ret_value.append(is_brief ? "" : "af最高价:").append(std::to_string(af_max_price)).append(",");
        ret_value.append(is_brief ? "" : "af最低价:").append(std::to_string(af_min_price)).append(",");

        ret_value.append(is_brief ? "" : "复权因子:").append(std::to_string(weight_factor)).append("\n");

        return ret_value;
    };

    /**
     * 从字符串当中重新构建出相应的stock_info，现在只支持简单方式to_string构建的字符串
     */
    void from_string(const std::string& input) {
        std::vector<std::string> v;
        boost::split(v, input, boost::is_any_of(","));
        int count = 0;
        using stock_info_fun = void (stock_info::*)(float);
        stock_info_fun fun_array[] = {&stock_info::set_begin_price, &stock_info::set_end_price,
                                      &stock_info::set_max_price, &stock_info::set_min_price,
                                      &stock_info::set_be_begin_price,&stock_info::set_be_end_price,
                                      &stock_info::set_be_max_price, &stock_info::set_be_min_price,
                                      &stock_info::set_af_begin_price,&stock_info::set_af_end_price,
                                      &stock_info::set_af_max_price, &stock_info::set_af_min_price,
                                      &stock_info::set_weight_factor};
        for(boost::iterator_range<std::string::iterator> i : v) {
            std::string::iterator s_begin = i.begin(), s_end = i.end();
            std::string temp_str(s_begin++, s_end++);
            switch(count) {
                case 0:
                    set_date_info(temp_str);
                    break;
                case 1:
                    set_stock_code(temp_str);
                    break;
                case 2:                    set_stock_name(temp_str);
                    break;
                default:
                    (this->*fun_array[count - 3])(std::strtof(temp_str.data(), NULL));
                    break;
            }
            count++;
        }
    }
};

#endif //INC_123_STOCK_INFO_H
