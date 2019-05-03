#include "k_line.h"
#include <iostream>

static GParamSpec* obj_properties[P_LENGTH] = { NULL, };

static gboolean k_line_draw(GtkWidget* widget, cairo_t* cr);
static void gtk_k_line_get_preferred_width(GtkWidget* widget, gint* minimum_width, gint* natural_width);
static void gtk_k_line_get_preferred_height(GtkWidget* widget, gint* minimum_height, gint* natural_height);

static void k_line_set_property(GObject* object, guint property_id, const GValue* value, GParamSpec* pspec);
static void k_line_get_property(GObject* object, guint property_id, GValue* value, GParamSpec* pspec);

static void draw_single_k_line(k_line* _k_line, cairo_t* t, int start_x, int start_y, int height, int width,
	float, float, float, float);

static gboolean print_hello(GtkWidget* widget, GdkEventCrossing* event) {
	std::cout << "Signal Print Hello" << std::endl;
	return FALSE;
}

static gboolean print_hello_realize(GtkWidget* widget, GdkEventCrossing* event) {
	std::cout << "Signal Print Hello realized!!" << std::endl;
	return FALSE;
}

static void k_line_init(k_line* _k_line_p) {
	_k_line_p->high = 0;
	_k_line_p->low = 0;
	_k_line_p->open = 0;
	_k_line_p->close = 0;
	//gtk_widget_set_receives_default(GTK_WIDGET(_k_line_p), true);
	//gtk_widget_set_has_window(GTK_WIDGET(_k_line_p), TRUE);
	gtk_widget_add_events(GTK_WIDGET(_k_line_p), GDK_POINTER_MOTION_MASK);
	// 下面一行代码监听鼠标移动事件
	// g_signal_connect(GTK_WIDGET(_k_line_p), "motion-notify-event", G_CALLBACK(print_hello), NULL);
	//g_signal_connect(GTK_WIDGET(_k_line_p), "realize", G_CALLBACK(print_hello_realize), NULL);

	_k_line_p->info_list = new price_info*[10];
	for (int i = 0; i < 10; i++) {
		_k_line_p->info_list[i] = new price_info{
			std::string("123213"),
			std::string("234234"),
			10.01f + i,
			11.01f + i,
			12.01f + i,
			8.01f + i
		};
	}
	_k_line_p->info_list_length = 10;
}

static void k_line_class_init(k_line_class* _k_line_class_p) {
	GObjectClass* gobject_class;
	GtkWidgetClass* widget_class;

	gobject_class = G_OBJECT_CLASS(_k_line_class_p);
	widget_class = (GtkWidgetClass*)_k_line_class_p;
	widget_class->draw = k_line_draw;
	/*widget_class->size_allocate = gtk_entry_size_allocate;*/
	widget_class->get_preferred_width = gtk_k_line_get_preferred_width;
	widget_class->get_preferred_height = gtk_k_line_get_preferred_height;
	gobject_class->set_property = k_line_set_property;
	gobject_class->get_property = k_line_get_property;

	obj_properties[PRICE_INFO] =
		g_param_spec_variant("price-info",
			"Price Info",
			"Price Information of this k line",
			G_VARIANT_TYPE_DICTIONARY,
			NULL,
			G_PARAM_READWRITE);
	obj_properties[PRICE_INFO_LIST] = 
		g_param_spec_pointer("price-info-list",
			"Price Info List",
			"All Price Information of k lines",
			G_PARAM_READWRITE);
	g_object_class_install_properties(gobject_class,
		P_LENGTH,
		obj_properties);
}

static GType k_line_get_type() {
	static GType k_line_type = 0;
	if (!k_line_type) {
		static const GTypeInfo k_line_info = {
			sizeof(k_line_class),
			NULL,NULL,
			(GClassInitFunc)k_line_class_init,
			NULL,NULL,
			sizeof(k_line),
			0,
			(GInstanceInitFunc)k_line_init
		};
		k_line_type = g_type_register_static(GTK_TYPE_MISC, "k_line", &k_line_info, static_cast<GTypeFlags>(0));
	}
	return k_line_type;
}

GtkWidget* k_line_new()
{
	return GTK_WIDGET(g_object_new(K_LINE_TYPE, 0));
}

GtkWidget* k_line_new_with_price_info(GVariant* price_info) {
	return GTK_WIDGET(g_object_new(K_LINE_TYPE, "price-info", price_info));
}

static gboolean k_line_draw(GtkWidget* widget, cairo_t* cr) {
	
	k_line* _k_line_p = K_LINE(widget);

	GtkAllocation allocation{};
	gtk_widget_get_clip(widget, &allocation);
	// 计算每根K线的宽度
	int each_width = allocation.width / _k_line_p->info_list_length;
	
	// 找出最低价以及最高价，计算每条K线的起始高度以及高度
	float min_price = 8.01;
	float max_price = 21.01;
	float between = max_price - min_price;
	std::cout << allocation.x << " " << allocation.y << " " << allocation.height << " " << allocation.width << std::endl;

	/*cairo_rectangle(cr, 20, 30, allocation.width / 2, allocation.height - 30);
	cairo_stroke(cr);*/

	for (int i = 0; i < _k_line_p->info_list_length; i++) {
		price_info* temp_price_info = _k_line_p->info_list[i];
		int start_y = (max_price - temp_price_info->high) / between * allocation.height;
		int height = (temp_price_info->high - temp_price_info->low) / between * allocation.height;
		draw_single_k_line(_k_line_p, cr, 0 + i * each_width, start_y, each_width, height, 
			temp_price_info->open, temp_price_info->close, temp_price_info->high, temp_price_info->low);
	}

	return FALSE;
}

//static void gtk_entry_size_allocate(GtkWidget* widget, GtkAllocation* allocation) {
//	GdkRectangle clip{
//		0,
//		0,100,100
//	};
//
//	std::cout << "allocation" << allocation->x << " " << allocation->y << " " << allocation->width << " " << allocation->height << std::endl;
//
//	gtk_widget_set_allocation(widget, allocation);
//	//allocation->width = 30;
//	//allocation->height = 200;
//
//	//gtk_widget_set_clip(widget, allocation);
//}

static void gtk_k_line_get_preferred_width(GtkWidget* widget, gint* minimum_width, gint* natural_width) {
	(*minimum_width) = 200;
	(*natural_width) = 200;
}

static void gtk_k_line_get_preferred_height(GtkWidget* widget, gint* minimum_height, gint* natural_height) {
	(*minimum_height) = 200;
	(*natural_height) = 200;
}
//}

static void k_line_set_property(GObject* object, guint property_id, const GValue* value, GParamSpec* pspec) {
	std::cout << "In set Property" << std::endl;
	GVariant* temp_price = 0;
	GVariant* variant_val = nullptr;
	k_line* _k_line = nullptr;
	gdouble price = 0;
	gpointer pointer = nullptr;
	switch (property_id)
	{
	case PRICE_INFO:
		_k_line = K_LINE(object);
		variant_val = g_value_get_variant(value);
		g_variant_lookup(variant_val, "open", "d", &price);
		_k_line->open = price;
		g_variant_lookup(variant_val, "close", "d", &price);
		_k_line->close = price;
		g_variant_lookup(variant_val, "high", "d", &price);
		_k_line->high = price;
		g_variant_lookup(variant_val, "low", "d", &price);
		_k_line->low = price;
		std::cout << "open : " << _k_line->open << ";close is " << _k_line->close << std::endl;
		break;
	case PRICE_INFO_LIST:
		_k_line = K_LINE(object);
		pointer = g_value_get_pointer(value);

	default:
		/* We don't have any other property... */
		G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
		break;
	}

	// 触发一次重新绘制操作
	gtk_widget_queue_draw(GTK_WIDGET(object));
}

static void k_line_get_property(GObject* object, guint property_id, GValue* value, GParamSpec* pspec) {
	k_line* self = K_LINE(object);

	switch (property_id)
	{
	case PRICE_INFO:
		g_value_set_double(value, 123213);
		break;

	default:
		/* We don't have any other property... */
		G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
		break;
	}
}

static void draw_single_k_line(k_line* _k_line, cairo_t* cr, int start_x, int start_y, int width, int height,
	float open, float close, float high, float low) {
	std::cout << "draw single K line price:" << open << " " << close << " " << high << " " << low << std::endl;
	std::cout << "draw single K line :" << start_x << " " << start_y << " " << width << " " << height << std::endl;
	float close_open_delta_abs = close > open ? close - open : open - close;
	float main_content_p = close_open_delta_abs / (high - low);

	// 左右留点白，不然太难看
	int margin = width * 0.05;
	start_x += margin;
	width -= 2 * margin;

	//计算上影线占比
	float up_line_delta_abs = high - (open > close ? open : close);
	float up_line_p = up_line_delta_abs / (high - low);

	//计算下影线占比
	float down_line_p = main_content_p + up_line_p;

	double main_start_x = start_x;
	double main_start_y = start_y + height * up_line_p;
	double main_width = width;
	double main_height = height * main_content_p;

	double up_line_start_x = start_x + (width / 2);
	double up_line_start_y = start_y;
	double up_line_end_x = start_x + (width / 2);
	double up_line_end_y = start_y + height * up_line_p;
	std::cout << up_line_start_y << std::endl;

	double down_line_start_y = start_y + height * down_line_p;

	// 说明是
	if (open > close) {
		cairo_set_source_rgb(cr, 0.36078, 0.733333, 0.46274);
	}
	else {
		cairo_set_source_rgb(cr, 1, 0.1764, 0.1686);
	}
	cairo_set_line_width(cr, 3);

	// 绘制K线实体部分
	cairo_rectangle(cr, main_start_x, main_start_y, main_width, main_height);
	if (open > close) {
		cairo_fill_preserve(cr);
	}
	cairo_stroke(cr);

	// 绘制K线上影线
	cairo_move_to(cr, up_line_start_x, up_line_start_y);
	cairo_line_to(cr, up_line_end_x, up_line_end_y);
	cairo_stroke(cr);

	// 绘制K线下影线
	cairo_move_to(cr, up_line_start_x, down_line_start_y);
	cairo_line_to(cr, up_line_end_x, start_y + height);
	cairo_stroke(cr);
}


