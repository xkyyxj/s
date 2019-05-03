#include "time_index_fetch.h"

const std::string time_index_fetch::URL = "hq.sinajs.cn";
const std::string time_index_fetch::TRAGET_PRE = "/list=";

std::string time_index_fetch::get_http_by_beast() {
	namespace http = boost::beast::http;
	//HTTP虽然可实现长连接，但是此处先使用短连接
	std::string target(TRAGET_PRE);
	for (int i = 0; i < stock_list.size(); i++) {
		if (i == 0) {
			target.append(stock_list[i]);
		}
		else {
			target.append(",").append(stock_list[i]);
		}
	}
	http::request<http::string_body> req{ http::verb::get, target, 11 };
	req.set(http::field::host, URL);
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
	while (ite != data.end()) {
		auto mu_buffer = *ite;
		ret_data_size += mu_buffer.size();
		std::string data_str(static_cast<const char*>(mu_buffer.data()), mu_buffer.size());
		ret_result.append(data_str);
		ite++;
	}

	return ret_result;
}

time_index_fetch::time_index_fetch(): context(), socket(context), stock_list(){
	tcp::resolver resolver(context);
	tcp::resolver::query q(URL, "80");
	tcp::resolver::iterator endpoint_iterator, end_it;
	try {
		endpoint_iterator = resolver.resolve(q);
	}
	catch (std::exception & e) {
		std::cout << e.what() << std::endl;
	}
	boost::system::error_code err_code = boost::asio::error::host_not_found;
	boost::asio::ip::tcp::endpoint end_point;
	while (err_code && endpoint_iterator != end_it) {
		end_point = *endpoint_iterator;
		socket.close();
		socket.connect(end_point, err_code);
		std::cout << end_point.address().to_string() << ":" << end_point.port() << std::endl;
		++endpoint_iterator;
	}

	// 将所有的股票信息记录到vector当中
	stock_list.push_back("sh601003");
	stock_list.push_back("sh601001");
}

time_index_fetch::time_index_fetch(time_index_fetch&):context(), socket(context), stock_list() {
	tcp::resolver resolver(context);
	tcp::resolver::query q(URL, "80");
	tcp::resolver::iterator endpoint_iterator, end_it;
	try {
		endpoint_iterator = resolver.resolve(q);
	}
	catch (std::exception & e) {
		std::cout << e.what() << std::endl;
	}
	boost::system::error_code err_code = boost::asio::error::host_not_found;
	boost::asio::ip::tcp::endpoint end_point;
	while (err_code && endpoint_iterator != end_it) {
		end_point = *endpoint_iterator;
		socket.close();
		socket.connect(end_point, err_code);
		std::cout << end_point.address().to_string() << ":" << end_point.port() << std::endl;
		++endpoint_iterator;
	}

	// 将所有的股票信息记录到vector当中
	stock_list.push_back("sh601003");
	stock_list.push_back("sh601001");
}

void time_index_fetch::operator()() {
	std::string ret_val = get_http_by_beast();
	std::cout << ret_val << std::endl;
}

stock_index_info time_index_fetch::parse(std::string& val) {
	stock_index_info info;
	size_t main_content_start = val.find_first_of("hq_str_");
	// 编码
	std::string _ts_code = val.substr(main_content_start + 7, 8);
	info.set_property(_ts_code, stock_index_info::TS_CODE);
	return info;
}