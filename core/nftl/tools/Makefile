.PHONY: clean all

all: clean
	@gcc -O2 mknftl.c -o mknftl
	@gcc -O2 -I.. -I../sim mksim.c -o mksim
	@gcc -O2 nftl2lfs.c -o nftl2lfs

mknftl: clean
	@gcc -O2 mknftl.c -o mknftl

mksim: clean
	@gcc -O2 -I.. -I../sim mksim.c -o mksim

nftl2lfs: clean
	@gcc -O2 nftl2lfs.c -o nftl2lfs

clean:
	@rm -fr *.o mknftl mksim nftl2lfs
