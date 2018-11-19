//
// Created by wangqchf on 2018/10/19.
//

#ifndef INC_123_FINDTARGETUP_H
#define INC_123_FINDTARGETUP_H

#include "../stock_base_info/stock_info.h"
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

    enum Direction {
        UP,
        DOWN,
        LEVEL
    };

    //转折点
    struct turn_point {
        //该转折点的基本信息
        stock_info* start_point = nullptr;
        //上一个转折点后的走势
        Direction origin_up = Direction::LEVEL;
        //从前一个转折点到该转折点的斜率
        float slope = 0;
        //从前一个转折点到该转折点的波动百分比
        float percent = 0;
        //该波上涨或者下降持续的时长
        unsigned int wave_days = 0;
    };

    int get_continue_days() {return continue_days;}
    int get_up_range() {return max_price - min_price;}
    void prbality_analy(std::list<stock_info> &);

    void slope_ana(std::list<stock_info>&, std::list<turn_point>&, PriceFlag = PriceFlag::END_PRICE, int = 9, int = 25);

    std::string buy_interest_ana(std::list<stock_info> &, int = 7, std::string = "0000-00-00",
                                 PriceFlag = PriceFlag::MAX_PRICE, PriceFlag = PriceFlag::END_PRICE);

    std::string patter_buy(std::list<stock_info>&, int = 2, int = 7, PriceFlag = PriceFlag::MAX_PRICE, PriceFlag = PriceFlag::END_PRICE);

    std::string limit_up_ana(std::list<stock_info>&, int = 7, PriceFlag = PriceFlag::MIN_PRICE, PriceFlag = PriceFlag::END_PRICE);

    std::string win_x_percent_days_ana(std::list<stock_info>& info_list, int percent, std::string begin_date = "0000-00-00");

    //计算移动均线，从1~60
    void calculate_ma(std::list<stock_info>&);

    //换种考虑方式，如果是连续三天上涨
    std::string up_buy(std::list<stock_info>&, std::list<turn_point>&, int = 10, int = 7, PriceFlag = PriceFlag::BEGIN_PRICE, PriceFlag = PriceFlag::END_PRICE);

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
