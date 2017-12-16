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
 * |vectors     |
 * |            |
 * |------------+
 * |text        |  
 * |data        |  632k
 * |            |  
 * +------------+ 
 *
 *
 * +------------+  0x0080000
 * |            |
 * |            |
 * |  OTA TEMP  |
 * |            |  
 * |            |
 * |            |  320
 * +------------+
 */


/* Split memory into area for vectors and ram */
MEMORY
{
	flash  (rx) : ORIGIN = 0x0012000, LENGTH = 632k + 256k
	ram   (rwx): ORIGIN = 0x00400020, LENGTH = 256k - 32
}
