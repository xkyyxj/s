#include <iostream>
#include "data_fetch/DataFetch.h"
#include "data_fetch/PreProcess.h"
#include "data_fetch/DataCenter.h"
#include "./algorithm/Calculator.h"
#include <fstream>
#include <gtk/gtk.h>
#include <boost/filesystem.hpp>
#include <hiredis.h>
#include "ui/ui_main.h"
std::map<std::string,std::string> res_map;
#ifdef WINDOWS_PLATFORM
#define DllExport __declspec( dllexport )//宏定义
#define ROOT_DIR "D:\\stock_workspace\\"
#else
#define DllExport
#define ROOT_DIR "~/stock_workspace"
#endif

#ifndef EXECUTION

DllExport std::string get_str() {
    return std::string("12213");
}

#else

static void print_hello(GtkWidget* widget, gpointer data) {
	std::cout << "Hello C++ & GTK+" << std::endl;
}

static void
activate(GtkApplication* app,
	gpointer        user_data)
{
	GtkWidget* window;
	GtkWidget* button;
	GtkWidget* button_box;

	window = gtk_application_window_new(app);
	gtk_window_set_title(GTK_WINDOW(window), "Window");
	gtk_window_set_default_size(GTK_WINDOW(window), 200, 200);

	button_box = gtk_button_box_new(GTK_ORIENTATION_HORIZONTAL);
	gtk_container_add(GTK_CONTAINER(window), button_box);

	button = gtk_button_new_with_label("Hello World");
	g_signal_connect(button, "clicked", G_CALLBACK(print_hello), NULL);
	g_signal_connect_swapped(button, "clicked", G_CALLBACK(gtk_widget_destroy), window);
	gtk_container_add(GTK_CONTAINER(button_box), button);
	gtk_widget_show_all(window);
}

int main(int argc, char* argv[]) {

	//redis默认监听端口为6379 可以再配置文件中修改
	//redisContext* c = redisConnect("127.0.0.1", 6379);
	//if (c->err)
	//{
	//	redisFree(c);
	//	printf("Connect to redisServer faile\n");
	//	//强制退出
	//	MessageBox(NULL, TEXT("redis连接失败，服务端即将退出，请先启动redis!"), TEXT("提示"), MB_ICONWARNING);
	//	exit(0);
	//	return NULL;
	//}
	//printf("Connect to redisServer Success\n");
	//void* reply = redisCommand(c, "SET foo bar");

	ui_main main_ui(argc, argv);
	/*GVariantDict* dict = g_variant_dict_new(NULL);
	g_variant_dict_insert(dict, "num1", "d", 123);
	unsigned int num = 0;
	g_variant_dict_lookup(dict, "num1", "d", &num);
	std::cout << num << std::endl;
	g_variant_dict_unref(dict);

	GVariantDict dict2;
	g_variant_dict_init(&dict2, NULL);
	g_variant_dict_insert(&dict2, "num2", "u", 234234);
	g_variant_dict_lookup(&dict2, "num2", "u", &num);
	std::cout << num << std::endl;*/

	/*GVariantBuilder* b;
	GVariant* dict;

	b = g_variant_builder_new(G_VARIANT_TYPE("a{sv}"));
	g_variant_builder_add(b, "{sv}", "name", g_variant_new_string("foo"));
	g_variant_builder_add(b, "{sv}", "timeout", g_variant_new_int32(10));
	dict = g_variant_builder_end(b);


	b = g_variant_builder_new(G_VARIANT_TYPE("a{sv}"));
	g_variant_builder_add(b, "{sv}", "name", g_variant_new_double(123));
	g_variant_builder_add(b, "{sv}", "timeout", g_variant_new_double(7777));
	dict = g_variant_builder_end(b);*/

	/*b = g_variant_builder_new(G_VARIANT_TYPE("a{ss}"));
	g_variant_builder_add(b, "{ss}", "name", "heihei");
	g_variant_builder_add(b, "{ss}", "timeout", "haha");
	dict = g_variant_builder_end(b);*/

	//GVariant* temp_char = NULL;
	/*gchar* temp_char;
	g_variant_lookup(dict, "name", "s", &temp_char);*/
	///*gsize size = 3;
	//const gchar* rel_char = g_variant_get_string(temp_char, &size);*/

	/*gdouble temp_char = 0;
	GVariant* ret_val = g_variant_lookup_value(dict, "name", G_VARIANT_TYPE_DOUBLE);
	gdouble d = g_variant_get_double(ret_val);*/
	/*gdouble d = 0;
	g_variant_lookup(dict, "name", "d", &d);*/
	//g_variant_unref(ret_val);
	//std::cout << d << std::endl;


	return 0;
	/*namespace rst_info = cassiopeia::stock_analyze::rst_info;
    using rst_info::operator<<;

	const std::string root_dir(ROOT_DIR);

	//读取资源文件，放置到map当中
	std::string res_file(root_dir);
	res_file.append("res_chinese");
	std::ifstream resource_file(res_file);
	while(!resource_file.eof()) {
		std::string temp_value;
		std::getline(resource_file, temp_value);
		const size_t index = temp_value.find('=');
		if(index < 0) {
			continue;
		}
		std::string key = temp_value.substr(0, index);
		const std::string value = temp_value.substr(index + 1, std::string::npos);
		res_map[key] = value;
	}
	resource_file.close();

	//测试斜率分析的函数
	DataCenter data_center;
	std::list<stock_info>& stock_list = data_center.get_stock_info("600072");

	std::string output_file_name(root_dir);
	output_file_name.append("processed_info\\600072\\ana_result\\").append("600072.csv");
	std::ofstream output_file(output_file_name);
	Calculator calculator;
	std::list<rst_info::turn_point> rst_list;
	calculator.section_ana(stock_list, rst_list);
	auto rst_begin = rst_list.begin();
	const auto rst_end = rst_list.end();
	output_file << rst_info::turn_point::to_csv_head();
	while(rst_begin != rst_end) {
		output_file << *rst_begin;
		++rst_begin;
	}*/

	return 0;
}

#endif