#include "ui_main.h"

enum
{
	COLUMN = 0,
	NUM_COLS
};

void ui_main::print_hello_(GtkWidget* widget, gpointer data) {
	std::cout << "Hello C++ & GTK+" << std::endl;
}

void ui_main::activate_fun(GtkApplication* app) {
	GtkWidget* window;
	GtkWidget* button;
	GtkWidget* button_box;
	window = gtk_application_window_new(app);
	gtk_window_set_title(GTK_WINDOW(window), "Window");
	gtk_window_set_default_size(GTK_WINDOW(window), 800, 1000);

	button_box = gtk_button_box_new(GTK_ORIENTATION_HORIZONTAL);
	gtk_container_add(GTK_CONTAINER(window), button_box);

	GValue g_val = G_VALUE_INIT;
	g_value_init(&g_val, G_TYPE_VARIANT);
	/*GVariantDict price_dict;
	g_variant_dict_init(&price_dict, nullptr);*/
	/*GVariant* variant = g_variant_new("{}");
	GVariantDict* price_dict = g_variant_dict_new(variant);
	g_variant_dict_insert(price_dict, "open", "d", 12.7);
	g_variant_dict_insert(price_dict, "close", "d", 13.7);
	g_variant_dict_insert(price_dict, "high", "d", 14.7);
	g_variant_dict_insert(price_dict, "low", "d", 10.7);
	g_value_set_variant(&g_val, variant);*/

	GVariantBuilder* b;
	GVariant* dict;

	b = g_variant_builder_new(G_VARIANT_TYPE("a{sv}"));
	g_variant_builder_add(b, "{sv}", "open", g_variant_new_double(11.7));
	g_variant_builder_add(b, "{sv}", "close", g_variant_new_double(12.7));
	g_variant_builder_add(b, "{sv}", "high", g_variant_new_double(13.7));
	g_variant_builder_add(b, "{sv}", "low", g_variant_new_double(10.7));
	dict = g_variant_builder_end(b);
	g_value_set_variant(&g_val, dict);

	//试验下新组件
	GtkWidget* _k_line = k_line_new_with_price_info(dict);
	g_variant_unref(dict);

	GtkWidget* temp = time_index_chart_new();

	//g_object_set_property(G_OBJECT(_k_line), "price-info", &g_val);

	//GValue g_val2 = G_VALUE_INIT;
	//g_value_init(&g_val, G_TYPE_VARIANT);
	/*g_value_set_variant(&g_val2, g_variant_new_uint64(123));
	g_object_set_property(G_OBJECT(_k_line), "foo", &g_val2);*/
	//gtk_container_add(GTK_CONTAINER(window), _k_line);

	//button = gtk_button_new_with_label("Hello World");
	/*button = gtk_button_new();
	GValue g_val = G_VALUE_INIT;
	g_value_init(&g_val, G_TYPE_STRING);
	g_value_set_string(&g_val, "123213213");
	g_object_set_property(G_OBJECT(button), "label", &g_val);
	g_signal_connect(button, "clicked", G_CALLBACK(print_hello_), NULL);
	g_signal_connect_swapped(button, "clicked", G_CALLBACK(gtk_widget_destroy), window);*/
	gtk_container_add(GTK_CONTAINER(button_box), temp);
	gtk_widget_show_all(window);
}

static GtkTreeModel* create_tree_model() {
	GtkTreeStore* treestore;
	GtkTreeIter toplevel, child;

	treestore = gtk_tree_store_new(NUM_COLS,
		G_TYPE_STRING);

	gtk_tree_store_append(treestore, &toplevel, NULL);
	gtk_tree_store_set(treestore, &toplevel,
		COLUMN, "Scripting languages",
		-1);

	gtk_tree_store_append(treestore, &child, &toplevel);
	gtk_tree_store_set(treestore, &child,
		COLUMN, "Python",
		-1);
	gtk_tree_store_append(treestore, &child, &toplevel);
	gtk_tree_store_set(treestore, &child,
		COLUMN, "Perl",
		-1);
	gtk_tree_store_append(treestore, &child, &toplevel);
	gtk_tree_store_set(treestore, &child,
		COLUMN, "PHP",
		-1);

	gtk_tree_store_append(treestore, &toplevel, NULL);
	gtk_tree_store_set(treestore, &toplevel,
		COLUMN, "Compiled languages",
		-1);

	gtk_tree_store_append(treestore, &child, &toplevel);
	gtk_tree_store_set(treestore, &child,
		COLUMN, "C",
		-1);

	gtk_tree_store_append(treestore, &child, &toplevel);
	gtk_tree_store_set(treestore, &child,
		COLUMN, "C++",
		-1);

	gtk_tree_store_append(treestore, &child, &toplevel);
	gtk_tree_store_set(treestore, &child,
		COLUMN, "Java",
		-1);

	return GTK_TREE_MODEL(treestore);
}

ui_main::ui_main(int argc, char** argv) {
	/*int status;

	app = gtk_application_new("org.gtk.example", G_APPLICATION_FLAGS_NONE);
	g_signal_connect(app, "activate", G_CALLBACK(activate_fun), NULL);
	status = g_application_run(G_APPLICATION(app), argc, argv);*/

	//GtkBuilder* builder;
	//GObject* window;
	//GObject* tree;
	//GObject* button;
	//GtkCellRenderer* renderer;
	//GError* error = NULL;

	//gtk_init(&argc, &argv);

	///* Construct a GtkBuilder instance and load our UI description */
	//builder = gtk_builder_new();
	//if (gtk_builder_add_from_file(builder, "D:\\builder.glade", &error) == 0)
	//{
	//	std::cout << " 123213 " << std::endl;
	//	g_printerr("Error loading file: %s\n", error->message);
	//	g_clear_error(&error);
	//	//return 1;
	//}

	///* Connect signal handlers to the constructed widgets. */
	//window = gtk_builder_get_object(builder, "window1");
	//g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);
	//gtk_window_maximize(GTK_WINDOW(window));
	//tree = gtk_builder_get_object(builder, "treeview1");
	//GtkTreeViewColumn* col;
	//col = gtk_tree_view_column_new();
	//gtk_tree_view_column_set_title(col, "Programming languages");
	//gtk_tree_view_append_column(GTK_TREE_VIEW(tree), col);

	//renderer = gtk_cell_renderer_text_new();
	//gtk_tree_view_column_pack_start(col, renderer, TRUE);
	//gtk_tree_view_column_add_attribute(col, renderer,
	//	"text", COLUMN);

	//GtkTreeModel* model;
	//model = create_tree_model();
	//gtk_tree_view_set_model(GTK_TREE_VIEW(tree), model);
	//g_object_unref(model);

	////试一下添加自己的新组件
	//GVariantBuilder* b;
	//GVariant* dict;

	//b = g_variant_builder_new(G_VARIANT_TYPE("a{sv}"));
	//g_variant_builder_add(b, "{sv}", "open", g_variant_new_double(11.7));
	//g_variant_builder_add(b, "{sv}", "close", g_variant_new_double(12.7));
	//g_variant_builder_add(b, "{sv}", "high", g_variant_new_double(13.7));
	//g_variant_builder_add(b, "{sv}", "low", g_variant_new_double(10.7));
	//dict = g_variant_builder_end(b);

	////试验下新组件
	//GtkWidget* _k_line = k_line_new_with_price_info(dict);
	////g_variant_unref(dict);

	//GtkWidget* time_index_chart = time_index_chart_new();


	//GObject* box = NULL;
	//box = gtk_builder_get_object(builder, "paned3");
	//GObject* widget = gtk_builder_get_object(builder, "label9");
	//gtk_widget_destroy(GTK_WIDGET(widget));
	//gtk_paned_pack1(GTK_PANED(box), time_index_chart, TRUE, TRUE);
	//g_signal_connect(button, "clicked", G_CALLBACK(print_hello_), NULL);
	//g_object_unref(model);
	/*button = gtk_builder_get_object(builder, "button1");
	g_signal_connect(button, "clicked", G_CALLBACK(print_hello_), NULL);

	button = gtk_builder_get_object(builder, "button2");
	g_signal_connect(button, "clicked", G_CALLBACK(print_hello_), NULL);

	button = gtk_builder_get_object(builder, "quit");
	g_signal_connect(button, "clicked", G_CALLBACK(gtk_main_quit), NULL);*/
	/*gtk_widget_show_all(GTK_WIDGET(window));
	gtk_main();*/
	this->app = G_APPLICATION(stock_app_new());
	g_application_run(this->app, argc, argv);
	
}

ui_main::~ui_main() {
	//g_application_quit(G_APPLICATION(app));
	//g_object_unref(app);
}