.PHONY: clean all

all: clean
	@gcc -O2 mknftl.c -o mknftl
	@gcc -O2 -I.. -I../sim mksim.c -o mksim
	@gcc -O2 cutlfs.c -o cutlfs
	@gcc -O2 nftl2lfs.c -o nftl2lfs

mknftl: clean
	@gcc -O2 mknftl.c -o mknftl

mksim: clean
	@gcc -O2 -I.. -I../sim mksim.c -o mksim

culfs: clean
	@gcc -O2 cutlfs.c -o cutlfs

nftl2lfs: clean
	@gcc -O2 nftl2lfs.c -o nftl2lfs

clean:
	@rm -fr *.o mknftl mksim cutlfs nftl2lfs
