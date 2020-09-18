.PHONY: clean all

all: clean
	@gcc ./main.c ../nftl_util.c ../nftl_partition.c ../nftl.c ../sim/aos/hal/flash.c ../sim/nftl_bd.c -I../../ -I../ -I../inc/ -I./ -I../../../include -I../sim -DNFTL_SIM_DEBUG -lpthread -g -DNFTL_YES_TRACE -o test.bin

mparttest: clean
	@gcc ./multiple_part_test.c ../nftl_util.c ../nftl_partition.c ../nftl.c ../sim/aos/hal/flash.c ../sim/nftl_bd.c -I../../ -I../ -I../inc/ -I./ -I../../../include -I../sim -DNFTL_SIM_DEBUG -lpthread -gdwarf-2 -g3 -DNFTL_YES_TRACE -o mparttest.bin
	@cp ../tools/sim062100.img /tmp/nftl_bd

clean:
	@rm -fr *.o test.bin mparttest.bin /tmp/nftl_bd
