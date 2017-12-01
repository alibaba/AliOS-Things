
#ifdef MBEDTLS_IN_KERNEL

/*
 * +------------+  0x00040000
 * Vect redirect   32
 * +------------+
 *
 * +------------+  0x00400020
 * app:         |
 * data         |  app:16k
 * bss          |
 * +------------+  0x00404020
 * framework:   |
 * data         |  framework:48k
 * bss          |
 * +------------+  0x00410020
 * kernel:      |
 * data         |
 * end
 * |(heap)      |
 * .            .  256k - 64k - 32
 * .            .
 * |(heap limit)|
 *
 * |- - - - - - |
 * stack bottom |
 * +------------+
 *
 *
 * +------------+  0x0000000
 * |Bootloader  |
 * |            |  64k
 * +------------+  0x0010000
 * |   Para 1   |   4k
 * +------------+
 * |   Para 2   |   4k
 * +------------+  0x0012000
 * |   Para 3   |   4k
 * +------------+  0x0013000
 * +------------+  0x0013200
 * |vectors     |
 * |            |
 * |------------+
 * |text        |
 * |data        |  kernel:455K
 * |            |
 * +------------+  0x0085000
 * +------------+  0x0085140
 * |text        |
 * |data        |  framework:159k
 * |            |
 * +------------+  0x00AD000
 * +------------+  0x00AD300
 * |text        |
 * |data        |  app:51k
 * |            |
 * +------------+  0x00BA000
 * +------------+  0x00BA340

 * |            |
 * |            |
 * |  OTA TEMP  |
 * |            |  279k(MAX * 60%)
 * |            |
 * |            |
 * +------------+  0x100000
*/

/* Framework's phy start address is 0x0085140.
 * Flash size = 0x00AD000-0x0085140 = 159K
 * Code size = 159K*32/34=149K
 */

/* Split memory into area for vectors and ram */
MEMORY
{
	flash  (rx) : ORIGIN = 0x007D400, LENGTH = 149K
	ram   (rwx): ORIGIN = 0x00404020, LENGTH = 48k
}

#else

/*
 * +------------+  0x00400000
 * Vect redirect   32
 * +------------+
 *
 * +------------+  0x00400020
 * app:         |
 * data         |  app:16k
 * bss          |
 * +------------+  0x00404020
 * framework:   |
 * data         |  framework:48k
 * bss          |
 * +------------+  0x00410020
 * kernel:      |
 * data         |
 * end
 * |(heap)      |
 * .            .  256k - 64k - 32
 * .            .
 * |(heap limit)|
 *
 * |- - - - - - |
 * stack bottom |
 * +------------+
 *
 * +------------+  0x0000000
 * |Bootloader  |
 * |            |  64k
 * +------------+  0x0010000
 * |   Para 1   |   4k
 * +------------+
 * |   Para 2   |   4k
 * +------------+  0x0012000
 * |   Para 3   |   4k
 * +------------+  0x0013000
 * +------------+  0x0013200
 * |vectors     |
 * |            |
 * |------------+
 * |text        |
 * |data        |  kernel:399k
 * |            |
 * +------------+  0x0077000
 * +------------+  0x0077440
 * |text        |
 * |data        |  framework:250k
 * |            |
 * +------------+  0x00B6000
 * +------------+  0X00B6380
 * |text        |
 * |data        |  app:55k
 * |            |
 * +------------+  0x00c4000
 * +------------+  0x00c4080
 * |            |
 * |            |
 * |  OTA TEMP  |
 * |            |  240k
 * |            |
 * |            |
 * +------------+  0x100000
*/

/* Framework's phy start address is 0x0077440.
 * Flash size = 0x00B6000-0x0077440 = 250K
 * Code size = 250K*32/34=235K
 */

/* Split memory into area for vectors and ram */
MEMORY
{
	flash  (rx) : ORIGIN = 0x0070400, LENGTH = 235K
	ram   (rwx): ORIGIN = 0x00404020, LENGTH = 48k
}

#endif

