#!/usr/bin/python

import sys
import re

def get_mem_info(map_file):
    total_ro = 0
    total_rw = 0
    map_lines = []
    with open(map_file, 'r') as f:
        s = f.read()
        # find memory map (without discard and debug sections)
        mem_map_list = re.findall('Linker script and memory map([\s\S]+?)OUTPUT\('+map_file.replace('.map','.elf'), s)
        mem_map = '' if not mem_map_list else mem_map_list[0]
        if not mem_map:
            print 'Can\'t parse memory info, memory info get fail!'
            return

        mem_map = mem_map.replace('\r', '')
        modules = []
        for l in mem_map.split('\n'):
            m = re.search(r'0x\w+\s+0x\w+\s+.+?([^/\\]+\.[ao])\((.+\.o)\)', l)
            if m == None:
                continue
            modules.append(m.groups()[0])
        modules = list(set(modules))

        # find sections address - length in memory map
        # modules = list(set(item[0] for item in re.findall('0x\w+\s+0x\w+\s+.+?([^/\\\]+\.[ao])(\(.+\.o\))?\r?\n', mem_map)))
        modules.sort(key = lambda x : x.upper())

        for module in modules:
            module = module.replace('+', r'\+')

            # get module's sections's address and size
            sec_text    = map(lambda arg : {'address':int(arg[0], 16), 'size':int(arg[1], 16)}, re.findall(r'\.text\.\S+\n?\s+(0x\w+)[ \t]+(0x\w+)[ \t]+.+[/\\]'+module+r'(\(.+\.o\))?\r?\n', mem_map))
            sec_rodata  = map(lambda arg : {'address':int(arg[0], 16), 'size':int(arg[1], 16)}, re.findall(r'\.rodata\.\S+\n?\s+(0x\w+)[ \t]+(0x\w+)[ \t]+.+[/\\]'+module+r'(\(.+\.o\))?\r?\n', mem_map))
            sec_data    = map(lambda arg : {'address':int(arg[0], 16), 'size':int(arg[1], 16)}, re.findall(r'\.data\.\S+\n?\s+(0x\w+)[ \t]+(0x\w+)[ \t]+.+[/\\]'+module+r'(\(.+\.o\))?\r?\n', mem_map))
            sec_bss     = map(lambda arg : {'address':int(arg[0], 16), 'size':int(arg[1], 16)}, re.findall(r'\.bss\.\S+\n?\s+(0x\w+)[ \t]+(0x\w+)[ \t]+.+[/\\]'+module+r'(\(.+\.o\))?\r?\n', mem_map))
            
            sec_ro      = sec_text + sec_rodata;
            sec_rw      = sec_data + sec_bss;

            ro_size = 0
            rw_size = 0
            for sec in sec_ro:
                ro_size += sec['size']
            for sec in sec_rw:
                rw_size += sec['size']

            total_ro += ro_size
            total_rw += rw_size

            map_lines.append(r'| %-40s | %-8d  | %-8d |'%(re.sub('\.[ao]','',module)[:40],ro_size,rw_size))

    print '\n                        AOS MEMORY MAP                            '
    print '|=================================================================|'
    print '| %-40s | %-8s  | %-8s |'%('MODULE','RO Size','RW Size')
    print '|=================================================================|'
    for line in map_lines:
        print line
    print '|=================================================================|'
    print '| %-40s | %-8d  | %-8d |'%('TOTAL (bytes)', total_ro, total_rw)
    print '|=================================================================|'

def main():
    get_mem_info(sys.argv[1])

if __name__ == "__main__":
    main()
