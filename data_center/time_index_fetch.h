#ifndef TIME_INDEX_FETCH_H_
#define TIME_INDEX_FETCH_H_

#include <boost/asio.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>
#include <boost/asio.hpp>

#include <string>
#include <iostream>
#include <vector>

#include "../stock_base_info/stock_index_info.h"

using boost::asio::ip::tcp;

class time_index_fetch {
	static const std::string URL;
	static const std::string TRAGET_PRE;
private:
	boost::asio::io_context context;
	tcp::socket socket;
	std::vector<std::string> stock_list;

	stock_index_info parse(std::string&);
public:
	explicit time_index_fetch();
	time_index_fetch(time_index_fetch&);
	void operator()();

	std::string get_http_by_beast();
};

#endif