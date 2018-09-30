#!/usr/bin/env python
# -*- coding: utf-8 -*-

import time
import datetime
import sys
import argparse
import logging
import os
import binascii
import struct


#
# MAIN top level application entry point
#
if __name__ == '__main__':
	parser = argparse.ArgumentParser()
	parser.add_argument('--debug', action='store_true')
	parser.add_argument('--infile', action='store')
	args = parser.parse_args()

	if args.debug:
		debug_level = logging.DEBUG
	else:
		debug_level = logging.ERROR
	FORMAT = '%(asctime)s :: %(levelname)s :: %(name)s :: %(funcName)s :: %(message)s'
	logging.basicConfig(level=debug_level, format=FORMAT)
	log = logging.getLogger(__name__)

	infile = os.path.abspath(args.infile)
	outfile = infile[:-4] + "_crc" + infile[-4:]
	log.info("IN  file: %s", infile)
	log.info("OUT file: %s", outfile)
	buf = open(args.infile,'rb').read()

	while len(buf) % 4 > 0:
		buf += struct.pack('<B', 0xFF)
		log.info("Padding 0xFF")

	log.info("File Length : %s", len(buf))
	crc32 = (binascii.crc32(buf) & 0xFFFFFFFF)
	log.info("Computed crc : %s",crc32)
	print("Computed CRC 0x{0:08X}".format(crc32))
	out = open(outfile,'wb')
	out.write(buf)
	record = struct.pack('<L',crc32)
	out.write(record)
	out.close()


