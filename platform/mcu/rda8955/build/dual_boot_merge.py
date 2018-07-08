#!/usr/bin/python

import os, sys
import re
import optparse

try:
    dict.iteritems
except AttributeError:
    def itervalues(d):
        return iter(d.values())
    def iteritems(d):
        return iter(d.items())
else:
    def itervalues(d):
        return d.itervalues()
    def iteritems(d):
        return d.iteritems()

class LOD(object):

    def __init__(self):
        self.attrnames = []
        self.attrs = {}
        self.blocks = {}

    def checksum(self):
        sum = 0
        for block_base, block_data in iteritems(self.blocks):
            for data in block_data:
                sum += data
        return sum & 0xffffffff

    def load(self, fname):
        try:
            fh = open(fname, 'r')
        except:
            print("failed to open %s for read" % fname)
            return False

        self.attrnames = []
        self.attrs = {}
        self.blocks = {}

        lineno = 0
        block_base_re = re.compile("@([0-9A-Fa-f]{8})")
        block_data_re = re.compile("([0-9A-Fa-f]{8})")
        block_base = None
        block_data = []
        for line in fh.readlines():
            lineno += 1
            if line.startswith("#$"):
                fields = line[2:].strip().split("=")
                if len(fields) != 2:
                    print("LOD syntax error at %s:%d" %(fname, lineno))
                    fh.close()
                    return False
                self.attrnames.append(fields[0])
                self.attrs[fields[0]] = fields[1]
            elif line.startswith("#checksum="):
                if block_base is not None:
                    self.blocks[block_base] = block_data
                    block_base = None
                    block_data = []
                chk = int(line.strip()[10:], 16)
                expected = self.checksum()
                if chk != expected:
                    print("LOD checksum error, read %08x expected %08x" %(chk, expected))
                    fh.close()
                    return False
            elif line.startswith("@"):
                m = block_base_re.search(line)
                if m is None:
                    print("LOD syntax error at %s:%d" %(fname, lineno))
                    fh.close()
                    return False
                if block_base is not None:
                    self.blocks[block_base] = block_data
                    block_base = None
                    block_data = []
                block_base = int(m.group(1), 16)
            else:
                m = block_data_re.search(line)
                if m is None or block_base is None:
                    print("LOD syntax error at %s:%d" %(fname, lineno))
                    fh.close()
                    return False
                block_data.append(int(m.group(1), 16))

        # in case there are no checksum
        if block_base is not None:
            self.blocks[block_base] = block_data
            block_base = None
            block_data = []

        fh.close()
        return True

    def store(self, fname):
        try:
            fh = open(fname, 'w')
        except:
            print("failed to open %s for write" % fname)
            return False

        for attr in self.attrnames:
            fh.write("#$%s=%s\n" %(attr, self.attrs[attr]))
        for block_base in sorted(self.blocks.keys()):
            fh.write("@%08x\n" % block_base)
            for data in self.blocks[block_base]:
                fh.write("%08x\n" % data)
        fh.write("#checksum=%08x\n" % self.checksum())
        fh.close()
        return True

    def end_address(self):
        ea = 0
        for block_base, block_data in iteritems(self.blocks):
            bea = block_base + len(block_data) *4
            if bea > ea:
                ea = bea
        return ea

    def start_address(self):
        return sorted(self.blocks.keys())[0]

def load_lod(fname):
    lod = LOD()
    if lod.load(fname):
        return lod
    return None

def dual_merge(bl, lod):
    if 'BOOT_SECTOR' not in bl.attrnames:
        print('BOOT_SECTOR not in bootloader')
        return None
    if 'BOOT_SECTOR' not in lod.attrnames:
        print('BOOT_SECTOR not in second LOD')
        return None

    bs1 = int(bl.attrs['BOOT_SECTOR'], 0) | 0x08000000
    bs2 = int(lod.attrs['BOOT_SECTOR'], 0) | 0x08000000
    if bs1 not in bl.blocks.keys():
        print('BOOT_SECTOR data not in bootloader')
        return None
    if bs2 not in lod.blocks.keys():
        print('BOOT_SECTOR data not in second LOD')
        return None

    sa1 = bl.start_address()
    sa2 = lod.start_address()
    if bs1 != sa1:
        print('BOOT_SECTOR of bootloader is not the start address')
        return None
    if bs2 != sa2:
        print('BOOT_SECTOR of second LOD is not the start address')
        return None

    ea1 = bl.end_address()
    if ea1 > sa2:
        print('BOOT_SECTOR of second LOD inside bootloader')
        return None

    output = LOD()
    output.blocks = lod.blocks
    output.blocks[bs2][0] = 0xD9EF0045

    pa = ea1
    while pa < sa2:
        block_base = pa
        block_size = sa2 - pa
        if pa % 0x10000 == 0 and block_size >= 0x10000:
            block_size = 0x10000
        elif pa % 0x1000 == 0 and block_size >= 0x1000:
            block_size = 0x1000
        else:
            print('failed to padding at 0x%x, size %d' % (block_base, block_size))
            return None

        block_data = [0xffffffff] * int(block_size/4)
        output.blocks[block_base] = block_data
        pa += block_size

    output.attrnames = lod.attrnames
    for attr in output.attrnames:
        output.attrs[attr] = lod.attrs[attr]
    output.attrs['BOOT_SECTOR'] = bl.attrs['BOOT_SECTOR']
    output.blocks.update(bl.blocks)
    return output

def main(argv):
    opt = optparse.OptionParser(usage="""%prog [options]

This utility will merge 2 LODs of dual boot into one LOD. During merge:
* check BOOT_SECTOR of both inputs;
* BOOT_SECTOR of the output LOD will follow bootloader LOD;
* first word of the 2nd LOD will be changed to magic number;
* padding from bootloader to second LOD;
* checksum will be regenerated.
""")

    opt.add_option("--bl", action="store", dest="bl",
                   help="bootloader LOD file name")
    opt.add_option("--lod", action="store", dest="lod",
                   help="second LOD file name")
    opt.add_option("--output", action="store", dest="output",
                   help="output LOD file name")

    opt, argv = opt.parse_args(argv)

    if opt.bl is None:
        print("bootloader LOD file name is not specified!")
        return 1
    if opt.lod is None:
        print("second LOD file name is not specified!")
        return 1
    if opt.output is None:
        print("output LOD file name is not specified!")
        return 1

    bl = load_lod(opt.bl)
    if bl is None:
        return 1
    lod = load_lod(opt.lod)
    if lod is None:
        return 1

    output = dual_merge(bl, lod)
    if output is None:
        return 1

    if not output.store(opt.output):
        return 1

    return 0


if __name__ == '__main__':
    sys.exit(main(sys.argv[1:]))
