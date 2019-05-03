#include "stock_app_window.h"
#include <iostream>
#include "stockapp.h"

struct StockAppWindowPrivate {
	GtkWidget* quit;
	GtkWidget* grid;
	GtkWidget* treeview;
};

struct _StockAppWindowClass {
	GtkApplicationWindowClass parent_class;
};

struct _StockAppWindow {
	GtkApplicationWindow window;
	StockAppWindowPrivate* priv;
};

// 初始化声明相关的结构体以及函数
G_DEFINE_TYPE_WITH_PRIVATE(StockAppWindow, stock_app_window, GTK_TYPE_APPLICATION_WINDOW)

/*
 *	方法声明
 */
StockAppWindow* stock_app_window_new(StockApp* app);
static void stock_app_window_dispose(GObject* obj);

// 树结构选中更改事件回调
static void tree_selected_changed(GtkWidget* widget, gpointer statusbar);

static void quit_activated(GSimpleAction* action, GVariant* parameter, gpointer app);

static void stock_app_window_class_init (StockAppWindowClass *win_class) {
	G_OBJECT_CLASS (win_class)->dispose = stock_app_window_dispose;
	//GResource* b = stockapp_get_resource();
	//GBytes* by = g_resource_lookup_data(b, "/org/gtk/stockapp/builder.glade", static_cast<GResourceLookupFlags>(0), NULL);
	GResource* resource = g_resource_load("C:\\Users\\wangqchf\\ClionProjects\\123\\ui\\stockapp.gresource", NULL);
	g_resources_register(resource);
	gtk_widget_class_set_template_from_resource (GTK_WIDGET_CLASS (win_class), "/org/gtk/stockapp/builder.glade");
	gtk_widget_class_bind_template_child_private (GTK_WIDGET_CLASS (win_class), StockAppWindow, grid);
	gtk_widget_class_bind_template_child_private (GTK_WIDGET_CLASS(win_class), StockAppWindow, treeview);
	//gtk_widget_class_bind_template_child_private (GTK_WIDGET_CLASS(win_class), StockAppWindow, quit);
	g_resources_unregister(resource);
	g_resource_unref(resource);
}

StockAppWindow* stock_app_window_new(StockApp *app) {
	return STOCK_APP_WINDOW(g_object_new(STOCK_TYPE_APP_WINDOW, "application", app, NULL));
}

void stock_app_window_init(StockAppWindow* window) {
	gtk_widget_init_template(GTK_WIDGET(window));
	StockAppWindowPrivate* priv = static_cast<StockAppWindowPrivate*>(stock_app_window_get_instance_private(window));
	GtkTreeSelection* selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(priv->treeview));
	GtkButton* quit = GTK_BUTTON(priv->quit);
	g_signal_connect(selection, "changed", G_CALLBACK(tree_selected_changed), NULL);
	//g_signal_connect(quit, "clicked", G_CALLBACK(quit_activated), NULL);
	g_signal_connect(G_OBJECT(window), "destroy", G_CALLBACK(quit_activated), NULL);
}

static void stock_app_window_dispose(GObject* obj) {

}

static void tree_selected_changed(GtkWidget* widget, gpointer statusbar) {
	std::cout << "selected" << std::endl;
}

static void quit_activated(GSimpleAction* action, GVariant* parameter, gpointer app) {
	g_application_quit(G_APPLICATION(app));
}
