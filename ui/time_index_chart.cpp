#include "time_index_chart.h"
#include <iostream>

static gboolean time_index_chart_draw(GtkWidget* widget, cairo_t* cr);

static void gtk_time_index_chart_get_preferred_width(GtkWidget* widget, gint* minimum_width, gint* natural_width);
static void gtk_time_index_chart_get_preferred_height(GtkWidget* widget, gint* minimum_height, gint* natural_height);

static void time_index_chart_set_property(GObject* object, guint property_id, const GValue* value, GParamSpec* pspec);
static void time_index_chart_get_property(GObject* object, guint property_id, GValue* value, GParamSpec* pspec);

static void time_index_chart_finalize();

static GType time_index_chart_get_type() {
	static GType time_index_chart_type = 0;
	if (!time_index_chart_type) {
		static const GTypeInfo time_index_chart_info = {
			sizeof(time_index_chart_class),
			NULL,NULL,
			(GClassInitFunc)time_index_chart_class_init,
			NULL,NULL,
			sizeof(time_index_chart),
			0,
			(GInstanceInitFunc)time_index_chart_init
		};
		time_index_chart_type = g_type_register_static(GTK_TYPE_MISC, "time_index_chart", &time_index_chart_info, static_cast<GTypeFlags>(0));
	}
	return time_index_chart_type;
}

void time_index_chart_init(time_index_chart* _time_index_chart) {
	_time_index_chart->time_delta = 1000;
	_time_index_chart->price = new std::vector<float>();
	_time_index_chart->price->push_back(20);
	_time_index_chart->price->push_back(10);
	_time_index_chart->price->push_back(25);
	_time_index_chart->price->push_back(46);
	/*for (int i = 3; i < 100; i++) {
		_time_index_chart->price->push_back(5 + i);
	}*/
}

GtkWidget* time_index_chart_new() {
	return GTK_WIDGET(g_object_new(TIME_INDEX_CHART_TYPE, NULL));
}

void time_index_chart_class_init(time_index_chart_class* _k_line_class_p) {
	GObjectClass* gobject_class;
	GtkWidgetClass* widget_class;

	gobject_class = G_OBJECT_CLASS(_k_line_class_p);
	widget_class = (GtkWidgetClass*)_k_line_class_p;
	widget_class->draw = time_index_chart_draw;
	/*widget_class->size_allocate = gtk_entry_size_allocate;*/
	widget_class->get_preferred_width = gtk_time_index_chart_get_preferred_width;
	widget_class->get_preferred_height = gtk_time_index_chart_get_preferred_height;
	gobject_class->set_property = time_index_chart_set_property;
	gobject_class->get_property = time_index_chart_get_property;

	/*obj_properties[PRICE_INFO] =
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
		obj_properties);*/
}

void gtk_time_index_chart_get_preferred_width(GtkWidget* widget, gint* minimum_width, gint* natural_width) {
	*minimum_width = 200;
}

void gtk_time_index_chart_get_preferred_height(GtkWidget* widget, gint* minimum_height, gint* natural_height) {
	*minimum_height = 200;
}

void time_index_chart_set_property(GObject* object, guint property_id, const GValue* value, GParamSpec* pspec) {

}

void time_index_chart_get_property(GObject* object, guint property_id, GValue* value, GParamSpec* pspec) {

}

gboolean time_index_chart_draw(GtkWidget* widget, cairo_t* cr) {
	time_index_chart* chart = TIME_INDEX_CHART(widget);

	GtkAllocation allocation{};
	gtk_widget_get_clip(widget, &allocation);

	int partation = 4 * 60 * 60 / chart->time_delta;
	int width_delta = allocation.width / partation;

	cairo_set_line_width(cr, 3);
	float max_value = 0, min_value = 1000000;
	float val = 0;
	for (int i = 0; i < chart->price->size();i++) {
		val = chart->price->at(i);
		if (max_value < val) {
			max_value = val;
		}

		min_value = min_value > val ? val : min_value;
	}

	int x = 0, y = 0, pre_x = -1, pre_y = -1;
	double pre_calcualte = (max_value - min_value) / static_cast<double>(allocation.height);
	std::cout << "start drawing ..." << allocation.width << " " << allocation.height << std::endl;
	for (int i = 0; i < chart->price->size(); i++) {
		// 根据价格计算坐标
		int x = i * width_delta;
		int y = (max_value - chart->price->at(i)) / pre_calcualte;
		if (pre_x != -1) {
			cairo_move_to(cr, pre_x, pre_y);
		}
		else {
			cairo_move_to(cr, x, y);
		}
		cairo_line_to(cr, x, y);
		pre_x = x;                                         
		pre_y = y;
	}
	cairo_stroke(cr);

	return FALSE;
}
