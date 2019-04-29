/* user1.bin @ 0xC000 */

/* Flash Map (512KB + 512KB), support 1MB/2MB/4MB SPI Flash */
/* |..|........................|.....|.....|..|........................|.....|....|                       */
/* ^  ^                        ^     ^     ^  ^                        ^     ^                            */
/* |_boot start(0x0000)        |     |     |_pad start(0x80000)        |     |                            */
/*    |_user1 start(0x1000)    |_user1 end    |_user2 start(0x81000)   |_user2 end                        */
/*                                   |_system param symmetric area(0x7b000)  |_system param area(0xfb000) */

/* NOTICE: */ 
/* 1. You can change irom0 len, but MUST make sure user1 end not overlap system param symmetric area. */
/* 2. Space between user1 end and pad start can be used as user param area.                           */
/* 3. Don't change any other seg.                                                                     */

MEMORY
{
  dport0_0_seg (rw) :      org = 0x3FF00000, len = 0x10
  dram0_0_seg  (rw) :      org = 0x3FFE8000, len = 0x18000
  iram1_0_seg  (rx) :      org = 0x40100000, len = 0x8000
  irom0_0_seg  (rx) :      org = 0x40206010, len = 0xDA000
}

INCLUDE "../ld/eagle.app.v6.common.ld"

_heap_size = 0x40000000 - _heap_start;
