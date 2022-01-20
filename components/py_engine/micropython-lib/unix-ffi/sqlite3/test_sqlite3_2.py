import sqlite3


conn = sqlite3.connect(":memory:")

cur = conn.cursor()
cur.execute("CREATE TABLE foo(a int)")
cur.execute("INSERT INTO foo VALUES (42)")
assert cur.lastrowid == 1
cur.execute("SELECT * FROM foo")
assert cur.fetchone() == (42,)
assert cur.fetchone() is None
