#ifndef STOCK_MAIN_WINDOW_H_
#define STOCK_MAIN_WINDOW_H_

#include <gtk/gtk.h>
#include "stock_app.h"

#define STOCK_TYPE_APP_WINDOW (stock_app_window_get_type ())
G_DECLARE_FINAL_TYPE (StockAppWindow, stock_app_window, STOCK, APP_WINDOW, GtkApplicationWindow)

StockAppWindow *stock_app_window_new(StockApp *app);

#endif