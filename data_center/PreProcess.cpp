#include "PreProcess.h"

/**
 * 从新浪财经上获取到的股票的基本信息都是按照时间顺序倒序排列的，
 * 所以此处该方法插入到@param out_list当中的数据也是按照时间顺序
 * 倒序排列的
 */
void PreProcess::pre_process_base_info(const std::string& input, std::list<stock_info>& out_list) {
    size_t curr_posi = input.find('\n', 0); //过滤掉第一行的标题栏
    size_t item_end = 0;
    while((item_end = input.find('\n', curr_posi + 1)) <= (input.length() - 1)) {
        /**
         * 第二个参数是要截取的字符串的长度，第一次写错了debug了好久，第二个参数额外减二是为了去掉末尾的
         * \r\n
         *
         * 此处还有一个技术细节：关于右值引用的东西，也就是两个“&”，详情可见印象笔记上
         */
        stock_info&& temp_stock = pre_process_item(input.substr(curr_posi + 1, item_end - curr_posi - 2));
        if(temp_stock.get_begin_price() != 0) {
            out_list.push_back(temp_stock);
        }
        curr_posi = item_end;
    }
}

/**
 * 从quotes.money.163.com/service/chddata.html?code=股票编码当中获取到的股票信息
 * 按照每一行转换为stock_info
 *
 * 先按照固定格式来，格式如下：·
 * 日期,股票代码,名称,收盘价,最高价,最低价,开盘价,前收盘,涨跌额,涨跌幅,换手率,成交量,成交金额,总市值,流通市值,成交笔数
 * 目前只取前六个字段，剩下的后续处理 TODO -- 处理剩余的信息字段
 *
 * @param input 单条的股票信息字符串，逗号分隔，格式如上
 */
stock_info PreProcess::pre_process_item(const std::string& input) {
    stock_info _stock_info;
    std::vector<std::string> v;
    boost::split(v, input, boost::is_any_of(","));
    int count = 0;
    using stock_info_fun = void (stock_info::*)(std::string&);
    stock_info_fun fun_array[] = {&stock_info::set_date_info,
                                  &stock_info::set_stock_code,&stock_info::set_stock_name,
                                  &stock_info::set_end_price,&stock_info::set_max_price,
                                  &stock_info::set_min_price,&stock_info::set_begin_price};
    for(boost::iterator_range<std::string::iterator> i : v) {
        if(count > 6) { //现在仅仅处理返回数据的前六个:日期,股票代码,名称,收盘价,最高价,最低价,开盘价,前收盘
            break;
        }
        std::string::iterator s_begin = i.begin(), s_end = i.end();
        std::string temp_str(s_begin, s_end);
        (_stock_info.*fun_array[count])(temp_str);
        count++;
    }
    return _stock_info;
}

/**
 * http://vip.stock.finance.sina.com.cn/corp/go.php/vMS_FuQuanMarketHistory/stockid/
 * 获取从上述网页当中爬虫获取到的复权因子，正则表达式为：\d{4}-\d{2}\s*</a>\n(.*\n){8}
 *
 * fix: 正则表达式替换为：\d{4}-\d{2}-\d{2}(\\t)*(</a>|</div></td>).*?</tr>
 * 因为std::string当中所有的相关的字符都被替换成了转义字符串
 */
void PreProcess::process_weight_info(const std::string& input,std::list<std::string>& ret_result) {
    using namespace boost::xpressive;
    //正则表达式：\d{4}-\d{2}\s*</a>\n(.*\n){8}
    /*sregex regex = repeat<4>(_d) >> "-" >> repeat<2>(_d) >> "-" >> repeat<2>(_d)
            >> *_s >> "</a>" >> _n
            >> (repeat<9>(*(_ >> _n)));*/
    //正则表达式：\\t\d{4}-\d{2}-\d{2}(\\t)*</a>.*?</tr>
    //sregex regex = "\\t" >> repeat<4>(_d) >> "-" >> repeat<2>(_d) >> "-" >> repeat<2>(_d);
    //>>*("\\t") >> "</a>" >> -*_ >> "</tr>";
    /**
     * 正则表达式：\d{4}-\d{2}-\d{2}(\\t)*(</a>|</div></td>).*?</tr>
     * 对于std::string当中制表符表示成\t的情况，Xpressive静态的正则表达式
     * 当中不要表示成\\t，直接写成\t即可。
     */
    sregex regex = repeat<4>(_d) >> "-" >> repeat<2>(_d) >> "-" >> repeat<2>(_d)
                                 >>*(keep("\t")) >> (keep("</a>") | "</div></td>") >> -*_ >> "</tr>";
    //sregex regex = *(keep("\t"));

    /*smatch match_result;
    regex_search(input, match_result, regex);
    unsigned long match_length = match_result.size();
    for(int i = 0;i < match_length;i++) {
        std::string item = match_result[i].str();
        ret_result.push_back(item);
        //ret_result.push_back(process_weight_item(item));
    }*/
    sregex_iterator begin(input.begin(), input.end(), regex), end;
    for(;begin != end;begin++) {
        ret_result.push_back(process_weight_item((*begin)[0].str()));
    }

}

/**
 * 因为@param stock_info以及@param weight_factor都是按照时间倒序排列的，所以可以一遍过滤
 * 实现逻辑，且复权因子总是能够多查询出结果来，因为复选因子是按照季度查询的
 *
 * @param stock_info 股票基本信息，@see stock_info，缺少复权因子以及复权前后价格
 * @param weight_factor 复权因子信息，字符串，形式为：yyyy-MM-dd_权重因子
 *
 * @return 返回当前股票当中最大的复权因子
 */
float PreProcess::set_stoinfo_weifactor(std::list<stock_info>& stock_info, std::list<std::string>& weight_factor) {
    float max_weight_factor = 0;
    auto stock_info_begin_ite = stock_info.begin(), stock_info_end_ite = stock_info.end();
    auto weight_info_begin_ite = weight_factor.begin(), weight_info_end_ite = weight_factor.end();
    float temp_weight_factor = 0;
    while(stock_info_begin_ite != stock_info_end_ite) {
        if((*stock_info_begin_ite).get_date_info_str().compare((*weight_info_begin_ite).substr(0,10)) > 0) {
			//某些情况下，即使没有停牌的，但是这只股票的复权因子信息还是没有，这种情况下取前一天的复权因子信息
			(*stock_info_begin_ite).set_weight_factor(temp_weight_factor);
            ++stock_info_begin_ite;
        }
        /**
         * 平高电气当中检测到一种情况：
         * 可能股票若干天停牌，获取股票基本信息是是有内容的，但是股票的复权因子查询当中没有停牌这些天的信息
         */
        else if((*stock_info_begin_ite).get_date_info_str().compare((*weight_info_begin_ite).substr(0,10)) < 0) {
            ++weight_info_begin_ite;
        }
        else {
            temp_weight_factor = std::strtof((*weight_info_begin_ite).substr(11).data(), nullptr); //截取到最后，即得复权因子,然后转为float
            max_weight_factor = temp_weight_factor > max_weight_factor ? temp_weight_factor : max_weight_factor;
            (*stock_info_begin_ite).set_weight_factor(temp_weight_factor);
            ++stock_info_begin_ite;
            ++weight_info_begin_ite;
        }
        //加个特殊判定，虽然可能永远不会走，亦即复权因子信息耗尽了（基本信息和复权因子信息匹配不上）
        if(weight_info_begin_ite == weight_info_end_ite) {
            break;
        }
    }

    return max_weight_factor;
}

std::string PreProcess::process_weight_item(const std::string& item) {
    std::string ret_result;
    if(item.length() > 0) {
        ret_result.append(item.substr(0,10));
		std::string temp_value = item.substr(0, 10);
		const char* value_char = temp_value.data();
        ret_result.append("_");
        size_t b_index = item.rfind("er\">");
        size_t e_index = item.rfind("</div>");
        b_index += 4;
        ret_result.append(item.substr(b_index,e_index - b_index));
    }

    return ret_result;
}

/**
 * 前复权似乎不止一种方式（起码我就知道两种）
 * 一种是通过复权因子的方式来进行复权，前复权的话就是：当天价格*当天复权因子/最大复权因子，
 * 这种方式存在问题，就是红利分配的问题，另外一种复权方式对于红利分配是直接减去每股分配的红利，
 * 但是这种通过乘以权重的方式就会导致和直接减去红利的方式存在差异
 *
 * 目前还不知道能够获取详细的除权信息的渠道，所以说只能先用这种复权的方式
 */
void PreProcess::pro_weight_front(std::list<stock_info>& info_list, float max_weight_factor) {
    auto b_info = info_list.begin(), e_info = info_list.end();
    stock_info* temp_info_p = nullptr;
    while(b_info != e_info) {
        temp_info_p = &(*b_info);
        temp_info_p->set_be_begin_price(temp_info_p->get_begin_price() * temp_info_p->get_weight_factor() / max_weight_factor);
        temp_info_p->set_be_end_price(temp_info_p->get_end_price() * temp_info_p->get_weight_factor() / max_weight_factor);
        temp_info_p->set_be_max_price(temp_info_p->get_max_price() * temp_info_p->get_weight_factor() / max_weight_factor);
        temp_info_p->set_be_min_price(temp_info_p->get_min_price() * temp_info_p->get_weight_factor() / max_weight_factor);
        b_info++;
    }
}

void PreProcess::pro_weight_back(std::list<stock_info>& info_list) {
    auto begin_iterator = info_list.begin(), end_iterator = info_list.end();
    stock_info* temp_stock = nullptr;
    while(begin_iterator != end_iterator) {
        temp_stock = &(*begin_iterator);
        temp_stock->set_af_begin_price(temp_stock->get_begin_price() * temp_stock->get_weight_factor());
        temp_stock->set_af_end_price(temp_stock->get_end_price() * temp_stock->get_weight_factor());
        temp_stock->set_af_max_price(temp_stock->get_max_price() * temp_stock->get_weight_factor());
        temp_stock->set_af_min_price(temp_stock->get_min_price() * temp_stock->get_weight_factor());
        begin_iterator++;
    }
}

/**
 * 从以前处理过后写入到磁盘当中的文件，重新构建出相应的股票基本信息
 */
void PreProcess::process_existed_info(const std::string& input, std::list<stock_info>& out_list) {
    size_t curr_posi = 0;
    size_t item_end = 0;
    while((item_end = input.find('\n', curr_posi)) <= (input.length() - 2)) {
        stock_info temp_stock;
        temp_stock.from_string(input.substr(curr_posi, item_end - curr_posi - 1));
        out_list.push_back(temp_stock);
        curr_posi = item_end + 1;
    }
}

void PreProcess::pro_weight(std::list<stock_info>& info_list, float max_weight_factor) {
    pro_weight_back(info_list);
    pro_weight_front(info_list, max_weight_factor);
}

void PreProcess::process_all_stock_list(const std::string& input, std::list<std::string>& result_list) {
    using namespace boost::xpressive;
    sregex regex = "(" >> repeat<6>(_d) >> ")";
    sregex_iterator begin(input.begin(), input.end(), regex), end;
    while(begin != end) {
        std::string&& item = (*begin)[0].str();
        result_list.push_back(item.substr(1, 6));
        begin++;
    }
}