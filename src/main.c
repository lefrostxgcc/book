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
	GtkWidget	*frame;
	GtkWidget	*frame_tree_view;
	GtkWidget	*frame_controls;
	GtkWidget	*hbox;
	GtkWidget	*vbox;

	frame_tree_view = gtk_frame_new(NULL);
	frame_controls = gtk_frame_new(NULL);

	gtk_container_set_border_width(GTK_CONTAINER(frame_tree_view), 5);
	gtk_container_set_border_width(GTK_CONTAINER(frame_controls), 5);
	
	hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
	vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);

	gtk_container_set_border_width(GTK_CONTAINER(vbox), 5);

	gtk_box_pack_start(GTK_BOX(vbox), gtk_label_new("Название предмета:"),
		FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(vbox), gtk_entry_new(), FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(vbox), gtk_button_new_with_label("Сохранить"),
		FALSE, FALSE, 0);

	gtk_container_add(GTK_CONTAINER(frame_tree_view),
		create_text_view_subject_list());
	gtk_container_add(GTK_CONTAINER(frame_controls), vbox);

	gtk_box_pack_start(GTK_BOX(hbox), frame_tree_view, TRUE, TRUE, 0);
	gtk_box_pack_start(GTK_BOX(hbox), frame_controls, FALSE, FALSE, 0);

	return hbox;
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
