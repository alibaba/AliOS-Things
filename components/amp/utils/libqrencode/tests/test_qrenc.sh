#!/bin/sh

COMMAND=../qrencode
TARGET_DIR="test_images"
VALGRIND_COMMAND="libtool --mode=execute valgrind"
VALGRIND_OPTIONS="--leak-check=full --show-reachable=yes"

if [ "x$1" = 'xvalgrind' ]; then
	COMMAND="$VALGRIND_COMMAND $VALGRIND_OPTIONS $COMMAND"
fi

repeatchar()
{
	printf %${2}s | tr ' ' ${1}
}

test_command_success()
{
	repeatchar ${1} ${2} | $COMMAND -o - -l L ${3} > /dev/null
	if [ $? -ne 0 ]; then
		echo "Failed to encode $fn.txt"
		exit 1
	fi
}

test_command_fail()
{
	repeatchar ${1} ${2} | $COMMAND -o - -l L ${3} > /dev/null
	if [ $? -eq 0 ]; then
		echo "Unexpectedly successed to encode '${1}'x'${2}' with '${3}'."
		exit 1
	else
		echo "^^^this is the expected error. Everything OK."
	fi
}

mkdir -p $TARGET_DIR

test_command_success '1' 7089
test_command_success 'A' 4296
test_command_success 'a' 2953
test_command_success '\211' 3634 '-k'

test_command_fail '1' 7090
test_command_fail 'A' 4297
test_command_fail 'a' 2954
test_command_fail '\211' 3636 '-k'
test_command_fail '1' 15000
