#!/bin/sh

for i in $(find . -name "*.[ch]"); do
    astyle --style=linux -Sw -fpXHU -jk3 -n -z2 $i
done
