#pragma once

#ifndef UI_MAIN_H_
#define UI_MAIN_H_
#include <gtk/gtk.h>
#include <iostream>
#include "k_line.h"
#include "time_index_chart.h"
#include "stock_app.h"
#include "stock_app_window.h"

class ui_main {
private:
	GApplication* app;

	static void print_hello_(GtkWidget* widget, gpointer data);
	static void activate_fun(GtkApplication*);
public:
	explicit ui_main(int, char**);

	~ui_main();
};

#endif