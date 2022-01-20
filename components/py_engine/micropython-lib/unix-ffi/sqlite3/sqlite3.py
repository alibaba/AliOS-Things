import sys
import ffilib


sq3 = ffilib.open("libsqlite3")

sqlite3_open = sq3.func("i", "sqlite3_open", "sp")
# int sqlite3_close(sqlite3*);
sqlite3_close = sq3.func("i", "sqlite3_close", "p")
# int sqlite3_prepare(
#  sqlite3 *db,            /* Database handle */
#  const char *zSql,       /* SQL statement, UTF-8 encoded */
#  int nByte,              /* Maximum length of zSql in bytes. */
#  sqlite3_stmt **ppStmt,  /* OUT: Statement handle */
#  const char **pzTail     /* OUT: Pointer to unused portion of zSql */
# );
sqlite3_prepare = sq3.func("i", "sqlite3_prepare", "psipp")
# int sqlite3_finalize(sqlite3_stmt *pStmt);
sqlite3_finalize = sq3.func("i", "sqlite3_finalize", "p")
# int sqlite3_step(sqlite3_stmt*);
sqlite3_step = sq3.func("i", "sqlite3_step", "p")
# int sqlite3_column_count(sqlite3_stmt *pStmt);
sqlite3_column_count = sq3.func("i", "sqlite3_column_count", "p")
# int sqlite3_column_type(sqlite3_stmt*, int iCol);
sqlite3_column_type = sq3.func("i", "sqlite3_column_type", "pi")
sqlite3_column_int = sq3.func("i", "sqlite3_column_int", "pi")
# using "d" return type gives wrong results
sqlite3_column_double = sq3.func("d", "sqlite3_column_double", "pi")
sqlite3_column_text = sq3.func("s", "sqlite3_column_text", "pi")
# sqlite3_int64 sqlite3_last_insert_rowid(sqlite3*);
# TODO: should return long int
sqlite3_last_insert_rowid = sq3.func("i", "sqlite3_last_insert_rowid", "p")
# const char *sqlite3_errmsg(sqlite3*);
sqlite3_errmsg = sq3.func("s", "sqlite3_errmsg", "p")

# Too recent
##const char *sqlite3_errstr(int);
# sqlite3_errstr = sq3.func("s", "sqlite3_errstr", "i")


SQLITE_OK = 0
SQLITE_ERROR = 1
SQLITE_BUSY = 5
SQLITE_MISUSE = 21
SQLITE_ROW = 100
SQLITE_DONE = 101

SQLITE_INTEGER = 1
SQLITE_FLOAT = 2
SQLITE_TEXT = 3
SQLITE_BLOB = 4
SQLITE_NULL = 5


class Error(Exception):
    pass


def check_error(db, s):
    if s != SQLITE_OK:
        raise Error(s, sqlite3_errmsg(db))


class Connections:
    def __init__(self, h):
        self.h = h

    def cursor(self):
        return Cursor(self.h)

    def close(self):
        s = sqlite3_close(self.h)
        check_error(self.h, s)


class Cursor:
    def __init__(self, h):
        self.h = h
        self.stmnt = None

    def execute(self, sql, params=None):
        if params:
            params = [quote(v) for v in params]
            sql = sql % tuple(params)
        print(sql)
        b = bytearray(4)
        s = sqlite3_prepare(self.h, sql, -1, b, None)
        check_error(self.h, s)
        self.stmnt = int.from_bytes(b, sys.byteorder)
        # print("stmnt", self.stmnt)
        self.num_cols = sqlite3_column_count(self.stmnt)
        # print("num_cols", self.num_cols)
        # If it's not select, actually execute it here
        # num_cols == 0 for statements which don't return data (=> modify it)
        if not self.num_cols:
            v = self.fetchone()
            assert v is None
            self.lastrowid = sqlite3_last_insert_rowid(self.h)

    def close(self):
        s = sqlite3_finalize(self.stmnt)
        check_error(self.h, s)

    def make_row(self):
        res = []
        for i in range(self.num_cols):
            t = sqlite3_column_type(self.stmnt, i)
            # print("type", t)
            if t == SQLITE_INTEGER:
                res.append(sqlite3_column_int(self.stmnt, i))
            elif t == SQLITE_FLOAT:
                res.append(sqlite3_column_double(self.stmnt, i))
            elif t == SQLITE_TEXT:
                res.append(sqlite3_column_text(self.stmnt, i))
            else:
                raise NotImplementedError
        return tuple(res)

    def fetchone(self):
        res = sqlite3_step(self.stmnt)
        # print("step:", res)
        if res == SQLITE_DONE:
            return None
        if res == SQLITE_ROW:
            return self.make_row()
        check_error(self.h, res)


def connect(fname):
    b = bytearray(4)
    sqlite3_open(fname, b)
    h = int.from_bytes(b, sys.byteorder)
    return Connections(h)


def quote(val):
    if isinstance(val, str):
        return "'%s'" % val
    return str(val)
