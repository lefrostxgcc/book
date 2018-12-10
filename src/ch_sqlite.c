#include <string.h>
#include <stdlib.h>
#include <sqlite3.h>
#include <ch_sqlite.h>

struct ch_sqlite_connection
{
	sqlite3 *db;
};

enum ch_sqlite_status
ch_sqlite_open(const char *filename, struct ch_sqlite_connection **connection)
{
	sqlite3		*db;
	int			rc;

	rc = sqlite3_open(filename, &db);
	if (rc != SQLITE_OK)
	{
		*connection = NULL;
		sqlite3_close(db);
		return CH_SQLITE_FAIL;
	}
	*connection = (struct ch_sqlite_connection *)
		malloc(sizeof(struct ch_sqlite_connection));
	(*connection)->db = db;
	return CH_SQLITE_OK;
}

enum ch_sqlite_status
ch_sqlite_close(struct ch_sqlite_connection **connection)
{
	if (sqlite3_close((*connection)->db) != SQLITE_OK)
		return CH_SQLITE_FAIL;

	free(*connection);
	*connection = NULL;
	return CH_SQLITE_OK;
}

enum ch_sqlite_status
ch_sqlite_exec(
	struct ch_sqlite_connection *connection,
	const char *query,
	int (callback)(void *, int, char **, char **),
	void *callback_opt_arg)
{
	int		rc;

	rc = sqlite3_exec(connection->db, query, callback, callback_opt_arg, NULL);

	return rc == SQLITE_OK ? CH_SQLITE_OK : CH_SQLITE_FAIL;
}

enum ch_sqlite_status
ch_sqlite_scalar(
	struct ch_sqlite_connection *connection,
	const char *query,
	char *result,
	int result_size)
{
	sqlite3_stmt	*res;
	int				rc;
	int				step;

	rc = sqlite3_prepare_v2(connection->db, query, -1, &res, 0);

	if (rc != SQLITE_OK)
		return CH_SQLITE_FAIL;

	step = sqlite3_step(res);
	if (step != SQLITE_ROW)
	{
		sqlite3_finalize(res);
		return CH_SQLITE_FAIL;
	}
	strncpy(result, sqlite3_column_text(res, 0), result_size);
	sqlite3_finalize(res);
	return CH_SQLITE_OK;
}

const char *
ch_sqlite_errormsg(struct ch_sqlite_connection *connection)
{
	return sqlite3_errmsg(connection->db);
}
