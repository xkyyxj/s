#include "stock_index_info.h"

stock_index_info::stock_index_info() {
	curr_price = 0;
}

void stock_index_info::set_property(std::string& val, property_type type) {
	switch (type) {
	case property_type::TS_CODE:
		this->ts_code = val;
		break;
	case property_type::STOCK_NAME:
		this->stock_name = val;
		break;
	case property_type::TIME_POINT:
		this->t_point = boost::posix_time::time_from_string(val);
		break;
	case property_type::CURR_PRICE:
		this->curr_price = std::strtof(val.data(), NULL);
		break;
	}
}