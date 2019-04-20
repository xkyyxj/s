#pragma once
#ifndef HEAD_BAR_H_
#define HEAD_BAR_H_

#include <gtk/gtk.h>

class head_bar {
private:
	GtkWidget* menu_bar;
public:
	explicit head_bar();
};

#endif