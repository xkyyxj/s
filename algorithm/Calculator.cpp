//
// Created by wangqchf on 2018/10/19.
//

#include "Calculator.h"

//资源文件内容，暂时由main函数加载
extern std::map<std::string,std::string> res_map;

price_fun Calculator::get_price_fun(PriceFlag flag) {
    price_fun ret_fun;
    switch(flag) {
        case PriceFlag::MAX_PRICE:
            ret_fun = &stock_info::get_af_max_price;
            break;
        case PriceFlag::MIN_PRICE:
            ret_fun = &stock_info::get_af_min_price;
            break;
        case PriceFlag::BEGIN_PRICE:
            ret_fun = &stock_info::get_af_begin_price;
            break;
        case PriceFlag::END_PRICE:
            ret_fun = &stock_info::get_af_end_price;
            break;
        default:
            ret_fun = &stock_info::get_af_max_price;
            break;
    }

    return ret_fun;
}

/**
 * 寻找期间当中最低价所在的天的信息
 * 如果存在多天最低价相同，那么返回最后一天的信息
 */
stock_info Calculator::find_low_point(std::list<stock_info>& info_list) {
    auto info_begin = info_list.begin(), info_end = info_list.end();
    stock_info* ret_value = nullptr;
    float min_price = 0;
    while(info_begin != info_end) {
        if((*info_begin).get_min_price() < min_price) {
            min_price = (*info_begin).get_min_price();
            ret_value = &(*info_begin);
        }
        info_begin++;
    }

    return ret_value ? *ret_value : stock_info();
}

/**
 * 寻找期间当中最高价所在的天的信息
 * 如果存在多天最高价相同，那么返回最后一天的信息
 */
stock_info Calculator::find_high_point(std::list<stock_info> &info_list) {
    auto info_begin = info_list.begin(), info_end = info_list.end();
    stock_info* ret_value = nullptr;
    float max_price = 0;
    while(info_begin != info_end) {
        if((*info_begin).get_max_price() < max_price) {
            max_price = (*info_begin).get_max_price();
            ret_value = &(*info_begin);
        }
        info_begin++;
    }

    return ret_value ? *ret_value : stock_info();
}

/**
 * 寻找期间的最大涨幅：
 * 期间最低价到期间最高价之间的差值
 * 后续可能会添加控制选项，实现其他统计
 */
void Calculator::calculate_max_min_r(std::list<stock_info>& info_list) {
    if(info_list.empty())
        return;

    stock_info min_price_info, max_price_info;
    auto list_begin = info_list.begin(), list_end = info_list.end();
    while(list_begin != list_end) {
        if ((*list_begin).get_min_price() < min_price) {
            min_price_info = (*list_begin);
        }

        if ((*list_begin).get_max_price() > max_price) {
            max_price_info = (*list_begin);
        }
    }
}

/**
 * 分析改段时间内连续N天上涨或者下跌的概率
 */
void Calculator::prbality_analy(std::list<stock_info> &) {

}

/**
 * 这个算法目前按照后复权的价格来进行统计计算，但是打印的时候可以按照除权价格来输出
 * 1. 在任意一天的收盘价买入之后，达到@param percent的涨幅，需要多少天，或者不能实现，不能实现的完成日期字段填写“xxxxxxx”
 * 2. 汇总统计一下，按照条件1买入，平均需要多少天达到@param percent的涨幅
 * 3. 汇总一下，成功率是百分之几。
 *
 * @param info_list 所有的基本信息
 * @param percent 要求达到的百分比，正整数，如要求达到20%，那么这个参数应为20
 * @param begin_date 开始日期，从这个日期往后进行统计，此参数有默认值："0000-00-00"
 */
std::string Calculator::win_x_percent_days_ana(std::list<stock_info>& info_list, int percent, std::string begin_date) {

    //最终的结果结构体
    struct percent_info {
        stock_info* base_info;  //在base_info这一天买入
        //resolve_time_info这一天达成了@param percent的百分比，如至今为止没达到，则为nullptr
        stock_info* resolve_time_info;
        //达成目标时候的上涨百分比
        float percent;
        //达成目标花费的实际天数
        long days;

        //判等操作，std::list的remove方法需要
        bool operator==(const percent_info& param) {
            return base_info == param.base_info;
        }
    };

    /**
     * 构建方式如下：
     * 维护一个已解决列表，和未解决列表
     * 未解决列表按照价格从低到高进行排列
     * 对于@param info_list所有基本信息列表进行遍历，对于每一个元素，检查是否能够解决未解决列表当中的元素：
     * 如果能够解决，则将其解决并移入已解决列表，然后将这个元素添加到未解决列表当中
     * 如果不能解决，则将这个元素添加到未解决列表当中
     */
    float target_percent = static_cast<float>(percent) / 100;   //构建最终百分比
    auto list_begin = info_list.rbegin(), list_end = info_list.rend();
    std::list<percent_info> unresolved_info;
    std::list<percent_info> resolved_info;
    while(list_begin != list_end) {
        stock_info* curr_info = &(*list_begin);
        if(curr_info->get_date_info_str().compare(begin_date) < 0) {
            //直到真正需要分析的开始日期为止
            continue;
        }
        auto unresolved_begin = unresolved_info.begin(), unresolved_end = unresolved_info.end();
        if(unresolved_begin != unresolved_end) {
            //看下能够解决到resolved当中，同时将当前天添加到unresolved_info当中，unresolved_info按照时间升序排列
            bool resolved_finished = false;
            while(unresolved_begin != unresolved_end && !resolved_finished) {
                percent_info temp_info = (*unresolved_begin);
                float af_end_price = temp_info.base_info->get_af_end_price();
                float cur_af_end_price = curr_info->get_af_end_price();
                //看是否涨到了要求的百分比
                if(cur_af_end_price > af_end_price && ((cur_af_end_price - af_end_price) / af_end_price) >= target_percent) {
                    unresolved_begin = unresolved_info.erase(unresolved_begin); //从未解决当中移除，同时指向下一个元素
                    temp_info.percent = (cur_af_end_price - af_end_price) / af_end_price; //权重
                    //完成百分比涨幅花费的天数
                    temp_info.days = (curr_info->get_date_info() - temp_info.base_info->get_date_info()).days();
                    //记录完成这一天的信息
                    temp_info.resolve_time_info = curr_info;
                    resolved_info.push_back(temp_info); //添加到已解决当中
                }
                //遵从插入排序的方式，将当天放入到未解决列表当中
                else if(cur_af_end_price > af_end_price) {
                    unresolved_begin++;
                }
                else if(cur_af_end_price <= af_end_price) {
                    percent_info temp_per_info{};
                    temp_per_info.base_info = curr_info;
                    temp_per_info.percent = 0;
                    temp_per_info.days = 0;
                    temp_per_info.resolve_time_info = nullptr;
                    unresolved_info.insert(unresolved_begin, temp_per_info);
                    resolved_finished = true;
                }
            }

            if(!resolved_finished) {
                percent_info temp_per_info{};
                temp_per_info.base_info = curr_info;
                temp_per_info.percent = 0;
                temp_per_info.days = 0;
                temp_per_info.resolve_time_info = nullptr;
                unresolved_info.push_back(temp_per_info);
            }
        }
        else {
            percent_info temp_per_info{};
            temp_per_info.base_info = curr_info;
            temp_per_info.percent = 0;
            temp_per_info.days = 0;
            unresolved_info.push_back(temp_per_info);
        }
        list_begin++;
    }

    //统计信息
    float resolved_percent = 0;
    long total_needed_days = 0;   //总计花费了多少天达到目标百分比
    //下面两个变量指明：达到目标百分比的有多少天，未达到目标百分比的有多少天
    long finished_days = 0, unfinished_days = 0;
    unsigned long min_days = -1, max_days = 0;
    finished_days = resolved_info.size();
    unfinished_days = unresolved_info.size();
    resolved_percent = static_cast<float>(finished_days) / (finished_days + unfinished_days);

    //处理完成后，按照日期进行排序
    auto unresolved_begin = unresolved_info.begin(), unresolved_end = unresolved_info.end();
    auto resolved_begin = resolved_info.begin();
    resolved_info.insert(resolved_begin, unresolved_begin, unresolved_end);
    resolved_info.sort([](const percent_info& first, const percent_info& second) -> bool {
        return first.base_info->get_date_info_str().compare(second.base_info->get_date_info_str()) <= 0;
    });

    //排序完成后，按照排序结果构建输出字符串
    //此外忽然有了一个想法：既然是构建输出字符串比较规整，可以输出为csv文件格式，有Excel进行排序操作
    resolved_begin = resolved_info.begin();
    auto resolved_end = resolved_info.end();
    std::string output_str;
    //构建表头信息
    //下面几个字段按照顺序分别是：
    //日期,编码,名称,达成获利目标日期,除权开盘价,除权收盘价,除权最高价,除权最低价,盈利百分比,盈利所需天数,
    output_str.append(res_map["100000CH00001"]).append(",").append(res_map["100000CH00002"]).append(",");
    output_str.append(res_map["100000CH00003"]).append(",").append(res_map["100000CH00017"]).append(",");
    output_str.append(res_map["100000CH00004"]).append(",").append(res_map["100000CH00005"]).append(",");
    output_str.append(res_map["100000CH00006"]).append(",").append(res_map["100000CH00007"]).append(",");
    output_str.append(res_map["100000CH00008"]).append(",").append(res_map["100000CH00009"]).append(",");
    output_str.append("\n");
    while(resolved_begin != resolved_end) {
        percent_info per_info = (*resolved_begin);
        output_str.append(per_info.base_info->get_date_info_str()).append(",");
        output_str.append(per_info.base_info->get_stock_code()).append(",");
        output_str.append(per_info.base_info->get_stock_name()).append(",");
        output_str.append(per_info.resolve_time_info ? per_info.resolve_time_info->get_date_info_str() : "xxxxxxxx").append(",");
        output_str.append(std::to_string(per_info.base_info->get_begin_price())).append(",");
        output_str.append(std::to_string(per_info.base_info->get_end_price())).append(",");
        output_str.append(std::to_string(per_info.base_info->get_max_price())).append(",");
        output_str.append(std::to_string(per_info.base_info->get_min_price())).append(",");
        output_str.append(std::to_string(per_info.percent * 100)).append("%,");
        output_str.append(std::to_string(per_info.days)).append(",");
        output_str.append("\n");

        //顺便循环体当中重新统计一下总计解决天数、最少需要天数以及最大需要天数
        total_needed_days += per_info.days;
        min_days = per_info.days > 0 && per_info.days < min_days ? per_info.days : min_days;
        max_days = per_info.days > 0 && per_info.days > max_days ? per_info.days : max_days;
        resolved_begin++;
    }

    //将统计信息写入到csv文件当中
    output_str.append(res_map["100000CH00010"]/*盈利天数百分比*/).append(std::to_string(resolved_percent)).append("\n");
    output_str.append(res_map["100000CH00011"]/*盈利天数*/).append(std::to_string(finished_days)).append("\n");
    output_str.append(res_map["100000CH00012"]/*不能获利天数*/).append(std::to_string(unfinished_days)).append("\n");
    output_str.append(res_map["100000CH00013"]/*获利所需总天数*/).append(std::to_string(total_needed_days)).append("\n");
    output_str.append(res_map["100000CH00014"]/*获利所需平均天数*/).append(std::to_string(static_cast<float>(total_needed_days) / finished_days));
    output_str.append("\n");
    output_str.append(res_map["100000CH00015"]/*获利所需最小天数*/).append(std::to_string(min_days)).append("\n");
    output_str.append(res_map["100000CH00016"]/*获利所需最长天数*/).append(std::to_string(max_days)).append("\n");

    return output_str;
}

/**
 * 在当天以@param buy_price指定价格买入的话，那么截止@param days天位置，以@param sold_price价格卖出:
 * 1.最高收益多少
 * 2.第@param days天收益如何？
 * 3.最高收益出现在第几天？（非交易日不算在天数当中）
 * 4.最少收益多少？
 * 5.达到@param days天的时候上涨的天数有多少？下降的天数有多少？
 * 得出百分比等信息。
 *
 * @param info_list 基本信息列表
 * @param days 要求获取days天后的结果，days只计算交易日，非交易日不计算到days当中
 * @param begin_date 从begin_date天之后开始分析
 * @param buy_price 买入价格
 * @param sold_price 卖出价格
 */
std::string Calculator::buy_interest_ana(std::list<stock_info> &info_list, int days, std::string begin_date,
        PriceFlag buy_price,PriceFlag sold_price) {

    struct max_buy_rst {
        stock_info* base_info;
        //base_info当天最高价买入，然后@param days那一天以收盘价卖出收益率
        float days_percent;
        //base_info当天最高价买入，@param days天之间最高的收益率如何（收盘价计算收益率）
        float max_win_percent;
        //base_info当天最高价买入，@param days天之间最高的损失率如何（收盘价计算收益率）
        float max_lost_percent;
        //实现@param days天当中最大收益率，是在买入多少天后实现的？（非交易日不算入天数当中）
        int to_max_win_days;
        //辅助上面的非交易日不算入天数当中而添加的变量
        int lower_max_win_days;
        //实现@param days天过程当汇总最大收益率的那天的基本信息
        stock_info* mid_max_win_info;
        //在历经@param days这些天当中，如能获利，则为true
        bool mid_has_up = false;
    };

    price_fun buy_price_fun = get_price_fun(buy_price), sold_price_fun = get_price_fun(sold_price);

    //统计信息
    int total_days = 0; //总天数，可能有部分天不盈利也不损失
    //@param days天的时候，如果获利，则此数加一
    int up_days_num = 0;
    //@param days天的时候，如果损失，则此数加一
    int down_days_num = 0;
    //在历经@param days这些天当中，如能获利，则此数加一，即中间历程可上涨天数
    int mid_up_days_num = 0;
    //在历经@param days这些天当中，如果始终未获利（包含一直损失），则此数加一，
    int mid_down_days_num = 0;
    //下面三个为@param days天后那一天的数据，所有天的统计信息
    float max_win_percent = 0; //最大的获利比例
    float min_win_percent = 10000;  //最小获利比例
    float max_lose_percent = 0; //最大损失比例
    //@param days天的过程当中最大收益率的平均值
    float ave_max_win_percent = 0;
    //到达@param days天的时候，收益率的平均值是多少
    float ave_target_win_percent = 0;

    //下面三个为@param days天之间的数据，所有天的统计信息
    float mid_max_win_percent = 0; //最大的获利比例
    float mid_min_win_percent = 10000;  //最小获利比例
    float mid_max_lose_percent = 0; //最大损失比例

    auto list_begin = info_list.rbegin(), list_end = info_list.rend();
    std::list<max_buy_rst> rst_list;

    /**
     *
     */
    stock_info* temp_info = nullptr;
    //初始化rst_list，添加一个元素
    max_buy_rst init_item{};
    temp_info = &(*list_begin);
    init_item.base_info = temp_info;
    init_item.max_win_percent = -10000;
    rst_list.push_back(init_item);
    list_begin++;
    auto rst_begin = rst_list.begin(), windows_p = rst_list.begin(), rst_end = rst_list.end();
    while(list_begin != list_end) {
        temp_info = &(*list_begin);
        windows_p = rst_begin, rst_end = rst_list.end();
        /**
         * 滑动窗格，rst_list的头部指针始终落后@param days天，
         * 每次新增加一个元素到rst_list当中的时候，如果rst_list长度大于等于@param days，
         * 那么就会将rst_list的头部指针向前移动一个元素
         */
        for(int i = 0;i < days && windows_p != rst_end;i++) {
            /**
             * 计算收益比率，按照当天的收盘价来算
             * 计算经历@param days的过程当中，可能的收益信息。
             */
            max_buy_rst& buy_rst = (*windows_p);
            float buy_price_val = (buy_rst.base_info->*buy_price_fun)();
            float cur_sold_price_val = (temp_info->*sold_price_fun)();
            float curr_win_percent = (cur_sold_price_val - buy_price_val) / buy_price_val;
            buy_rst.max_win_percent = buy_rst.max_win_percent > curr_win_percent ? buy_rst.max_win_percent : curr_win_percent;
            if(buy_rst.max_win_percent > curr_win_percent) {
                buy_rst.lower_max_win_days++;
            }
            else {
                buy_rst.to_max_win_days = buy_rst.to_max_win_days + buy_rst.lower_max_win_days + 1;
                buy_rst.lower_max_win_days = 0;
            }
            buy_rst.mid_max_win_info = curr_win_percent > mid_max_win_percent ? temp_info : buy_rst.mid_max_win_info;
            buy_rst.max_lost_percent = buy_rst.max_lost_percent > curr_win_percent ?  curr_win_percent : buy_rst.max_lost_percent;

            mid_max_win_percent = curr_win_percent > mid_max_win_percent ? curr_win_percent : mid_max_win_percent;
            mid_max_lose_percent = mid_max_lose_percent < curr_win_percent ? mid_max_lose_percent : curr_win_percent;
            mid_min_win_percent = curr_win_percent > 0 && curr_win_percent < mid_min_win_percent ? curr_win_percent : mid_min_win_percent;
            buy_rst.mid_has_up = curr_win_percent > 0 || buy_rst.mid_has_up;
            if(i == days - 1) {
                /**
                 * 计算最终达到@param days天的时候，收益率如何
                 * 还要有一个C++的语法陷阱：C++当中的引用是不能被重新指向新对象的，
                 * 所以说如下的代码实际上会调用type operator=(type)，编译器提供的默认的等号操作符重载
                 * 对于自己代码没有重写等号赋值操作的来说，就会像是默认拷贝构造函数一样，对类中的每个子对象调用
                 * oeprator=的等号赋值操作。
                 * buy_rst = (*rst_begin);
                 */
                max_buy_rst& target_buy_rst = (*rst_begin);
                buy_price_val = (target_buy_rst.base_info->*buy_price_fun)();
                curr_win_percent = (cur_sold_price_val - buy_price_val) / buy_price_val;
                target_buy_rst.days_percent = curr_win_percent;
                max_win_percent = curr_win_percent > max_win_percent ? curr_win_percent : max_win_percent;
                max_lose_percent = max_lose_percent < curr_win_percent ? max_lose_percent : curr_win_percent;
                min_win_percent = curr_win_percent > 0 && curr_win_percent < min_win_percent ? curr_win_percent : min_win_percent;
                curr_win_percent > 0 ? up_days_num++ : 1;
                curr_win_percent < 0 ? down_days_num++ : 1;

                //统计最大收益率的平均值
                ave_max_win_percent += buy_rst.max_win_percent;
                //统计到达@param days天是收益率的平均值
                ave_target_win_percent += buy_rst.days_percent;
                rst_begin++;
            }
            windows_p++;
        }

        max_buy_rst insert_item{};
        insert_item.base_info = temp_info;
        insert_item.max_win_percent = -10000;
        rst_list.push_back(insert_item);

        list_begin++;
    }

    //收益率平均值方面的重新计算
    ave_max_win_percent /= rst_list.size();
    ave_target_win_percent /= rst_list.size();

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
    rst_begin = rst_list.begin(), rst_end = rst_list.end();
    while(rst_begin != rst_end) {
        max_buy_rst& buy_rst = (*rst_begin);
        //统计一下相关信息,@param days的中间历程当中是否上涨过
        buy_rst.mid_has_up ? mid_up_days_num++ : mid_down_days_num++;

        output_str.append(buy_rst.base_info->get_date_info_str()).append(",");
        output_str.append(buy_rst.base_info->get_stock_code()).append(",");
        output_str.append(buy_rst.base_info->get_stock_name()).append(",");
        output_str.append(std::to_string(buy_rst.base_info->get_begin_price())).append(",");
        output_str.append(std::to_string(buy_rst.base_info->get_end_price())).append(",");
        output_str.append(std::to_string(buy_rst.base_info->get_max_price())).append(",");
        output_str.append(std::to_string(buy_rst.base_info->get_min_price())).append(",");
        output_str.append(std::to_string(buy_rst.days_percent * 100)).append("%,");
        output_str.append(std::to_string(buy_rst.max_win_percent * 100)).append("%,");
        output_str.append(std::to_string(buy_rst.max_lost_percent * 100)).append("%,");
        output_str.append(std::to_string(buy_rst.to_max_win_days)).append(",");
        output_str.append(buy_rst.mid_has_up ? "Y" : "N").append(",");
        output_str.append("\n");
        rst_begin++;
    }

    //输出统计信息到字符串当中
    output_str.append(res_map["100000CH00023"]/*上涨天数：*/).append(std::to_string(up_days_num)).append("\n");
    output_str.append(res_map["100000CH00024"]/*下跌天数：*/).append(std::to_string(down_days_num)).append("\n");
    output_str.append(res_map["100000CH00025"]/*中间历程上涨天数：*/).append(std::to_string(mid_up_days_num)).append("\n");
    output_str.append(res_map["100000CH00026"]/*中间历程未上涨天数：*/).append(std::to_string(mid_down_days_num)).append("\n");
    output_str.append(res_map["100000CH00028"]/*目标日最大盈利百分比*/).append(std::to_string(max_win_percent)).append("\n");
    output_str.append(res_map["100000CH00027"]/*目标日最小盈利百分比*/).append(std::to_string(min_win_percent)).append("\n");
    output_str.append(res_map["100000CH00029"]/*目标日最大损失百分比：*/).append(std::to_string(max_lose_percent)).append("\n");
    output_str.append(res_map["100000CH00019"]/*中间最大盈利百分比*/).append(":").append(std::to_string(mid_max_win_percent)).append("\n");
    output_str.append(res_map["100000CH00030"]/*中间最小盈利百分比*/).append(std::to_string(mid_min_win_percent)).append("\n");
    output_str.append(res_map["100000CH00020"]/*中间最大损失百分比*/).append(":").append(std::to_string(mid_max_lose_percent)).append("\n");
    output_str.append(res_map["100000CH00031"]/*平均最大盈利百分比*/).append(std::to_string(ave_max_win_percent)).append("\n");
    output_str.append(res_map["100000CH00032"]/*平均目标日盈利百分比*/).append(std::to_string(ave_target_win_percent)).append("\n");

    return output_str;
}

/**
 * 特定的买入模式分析收益率如何,下降@param down_days天之后买入的效果如何，其余的和buy_interest_ana类似
 */
std::string Calculator::patter_buy(std::list<stock_info>& info_list, int down_days, int days, PriceFlag buy_price, PriceFlag sold_price) {
    struct buy_rst {
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
        int cal_days_count= 0;
    };

    //统计信息
    int total_days = 0; //总天数，可能有部分天不盈利也不损失
    //@param days天的时候，如果获利，则此数加一
    int up_days_num = 0;
    //@param days天的时候，如果损失，则此数加一
    int down_days_num = 0;
    //在历经@param days这些天当中，如能获利，则此数加一，即中间历程可上涨天数
    int mid_up_days_num = 0;
    //在历经@param days这些天当中，如果始终未获利（包含一直损失），则此数加一，
    int mid_down_days_num = 0;
    //下面三个为@param days天后那一天的数据，所有天的统计信息
    float max_win_percent = 0; //最大的获利比例
    float min_win_percent = 10000;  //最小获利比例
    float max_lose_percent = 0; //最大损失比例
    //@param days天的过程当中最大收益率的平均值
    float ave_max_win_percent = 0;
    //到达@param days天的时候，收益率的平均值是多少
    float ave_target_win_percent = 0;

    //下面三个为@param days天之间的数据，所有天的统计信息
    float mid_max_win_percent = 0; //最大的获利比例
    float mid_min_win_percent = 10000;  //最小获利比例
    float mid_max_lose_percent = 0; //最大损失比例

    price_fun buy_fun = get_price_fun(buy_price), sold_fun = get_price_fun(sold_price);

    auto list_begin = info_list.rbegin(), list_end = info_list.rend();
    std::list<buy_rst> rst_list;
    std::list<buy_rst>::iterator rst_begin = rst_list.begin(), rst_end;

    stock_info* temp_stock;
    int _down_days = 0;
    while(list_begin != list_end) {
        temp_stock = &(*list_begin);
        if(temp_stock->get_af_end_price() < temp_stock->get_af_begin_price()) {
            _down_days++;
        }
        else {
            _down_days = 0;
        }

        //将下面原先的==判定改为>=,就可以实现@param down_days为零的时候，任意一天买入，判定days之内的收益如何
        if(_down_days >= down_days) {
            buy_rst insert_rst{};
            insert_rst.base_info = temp_stock;
            rst_list.push_back(insert_rst);
            if(rst_list.size() == 1) {
                rst_begin = rst_list.begin();
                continue;
            }
        }

        auto windows_p = rst_begin;
        rst_end = rst_list.end();
        while(windows_p != rst_end) {
            buy_rst& rst_info = *windows_p;
            //特殊处理这种情况，避免当天是连续下降@param days的那一天，然后该天自己和自己计算收益
            if(rst_info.base_info == temp_stock) {
                //如果此种情况达成，说明已经到了rst_list的末尾
                break;
            }
            if(rst_info.cal_days_count >= days) {
                //永远不使rst_begin指向rst_list的末尾
                auto temp_ite = rst_begin;
                if(++temp_ite != rst_end) {
                    rst_begin++;
                }
                if(rst_info.cal_days_count == days) {
                    buy_rst& target_buy_rst = (*rst_begin);
                    float buy_price_val = (target_buy_rst.base_info->*buy_fun)();
                    float cur_sold_price_val = (temp_stock->*sold_fun)();
                    float curr_win_percent = (cur_sold_price_val - buy_price_val) / buy_price_val;
                    target_buy_rst.days_percent = curr_win_percent;
                    max_win_percent = curr_win_percent > max_win_percent ? curr_win_percent : max_win_percent;
                    max_lose_percent = max_lose_percent < curr_win_percent ? max_lose_percent : curr_win_percent;
                    min_win_percent = curr_win_percent > 0 && curr_win_percent < min_win_percent ? curr_win_percent : min_win_percent;
                    curr_win_percent > 0 ? up_days_num++ : 1;
                    curr_win_percent < 0 ? down_days_num++ : 1;

                    //统计最大收益率的平均值
                    ave_max_win_percent += rst_info.max_win_percent;
                    //统计到达@param days天是收益率的平均值
                    ave_target_win_percent += rst_info.days_percent;
                    rst_info.cal_days_count++;
                }
            }
            else {
                float buy_price_val = (rst_info.base_info->*buy_fun)();
                float cur_sold_price_val = (temp_stock->*sold_fun)();
                float curr_win_percent = (cur_sold_price_val - buy_price_val) / buy_price_val;
                rst_info.max_win_percent = rst_info.max_win_percent > curr_win_percent ? rst_info.max_win_percent : curr_win_percent;
                if(rst_info.max_win_percent > curr_win_percent) {
                    rst_info.lower_max_win_days++;
                }
                else {
                    rst_info.to_max_win_days = rst_info.to_max_win_days + rst_info.lower_max_win_days + 1;
                    rst_info.lower_max_win_days = 0;
                }
                rst_info.mid_max_win_info = curr_win_percent > mid_max_win_percent ? temp_stock : rst_info.mid_max_win_info;
                rst_info.max_lost_percent = rst_info.max_lost_percent > curr_win_percent ?  curr_win_percent : rst_info.max_lost_percent;

                mid_max_win_percent = curr_win_percent > mid_max_win_percent ? curr_win_percent : mid_max_win_percent;
                mid_max_lose_percent = mid_max_lose_percent < curr_win_percent ? mid_max_lose_percent : curr_win_percent;
                mid_min_win_percent = curr_win_percent > 0 && curr_win_percent < mid_min_win_percent ? curr_win_percent : mid_min_win_percent;
                rst_info.mid_has_up = curr_win_percent > 0 || rst_info.mid_has_up;
                rst_info.cal_days_count++;
            }
            windows_p++;
            
        }
        list_begin++;
    }

    //收益率平均值方面的重新计算
    ave_max_win_percent /= rst_list.size();
    ave_target_win_percent /= rst_list.size();

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
    rst_begin = rst_list.begin(), rst_end = rst_list.end();
    while(rst_begin != rst_end) {
        buy_rst& rst_info = (*rst_begin);
        //统计一下相关信息,@param days的中间历程当中是否上涨过
        rst_info.mid_has_up ? mid_up_days_num++ : mid_down_days_num++;

        output_str.append(rst_info.base_info->get_date_info_str()).append(",");
        output_str.append(rst_info.base_info->get_stock_code()).append(",");
        output_str.append(rst_info.base_info->get_stock_name()).append(",");
        output_str.append(std::to_string(rst_info.base_info->get_begin_price())).append(",");
        output_str.append(std::to_string(rst_info.base_info->get_end_price())).append(",");
        output_str.append(std::to_string(rst_info.base_info->get_max_price())).append(",");
        output_str.append(std::to_string(rst_info.base_info->get_min_price())).append(",");
        output_str.append(std::to_string(rst_info.days_percent * 100)).append("%,");
        output_str.append(std::to_string(rst_info.max_win_percent * 100)).append("%,");
        output_str.append(std::to_string(rst_info.max_lost_percent * 100)).append("%,");
        output_str.append(std::to_string(rst_info.to_max_win_days)).append(",");
        output_str.append(rst_info.mid_has_up ? "Y" : "N").append(",");
        output_str.append("\n");
        rst_begin++;
    }

    //输出统计信息到字符串当中
    output_str.append(res_map["100000CH00023"]/*上涨天数：*/).append(std::to_string(up_days_num)).append("\n");
    output_str.append(res_map["100000CH00024"]/*下跌天数：*/).append(std::to_string(down_days_num)).append("\n");
    output_str.append(res_map["100000CH00025"]/*中间历程上涨天数：*/).append(std::to_string(mid_up_days_num)).append("\n");
    output_str.append(res_map["100000CH00026"]/*中间历程未上涨天数：*/).append(std::to_string(mid_down_days_num)).append("\n");
    output_str.append(res_map["100000CH00028"]/*目标日最大盈利百分比*/).append(std::to_string(max_win_percent)).append("\n");
    output_str.append(res_map["100000CH00027"]/*目标日最小盈利百分比*/).append(std::to_string(min_win_percent)).append("\n");
    output_str.append(res_map["100000CH00029"]/*目标日最大损失百分比：*/).append(std::to_string(max_lose_percent)).append("\n");
    output_str.append(res_map["100000CH00019"]/*中间最大盈利百分比*/).append(":").append(std::to_string(mid_max_win_percent)).append("\n");
    output_str.append(res_map["100000CH00030"]/*中间最小盈利百分比*/).append(std::to_string(mid_min_win_percent)).append("\n");
    output_str.append(res_map["100000CH00020"]/*中间最大损失百分比*/).append(":").append(std::to_string(mid_max_lose_percent)).append("\n");
    output_str.append(res_map["100000CH00031"]/*平均最大盈利百分比*/).append(std::to_string(ave_max_win_percent)).append("\n");
    output_str.append(res_map["100000CH00032"]/*平均目标日盈利百分比*/).append(std::to_string(ave_target_win_percent)).append("\n");

    return output_str;
}

/**
 * 特定的买入模式分析收益率如何,上升@param up_percent之后买入的效果如何，达到up_percent的后一天买入，
 * 其余的和buy_interest_ana类似
 * 
 * 需要关注的是这个过程当中
 */
std::string Calculator::up_buy(std::list<stock_info>& info_list, std::list<turn_point>& turn_list, int up_percent, int days, PriceFlag buy_price, PriceFlag sold_price) {
    struct buy_rst {
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
        int cal_days_count= 0;
    };

    //统计信息
    int total_days = 0; //总天数，可能有部分天不盈利也不损失
    //@param days天的时候，如果获利，则此数加一
    int up_days_num = 0;
    //@param days天的时候，如果损失，则此数加一
    int down_days_num = 0;
    //在历经@param days这些天当中，如能获利，则此数加一，即中间历程可上涨天数
    int mid_up_days_num = 0;
    //在历经@param days这些天当中，如果始终未获利（包含一直损失），则此数加一，
    int mid_down_days_num = 0;
    //下面三个为@param days天后那一天的数据，所有天的统计信息
    float max_win_percent = 0; //最大的获利比例
    float min_win_percent = 10000;  //最小获利比例
    float max_lose_percent = 0; //最大损失比例
    //@param days天的过程当中最大收益率的平均值
    float ave_max_win_percent = 0;
    //到达@param days天的时候，收益率的平均值是多少
    float ave_target_win_percent = 0;

    //下面三个为@param days天之间的数据，所有天的统计信息
    float mid_max_win_percent = 0; //最大的获利比例
    float mid_min_win_percent = 10000;  //最小获利比例
    float mid_max_lose_percent = 0; //最大损失比例

    float real_up_percent = static_cast<float>(up_percent) / 100;
    price_fun buy_fun = get_price_fun(buy_price), sold_fun = get_price_fun(sold_price);

    auto list_begin = info_list.rbegin(), list_end = info_list.rend();
    std::list<buy_rst> rst_list;
    std::list<buy_rst>::iterator rst_begin = rst_list.begin(), rst_end;

    stock_info* temp_stock, *up_start_stock, *up_start_next_day, *pre_day_info;

    if(list_begin != list_end) {
        up_start_stock = &(*list_begin);
        pre_day_info = up_start_stock;
        list_begin++;
    }
    while(list_begin != list_end) {
        temp_stock = &(*list_begin);
        if(temp_stock->get_af_end_price() > pre_day_info->get_af_end_price()) {
            if((temp_stock->get_af_end_price() - up_start_stock->get_af_end_price()) / up_start_stock->get_af_end_price() >= real_up_percent) {
                buy_rst insert_rst{};
                //为了不因为list_begin++而导致的空缺了一天
                auto next_day_ite = list_begin;
                next_day_ite++;
                //如果是当前天达到了目标涨幅，那么就在下一天买入
                if(next_day_ite != list_end) {
                    temp_stock = &(*next_day_ite);
                    insert_rst.base_info = temp_stock;
                    rst_list.push_back(insert_rst);
                    if(rst_list.size() == 1) {
                        rst_begin = rst_list.begin();
                        continue;
                    }
                }
            }
        }
        else {
            up_start_stock = temp_stock;
        }
        pre_day_info = temp_stock;


        //将下面原先的==判定改为>=,就可以实现@param down_days为零的时候，任意一天买入，判定days之内的收益如何

        auto windows_p = rst_begin;
        rst_end = rst_list.end();
        while(windows_p != rst_end) {
            buy_rst& rst_info = *windows_p;
            //特殊处理这种情况，避免后面的天通前面的天计算收益
            if(rst_info.base_info->get_date_info() >= temp_stock->get_date_info()) {
                //如果此种情况达成，说明已经到了rst_list的末尾
                break;
            }
            if(rst_info.cal_days_count >= days) {
                //永远不使rst_begin指向rst_list的末尾
                auto temp_ite = rst_begin;
                if(++temp_ite != rst_end) {
                    rst_begin++;
                }
                if(rst_info.cal_days_count == days) {
                    buy_rst& target_buy_rst = (*rst_begin);
                    float buy_price_val = (target_buy_rst.base_info->*buy_fun)();
                    float cur_sold_price_val = (temp_stock->*sold_fun)();
                    float curr_win_percent = (cur_sold_price_val - buy_price_val) / buy_price_val;
                    target_buy_rst.days_percent = curr_win_percent;
                    max_win_percent = curr_win_percent > max_win_percent ? curr_win_percent : max_win_percent;
                    max_lose_percent = max_lose_percent < curr_win_percent ? max_lose_percent : curr_win_percent;
                    min_win_percent = curr_win_percent > 0 && curr_win_percent < min_win_percent ? curr_win_percent : min_win_percent;
                    curr_win_percent > 0 ? up_days_num++ : 1;
                    curr_win_percent < 0 ? down_days_num++ : 1;

                    //统计最大收益率的平均值
                    ave_max_win_percent += rst_info.max_win_percent;
                    //统计到达@param days天是收益率的平均值
                    ave_target_win_percent += rst_info.days_percent;
                    rst_info.cal_days_count++;
                }
            }
            else {
                float buy_price_val = (rst_info.base_info->*buy_fun)();
                float cur_sold_price_val = (temp_stock->*sold_fun)();
                float curr_win_percent = (cur_sold_price_val - buy_price_val) / buy_price_val;
                rst_info.max_win_percent = rst_info.max_win_percent > curr_win_percent ? rst_info.max_win_percent : curr_win_percent;
                if(rst_info.max_win_percent > curr_win_percent) {
                    rst_info.lower_max_win_days++;
                }
                else {
                    rst_info.to_max_win_days = rst_info.to_max_win_days + rst_info.lower_max_win_days + 1;
                    rst_info.lower_max_win_days = 0;
                }
                rst_info.mid_max_win_info = curr_win_percent > mid_max_win_percent ? temp_stock : rst_info.mid_max_win_info;
                rst_info.max_lost_percent = rst_info.max_lost_percent > curr_win_percent ?  curr_win_percent : rst_info.max_lost_percent;

                mid_max_win_percent = curr_win_percent > mid_max_win_percent ? curr_win_percent : mid_max_win_percent;
                mid_max_lose_percent = mid_max_lose_percent < curr_win_percent ? mid_max_lose_percent : curr_win_percent;
                mid_min_win_percent = curr_win_percent > 0 && curr_win_percent < mid_min_win_percent ? curr_win_percent : mid_min_win_percent;
                rst_info.mid_has_up = curr_win_percent > 0 || rst_info.mid_has_up;
                rst_info.cal_days_count++;
            }
            windows_p++;
            
        }
        list_begin++;
    }

    //收益率平均值方面的重新计算
    ave_max_win_percent /= rst_list.size();
    ave_target_win_percent /= rst_list.size();

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
    rst_begin = rst_list.begin(), rst_end = rst_list.end();
    while(rst_begin != rst_end) {
        buy_rst& rst_info = (*rst_begin);
        //统计一下相关信息,@param days的中间历程当中是否上涨过
        rst_info.mid_has_up ? mid_up_days_num++ : mid_down_days_num++;

        output_str.append(rst_info.base_info->get_date_info_str()).append(",");
        output_str.append(rst_info.base_info->get_stock_code()).append(",");
        output_str.append(rst_info.base_info->get_stock_name()).append(",");
        output_str.append(std::to_string(rst_info.base_info->get_begin_price())).append(",");
        output_str.append(std::to_string(rst_info.base_info->get_end_price())).append(",");
        output_str.append(std::to_string(rst_info.base_info->get_max_price())).append(",");
        output_str.append(std::to_string(rst_info.base_info->get_min_price())).append(",");
        output_str.append(std::to_string(rst_info.days_percent * 100)).append("%,");
        output_str.append(std::to_string(rst_info.max_win_percent * 100)).append("%,");
        output_str.append(std::to_string(rst_info.max_lost_percent * 100)).append("%,");
        output_str.append(std::to_string(rst_info.to_max_win_days)).append(",");
        output_str.append(rst_info.mid_has_up ? "Y" : "N").append(",");
        output_str.append("\n");
        rst_begin++;
    }

    //输出统计信息到字符串当中
    output_str.append(res_map["100000CH00023"]/*上涨天数：*/).append(std::to_string(up_days_num)).append("\n");
    output_str.append(res_map["100000CH00024"]/*下跌天数：*/).append(std::to_string(down_days_num)).append("\n");
    output_str.append(res_map["100000CH00025"]/*中间历程上涨天数：*/).append(std::to_string(mid_up_days_num)).append("\n");
    output_str.append(res_map["100000CH00026"]/*中间历程未上涨天数：*/).append(std::to_string(mid_down_days_num)).append("\n");
    output_str.append(res_map["100000CH00028"]/*目标日最大盈利百分比*/).append(std::to_string(max_win_percent)).append("\n");
    output_str.append(res_map["100000CH00027"]/*目标日最小盈利百分比*/).append(std::to_string(min_win_percent)).append("\n");
    output_str.append(res_map["100000CH00029"]/*目标日最大损失百分比：*/).append(std::to_string(max_lose_percent)).append("\n");
    output_str.append(res_map["100000CH00019"]/*中间最大盈利百分比*/).append(":").append(std::to_string(mid_max_win_percent)).append("\n");
    output_str.append(res_map["100000CH00030"]/*中间最小盈利百分比*/).append(std::to_string(mid_min_win_percent)).append("\n");
    output_str.append(res_map["100000CH00020"]/*中间最大损失百分比*/).append(":").append(std::to_string(mid_max_lose_percent)).append("\n");
    output_str.append(res_map["100000CH00031"]/*平均最大盈利百分比*/).append(std::to_string(ave_max_win_percent)).append("\n");
    output_str.append(res_map["100000CH00032"]/*平均目标日盈利百分比*/).append(std::to_string(ave_target_win_percent)).append("\n");

    return output_str;
}

/**
 * 斜率分析，通过斜率来获取上涨下跌以及相关信息
 *
 * TODO -- 存在问题，应当同前一天价格比较，来确定是否上升，而不是同上一个高点或低点比较从而确定是否上涨或者下跌
 *  否则会忽略长线当中的短期波动
 */
void Calculator::slope_ana(std::list<stock_info>& info_list, std::list<turn_point>& turn_point_list, PriceFlag price, int ignore_days, int unignore_percent) {

    //统计数据
    int small_wave_count = 0;

    //根据哪个价格来计算斜率
    price_fun cal_price = get_price_fun(price);

    stock_info* curr_info, *pre_info;
    auto info_begin = info_list.rbegin(), info_end = info_list.rend();

    //初始化
    float un_ig_percent = static_cast<float>(unignore_percent) / 100;
    turn_point temp_turn{};
    temp_turn.start_point = &(*info_begin);
    pre_info = &(*info_begin);
    info_begin++;
    temp_turn.origin_up = ((*info_begin).*cal_price)() > (temp_turn.start_point->*cal_price)();
    turn_point_list.push_back(temp_turn);
    info_begin++;

    unsigned int turn_last_days = 0;

    while(info_begin != info_end) {
        curr_info = &(*info_begin);
        turn_point& turn_before = turn_point_list.back();
        turn_last_days++;

        float cur_price = (curr_info->*cal_price)();
        float turn_before_price = (turn_before.start_point->*cal_price)();
        float slope = (cur_price - turn_before_price) / turn_last_days;
        float percent = (cur_price - turn_before_price) / turn_before_price;
        bool is_up = cur_price > turn_before_price;
        if(is_up && turn_before.origin_up) {
            info_begin++;
            continue;
        }
        else if((!is_up) && (!turn_before.origin_up)) {
            info_begin++;
            continue;
        }
        else if(turn_last_days >= ignore_days || percent >= un_ig_percent){
            turn_point insert_point{};
            insert_point.start_point = curr_info;
            insert_point.origin_up = is_up;
            insert_point.slope = slope;
            insert_point.percent = percent;
            insert_point.wave_days = turn_last_days;
            turn_point_list.push_back(insert_point);
            turn_last_days = 0;
        }
        info_begin++;
    }
}

std::string Calculator::limit_up_ana(std::list<stock_info>& info_list, int days, PriceFlag buy_price, PriceFlag sold_price) {
    struct buy_rst {
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
        int cal_days_count= 0;
        //涨停之后的那一天开盘价是否就是最低价
        bool is_begin_the_max = false;
    };

    //统计信息
    int total_days = 0; //总天数，可能有部分天不盈利也不损失
    //@param days天的时候，如果获利，则此数加一
    int up_days_num = 0;
    //@param days天的时候，如果损失，则此数加一
    int down_days_num = 0;
    //在历经@param days这些天当中，如能获利，则此数加一，即中间历程可上涨天数
    int mid_up_days_num = 0;
    //在历经@param days这些天当中，如果始终未获利（包含一直损失），则此数加一，
    int mid_down_days_num = 0;
    //下面三个为@param days天后那一天的数据，所有天的统计信息
    float max_win_percent = 0; //最大的获利比例
    float min_win_percent = 10000;  //最小获利比例
    float max_lose_percent = 0; //最大损失比例
    //@param days天的过程当中最大收益率的平均值
    float ave_max_win_percent = 0;
    //到达@param days天的时候，收益率的平均值是多少
    float ave_target_win_percent = 0;

    //下面三个为@param days天之间的数据，所有天的统计信息
    float mid_max_win_percent = 0; //最大的获利比例
    float mid_min_win_percent = 10000;  //最小获利比例
    float mid_max_lose_percent = 0; //最大损失比例

    price_fun buy_fun = get_price_fun(buy_price), sold_fun = get_price_fun(sold_price);

    auto list_begin = info_list.rbegin(), list_end = info_list.rend();
    std::list<buy_rst> rst_list;
    std::list<buy_rst>::iterator rst_begin = rst_list.begin(), rst_end;

    //初始化rst_list
    stock_info* day_before_yesterday = list_begin != list_end ? &(*list_begin) : nullptr;
    list_begin++;
    stock_info* yesterday = list_begin != list_end ? &(*list_begin) : nullptr;
    list_begin++;
    stock_info* cur_day = nullptr;

    stock_info* before_days, *after_days, *buy_day, *sold_day;
    while(list_begin != list_end) {
        cur_day = &(*list_begin);

        //如果当天是涨停的话,>1.098是为了留点冗余空间
        if(yesterday->get_end_price() / day_before_yesterday->get_end_price() > 1.098) {
            buy_rst insert_rst{};
            insert_rst.base_info = cur_day;
            rst_list.push_back(insert_rst);
            if(rst_list.size() == 1) {
                rst_begin = rst_list.begin();
            }
        }

        auto windows_p = rst_begin;
        rst_end = rst_list.end();
        while(windows_p != rst_end) {
            buy_rst& rst_info = *windows_p;
            //如果是当天的话，当天不会和自己计算的
            if(rst_info.base_info == cur_day) {
                break;
            }
            if(rst_info.cal_days_count >= days) {
                //永远不使rst_begin指向rst_list的末尾
                auto temp_ite = rst_begin;
                if(++temp_ite != rst_end) {
                    rst_begin++;
                }
                if(rst_info.cal_days_count == days) {
                    buy_rst& target_buy_rst = (*rst_begin);
                    float buy_price_val = (target_buy_rst.base_info->*buy_fun)();
                    float cur_sold_price_val = (cur_day->*sold_fun)();
                    float curr_win_percent = (cur_sold_price_val - buy_price_val) / buy_price_val;
                    target_buy_rst.days_percent = curr_win_percent;
                    max_win_percent = curr_win_percent > max_win_percent ? curr_win_percent : max_win_percent;
                    max_lose_percent = max_lose_percent < curr_win_percent ? max_lose_percent : curr_win_percent;
                    min_win_percent = curr_win_percent > 0 && curr_win_percent < min_win_percent ? curr_win_percent : min_win_percent;
                    curr_win_percent > 0 ? up_days_num++ : 1;
                    curr_win_percent < 0 ? down_days_num++ : 1;

                    //统计最大收益率的平均值
                    ave_max_win_percent += rst_info.max_win_percent;
                    //统计到达@param days天是收益率的平均值
                    ave_target_win_percent += rst_info.days_percent;
                    rst_info.cal_days_count++;
                }
            }
            else {
                float buy_price_val = (rst_info.base_info->*buy_fun)();
                float cur_sold_price_val = (cur_day->*sold_fun)();
                float curr_win_percent = (cur_sold_price_val - buy_price_val) / buy_price_val;
                rst_info.max_win_percent = rst_info.max_win_percent > curr_win_percent ? rst_info.max_win_percent : curr_win_percent;
                if(rst_info.max_win_percent > curr_win_percent) {
                    rst_info.lower_max_win_days++;
                }
                else {
                    rst_info.to_max_win_days = rst_info.to_max_win_days + rst_info.lower_max_win_days + 1;
                    rst_info.lower_max_win_days = 0;
                }
                rst_info.mid_max_win_info = curr_win_percent > mid_max_win_percent ? before_days : rst_info.mid_max_win_info;
                rst_info.max_lost_percent = rst_info.max_lost_percent > curr_win_percent ?  curr_win_percent : rst_info.max_lost_percent;

                mid_max_win_percent = curr_win_percent > mid_max_win_percent ? curr_win_percent : mid_max_win_percent;
                mid_max_lose_percent = mid_max_lose_percent < curr_win_percent ? mid_max_lose_percent : curr_win_percent;
                mid_min_win_percent = curr_win_percent > 0 && curr_win_percent < mid_min_win_percent ? curr_win_percent : mid_min_win_percent;
                rst_info.mid_has_up = curr_win_percent > 0 || rst_info.mid_has_up;

                //判定涨停的后一天，开盘价是否就是最低价
                boost::gregorian::date::duration_type diff = cur_day->get_date_info() - rst_info.base_info->get_date_info();
                rst_info.is_begin_the_max = diff.days() == 1 && (cur_day->get_begin_price() - cur_day->get_min_price()) < 0.01;
                rst_info.cal_days_count++;
            }
            windows_p++;
        }
        day_before_yesterday = yesterday;
        yesterday = cur_day;
        list_begin++;
    }

    //收益率平均值方面的重新计算
    ave_max_win_percent /= rst_list.size();
    ave_target_win_percent /= rst_list.size();

    //构建输出信息，csv格式
    std::string output_str;
    //下面几个字段按照顺序分别是：
    //日期,编码,名称,除权开盘价,除权收盘价,除权最高价,除权最低价,目标日盈利百分比,中间最大盈利百分比,中间最大损失百分比
    //中间最大盈利所需天数,中间是否盈利,涨停后一天开盘价是否最低价
    output_str.append(res_map["100000CH00001"]).append(",").append(res_map["100000CH00002"]).append(",");
    output_str.append(res_map["100000CH00003"]).append(",").append(res_map["100000CH00004"]).append(",");
    output_str.append(res_map["100000CH00005"]).append(",").append(res_map["100000CH00006"]).append(",");
    output_str.append(res_map["100000CH00007"]).append(",").append(res_map["100000CH00018"]).append(",");
    output_str.append(res_map["100000CH00019"]).append(",").append(res_map["100000CH00020"]).append(",");
    output_str.append(res_map["100000CH00021"]).append(",").append(res_map["100000CH00022"]).append(",");
    output_str.append(res_map["100000CH00033"]).append(",");
    output_str.append("\n");
    rst_begin = rst_list.begin(), rst_end = rst_list.end();
    while(rst_begin != rst_end) {
        buy_rst& rst_info = (*rst_begin);
        //统计一下相关信息,@param days的中间历程当中是否上涨过
        rst_info.mid_has_up ? mid_up_days_num++ : mid_down_days_num++;

        output_str.append(rst_info.base_info->get_date_info_str()).append(",");
        output_str.append(rst_info.base_info->get_stock_code()).append(",");
        output_str.append(rst_info.base_info->get_stock_name()).append(",");
        output_str.append(std::to_string(rst_info.base_info->get_begin_price())).append(",");
        output_str.append(std::to_string(rst_info.base_info->get_end_price())).append(",");
        output_str.append(std::to_string(rst_info.base_info->get_max_price())).append(",");
        output_str.append(std::to_string(rst_info.base_info->get_min_price())).append(",");
        output_str.append(std::to_string(rst_info.days_percent * 100)).append("%,");
        output_str.append(std::to_string(rst_info.max_win_percent * 100)).append("%,");
        output_str.append(std::to_string(rst_info.max_lost_percent * 100)).append("%,");
        output_str.append(std::to_string(rst_info.to_max_win_days)).append(",");
        output_str.append(rst_info.mid_has_up ? "Y" : "N").append(",");
        output_str.append(rst_info.is_begin_the_max ? "Y" : "N").append(",");
        output_str.append("\n");
        rst_begin++;
    }

    //输出统计信息到字符串当中
    output_str.append(res_map["100000CH00023"]/*上涨天数：*/).append(std::to_string(up_days_num)).append("\n");
    output_str.append(res_map["100000CH00024"]/*下跌天数：*/).append(std::to_string(down_days_num)).append("\n");
    output_str.append(res_map["100000CH00025"]/*中间历程上涨天数：*/).append(std::to_string(mid_up_days_num)).append("\n");
    output_str.append(res_map["100000CH00026"]/*中间历程未上涨天数：*/).append(std::to_string(mid_down_days_num)).append("\n");
    output_str.append(res_map["100000CH00028"]/*目标日最大盈利百分比*/).append(std::to_string(max_win_percent)).append("\n");
    output_str.append(res_map["100000CH00027"]/*目标日最小盈利百分比*/).append(std::to_string(min_win_percent)).append("\n");
    output_str.append(res_map["100000CH00029"]/*目标日最大损失百分比：*/).append(std::to_string(max_lose_percent)).append("\n");
    output_str.append(res_map["100000CH00019"]/*中间最大盈利百分比*/).append(":").append(std::to_string(mid_max_win_percent)).append("\n");
    output_str.append(res_map["100000CH00030"]/*中间最小盈利百分比*/).append(std::to_string(mid_min_win_percent)).append("\n");
    output_str.append(res_map["100000CH00020"]/*中间最大损失百分比*/).append(":").append(std::to_string(mid_max_lose_percent)).append("\n");
    output_str.append(res_map["100000CH00031"]/*平均最大盈利百分比*/).append(std::to_string(ave_max_win_percent)).append("\n");
    output_str.append(res_map["100000CH00032"]/*平均目标日盈利百分比*/).append(std::to_string(ave_target_win_percent)).append("\n");

    return output_str;
}

/**
 * 计算移动均线，移动均线的数据存储到stock_info当中，为此重构了一下stock_info的类
 * 
 * @param info_list 输入的所有基本信息
 */
void Calculator::calculate_ma(std::list<stock_info>& info_list) {
    auto l_be = info_list.rbegin(), l_end = info_list.rend();
    stock_info* curr = nullptr;
    int ma_count[60] = {0};
    std::list<stock_info>::iterator ma_be_val[60] = {info_list.begin()};
    float ma_all_val[60] = {0};
    while(l_be != l_end) {
        curr = &(*l_be);
        if(!curr->get_ma()) {
            curr->set_ma(new float[60]);
        }
        for(int i = 0;i < 60;i++) {
            if(ma_count[i] == i + 1) {
                ma_all_val[i] = ma_all_val[i] - ((*ma_be_val[i]).get_af_end_price());
                (ma_be_val[i])++;
                ma_all_val[i] += curr->get_af_end_price();
                curr->get_ma()[i] / (i + 1);
            }
            else {
                ma_count[i] = ma_count[i] + 1;
            }
        }
        l_be++;
    }
}
