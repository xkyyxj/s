//
// Created by wangqchf on 2018/11/22.
//

#ifndef INC_123_RESULT_INFO_H
#define INC_123_RESULT_INFO_H

#include "../stock_base_info/stock_info.h"
#include <string>
#include <iostream>

namespace cassiopeia {
	namespace stock_analyze{
		namespace rst_info {
			enum Direction {
				UP,
				DOWN,
				LEVEL,
				SLOW_UP,
				SLOW_DOWN
			};

			//转折点
			class turn_point {
			public:
				//该转折点的基本信息
				stock_info* start_point = nullptr;
				//上一个转折点后的走势
				Direction next_direction = Direction::LEVEL;
				//从前一个转折点到该转折点的斜率
				float slope = 0;
				//从前一个转折点到该转折点的波动百分比
				float percent = 0;
				//该波上涨或者下降持续的时长
				unsigned int wave_days = 0;

				//将单条记录转换成scv格式记录
				std::string to_csv_string() const;

				//输出scv文件头
				static std::string to_csv_head();
			};

			std::ostream& operator<<(std::ostream& output, const rst_info::turn_point& _point);
		}
	}
}

#endif //INC_123_RESULT_INFO_H
