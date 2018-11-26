//
// Created by wangqchf on 2018/10/19.
//

#ifndef INC_123_FINDTARGETUP_H
#define INC_123_FINDTARGETUP_H

#include "../stock_base_info/stock_info.h"
#include "circle_array.h"
#include "result_info.h"
#include <map>
#include <list>
#include <string>

/**
 * 连续上涨或下跌若干天数的概率
 */
class ProbabilityDist {

};

struct MA {
    float ma2;
    float ma5;
    float ma7;
    float ma15;
};

using price_fun = float (stock_info::*)();

class Calculator {
public:
    enum PriceFlag {
        BEGIN_PRICE,
        END_PRICE,
        MAX_PRICE,
        MIN_PRICE
    };
    int get_continue_days() {return continue_days;}
    int get_up_range() {return max_price - min_price;}
    void prbality_analy(std::list<stock_info> &);

    void section_ana(std::list<stock_info>&, std::list<cassiopeia::stock_analyze::rst_info::turn_point>&,
		PriceFlag = PriceFlag::END_PRICE, int = 7, int = 8, bool = true);

    std::string buy_interest_ana(std::list<stock_info> &, int = 7, std::string = "0000-00-00",
                                 PriceFlag = PriceFlag::MAX_PRICE, PriceFlag = PriceFlag::END_PRICE);

    std::string patter_buy(std::list<stock_info>&, int = 2, int = 7, PriceFlag = PriceFlag::MAX_PRICE, PriceFlag = PriceFlag::END_PRICE);

    std::string limit_up_ana(std::list<stock_info>&, int = 7, PriceFlag = PriceFlag::MIN_PRICE, PriceFlag = PriceFlag::END_PRICE);

    std::string win_x_percent_days_ana(std::list<stock_info>& info_list, int percent, std::string begin_date = "0000-00-00");

    //计算移动均线，从1~60
    void calculate_ma(std::list<stock_info>&);

    //换种考虑方式，如果是连续三天上涨
    std::string up_buy(std::list<stock_info>&, std::list<cassiopeia::stock_analyze::rst_info::turn_point>&, int = 10, int = 7, PriceFlag = PriceFlag::BEGIN_PRICE, PriceFlag = PriceFlag::END_PRICE);

	//将上涨下跌的趋势考虑进来，结合动能突破的思路
	std::string kenetic_energy_ana(std::list<stock_info>&, std::list<cassiopeia::stock_analyze::rst_info::turn_point>&);

private:
    std::map<int,float> probalityDist;
    int min_price, max_price, cur_price;
    int continue_days;

    void calculate_max_min_r(std::list<stock_info>&);
    stock_info find_low_point(std::list<stock_info>&);
    stock_info find_high_point(std::list<stock_info>&);
    price_fun get_price_fun(PriceFlag flag);
};


#endif //INC_123_FINDTARGETUP_H
