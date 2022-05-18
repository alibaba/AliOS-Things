#!/bin/sh

cmakeFormat="cmake-format"

path_to_cmake_format="$(which $cmakeFormat)"
echo "$path_to_cmake_format"

if [ -z "$cmakeFormat" ]
then
    echo "$cmakeFormat is not installed. Cannot format cmake files..."
    echo "run: pip install cmake-format"
    exit 1
fi

echo "$cmakeFormat was found, going to format your cmake scripts..." >&2

find $(dirname "$0")/ \
-not \( -path "*/build/*" -prune \) \
\( -name *.cmake -o -name CMakeLists.txt \) \
| xargs $cmakeFormat -c cmake-format.yaml -i


echo "done formatting with cmake-format"
