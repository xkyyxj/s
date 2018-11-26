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

			class  buy_rst {
			public:
				stock_info* base_info = nullptr;
				//base_info当天最高价买入，然后@param days那一天以收盘价卖出收益率
				float days_percent = 0;
				//base_info当天最高价买入，@param days天之间最高的收益率如何（收盘价计算收益率）
				float max_win_percent = 0;
				//base_info当天最高价买入，@param days天之间最高的损失率如何（收盘价计算收益率）
				float max_lost_percent = 0;
				//实现@param days天当中最大收益率，是在买入多少天后实现的？（非交易日不算入天数当中）
				int to_max_win_days = 0;
				//辅助上面的非交易日不算入天数当中而添加的变量
				int lower_max_win_days = 0;
				//实现@param days天过程当汇总最大收益率的那天的基本信息
				stock_info* mid_max_win_info = nullptr;
				//在历经@param days这些天当中，如能获利，则为true
				bool mid_has_up = false;
				//计量在base_info天买进的话，已经往后计算了多少天的收益
				int cal_days_count = 0;

				//将单条记录转换成scv格式记录
				std::string to_csv_string() const;

				//输出scv文件头
				static std::string to_csv_head();

			};

			std::ostream& operator<<(std::ostream& output, const turn_point& _point);

			std::ostream& operator<<(std::ostream& output, const buy_rst& _point);
		}
	}
}

#endif //INC_123_RESULT_INFO_H
