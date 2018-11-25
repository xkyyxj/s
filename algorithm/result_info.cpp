//
// Created by wangqchf on 2018/11/23.
//

#include "result_info.h"

namespace rst_info = cassiopeia::stock_analyze::rst_info;

//TODO -- 此处资源文件的key-value的Map是在其他cpp当中初始化，可以考虑以后放到这个地方做初始化
extern std::map<std::string, std::string> res_map;

std::ostream& rst_info::operator<<(std::ostream& output, const rst_info::turn_point& _point) {
    output << _point.to_csv_string();
    return output;
}

std::string rst_info::turn_point::to_csv_head() {
	std::string ret_value;
	ret_value.append(res_map["100000CH00001"]/*日期*/).append(",");
	ret_value.append(res_map["100000CH00002"]/*编码*/).append(",");
	ret_value.append(res_map["100000CH00003"]/*名称*/).append(",");
	ret_value.append(res_map["100000CH00004"]/*除权开盘价*/).append(",");
	ret_value.append(res_map["100000CH00005"]/*除权收盘价*/).append(",");
	ret_value.append(res_map["100000CH00006"]/*除权最高价*/).append(",");
	ret_value.append(res_map["100000CH00007"]/*除权最低价*/).append(",");
	ret_value.append(res_map["100000CH00034"]/*持续时长*/).append(",");
	ret_value.append(res_map["100000CH00035"]/*波动幅度*/).append(",");
	ret_value.append(res_map["100000CH00036"]/*斜率*/).append(",");
	ret_value.append(res_map["100000CH00037"]/*波动方向*/).append(",");
	ret_value.append("\n");
	return ret_value;
}

std::string rst_info::turn_point::to_csv_string() const {
    std::string ret_value;
    ret_value.append(start_point->get_date_info_str()).append(",");
    ret_value.append(start_point->get_stock_code()).append(",");
    ret_value.append(start_point->get_stock_name()).append(",");
    ret_value.append(std::to_string(start_point->get_begin_price())).append(",");
    ret_value.append(std::to_string(start_point->get_end_price())).append(",");
    ret_value.append(std::to_string(start_point->get_max_price())).append(",");
    ret_value.append(std::to_string(start_point->get_min_price())).append(",");
	ret_value.append(std::to_string(wave_days)).append(",");
	ret_value.append(std::to_string(percent * 100)).append("%,");
	ret_value.append(std::to_string(slope)).append(",");
	switch(next_direction) {
	case Direction::UP:
		ret_value.append("up");
		break;
	case Direction::SLOW_UP:
		ret_value.append("slow_up");
		break;
	case Direction::LEVEL:
		ret_value.append("level");
		break;
	case Direction::SLOW_DOWN:
		ret_value.append("slow_down");
		break;
	case Direction::DOWN:
		ret_value.append("down");
		break;
	}
	ret_value.append(",");
    ret_value.append("\n");

    return ret_value;
}