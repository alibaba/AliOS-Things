#!/bin/bash
ROOT_DIR=$PWD
DOXYGEN="doxygen"
DIR_CONFIG_DOX=$PWD/tools
FILE_CONFIG_DOX=$PWD/tools/Doxyfile
OUT_PUT_DIR1=$PWD/out
OUT_PUT_DIR2=$PWD/out/target
OUT_PUT_DIR3=$PWD/out/target/docs
# search doxygen in host 
which "$DOXYGEN"  > /dev/null 2>&1
if [ $? == 0 ]; then
        echo "start to create doc"
else
        echo "can not find $DOXYGEN"
	echo "we will install it"
	sudo apt-get install $DOXYGEN
fi
cd $DIR_CONFIG_DOX
echo $OUTTA |  grep "Doxyfile" > /dev/null 2>&1
if [ $? -eq 0 ]; then
        echo "HOST HAVE INSTALL Doxyfile config"
        exit 0
fi

cd $ROOT_DIR

if [ ! -d $OUT_PUT_DIR1 ] ;then
mkdir $OUT_PUT_DIR1
fi

if [ ! -d $OUT_PUT_DIR2 ] ;then
mkdir $OUT_PUT_DIR2
fi

if [ ! -d $OUT_PUT_DIR3 ] ;then
mkdir $OUT_PUT_DIR3
fi

$DOXYGEN $FILE_CONFIG_DOX

if [ -f "Doxyfile" ] ;then
rm -f Doxyfile
fi
