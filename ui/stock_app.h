#ifndef STOCK_APP_H_
#define STOCK_APP_H_

#include <gtk/gtk.h>

#define STOCK_TYPE_APP (stock_app_get_type())

G_DECLARE_DERIVABLE_TYPE(StockApp, stock_app, STOCK, APP, GtkApplication)

StockApp* stock_app_new();

#endif

