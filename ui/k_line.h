#pragma once
#ifndef K_LINE_H_
#define K_LINE_H_

#include <gtk/gtk.h>

typedef struct _k_line k_line;
typedef struct _k_line_class k_line_class;

#define K_LINE_TYPE (k_line_get_type())
#define K_LINE(obj)                  (G_TYPE_CHECK_INSTANCE_CAST ((obj), K_LINE_TYPE, k_line))

enum {
	PROP_0, PRICE_INFO, P_LENGTH
};

struct _k_line {
	GtkMisc button;

	float high;
	float low;
	float open, close;
};

struct _k_line_class {
	GtkMiscClass parent_class;
};

GtkWidget* k_line_new();

GtkWidget* k_line_new_with_price_info(GVariant* price_info);

static void k_line_init(k_line* _k_line_p);

static void k_line_class_init(k_line_class* _k_line_class_p);

static GType k_line_get_type();

#endif