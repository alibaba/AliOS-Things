
#ifdef MBEDTLS_IN_KERNEL

/*
 * +------------+  0x00040000
 * Vect redirect   32
 * +------------+
 *
 * +------------+  0x00400020
 * data         |  64k
 * bss          |
 * +------------+  0x00410020
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
 * |data        |  kernel:399k
 * |            |
 * +------------+  0x0077000
 * +------------+  0x0077440
 * |text        |
 * |data        |  app:147K
 * |            |
 * +------------+  0x009c000
 * +------------+  0x009c300
 * |            |
 * |            |
 * |  OTA TEMP  |
 * |            |  399k
 * |            |
 * |            |
 * +------------+  0x100000
*/

/* Application's phy start address is 0x006E3C0.
 * Flash size = 0x009c000-0x0077440 = 147K
 * Code size = 147K*32/34=138K
 */

/* Split memory into area for vectors and ram */
MEMORY
{
	flash  (rx) : ORIGIN = 0x0070400, LENGTH = 138K
	ram   (rwx): ORIGIN = 0x00400020, LENGTH = 64k
}

#else

/*
 * +------------+  0x00400000
 * Vect redirect   32
 * +------------+
 *
 * +------------+  0x00400020
 * data         |  64k
 * bss          |
 * +------------+  0x00410020
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
 * |data        |  kernel:363k
 * |            |
 * +------------+  0x006E000
 * +------------+  0x006E3C0
 * |text        |
 * |data        |  app:219K
 * |            |
 * +------------+  0x00A5000
 * +------------+  0x00A5380

 * |            |
 * |            |
 * |  OTA TEMP  |
 * |            |  363k
 * |            |
 * |            |
 * +------------+  0x100000
*/

/* kernel's phy start address is 0x006E3C0.
 * Flash size = 0x00A5000-0x006E3C0 = 219K
 * Code size = 219K*32/34=206K
 */

 /* Split memory into area for vectors and ram */
MEMORY
{
     flash  (rx) : ORIGIN = 0x0067C00, LENGTH = 206K
     ram   (rwx): ORIGIN = 0x00400020, LENGTH = 64k
}
#endif

