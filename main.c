
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mysql.h> // mysql

#define MYSQL_HOST "localhost"
#define MYSQL_USERNAME "cptaffe"
#define MYSQL_PASSWORD "cptaffe"
#define MYSQL_DB "testdb"

// query function
static int query(MYSQL *con, char *query) {
	if (mysql_query(con, query)) {
		return 1;
	}
	return 0;
}

static int loop_query(MYSQL *con, char **q, int len) {
	for (int i = 0; i < len; i++) {
		if (query(con, q[i])) { return 1; }
	}
	return 0;
}

// example of parameterized mysql query for user input of data.
static int param_query(MYSQL *con, char *stmnt_str, int id, char *str, int price) {
	// alloc statement
	MYSQL_STMT *stmnt = mysql_stmt_init(con);
	if (!stmnt) { return 1; }

	// statement
	if (mysql_stmt_prepare(stmnt, stmnt_str, strlen(stmnt_str))) { return 2; }

	// all uninitialized portions of the struct are initialized to 0.
	MYSQL_BIND param[3] = {
		{.buffer_type = MYSQL_TYPE_LONG, .buffer = (void *) &id},
	{.buffer_type = MYSQL_TYPE_STRING, .buffer = (void *) str, .buffer_length = strlen(str)},
		{.buffer_type = MYSQL_TYPE_LONG, .buffer = (void *) &price}
	};

	if (mysql_stmt_bind_param(stmnt, param)) { return 3; }

	if (mysql_stmt_execute(stmnt)) { return 4; }

	mysql_stmt_close(stmnt); // dealloc
	return 0;
}

int main() {
	// client version information
	printf("MySQL client version: %s\n", mysql_get_client_info());

	// init mysql connection
	MYSQL *con = mysql_init(NULL);
	if (con == NULL) {
		fprintf(stderr, "%s\n", mysql_error(con));
		mysql_close(con);
		return 1;
	}

	// connect to database
	if (mysql_real_connect(con,
		MYSQL_HOST,
		MYSQL_USERNAME,
		MYSQL_PASSWORD,
		MYSQL_DB,
		0, NULL, 0) == NULL) {
		fprintf(stderr, "%s\n", mysql_error(con));
		mysql_close(con);
		return 2;
	}

	char *q[] = {
		"DROP TABLE IF EXISTS Cars",
		"CREATE TABLE Cars(Id INT, Name TEXT, Price INT)",
	};

	// loop through queries, fail if failure.
	if (loop_query(con, q, (sizeof(q) / sizeof(char *)))) {
		fprintf(stderr, "%s\n", mysql_error(con));
		mysql_close(con);
		return 3;
	}

	char *c_stmnt = "INSERT INTO Cars(Id, Name, Price) VALUES(?,?,?)";

	// local struct declaration to simplify data listing.
	struct car_stmnt {int id; char *str; int price;};
	struct car_stmnt cars[] = {
		{.id = 1, .str = "Audi", .price = 52642},
		{.id = 2, .str = "Mercedes", .price = 57127},
		{.id = 3, .str = "Skoda", .price = 9000},
		{.id = 4, .str = "Volvo", .price = 29000}
	};

	for (int i = 0; i < (sizeof(cars) / sizeof(struct car_stmnt)); i++) {
		int ret;
		if ((ret = param_query(con, c_stmnt, cars[i].id, cars[i].str, cars[i].price))) {
			printf("param_query failed with value %d.\n", ret);
			return ret + 3;
		}
	}

	mysql_close(con);
}
