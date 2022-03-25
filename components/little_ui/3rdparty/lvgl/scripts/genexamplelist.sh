#!/bin/bash
echo "/* Autogenerated */"
echo '#include <stddef.h>'
echo '#include "examplelist.h"'
TMPFILE=$(mktemp)
find examples -name \*.h | xargs grep -h "^void lv_example" | sed 's/(/ /g' | awk '{print $2}' > $TMPFILE
cat $TMPFILE | while read -r line; do
echo "extern void ${line}(void);"
done
echo "const struct lv_ci_example lv_ci_example_list[] = {"
cat $TMPFILE | while read -r line; do
echo "    { \"$line\", $line },";
done
echo "    { NULL, NULL }"
echo "};"
