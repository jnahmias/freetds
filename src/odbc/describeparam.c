#include "common.h"

/* Test SQLDescribeParam() */

#define CHECK_VARIABLE_VALUE(var, val, msg_if_unequal) if (var != val) \
	{ rc = 1; fprintf(stderr, msg_if_unequal, var, val); }

int main(void)
{
	int rc = 0;
	SQLSMALLINT ParamCount, DataType, Scale, Nullable;
	SQLULEN ParamSize;

	if ( !odbc_driver_is_freetds()
	  || !odbc_db_is_microsoft()
	  ||  odbc_db_version_int() < TDS_MS_VER(11,0,0)
	) {
		/* skip if unimplemented */
		return 0;
	}

	odbc_connect();
	odbc_command("CREATE TABLE #t (i int PRIMARY KEY, v nvarchar(30))");

	odbc_reset_statement();
	CHKPrepare(T("INSERT INTO #t (i, v) VALUES (?, ?)"), SQL_NTS, "S");

	CHKDescribeParam((SQLUSMALLINT) 1, &DataType, &ParamSize, &Scale, &Nullable, "S");
	CHECK_VARIABLE_VALUE(DataType, SQL_INTEGER, "Wrong DataType %d, should be %d.\n")
	CHECK_VARIABLE_VALUE(ParamSize, 0, "Wrong ParamSize %ld, should be %ld.\n")
	CHECK_VARIABLE_VALUE(Scale, 10, "Wrong Scale %d, should be %d.\n")
	CHECK_VARIABLE_VALUE(Nullable, SQL_NO_NULLS, "Wrong Nullity %d, should be %d.\n")
	if (rc)
		goto cleanup;

	CHKDescribeParam((SQLUSMALLINT) 2, &DataType, &ParamSize, &Scale, &Nullable, "S");
	CHECK_VARIABLE_VALUE(DataType, SQL_WVARCHAR, "Wrong DataType %d, should be %d.\n")
	CHECK_VARIABLE_VALUE(ParamSize, 30UL, "Wrong ParamSize %ld, should be %ld.\n")
	CHECK_VARIABLE_VALUE(Scale, 0, "Wrong Scale %d, should be %d.\n")
	CHECK_VARIABLE_VALUE(Nullable, SQL_NULLABLE, "Wrong Nullity %d, should be %d.\n")
	if (rc)
		goto cleanup;

cleanup:
	odbc_reset_statement();
	odbc_command("drop table #t");
	odbc_disconnect();
	fprintf(stderr, "Test Complete.\n");
	return rc;
}
