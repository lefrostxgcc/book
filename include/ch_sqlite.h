#ifndef CH_SQLITE_H
#define CH_SQLITE_H

struct ch_sqlite_connection;
enum ch_sqlite_status { CH_SQLITE_OK, CH_SQLITE_FAIL };

enum ch_sqlite_status
ch_sqlite_open(const char *filename, struct ch_sqlite_connection **connection);

enum ch_sqlite_status
ch_sqlite_close(struct ch_sqlite_connection **connection);

enum ch_sqlite_status
ch_sqlite_exec(
	struct ch_sqlite_connection *connection,
	const char *query,
	int (callback)(void *, int, char **, char **),
	void *callback_opt_arg);

enum ch_sqlite_status
ch_sqlite_scalar(
	struct ch_sqlite_connection *connection,
	const char *query,
	char *result,
	int result_size);

const char *
ch_sqlite_errormsg(struct ch_sqlite_connection *connection);

const char *
ch_sqlite_last_query(struct ch_sqlite_connection *connection);

const char *
ch_sqlite_last_error(struct ch_sqlite_connection *connection);

int
ch_sqlite_rows_modified(struct ch_sqlite_connection *connection);

#endif /* CH_SQLITE_H */
