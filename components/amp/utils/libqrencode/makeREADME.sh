#!/bin/sh

sed '/^```$/d
/^\*\*Attention/d
s/DWITH\\_TESTS/DWITH_TESTS/
1 {
	s/^# //
	s/encoding library.*/encoding library/
}
' README.md > README
