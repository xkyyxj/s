#include "k_line.h"
#include <iostream>

static GParamSpec* obj_properties[P_LENGTH] = { NULL, };

static gboolean k_line_draw(GtkWidget* widget, cairo_t* cr);
//static void gtk_entry_size_allocate(GtkWidget* widget, GtkAllocation* allocation);
//static void gtk_k_line_get_preferred_width(GtkWidget* widget, gint* minimum_width, gint* natural_width);
//static void gtk_k_line_get_preferred_height(GtkWidget* widget, gint* minimum_height, gint* natural_height);

static void k_line_set_property(GObject* object, guint property_id, const GValue* value, GParamSpec* pspec);
static void k_line_get_property(GObject* object, guint property_id, GValue* value, GParamSpec* pspec);

static void draw_single_k_line(k_line* _k_line, cairo_t* t, int start_x, int start_y);

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
	g_signal_connect(GTK_WIDGET(_k_line_p), "motion-notify-event", G_CALLBACK(print_hello), NULL);
	//g_signal_connect(GTK_WIDGET(_k_line_p), "realize", G_CALLBACK(print_hello_realize), NULL);
}

static void k_line_class_init(k_line_class* _k_line_class_p) {
	GObjectClass* gobject_class;
	GtkWidgetClass* widget_class;

	gobject_class = G_OBJECT_CLASS(_k_line_class_p);
	widget_class = (GtkWidgetClass*)_k_line_class_p;
	widget_class->draw = k_line_draw;
	/*widget_class->size_allocate = gtk_entry_size_allocate;
	widget_class->get_preferred_width = gtk_k_line_get_preferred_width;
	widget_class->get_preferred_height = gtk_k_line_get_preferred_height;*/
	gobject_class->set_property = k_line_set_property;
	gobject_class->get_property = k_line_get_property;

	obj_properties[PRICE_INFO] =
		g_param_spec_variant("price-info",
			"Price Info",
			"Price Information of this k line",
			G_VARIANT_TYPE_DICTIONARY,
			NULL,
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
	//int width_val = gtk_widget_get_allocated_width(widget);
	//int height_val = gtk_widget_get_allocated_height(widget);
	//GtkAllocation val_allocation{};
	//gtk_widget_get_allocation(widget, &val_allocation);

	//GtkAllocation val_clip{};
	//gtk_widget_get_clip(widget, &val_clip);
	//std::cout << "x is " << val_allocation.x << "y is " << val_allocation.y << std::endl;
	//std::cout << "width is " << width_val << "height is " << height_val << std::endl;

	//// ����һ��K�ߵĿ�Ⱥ͸߶��Լ�����λ��
	//k_line* _k_line = K_LINE(widget);

	////����ʵ�岿�ֵ�ռ��
	//float close_open_delta_abs = _k_line->close > _k_line->open ? _k_line->close - _k_line->open : _k_line->open - _k_line->close;
	//float main_content_p = close_open_delta_abs / (_k_line->high - _k_line->low);

	////������Ӱ��ռ��
	//float up_line_delta_abs = _k_line->high - (_k_line->open > _k_line->close ? _k_line->open : _k_line->close);
	//float up_line_p = up_line_delta_abs / (_k_line->high - _k_line->low);

	////������Ӱ��ռ��
	//float down_line_p = main_content_p + up_line_p;

	//double start_x = 0;
	//double start_y = val_clip.height * up_line_p;
	//double width = val_clip.width;
	//double height = val_clip.height * main_content_p;

	//double up_line_start_x = (val_clip.width / 2);
	//double up_line_start_y = 0;
	//double up_line_end_x = (val_clip.width / 2);
	//double up_line_end_y = val_clip.height * up_line_p;

	//double down_line_start_y = val_clip.height * down_line_p;

	//cairo_set_source_rgb(cr, 0.36078, 0.733333, 0.46274);
	//cairo_set_line_width(cr, 3);

	//// ����K��ʵ�岿��
	//cairo_rectangle(cr, start_x, start_y, width, height);
	//cairo_fill_preserve(cr);
	//cairo_stroke(cr);

	//std::cout << "up line " << up_line_start_x << " " << up_line_start_y << " "
	//	<< " " << up_line_end_x << " " << up_line_end_y << std::endl;

	//// ����K����Ӱ��
	//cairo_move_to(cr, up_line_start_x, up_line_start_y);
	//cairo_line_to(cr, up_line_end_x, up_line_end_y);
	//cairo_stroke(cr);

	//// ����K����Ӱ��
	//cairo_move_to(cr, up_line_start_x, down_line_start_y);
	//cairo_line_to(cr, up_line_end_x, val_clip.height);
	//cairo_stroke(cr);

	GtkAllocation val_clip{};
	gtk_widget_get_clip(widget, &val_clip);

	// ����һ��K�ߵĿ�Ⱥ͸߶��Լ�����λ��
	k_line* _k_line = K_LINE(widget);

	//����ʵ�岿�ֵ�ռ��
	float close_open_delta_abs = _k_line->close > _k_line->open ? _k_line->close - _k_line->open : _k_line->open - _k_line->close;
	float main_content_p = close_open_delta_abs / (_k_line->high - _k_line->low);

	//������Ӱ��ռ��
	float up_line_delta_abs = _k_line->high - (_k_line->open > _k_line->close ? _k_line->open : _k_line->close);
	float up_line_p = up_line_delta_abs / (_k_line->high - _k_line->low);

	//������Ӱ��ռ��
	float down_line_p = main_content_p + up_line_p;

	double start_x = 0;
	double start_y = 0 + val_clip.height * up_line_p;
	double width = val_clip.width;
	double height = val_clip.height * main_content_p;

	double up_line_start_x = val_clip.x + (val_clip.width / 2);
	double up_line_start_y = val_clip.y;
	double up_line_end_x = val_clip.x + (val_clip.width / 2);
	double up_line_end_y = val_clip.y + val_clip.height * up_line_p;

	double down_line_start_y = val_clip.y + val_clip.height * down_line_p;

	if (_k_line->open > _k_line->close) {
		cairo_set_source_rgb(cr, 0.36078, 0.733333, 0.46274);
	}
	else {
		cairo_set_source_rgb(cr, 1, 0.1764, 0.1686);
	}
	cairo_set_line_width(cr, 3);

	// ����K��ʵ�岿��
	cairo_rectangle(cr, start_x, start_y, width, height);
	if (_k_line->open > _k_line->close) {
		cairo_fill_preserve(cr);
	}
	cairo_stroke(cr);
	std::cout << "x: " << val_clip.x << " y: " << val_clip.y << " width: " << val_clip.width
		<< " height: " << val_clip.height << std::endl;

	std::cout << "open:" << _k_line->open << " close:" << _k_line->close << " high:" << _k_line->high << " low:" << _k_line->low << std::endl;

	// ����K����Ӱ��
	cairo_move_to(cr, up_line_start_x, up_line_start_y);
	cairo_line_to(cr, up_line_end_x, up_line_end_y);
	cairo_stroke(cr);

	// ����K����Ӱ��
	cairo_move_to(cr, up_line_start_x, down_line_start_y);
	cairo_line_to(cr, up_line_end_x, val_clip.height);
	cairo_stroke(cr);
	

	// ͳ����Ϣ
	std::cout << "width is:" << width << " height is :" << height << std::endl;

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

//static void gtk_k_line_get_preferred_width(GtkWidget* widget, gint* minimum_width, gint* natural_width) {
//	(*minimum_width) = 200;
//	(*natural_width) = 200;
//}
//
//static void gtk_k_line_get_preferred_height(GtkWidget* widget, gint* minimum_height, gint* natural_height) {
//	(*minimum_height) = 200;
//	(*natural_height) = 200;
//}
//}

static void k_line_set_property(GObject* object, guint property_id, const GValue* value, GParamSpec* pspec) {
	std::cout << "In set Property" << std::endl;
	GVariant* temp_price = 0;
	GVariant* variant_val = nullptr;
	k_line* _k_line = nullptr;
	gdouble price = 0;
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

	default:
		/* We don't have any other property... */
		G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
		break;
	}

	// ����һ�����»��Ʋ���
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

static void draw_single_k_line(k_line* _k_line, cairo_t* t, int start_x, int start_y) {
	
}


