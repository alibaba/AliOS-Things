#!/usr/bin/python

"Test spiffs filesystem with a range of parameters"

import spiffs

class SpiffsInstr(spiffs.SpiffsCharsBack):
    "Keeps count of issued reads / writes / erases"
    def __init__(self, *args, **kwargs):
        self.reset_counters()
        supe = super(SpiffsInstr, self)

        self.super_read = supe.on_read
        self.super_write = supe.on_write
        self.super_erase = supe.on_erase

        supe.__init__(*args, **kwargs)

    def reset_counters(self):
        self.read_requests = []
        self.write_requests = []
        self.erase_requests = []

    def on_read(self, addr, size):
        self.read_requests.append([addr, size])
        return self.super_read(addr, size)
    def on_write(self, addr, data):
        self.write_requests.append([addr, len(data)])
        return self.super_write(addr, data)
    def on_erase(self, addr, size):
        self.erase_requests.append([addr, size])
        return self.super_erase(addr, size)

# Physical parameters
flash_size = 8*1024*1024
erase_size = 256

print "filesystem size =",flash_size

header = "log_block_size","log_page_size","reads","read_bytes","writes","written_bytes"
print '| %s |'%(' | '.join(header))
header2 = ['-'*len(x) for x in header]
print '| %s |'%(' | '.join(header2))

for log2_log_block_size in range(14,19):
    log_block_size = 1<<log2_log_block_size
    for log2_log_page_size in range(8,11):
        log_page_size = 1<<log2_log_page_size

        backing = ['\xff']*flash_size

        s = SpiffsInstr(backing,
                        0,
                        erase_size,
                        log_page_size,
                        log_block_size)

        s.dir()

        #for i in range(100):
        #    with s.open("Test File %d.txt"%i,"w") as fd:
        #        fd.write('.'*1024*10)

        s.unmount()

        result = (log_block_size,log_page_size,
                  len(s.read_requests), sum(size for block,size in s.read_requests),
                  len(s.write_requests), sum(size for block,size in s.write_requests))

        print '| %s |'%(' | '.join(map(str,result)))
