#include <gtk/gtk.h>

#define	MAIN_WINDOW_TITLE	"Книжка оценок"

static GtkWidget *create_main_window(void);
static GtkWidget *create_subject_list_page(void);
static GtkWidget *create_pupil_list_page(void);
static GtkWidget *create_text_view_subject_list(void);

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
	GtkWidget	*notebook;

	window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title(GTK_WINDOW(window), MAIN_WINDOW_TITLE);

	notebook = gtk_notebook_new();
	gtk_notebook_append_page(GTK_NOTEBOOK(notebook),
		create_subject_list_page(),
		gtk_label_new("Список предметов"));

	gtk_notebook_append_page(GTK_NOTEBOOK(notebook),
		create_pupil_list_page(),
		gtk_label_new("Список учеников"));

	gtk_container_add(GTK_CONTAINER(window), notebook);

	return window;
}

static GtkWidget *create_subject_list_page(void)
{
	GtkWidget	*text_view_subject_list;

	text_view_subject_list = create_text_view_subject_list();
	return text_view_subject_list;
}

static GtkWidget *create_pupil_list_page(void)
{
	return gtk_label_new("Вкладка с учениками");
}

static GtkWidget *create_text_view_subject_list(void)
{
	return gtk_text_view_new();
}
