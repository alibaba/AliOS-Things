import sqlite3


conn = sqlite3.connect(":memory:")

cur = conn.cursor()
cur.execute("SELECT 1, 'foo', 3.0 UNION SELECT 3, 3, 3")

expected = [(1, "foo", 3.0), (3, 3, 3)]

while True:
    row = cur.fetchone()
    if row is None:
        break
    print(row)
    e = expected.pop(0)
    assert row == e

assert expected == []
