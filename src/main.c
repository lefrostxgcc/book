#include <gtk/gtk.h>

#define	MAIN_WINDOW_TITLE	"Книжка оценок"

static GtkWidget *create_main_window(void);

int main(int argc, char *argv[])
{
	GtkWidget	*window;

	gtk_init(&argc, &argv);

	window = create_main_window();

	g_signal_connect(G_OBJECT(window), "destroy",
		G_CALLBACK(gtk_main_quit), NULL);

	gtk_widget_show_all(window);
	gtk_main();
}

static GtkWidget *create_main_window(void)
{
	GtkWidget	*window;

	window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title(GTK_WINDOW(window), MAIN_WINDOW_TITLE);

	return window;
}
