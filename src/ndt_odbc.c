/*
 * This file contains functions needed to handle database support (ODBC).
 *
 * Jakub S�awi�ski 2007-07-23
 * jeremian@poczta.fm
 */

#include "../config.h"

#include <stdio.h>
#include <string.h>
#if defined(HAVE_ODBC) && defined(DATABASE_ENABLED) && defined(HAVE_SQL_H)
#include <sql.h>
#include <sqlext.h>
#endif

#include "ndt_odbc.h"
#include "logging.h"
#include "strlutils.h"

#if defined(HAVE_ODBC) && defined(DATABASE_ENABLED) && defined(HAVE_SQL_H)
SQLHENV env;
SQLHDBC dbc;
SQLHSTMT stmt = NULL;
char* ctStmt_1 = "CREATE TABLE ndt_test_results ("
"spds1 TEXT,"
"spds2 TEXT,"
"spds3 TEXT,"
"spds4 TEXT,"
"runave1 FLOAT,"
"runave2 FLOAT,"
"runave3 FLOAT,"
"runave4 FLOAT,"
"cputimelog TEXT,"
"snaplog TEXT,"
"c2s_snaplog TEXT,"
"hostName TEXT,"
"testPort INT,"
"date TEXT,"
"rmt_host TEXT,";
char* ctStmt_2 = "s2c2spd INT,"
"s2cspd INT,"
"c2sspd INT,"
"Timeouts INT,"
"SumRTT INT,"
"CountRTT INT,"
"PktsRetrans INT,"
"FastRetran INT,"
"DataPktsOut INT,"
"AckPktsOut INT,"
"CurrentMSS INT,"
"DupAcksIn INT,"
"AckPktsIn INT,"
"MaxRwinRcvd INT,"
"Sndbuf INT,"
"MaxCwnd INT,";
char* ctStmt_3 = "SndLimTimeRwin INT,"
"SndLimTimeCwnd INT,"
"SndLimTimeSender INT,"
"DataBytesOut INT,"
"SndLimTransRwin INT,"
"SndLimTransCwnd INT,"
"SndLimTransSender INT,"
"MaxSsthresh INT,"
"CurrentRTO INT,"
"CurrentRwinRcvd INT,"
"link INT,"
"mismatch INT,"
"bad_cable INT,"
"half_duplex INT,"
"congestion INT,"
"c2sdata INT,";
char* ctStmt_4 = "c2sack INT,"
"s2cdata INT,"
"s2cack INT,"
"CongestionSignals INT,"
"PktsOut INT,"
"MinRTT INT,"
"RcvWinScale INT,"
"autotune INT,"
"CongAvoid INT,"
"CongestionOverCount INT,"
"MaxRTT INT,"
"OtherReductions INT,"
"CurTimeoutCount INT,"
"AbruptTimeouts INT,"
"SendStall INT,"
"SlowStart INT,"
"SubsequentTimeouts INT,"
"ThruBytesAcked INT,"
"minPeak INT,"
"maxPeak INT,"
"peaks INT"
");";
char createTableStmt[2048];

/**
 * Retrieves all the diagnostics
 *  associated with that a given SQL handle
 * @param fn pointer to function name string
 * @param handle SQLHandle
 * @param type SQLHandle type
 * */

static void extract_error(char *fn, SQLHANDLE handle, SQLSMALLINT type) {
  SQLINTEGER i = 0;
  SQLINTEGER native;
  SQLCHAR state[ 7 ];
  SQLCHAR text[256];
  SQLSMALLINT len;
  SQLRETURN ret;

  log_println(2, "\nThe driver reported the following diagnostics whilst "
              "running %s:\n", fn);
  do {
    // get current values of multiple fields(error, warning, and status) of
    // diagnostic record and see if return value indicated success
    ret = SQLGetDiagRec(type, handle, ++i, state, &native, text, sizeof(text),
                        &len);
    if (SQL_SUCCEEDED(ret))
      log_println(2, "%s:%ld:%ld:%s", state, i, native, text);
  }
  while (ret == SQL_SUCCESS);
}
#endif

/**
 * Initialize Database
 * @param options integer indicating whether DB should be used
 * @param dsn data source name string pointer
 * @param uid User name string pointer the db uses for authentication
 * @param pwd pointer to password string pointer the db uses for authentication
 * @return integer 0 if success, 1 if failure
 * */
int initialize_db(int options, char* dsn, char* uid, char* pwd) {
#if defined(HAVE_ODBC) && defined(DATABASE_ENABLED) && defined(HAVE_SQL_H)
  if (options) {
    SQLRETURN ret; /* ODBC API return status */
    SQLSMALLINT columns; /* number of columns in result-set */
    SQLCHAR outstr[1024];
    SQLSMALLINT outstrlen;
    char loginstring[1024];

    log_println(1, "Initializing DB with DSN='%s', UID='%s', PWD=%s", dsn, uid,
                pwd ? "yes" : "no");
    snprintf(createTableStmt, sizeof(createTableStmt), "%s%s%s%s", ctStmt_1,
             ctStmt_2, ctStmt_3, ctStmt_4);

    // Allocate an environment handle
    SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &env);
    // We want ODBC 3 support
    SQLSetEnvAttr(env, SQL_ATTR_ODBC_VERSION, (void *) SQL_OV_ODBC3, 0);
    // Allocate a connection handle
    SQLAllocHandle(SQL_HANDLE_DBC, env, &dbc);
    // Connect to the DSN after creating summarizing login data
    // into "loginstring"
    memset(loginstring, 0, 1024);
    snprintf(loginstring, sizeof(loginstring), "DSN=%s;", dsn);
    if (uid) {
      // strcat(loginstring, "UID=");
      // strncat(loginstring, uid, 256);
      // strcat(loginstring, ";");
      strlcat(loginstring, "UID=", sizeof(loginstring));
      strlcat(loginstring, uid, sizeof(loginstring));
      strlcat(loginstring, ";", sizeof(loginstring));
    }
    if (pwd) {
      // strcat(loginstring, "PWD=");
      strlcat(loginstring, "PWD=", sizeof(loginstring));
      // strncat(loginstring, pwd, 256);
      strlcat(loginstring, pwd, sizeof(loginstring));
    }
    ret = SQLDriverConnect(dbc, NULL, (unsigned char*) loginstring, SQL_NTS,
                           outstr, sizeof(outstr), &outstrlen,
                           SQL_DRIVER_NOPROMPT);
    if (SQL_SUCCEEDED(ret)) {
      log_println(2, "  Connected");
      log_println(2, "  Returned connection string was:\n\t%s", outstr);
      if (ret == SQL_SUCCESS_WITH_INFO) {
        log_println(2, "Driver reported the following diagnostics");
        extract_error("SQLDriverConnect", dbc, SQL_HANDLE_DBC);
      }
    } else {
      log_println(0, "  Failed to connect to the DSN\n Continuing without DB "
                  "logging");
      extract_error("SQLDriverConnect", dbc, SQL_HANDLE_DBC);
      return 1;
    }
    // Allocate a statement handle
    ret = SQLAllocHandle(SQL_HANDLE_STMT, dbc, &stmt);
    if (!SQL_SUCCEEDED(ret)) {
      log_println(0, "  Failed to alloc statement handle\n Continuing without "
                  "DB logging");
      extract_error("SQLAllocHandle", dbc, SQL_HANDLE_DBC);
      return 1;
    }
    // Retrieve a list of tables
    ret = SQLTables(stmt, NULL, 0, NULL, 0, NULL, 0, (unsigned char*) "TABLE",
                    SQL_NTS);
    if (!SQL_SUCCEEDED(ret)) {
      log_println(0, "  Failed to fetch table info\n Continuing without DB "
                  "logging");
      extract_error("SQLTables", dbc, SQL_HANDLE_DBC);
      return 1;
    }
    // How many columns are there?
    SQLNumResultCols(stmt, &columns);
    log_println(3, "Fetched SQLNumResults:%d", columns);
    // Loop through the rows in the result-set
    while (SQL_SUCCEEDED(ret = SQLFetch(stmt))) {
      SQLUSMALLINT i;
      // Loop through the columns
      for (i = 2; i <= columns; i++) {
        SQLINTEGER indicator;
        char buf[512];
        // retrieve column data as a string
        ret = SQLGetData(stmt, i, SQL_C_CHAR,
                         buf, sizeof(buf), &indicator);
        if (SQL_SUCCEEDED(ret)) {
          // Handle null columns
          if (indicator == SQL_NULL_DATA)
            // strcpy(buf, "NULL");
            strlcpy(buf, "NULL", sizeof(buf));
          if (strcmp(buf, "ndt_test_results") == 0) {
            // the table exists - do nothing
            SQLFreeStmt(stmt, SQL_CLOSE);
            return 0;
          }
        }
      }
    }

    // the table doesn't exist - create it
    SQLFreeStmt(stmt, SQL_CLOSE);
    log_print(1, "The table 'ndt_test_results' doesn't exist, creating...");
    ret = SQLExecDirect(stmt, (unsigned char*) createTableStmt,
                        strlen(createTableStmt));
    if (!SQL_SUCCEEDED(ret)) {
      log_println(0, "  Failed to create table\n Continuing without DB "
                  "logging");
      extract_error("SQLExecDirect", dbc, SQL_HANDLE_DBC);
      stmt = NULL;
      return 1;
    }
    log_println(1, " SUCCESS!");
  }
  return 0;
#else
  return 1;
#endif
}

/**
 * Insert row of test results into Database
 * @params All parameters related to test results collected
 * @return integer 0 if success, 1 if failure
 * */
int db_insert(char spds[4][256], float runave[], char* cputimelog,
              char* snaplog, char* c2s_snaplog, char* hostName, int testPort,
              char* date, char* rmt_addr, int s2c2spd, int s2cspd, int c2sspd,
              int Timeouts, int SumRTT, int CountRTT, int PktsRetrans,
              int FastRetran, int DataPktsOut, int AckPktsOut, int CurrentMSS,
              int DupAcksIn, int AckPktsIn, int MaxRwinRcvd, int Sndbuf,
              int MaxCwnd, int SndLimTimeRwin, int SndLimTimeCwnd,
              int SndLimTimeSender, int DataBytesOut, int SndLimTransRwin,
              int SndLimTransCwnd, int SndLimTransSender, int MaxSsthresh,
              int CurrentRTO, int CurrentRwinRcvd, int link, int mismatch,
              int bad_cable, int half_duplex, int congestion,
              int c2s_linkspeed_data, int c2s_linkspeed_ack,
              int s2c_linkspeed_data, int s2c_linkspeed_ack,
              int CongestionSignals, int PktsOut, int MinRTT, int RcvWinScale,
              int autotune, int CongAvoid, int CongestionOverCount, int MaxRTT,
              int OtherReductions, int CurTimeoutCount, int AbruptTimeouts,
              int SendStall, int SlowStart, int SubsequentTimeouts,
              int ThruBytesAcked, int minPeak, int maxPeak, int peaks) {
#if defined(HAVE_ODBC) && defined(DATABASE_ENABLED) && defined(HAVE_SQL_H)
  SQLRETURN ret;
  char insertStmt[2048];
  int i_iter = 0, arr_len = 4;  // 4 runspeed-averages are obtained
  if (!stmt) {
    return 1;
  }
  // Check if any of the run averages are nan numbers
  for (i_iter = 0; i_iter < arr_len; i_iter++) {
    log_println(3, "Odbc:Padding %f?", runave[i_iter]);
    pad_NaN(&runave[i_iter]);
    log_println(3, "Odbc:After Padding %f", runave[i_iter]);
  }
  snprintf(insertStmt, sizeof(insertStmt),
           "INSERT INTO ndt_test_results VALUES ("
           "'%s','%s','%s','%s',%f,%f,%f,%f,"
           "'%s','%s','%s','%s',%d,"
           "'%s','%s',%d,%d,%d,%d,"
           "%d,%d,%d,%d,%d,"
           "%d,%d,%d,%d,%d,"
           "%d,%d,%d,%d,%d,"
           "%d,%d,%d,%d,"
           "%d,%d,%d,%d,%d,"
           "%d,%d,%d,%d,%d,%d,"
           "%d,%d,%d,%d,%d,"
           "%d,%d,%d,%d,%d,"
           "%d,%d,%d,%d,"
           "%d,%d,%d,%d,%d"
           ");",
           spds[0], spds[1], spds[2], spds[3], runave[0], runave[1], runave[2],
           runave[3], cputimelog, snaplog, c2s_snaplog, hostName, testPort,
           date, rmt_addr, s2c2spd, s2cspd, c2sspd, Timeouts,
           SumRTT, CountRTT, PktsRetrans, FastRetran, DataPktsOut,
           AckPktsOut, CurrentMSS, DupAcksIn, AckPktsIn, MaxRwinRcvd,
           Sndbuf, MaxCwnd, SndLimTimeRwin, SndLimTimeCwnd, SndLimTimeSender,
           DataBytesOut, SndLimTransRwin, SndLimTransCwnd, SndLimTransSender,
           MaxSsthresh, CurrentRTO, CurrentRwinRcvd, link, mismatch,
           bad_cable, half_duplex, congestion, c2s_linkspeed_data,
           c2s_linkspeed_ack, s2c_linkspeed_data, s2c_linkspeed_ack,
           CongestionSignals, PktsOut, MinRTT, RcvWinScale, autotune, CongAvoid,
           CongestionOverCount, MaxRTT, OtherReductions, CurTimeoutCount,
           AbruptTimeouts, SendStall, SlowStart, SubsequentTimeouts,
           ThruBytesAcked, minPeak, maxPeak, peaks);
  ret = SQLExecDirect(stmt, (unsigned char*) insertStmt, strlen(insertStmt));
  log_println(3, "Trying to insert data using the statement:%s", insertStmt);
  if (!SQL_SUCCEEDED(ret)) {
    log_println(0, "  Failed to insert test results into the table\n "
                "Continuing without DB logging");
    extract_error("SQLExecDirect", dbc, SQL_HANDLE_DBC);
    stmt = NULL;
    return 1;
  }

  return 0;
#else
  return 1;
#endif
}

/***
 * Check if the value is a NaN by comparing it with itself.
 * @args float_val value of the float variable which has to be checked for Nan
 * 
 **/
void pad_NaN(float *float_val) {
  if (*float_val != *float_val) {
    *float_val = -1;
    log_println(0, " A float value you tried to insert into the DB was NaN");
  }
}
