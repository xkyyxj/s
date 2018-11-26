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

std::ostream& operator<<(std::ostream& output, const rst_info::buy_rst& _point) {
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

std::string rst_info::buy_rst::to_csv_head() {
	//构建输出信息，csv格式
	std::string output_str;
	//下面几个字段按照顺序分别是：
	//日期,编码,名称,除权开盘价,除权收盘价,除权最高价,除权最低价,目标日盈利百分比,中间最大盈利百分比,中间最大损失百分比
	//中间最大盈利所需天数,中间是否盈利
	output_str.append(res_map["100000CH00001"]).append(",").append(res_map["100000CH00002"]).append(",");
	output_str.append(res_map["100000CH00003"]).append(",").append(res_map["100000CH00004"]).append(",");
	output_str.append(res_map["100000CH00005"]).append(",").append(res_map["100000CH00006"]).append(",");
	output_str.append(res_map["100000CH00007"]).append(",").append(res_map["100000CH00018"]).append(",");
	output_str.append(res_map["100000CH00019"]).append(",").append(res_map["100000CH00020"]).append(",");
	output_str.append(res_map["100000CH00021"]).append(",").append(res_map["100000CH00022"]).append(",");
	output_str.append("\n");

	return output_str;
}

std::string rst_info::buy_rst::to_csv_string() const {
	std::string output_str;
	output_str.append(base_info->get_date_info_str()).append(",");
	output_str.append(base_info->get_stock_code()).append(",");
	output_str.append(base_info->get_stock_name()).append(",");
	output_str.append(std::to_string(base_info->get_begin_price())).append(",");
	output_str.append(std::to_string(base_info->get_end_price())).append(",");
	output_str.append(std::to_string(base_info->get_max_price())).append(",");
	output_str.append(std::to_string(base_info->get_min_price())).append(",");
	output_str.append(std::to_string(days_percent * 100)).append("%,");
	output_str.append(std::to_string(max_win_percent * 100)).append("%,");
	output_str.append(std::to_string(max_lost_percent * 100)).append("%,");
	output_str.append(std::to_string(to_max_win_days)).append(",");
	output_str.append(mid_has_up ? "Y" : "N").append(",");
	output_str.append("\n");

	return output_str;
}