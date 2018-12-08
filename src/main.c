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
	const char			*headers[] = {"ID", "Предмет"};
	GtkListStore		*store_subject_list;
	GtkWidget			*tree_view;
	GtkTreeViewColumn	*column;
	GtkCellRenderer		*render;

	store_subject_list = gtk_list_store_new(2, G_TYPE_STRING, G_TYPE_STRING);
	tree_view = gtk_tree_view_new();
	gtk_tree_view_set_grid_lines(GTK_TREE_VIEW(tree_view),
		GTK_TREE_VIEW_GRID_LINES_BOTH);
	gtk_tree_view_set_model(GTK_TREE_VIEW(tree_view),
		GTK_TREE_MODEL(store_subject_list));
	for (int i = 0; i < sizeof headers / sizeof headers[0]; i++)
	{
		render = gtk_cell_renderer_text_new();
		column = gtk_tree_view_column_new_with_attributes(headers[i],
			render, "text", i, NULL);
		gtk_tree_view_column_set_resizable(column, TRUE);
		gtk_tree_view_column_set_min_width(column, 100);
		gtk_tree_view_append_column(GTK_TREE_VIEW(tree_view), column);
	}

	return tree_view;
}
