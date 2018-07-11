/*
 * +------------+  0x0040000
 * Vect redirect   32
 * +------------+
 *
 * +------------+  0x00400020
 * data         |
 * end
 * |(heap)      |
 * .            .
 * .            .
 * |(heap limit)|
 *
 * |- - - - - - |
 * stack bottom    256k
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
 * |vectors     |
 * |            |
 * |------------+
 * |text        |
 * |data        |  948k
 * |            |
 * +------------+  0xA1000
 * |            |
 * |            |
 * |  OTA TEMP  |
 * |            |  664k
 * |            |
 * |            |
 * +------------+  0x100000
 */

/* Application's phy start address is 0x13200.
 * Flash size = 0xA1000-0x13200 = 0x8DE00
 * Code size = 0x8DE00*32/34=546936
 */

/* Split memory into area for vectors and ram */
MEMORY
{
	flash  (rx) : ORIGIN = 0x0012000, LENGTH = 948K
	ram   (rwx): ORIGIN = 0x00400020, LENGTH = 256k - 32
}
