/*
 * flash(physical addr, flash driver perspective):
 * +------------+  0x00000000
 * |Bootloader  |   64k
 * |            |
 * +------------+  0x00010000
 * |Para 1      |   4k
 * +------------+
 * |Para 2      |   4k
 * +------------+
 * |Para 3      |   4k
 * +------------+  0x00013000
 * |rec text    |   16k
 * +------------+  0x00017000
 * |rec flags   |   4k
 * +------------+  0x00018000
 * |vectors     |
 * |text        |   928k
 * |data        |
 * |            |
 * +------------+  0x000A1000
 * |OTA temp    |
 * |            |   948k
 * |            |
 * +------------+  0x00100000
 *
 * flash(logical addr, mem pointer perspective):
 * +------------+  0x00000000
 * |Bootloader  |   64k
 * |Para 1 2 3  |
 * +------------+
 * | NULL       |   (phy: 0x13000~0x13200)
 * +------------+  0x00012000
 * |rec text    |   14.5k  (phy: 0x13200~0x16FA0)
 * +------------+  0x00015A00
 * | NULL       |   0.5k
 * +------------+  0x00015C00
 * |rec flags   |   3k  (phy: 0x171C0~0x17E80)
 * +------------+  0x00016800
 * | NULL       |   1k
 * +------------+  0x00016c00
 * |vectors     |
 * |text        |   515k (phy: 0x182C0~0xA0F80)
 * |data        |
 * |            |
 * +------------+  0x00097800
 * | NULL       |
 * +------------+
 * |OTA temp    |
 * |            |
 * |            |
 * +------------+
 *
 * ram
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
 */

/* Split memory into area for vectors and ram */
MEMORY
{
   flash  (rx) : ORIGIN = 0x1a5c0, LENGTH = 892K
   ram   (rwx): ORIGIN = 0x00400020, LENGTH = 256k-32
}
