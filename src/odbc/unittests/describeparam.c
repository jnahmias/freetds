#include "common.h"

/* Test SQLDescribeParam() */

#define CHECK_VARIABLE_VALUE(var, val, msg_if_unequal) if (var != val) \
	{ rc = 1; fprintf(stderr, msg_if_unequal, var, val); }

static const char *bool2str(bool b) { return (b) ? "true" : "false"; }

int main(void)
{
	int rc = 0;
	const unsigned int MSSQL_2012_VER = 0xB000000;  /* 11.0.0 */
	/* ref: https://learn.microsoft.com/en-us/troubleshoot/sql/releases/download-and-install-latest-updates */
	SQLSMALLINT DataType, Scale, Nullable;
	SQLULEN ParamSize;

	odbc_use_version3 = 1;
	odbc_conn_additional_params = "ClientCharset=UTF-8;";
	odbc_connect();

	if ( !odbc_driver_is_freetds()
	  || !odbc_db_is_microsoft()
	  ||  odbc_db_version_int() < MSSQL_2012_VER
	) {
		/* skip if unimplemented */
		fprintf(stderr, "Test skipped: odbc_driver_is_freetds=%s.\n", bool2str(odbc_driver_is_freetds()));
		fprintf(stderr, "\t\todbc_db_is_microsoft=%s\n", bool2str(odbc_db_is_microsoft()));
		fprintf(stderr, "\t\todbc_db_version=%s (%#x).\n", odbc_db_version(), odbc_db_version_int());
		return 77;
	}

	odbc_command("IF OBJECT_ID('descparam01') IS NOT NULL DROP TABLE descparam01");
	odbc_command("CREATE TABLE descparam01 (i int PRIMARY KEY, v nvarchar(30))");
	odbc_reset_statement();
	CHKPrepare(T("INSERT INTO descparam01 (i, v) VALUES (?, ?)"), SQL_NTS, "S");

	CHKDescribeParam((SQLUSMALLINT) 1, &DataType, &ParamSize, &Scale, &Nullable, "S");
	CHECK_VARIABLE_VALUE(DataType, SQL_INTEGER, "Param #1: wrong DataType %d, should be %d.\n")
	CHECK_VARIABLE_VALUE(ParamSize, 0UL, "Param #1: wrong ParamSize %ld, should be %ld.\n")
	CHECK_VARIABLE_VALUE(Scale, 10, "Param #1: wrong Scale %d, should be %d.\n")
	CHECK_VARIABLE_VALUE(Nullable, SQL_NO_NULLS, "Param #1: wrong Nullity %d, should be %d.\n")

	CHKDescribeParam((SQLUSMALLINT) 2, &DataType, &ParamSize, &Scale, &Nullable, "S");
	CHECK_VARIABLE_VALUE(DataType, SQL_WVARCHAR, "Param #2: wrong DataType %d, should be %d.\n")
	CHECK_VARIABLE_VALUE(ParamSize, 30UL, "Param #2: wrong ParamSize %ld, should be %ld.\n")
	CHECK_VARIABLE_VALUE(Scale, 0, "Param #2: wrong Scale %d, should be %d.\n")
	CHECK_VARIABLE_VALUE(Nullable, SQL_NULLABLE, "Param #2: wrong Nullity %d, should be %d.\n")

	odbc_reset_statement();
	odbc_command("DROP TABLE descparam01");
	odbc_disconnect();
	fprintf(stderr, "Test Complete.\n");
	return rc;
}
