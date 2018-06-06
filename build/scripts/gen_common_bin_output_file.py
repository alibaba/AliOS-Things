#! /usr/bin/env python

# Load application to ram helper script
# Note: sys.stdout.flush() and sys.stderr.flush() are required for proper
# console output in eclipse

import os, sys, re, struct, platform, getopt, subprocess
from sys import platform as _platform


def print_usage():
    print ""
    print "Usage: Merge a bin file into an exist bin file, create one if target is not exist"
    print sys.argv[0]
    print "Optional Usage:"
    print " [<-o> <target binary file>]"
    print "          The file that will be merged into, create <input binary file>_out if not defined."
    print " [<-f> <the offset>]"
    print "          The input file will be merged into this address. 0x0 if not defined."
    print " [-h | --help]"
    print "          Display usage"
    print " [<input binary file>]"
    print "          The input file need to be merged into the exist file."
    sys.stdout.flush()

def main():
    try:
        opts, args = getopt.getopt(sys.argv[1:], 'o:f:h')
    except getopt.GetoptError as err:
        print str(err)
        print_usage()
        sys.exit(2)

    OFFSET = 0
    OUTPUT_FILE = 0

    if not len(args) == 1:
        print_usage()
        sys.exit(2)
    else:
        INPUT_FILE = args[0]

    if not os.path.exists(INPUT_FILE):
        print "Please input an existed input binary file"
        sys.exit(2)

    for opt, arg in opts:
        if opt == "-o":
            OUTPUT_FILE = arg
        elif opt == "-f":
            OFFSET = int(arg, 16)
        elif opt == "-h":
            print_usage()
            sys.exit()

    if OUTPUT_FILE == 0:
        OUTPUT_FILE = re.sub(r'.bin$', '.output.bin', INPUT_FILE)

    input_file = open(INPUT_FILE, 'rb')
    output_file = open(OUTPUT_FILE, 'ab')

    input = input_file.read()
    gap_szie = OFFSET - os.path.getsize(OUTPUT_FILE)

    if gap_szie > 0:
        output_file.seek(os.path.getsize(OUTPUT_FILE))
        output_file.write('\xFF'*gap_szie)

    output_file.write(input)
    input_file.close()
    output_file.close()


#app = moc_app.read()
# kernel = moc_kernel.read()
    
#  crc = CRC16( bytes(app[8::]) )

#  moc_app_output.write( struct.pack('<L', os.path.getsize(MOC_APP_FILE)-8 ) )
# moc_app_output.write( struct.pack('<HH', crc, crc ) )
# moc_app_output.write( bytes(app[8::]) )

#  moc_ota_output.write( bytes(kernel) )
# moc_ota_output.seek(MOC_APP_OFFSET)
# # moc_ota_output.write( struct.pack('<L', os.path.getsize(MOC_APP_FILE)-8 ) )
#  moc_ota_output.write( struct.pack('<HH', crc, crc ) )
# moc_ota_output.write( bytes(app[8::]) )

#  moc_app.close()
#   moc_kernel.close()
# moc_app_output.close()
#  moc_ota_output.close()


if __name__ == "__main__":
    try:
        main()
    except KeyboardInterrupt:
        pass
