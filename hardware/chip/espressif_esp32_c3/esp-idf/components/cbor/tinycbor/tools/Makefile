CFLAGS = -O2 -g
CPPFLAGS = -I../src
VPATH = cbordump:../src

all: ../bin ../bin/cbordump
../bin:
	@-mkdir ../bin

../bin/cbordump: cbordump.o cborparser.o cborparser_dup_string.o cbortojson.o cborerrorstrings.o cborpretty.o
	$(CC) -o $@ $^
	$(RM) $^

