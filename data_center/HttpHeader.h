//
// Created by wangqchf on 2018/10/20.
//

#ifndef HTTPHEADER_H
#define HTTPHEADER_H

#include <iostream>
#include <map>

class HttpHeader {
    std::map<std::string,std::string> header_items;
public:
    HttpHeader(): header_items() {};
    void clear() {header_items.clear();};
    void put(std::string key, std::string value) {header_items[key] = value;};
};


#endif //HTTPHEADER_H
