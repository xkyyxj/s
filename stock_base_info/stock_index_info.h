#ifndef STOCK_INDEX_INFO_H_
#define STOCK_INDEX_INFO_H_

#include <string>
#include <boost/date_time/posix_time/posix_time.hpp>

class stock_index_info {
	std::string ts_code, stock_name;
	boost::posix_time::ptime t_point;
	float* all_price;
	float curr_price;
public:
	enum property_type {
		TS_CODE, STOCK_NAME, TIME_POINT, CURR_PRICE
	};

	explicit stock_index_info();
	void set_property(std::string&, property_type);
};

#endif