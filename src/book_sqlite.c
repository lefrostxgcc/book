#include <sqlite3.h>
#include "book_sqlite.h"

struct sqlite_connection
{
	sqlite3 *db;
};

enum sqlite_status
sqlite_open(const char *filename, struct sqlite_connection *connection)
{
	sqlite3		*db;
	int			rc;

	rc = sqlite3_open(filename, &db);
	if (rc != SQLITE_OK)
	{
		sqlite3_close(db);
		return SQL_FAIL;
	}
	connection->db = db;
	return SQLITE_OK;
}
