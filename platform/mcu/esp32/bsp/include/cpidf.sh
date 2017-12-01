#!bash
target=$(dirname $0)
cd $target
dirs="esp32/include soc/esp32/include soc/include driver/include ethernet/include log/include nvs_flash/include spi_flash/include container/include"
for d in $dirs;
do
    t=$(dirname $d)
    mkdir -p $t
    cp -r $IDF_PATH/components/$d $t
done
cd -
