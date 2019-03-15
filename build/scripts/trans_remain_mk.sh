
cd `dirname $0`/../..
pwd

mk_paths=$(find . -type d \( -path ./build -o -path ./platform -o -path ./board \) -prune -o -name *.mk -a -print)

make_path="build/cmd/win32/make"

count=0
echo creating py_files:
for mk_path in ${mk_paths}; do
	py_path=$(dirname $mk_path)"/ucube.py"
	if [ ! -f $py_path ]; then 
		echo $py_path
		((count=$count+1)) 
		$make_path -f build/scripts/trans_mk2py.mk mk_path=$mk_path
	fi
done
echo count: $count
