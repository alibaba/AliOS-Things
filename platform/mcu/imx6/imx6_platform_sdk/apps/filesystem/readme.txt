filesystem application
======================

Simple performance test for the FAT32 filesystem on an SD card.


Description
-----------

This applications writes and reads files from a FAT formatted SD card in slot SD3
and displays performance statistics. Files are accessed using various chunk sizes from
512B to 1MB. Both buffered (filesystem) and raw read/write is tested. Both PIO and
ADMA modes are tested.


Requirements
------------

Requires a FAT formatted SD card in SD3. For the write test, files named TESTOUTx.DAT
are created. For the read test, a file named TESTIIN.DAT is opened. If TESTIN.DAT does not
exist it is created.


Build options
-------------

None.


Code organization
-----------------

This application contains one source file: fs_test.c. To enable extra debug messages,
define FS_DEBUG in this file.
