#include "stock_app.h"
#include "stock_app_window.h"

struct _StockAppClass
{
	GtkApplicationClass parent_class;
};

struct StockAppPrivate {};

G_DEFINE_TYPE_WITH_PRIVATE (StockApp, stock_app, GTK_TYPE_APPLICATION)

static void
stock_app_dispose (GObject *object)
{

	G_OBJECT_CLASS (stock_app_parent_class)->dispose (object);
}

static void
quit_activated(GSimpleAction* action, GVariant* parameter, gpointer app) {
	g_application_quit(G_APPLICATION(app));
}

static GActionEntry app_entries[] = {
  { "quit", quit_activated, NULL, NULL, NULL }
};

static void stock_app_startup(GApplication* app) {
	G_APPLICATION_CLASS(stock_app_parent_class)->startup(app);
	g_action_map_add_action_entries(G_ACTION_MAP(app), app_entries, G_N_ELEMENTS(app_entries), app);
	const gchar* quit_accels[2] = { "<Ctrl>Q", NULL };
	gtk_application_set_accels_for_action(GTK_APPLICATION(app),
		"app.quit",
		quit_accels);
}

static void stock_app_activate(GApplication *app)
{
  StockAppWindow *win;

  win = stock_app_window_new (STOCK_APP (app));
  //gtk_window_present (GTK_WINDOW (win));
}

static void stock_app_class_init(StockAppClass* stock_class) {
	GObjectClass *object_class = G_OBJECT_CLASS (stock_class);
	GApplicationClass *app_class = G_APPLICATION_CLASS (stock_class);

	app_class->startup = stock_app_startup;
	app_class->activate = stock_app_activate;
	object_class->dispose = stock_app_dispose;
}

void stock_app_init(StockApp* app) {}

StockApp* stock_app_new() {
	return STOCK_APP(g_object_new(STOCK_TYPE_APP, "application-id", "com.gtk.stockapp", "flags", G_APPLICATION_HANDLES_OPEN, NULL));
}