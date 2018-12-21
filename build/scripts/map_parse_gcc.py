#!/usr/bin/python

import sys
import re

def get_mem_info(map_file):
    total_ram = 0
    total_rom = 0
    map_lines = []
    with open(map_file, 'r') as f:
        s = f.read()

        # find the memory configuration
        mem_config_list = re.findall('Memory Configuration\r?\n\r?\nName             Origin             Length             Attributes\r?\n([\s\S]+)\r?\nLinker script and memory map', s)
        mem_config_text =  '' if not mem_config_list else mem_config_list[0]
        if not mem_config_text:
            print 'Can\'t parse memory configure, memory info get fail!'
            return

        # find the ROM configuration
        rom_config_text = re.findall('\w+\s+(0x\w+)\s+(0x\w+)\s+(?:xr|r)\r?\n',mem_config_text)
        # get every ROM configuration's start - end address
        rom_config = []
        for rom in rom_config_text:
            rom_config += [{'start':int(rom[0], 16), 'end':int(rom[0], 16) + int(rom[1], 16)}]

        # find the RAM configuration
        ram_config_text = re.findall('\w+\s+(0x\w+)\s+(0x\w+)\s+(?:xrw|rw)\r?\n',mem_config_text)
        if (len(ram_config_text)+len(rom_config_text)) == 0:
            ram_config_text = re.findall('\*default\*\s+(0x\w+)\s+(0x\w+)\r?\n',mem_config_text)
            print ('no definite address hint,using default mem configuration')
        # get every RAM configuration's  start - end address
        ram_config = []
        for ram in ram_config_text:
            ram_config += [{'start':int(ram[0], 16), 'end':int(ram[0], 16) + int(ram[1], 16)}]

        # find memory map (without discard and debug sections)
        mem_map_list = re.findall('Linker script and memory map([\s\S]+?)OUTPUT\('+map_file.replace('.map','.elf'), s)
        mem_map = '' if not mem_map_list else mem_map_list[0]
        if not mem_map:
            print 'Can\'t parse memory info, memory info get fail!'
            return

        mem_map = mem_map.replace('\r', '')
        modules = []
        for l in mem_map.split('\n'):
            m = re.search('0x\w+\s+0x\w+\s+.+?([^/\\\]+\.[ao])\((.+\.o)\)', l)
            if m == None:
                continue
            modules.append(m.groups()[0])
        modules = list(set(modules))

        # find sections address - length in memory map
        # modules = list(set(item[0] for item in re.findall('0x\w+\s+0x\w+\s+.+?([^/\\\]+\.[ao])(\(.+\.o\))?\r?\n', mem_map)))
        modules.sort(key = lambda x : x.upper())
        modules += ['*fill*']

        for module in modules:
            rom_size = 0
            ram_size = 0
            module = module.replace('+', '\+')
            # get module's sections's address and size
            if(module == '*fill*'):
                sections = map(lambda arg : {'address':int(arg[0], 16), 'size':int(arg[1], 16)}, re.findall('\*fill\*[ \t]+(0x\w+)[ \t]+(0x\w+)[ \t]+\r?\n', mem_map))
            else:
                sections = map(lambda arg : {'address':int(arg[0], 16), 'size':int(arg[1], 16)}, re.findall('(0x\w+)[ \t]+(0x\w+)[ \t]+.+[/\\\]'+module+'(\(.+\.o\))?\r?\n', mem_map))
            if(not sections):
                continue

            def ram_size_def(arg):
                for ram_info in ram_config:
                    if(ram_info['start'] <= arg['address'] < ram_info['end']):
                        return arg['size']
                return 0

            def rom_size_def(arg):
                for rom_info in rom_config:
                    if(rom_info['start'] <= arg['address'] < rom_info['end']):
                        return arg['size']
                return 0

            ram_size = reduce(lambda x,y:x+y, map(ram_size_def, sections))
            rom_size = reduce(lambda x,y:x+y, map(rom_size_def, sections))

            total_ram += ram_size
            total_rom += rom_size

            map_lines.append('| %-40s | %-8d  | %-8d |'%(re.sub('\.[ao]','',module)[:40],rom_size,ram_size))

    print '\n                        AOS MEMORY MAP                            '
    print '|=================================================================|'
    print '| %-40s | %-8s  | %-8s |'%('MODULE','ROM','RAM')
    print '|=================================================================|'
    for line in map_lines:
        print line
    print '|=================================================================|'
    print '| %-40s | %-8d  | %-8d |'%('TOTAL (bytes)', total_rom, total_ram)
    print '|=================================================================|'

def main():
    get_mem_info(sys.argv[1])

if __name__ == "__main__":
    main()
