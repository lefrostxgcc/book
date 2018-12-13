#include <gtk/gtk.h>
#include <config.h>
#include <ch_sqlite.h>

#define		MAIN_WINDOW_TITLE	"Книжка оценок"
#define		DATABASE_FILENAME	(DATA_PATH "/" DATABASE_NAME)

enum msgbox_responce { RESPONCE_ABORT, RESPONCE_RETRY, RESPONCE_IGNORE };
enum {WIN_WIDTH = 600, WIN_HEIGHT = 400};

static GtkWidget		*create_main_window(void);
static GtkWidget		*create_login_page(void);
static GtkWidget		*create_subject_list_page(void);
static GtkWidget		*create_pupil_list_page(void);
static GtkWidget		*create_text_view_subject_list(void);
static void				show_message_box(const char *message);
static enum msgbox_responce	show_error_message_box(const char *message);
static void
on_button_add_subject_clicked(GtkWidget *button, gpointer data);
static void
on_button_save_subject_clicked(GtkWidget *button, gpointer data);
static void
on_button_delete_subject_clicked(GtkWidget *button, gpointer data);
static void on_button_pupil_login_clicked(GtkWidget *button, gpointer data);
static void on_button_teacher_login_clicked(GtkWidget *button, gpointer data);
static int db_error(struct ch_sqlite_connection *connection);
static void load_subject_table(GtkListStore	*store);
static void load_pupil_store(GtkListStore *store);
static void load_teacher_login(void);
static void login_pupil(int id);
static void login_teacher(void);
static gboolean check_pupil_login(int id, const gchar *password);
static int is_selected_subject_list_row(void);
static gboolean check_teacher_login(int id, const gchar *password);
static void on_tree_view_subject_list_row_activated(GtkTreeView *tree_view,
	GtkTreePath *path, GtkTreeViewColumn *column, gpointer user_data);
static int add_subject_to_store_cb(void *opt_arg, int col_count,
	char **cols, char **col_names);
static int add_pupil_to_store_cb(void *store, int col_count,
	char **cols, char **col_names);

static GtkListStore		*store_subject_list;
static GtkListStore		*store_pupil;
static GtkWidget		*window;
static GtkWidget		*notebook;
static GtkWidget		*tree_view_subject_list;
static GtkWidget		*combo_box_pupil;
static char				*teacher_login;
static int				selected_subject_id;

int main(int argc, char *argv[])
{
	gtk_init(&argc, &argv);

	store_pupil = gtk_list_store_new(2, G_TYPE_STRING, G_TYPE_STRING);
	store_subject_list = gtk_list_store_new(2, G_TYPE_STRING, G_TYPE_STRING);

	load_pupil_store(store_pupil);
	load_teacher_login();

	window = create_main_window();

	g_signal_connect(G_OBJECT(window), "destroy",
		G_CALLBACK(gtk_main_quit), NULL);

	gtk_widget_show_all(window);
	gtk_main();

	g_free(teacher_login);
	g_object_unref(store_pupil);
	g_object_unref(store_subject_list);
}

static GtkWidget *create_main_window(void)
{
	GtkWidget	*win;

	win = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title(GTK_WINDOW(win), MAIN_WINDOW_TITLE);
	gtk_window_set_default_size(GTK_WINDOW(win), WIN_WIDTH, WIN_HEIGHT);

	notebook = gtk_notebook_new();
	gtk_notebook_append_page(GTK_NOTEBOOK(notebook),
		create_login_page(),
		gtk_label_new("Вход в книжку оценок"));
	gtk_notebook_append_page(GTK_NOTEBOOK(notebook),
		create_subject_list_page(),
		gtk_label_new("Список предметов"));
	gtk_notebook_append_page(GTK_NOTEBOOK(notebook),
		create_pupil_list_page(),
		gtk_label_new("Список учеников"));

	gtk_container_add(GTK_CONTAINER(win), notebook);

	return win;
}

static GtkWidget *create_login_page(void)
{
	GtkWidget		*grid_login;
	GtkWidget		*label_space;
	GtkWidget		*label_pupil;
	GtkWidget		*label_teacher;
	GtkWidget		*entry_pupil_password;
	GtkWidget		*entry_teacher_login;
	GtkWidget		*entry_teacher_password;
	GtkWidget		*button_pupil_login;
	GtkWidget		*button_teacher_login;
	GtkCellRenderer	*render;

	label_space = gtk_label_new(NULL);
	label_pupil = gtk_label_new("Ученик");
	label_teacher = gtk_label_new("Учитель");
	entry_pupil_password = gtk_entry_new();
	entry_teacher_login = gtk_entry_new();
	entry_teacher_password = gtk_entry_new();
	button_pupil_login = gtk_button_new_with_label("Вход");
	button_teacher_login = gtk_button_new_with_label("Вход");

	gtk_entry_set_text(GTK_ENTRY(entry_teacher_login), teacher_login);
	gtk_entry_set_alignment(GTK_ENTRY(entry_teacher_login), 0.5);
	gtk_widget_set_sensitive(entry_teacher_login, FALSE);

	gtk_entry_set_visibility(GTK_ENTRY(entry_pupil_password), FALSE);
	gtk_entry_set_visibility(GTK_ENTRY(entry_teacher_password), FALSE);

	combo_box_pupil = gtk_combo_box_new_with_model(GTK_TREE_MODEL(store_pupil));

	render = gtk_cell_renderer_text_new();
	gtk_cell_layout_pack_start(GTK_CELL_LAYOUT(combo_box_pupil), render, TRUE);
	gtk_cell_layout_set_attributes(GTK_CELL_LAYOUT(combo_box_pupil), render,
									"text", 1,
									NULL);

	gtk_combo_box_set_active(GTK_COMBO_BOX(combo_box_pupil), 0);

	grid_login = gtk_grid_new();
	gtk_container_set_border_width(GTK_CONTAINER(grid_login), 10);
	gtk_widget_set_halign(grid_login, GTK_ALIGN_CENTER);
	gtk_widget_set_valign(grid_login, GTK_ALIGN_CENTER);
	gtk_grid_set_row_spacing(GTK_GRID(grid_login), 10);
	gtk_grid_set_column_spacing(GTK_GRID(grid_login), 10);
	gtk_grid_set_row_homogeneous(GTK_GRID(grid_login), FALSE);
	gtk_grid_set_column_homogeneous(GTK_GRID(grid_login), FALSE);

	gtk_grid_attach(GTK_GRID(grid_login), label_pupil, 0, 0, 1, 1);
	gtk_grid_attach(GTK_GRID(grid_login), combo_box_pupil, 1, 0, 1, 1);
	gtk_grid_attach(GTK_GRID(grid_login), entry_pupil_password, 1, 1, 1, 1);
	gtk_grid_attach(GTK_GRID(grid_login), button_pupil_login, 2, 0, 1, 2);
	gtk_grid_attach(GTK_GRID(grid_login), label_space, 0, 2, 3, 1);
	gtk_grid_attach(GTK_GRID(grid_login), label_teacher, 0, 3, 1, 1);
	gtk_grid_attach(GTK_GRID(grid_login), entry_teacher_login, 1, 3, 1, 1);
	gtk_grid_attach(GTK_GRID(grid_login), entry_teacher_password, 1, 4, 1, 1);
	gtk_grid_attach(GTK_GRID(grid_login), button_teacher_login, 2, 3, 1, 2);

	g_signal_connect(G_OBJECT(button_pupil_login), "clicked",
						G_CALLBACK(on_button_pupil_login_clicked),
						(gpointer)entry_pupil_password);
	g_signal_connect(G_OBJECT(button_teacher_login), "clicked",
						G_CALLBACK(on_button_teacher_login_clicked),
						(gpointer)entry_teacher_password);

	return grid_login;
}

static GtkWidget *create_subject_list_page(void)
{
	GtkWidget	*frame;
	GtkWidget	*frame_tree_view;
	GtkWidget	*frame_controls;
	GtkWidget	*hbox;
	GtkWidget	*vbox;
	GtkWidget	*entry_subject;
	GtkWidget	*button_add_subject;
	GtkWidget	*button_save_subject;
	GtkWidget	*button_delete_subject;

	tree_view_subject_list = create_text_view_subject_list();
	entry_subject = gtk_entry_new();
	button_add_subject = gtk_button_new_with_label("Добавить");
	button_save_subject = gtk_button_new_with_label("Сохранить");
	button_delete_subject = gtk_button_new_with_label("Удалить");

	frame_tree_view = gtk_frame_new(NULL);
	frame_controls = gtk_frame_new(NULL);

	gtk_container_set_border_width(GTK_CONTAINER(frame_tree_view), 5);
	gtk_container_set_border_width(GTK_CONTAINER(frame_controls), 5);
	
	hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
	vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);

	gtk_container_set_border_width(GTK_CONTAINER(vbox), 5);

	gtk_box_pack_start(GTK_BOX(vbox), gtk_label_new("Название предмета:"),
		FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(vbox), entry_subject, FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(vbox), button_add_subject,
		FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(vbox), button_save_subject,
		FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(vbox), button_delete_subject,
		FALSE, FALSE, 0);

	gtk_container_add(GTK_CONTAINER(frame_tree_view),
		tree_view_subject_list);
	gtk_container_add(GTK_CONTAINER(frame_controls), vbox);

	gtk_box_pack_start(GTK_BOX(hbox), frame_tree_view, TRUE, TRUE, 0);
	gtk_box_pack_start(GTK_BOX(hbox), frame_controls, FALSE, FALSE, 0);

	g_signal_connect(G_OBJECT(button_add_subject), "clicked",
						G_CALLBACK(on_button_add_subject_clicked),
						(gpointer)entry_subject);
	g_signal_connect(G_OBJECT(button_save_subject), "clicked",
						G_CALLBACK(on_button_save_subject_clicked),
						(gpointer)entry_subject);
	g_signal_connect(G_OBJECT(button_delete_subject), "clicked",
						G_CALLBACK(on_button_delete_subject_clicked),
						(gpointer)entry_subject);
	g_signal_connect(G_OBJECT(tree_view_subject_list), "row-activated",
						G_CALLBACK(on_tree_view_subject_list_row_activated),
						(gpointer)entry_subject);

	return hbox;
}

static GtkWidget *create_pupil_list_page(void)
{
	return gtk_label_new("Вкладка с учениками");
}

static GtkWidget *create_text_view_subject_list(void)
{
	const char			*headers[] = {"ID", "Предмет"};
	GtkWidget			*tree_view;
	GtkTreeViewColumn	*column;
	GtkCellRenderer		*render;
	GtkTreeSelection	*selection;

	load_subject_table(store_subject_list);
	tree_view = gtk_tree_view_new();
	gtk_tree_view_set_model(GTK_TREE_VIEW(tree_view),
		GTK_TREE_MODEL(store_subject_list));
	gtk_tree_view_set_grid_lines(GTK_TREE_VIEW(tree_view),
		GTK_TREE_VIEW_GRID_LINES_BOTH);
	g_object_set(tree_view, "activate-on-single-click", TRUE, NULL);
	selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(tree_view));
	gtk_tree_selection_set_mode(selection, GTK_SELECTION_BROWSE);

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

static void
on_button_add_subject_clicked(GtkWidget *button, gpointer data)
{
	char						result[20];
	char						*query;
	struct ch_sqlite_connection *connection;
	int							max_subject_id;

	if (gtk_entry_get_text_length(GTK_ENTRY(data)) == 0)
		return;

	do ch_sqlite_open(DATABASE_FILENAME, &connection);
	while (db_error(connection));

	query = "SELECT MAX(id) FROM subject";

	do ch_sqlite_scalar(connection, query, result, sizeof result);
	while (db_error(connection));

	max_subject_id = (int) g_ascii_strtoll(result, NULL, 10);

	query = g_strdup_printf(
		"INSERT INTO subject (id, subject) VALUES ('%d', '%s')",
		max_subject_id + 1, gtk_entry_get_text(GTK_ENTRY(data)));

	do ch_sqlite_exec(connection, query, NULL, NULL);
	while (db_error(connection));

	g_free(query);

	do ch_sqlite_close(&connection);
	while (db_error(connection));

	load_subject_table(store_subject_list);
}

static void
on_button_save_subject_clicked(GtkWidget *button, gpointer data)
{
	char						*query;
	struct ch_sqlite_connection *connection;

	if (!is_selected_subject_list_row())
		return;

	if (selected_subject_id < 0)
		return;

	do ch_sqlite_open(DATABASE_FILENAME, &connection);
	while (db_error(connection));

	query = g_strdup_printf(
		"UPDATE subject SET subject = '%s' WHERE id = '%d'",
		gtk_entry_get_text(GTK_ENTRY(data)), selected_subject_id);

	do ch_sqlite_exec(connection, query, NULL, NULL);
	while (db_error(connection));

	g_free(query);

	do ch_sqlite_close(&connection);
	while (db_error(connection));

	load_subject_table(store_subject_list);
}

static void
on_button_delete_subject_clicked(GtkWidget *button, gpointer data)
{
	char						*query;
	struct ch_sqlite_connection *connection;

	if (!is_selected_subject_list_row())
		return;

	if (selected_subject_id < 0)
		return;

	do ch_sqlite_open(DATABASE_FILENAME, &connection);
	while (db_error(connection));

	query = g_strdup_printf(
		"DELETE FROM subject WHERE id = %d", selected_subject_id);

	do ch_sqlite_exec(connection, query, NULL, NULL);
	while (db_error(connection));

	g_free(query);

	do ch_sqlite_close(&connection);
	while (db_error(connection));

	load_subject_table(store_subject_list);
}

static void on_button_pupil_login_clicked(GtkWidget *button, gpointer data)
{
	GtkTreeIter		iter;
	const gchar		*id_str;
	const gchar		*pupil_name;
	int				id;

	if (!gtk_combo_box_get_active_iter(GTK_COMBO_BOX(combo_box_pupil), &iter))
		return;

	gtk_tree_model_get(GTK_TREE_MODEL(store_pupil), &iter, 0, &id_str,
		1, &pupil_name, -1);
	id = (int) g_ascii_strtoll(id_str, NULL, 10);
	if (check_pupil_login(id, gtk_entry_get_text(GTK_ENTRY(data))))
		login_pupil(id);
	else
		show_message_box("Неверный логин или пароль пользователя");
}

static void on_button_teacher_login_clicked(GtkWidget *button, gpointer data)
{
	if (check_teacher_login(1, gtk_entry_get_text(GTK_ENTRY(data))))
		login_teacher();
	else
		show_message_box("Неверный логин или пароль пользователя");
}

static void load_subject_table(GtkListStore	*store)
{
	struct ch_sqlite_connection *connection;
	const char					*query;

	do ch_sqlite_open(DATABASE_FILENAME, &connection);
	while (db_error(connection));

	gtk_list_store_clear(store);
	query = "SELECT id, subject FROM subject ORDER BY subject";
	do ch_sqlite_exec(connection, query, add_subject_to_store_cb, store);
	while (db_error(connection));

	do ch_sqlite_close(&connection);
	while (db_error(connection));

	selected_subject_id = -1;
}

static void load_pupil_store(GtkListStore *store)
{
	struct ch_sqlite_connection *connection;
	const char					*query;

	do ch_sqlite_open(DATABASE_FILENAME, &connection);
	while (db_error(connection));

	gtk_list_store_clear(store);
	query = "SELECT id, pupil FROM pupil ORDER BY pupil";
	do ch_sqlite_exec(connection, query, add_pupil_to_store_cb, store);
	while (db_error(connection));

	do ch_sqlite_close(&connection);
	while (db_error(connection));
}

static void load_teacher_login(void)
{
	char						result[20];
	struct ch_sqlite_connection *connection;
	const char					*query;

	do ch_sqlite_open(DATABASE_FILENAME, &connection);
	while (db_error(connection));

	query = "SELECT teacher FROM teacher WHERE id = 1";
	do ch_sqlite_scalar(connection, query, result, sizeof result);
	while (db_error(connection));

	g_free(teacher_login);
	teacher_login = g_strdup(result);

	do ch_sqlite_close(&connection);
	while (db_error(connection));
}

static gboolean check_pupil_login(int id, const gchar *password)
{
	char						result[20];
	struct ch_sqlite_connection *connection;
	char						*query;
	int							match_count;

	do ch_sqlite_open(DATABASE_FILENAME, &connection);
	while (db_error(connection));

	query = g_strdup_printf(
		"SELECT COUNT(*) FROM pupil WHERE id = '%d' AND password = '%s';",
		id, password);

	do ch_sqlite_scalar(connection, query, result, sizeof result);
	while (db_error(connection));

	g_free(query);

	match_count = (int) g_ascii_strtoll(result, NULL, 10);

	do ch_sqlite_close(&connection);
	while (db_error(connection));

	return match_count != 0;
}

static gboolean check_teacher_login(int id, const gchar *password)
{
	char						result[20];
	struct ch_sqlite_connection *connection;
	char						*query;
	int							match_count;

	do ch_sqlite_open(DATABASE_FILENAME, &connection);
	while (db_error(connection));

	query = g_strdup_printf(
		"SELECT COUNT(*) FROM teacher WHERE id = '%d' AND password = '%s';",
		id, password);

	do ch_sqlite_scalar(connection, query, result, sizeof result);
	while (db_error(connection));

	g_free(query);

	match_count = (int) g_ascii_strtoll(result, NULL, 10);

	do ch_sqlite_close(&connection);
	while (db_error(connection));

	return match_count != 0;
}

static void login_pupil(int id)
{
	show_message_box("Вход выполнен успешно!");
}

static void login_teacher(void)
{
	gtk_notebook_set_current_page(GTK_NOTEBOOK(notebook), 1);
}

static int db_error(struct ch_sqlite_connection *connection)
{
	char					*message;
	enum msgbox_responce	responce;

	if (connection == NULL || ch_sqlite_last_error(connection) == NULL)
		return 0;
	message = g_strdup_printf("%s\n%s",
		ch_sqlite_last_error(connection), ch_sqlite_last_query(connection));
	responce = show_error_message_box(message);
	g_free(message);
	switch (responce)
	{
		case GTK_RESPONSE_DELETE_EVENT:
		case RESPONCE_ABORT: exit(1); return 0;
		case RESPONCE_RETRY: return 1;
		case RESPONCE_IGNORE: return 0;
	}
	return 0;
}

static enum msgbox_responce show_error_message_box(const char *message)
{
	GtkWidget				*dialog;
	GtkWidget				*content_area;
	GtkWidget				*vbox;
	GtkWidget				*label_message;
	enum msgbox_responce	responce;

	dialog = gtk_dialog_new_with_buttons("Ошибка базы данных",
		GTK_WINDOW(window), GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
		"_Abort", RESPONCE_ABORT,
		"_Retry", RESPONCE_RETRY,
		"_Ignore", RESPONCE_IGNORE,
		NULL);

	gtk_window_set_resizable(GTK_WINDOW(dialog), FALSE);
	content_area = gtk_dialog_get_content_area(GTK_DIALOG(dialog));
	vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
	label_message = gtk_label_new(message);

	gtk_container_set_border_width(GTK_CONTAINER(vbox), 20);
	gtk_container_set_border_width(GTK_CONTAINER(content_area), 5);
	gtk_container_add(GTK_CONTAINER(vbox), label_message);
	gtk_container_add(GTK_CONTAINER(content_area), vbox);

	gtk_widget_show_all(vbox);
	responce = gtk_dialog_run(GTK_DIALOG(dialog));
	gtk_widget_destroy(dialog);
	return responce;
}

static void show_message_box(const char *message)
{
	GtkWidget *dialog;

	dialog = gtk_message_dialog_new(GTK_WINDOW(window), GTK_DIALOG_MODAL,
									GTK_MESSAGE_INFO, GTK_BUTTONS_OK,
									message);

	gtk_window_set_title(GTK_WINDOW(dialog), MAIN_WINDOW_TITLE);
	gtk_dialog_run(GTK_DIALOG(dialog));
	gtk_widget_destroy(dialog);
}

static void on_tree_view_subject_list_row_activated(GtkTreeView *tree_view,
	GtkTreePath *path, GtkTreeViewColumn *column, gpointer user_data)
{
	GtkTreeIter		iter;
	GtkTreeModel	*model;
	const gchar		*id;
	const gchar		*subject;

	model = gtk_tree_view_get_model(tree_view);
	if (gtk_tree_model_get_iter(model, &iter, path))
		gtk_tree_model_get(model, &iter, 0, &id, 1, &subject, -1);

	selected_subject_id = (int) g_ascii_strtoll(id, NULL, 10);
	gtk_entry_set_text(GTK_ENTRY(user_data), subject);
}

static int is_selected_subject_list_row(void)
{
	GtkTreeSelection	*selection;

	selection = gtk_tree_view_get_selection(
		GTK_TREE_VIEW(tree_view_subject_list));
	return gtk_tree_selection_get_selected(selection, NULL, NULL);
}

static int add_subject_to_store_cb(void *store, int col_count,
	char **cols, char **col_names)
{
	GtkTreeIter		iter;

	gtk_list_store_append(GTK_LIST_STORE(store), &iter);
	gtk_list_store_set(GTK_LIST_STORE(store), &iter, 0, cols[0],
		1, cols[1], -1);
}

static int add_pupil_to_store_cb(void *store, int col_count,
	char **cols, char **col_names)
{
	GtkTreeIter		iter;

	gtk_list_store_append(GTK_LIST_STORE(store), &iter);
	gtk_list_store_set(GTK_LIST_STORE(store), &iter, 0, cols[0],
		1, cols[1], -1);
}
