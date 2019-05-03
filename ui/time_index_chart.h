#ifndef TIMELINE_H_
#define TIMELINE_H_

#include <gtk/gtk.h>
#include <vector>

typedef struct _time_index_chart time_index_chart;
typedef struct _time_index_chart_class time_index_chart_class;

enum {
	TIME_INDEX_PROP_0
};

#define TIME_INDEX_CHART_TYPE (time_index_chart_get_type ())
#define TIME_INDEX_CHART(obj)                  (G_TYPE_CHECK_INSTANCE_CAST ((obj), TIME_INDEX_CHART_TYPE, time_index_chart))

struct _time_index_chart {
	GtkMisc button;
	std::vector<float>* price;
	int time_delta;
};

struct _time_index_chart_class {
	GtkMiscClass parent_class;
};

GtkWidget* time_index_chart_new();

static void time_index_chart_init(time_index_chart* _k_line_p);

static void time_index_chart_class_init(time_index_chart_class* _k_line_class_p);

#endif