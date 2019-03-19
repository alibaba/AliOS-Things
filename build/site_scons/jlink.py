#!/usr/bin/env python
#
# Tool for run jlink execute, used by aos upload/debug
#   ./jlink.py -h
#

import sys, os
import getopt
import platform
import subprocess

VERSION = "0.0.1"
device = "nRF52840_xxAA"
interface = "swd"
speed = "4000k"
loadfile = None
address = "0x00010000"
jlink_cmdfile = ""

def usage():
    print VERSION
    print "-v | --version   version"
    print "-h | --help      help"
    print "-d | --device    select target device"
    print "-i | --if        configures the target interface., jtag or swd"
    print "-s | --speed     configures the target interface speed."
    print "-f | --loadfile  load the binary or other files"
    print "-p | --addr      address of binary"

def gen_jlink_cmdfile():
    global loadfile, address, jlink_cmdfile

    dirname = os.path.dirname(os.path.abspath(loadfile))
    jlink_cmdfile = os.path.join(dirname, "%s.jlink" % device)
    print "gen jlink commands file: %s" % jlink_cmdfile

    try:
        f = open(jlink_cmdfile, 'w')
        f.write("h\n")
        f.write("loadfile %s %s\n" % (loadfile, address))
        f.write("g\n")
        f.write("qc")
    except IOError:
        print IOError  # will print something like "option -a not recognized"
        sys.exit(2)

def jlink_run():
    global device, interface, speed, jlink_cmdfile
    
    host_os = platform.system()
    print "host os: %s" % host_os
    gen_jlink_cmdfile()

    if host_os == 'Windows':
        jlink_exe = "JLink.exe"

    elif host_os == 'Linux':
        jlink_exe = "JLinkExe"
    elif host_os == 'Darwin':
        jlink_exe = "JLinkExe"
    else:
        print "unsupport platform"
        sys.exit(2)

    exec_cmd = "%s -device %s -if %s -speed %s -CommanderScript %s" % (jlink_exe, device, interface, speed, jlink_cmdfile)
    print "execute cmd: %s" % exec_cmd

    subprocess.call(exec_cmd, shell=True)

def check_args():
    # check args
    if interface != "swd" and interface != "jtag":
        print "interface is unrecognized"
        sys.exit(2)
    
    if loadfile == None:
        print "loadfile must be set"
        sys.exit(2)

    if not os.path.isfile(loadfile):
        print "loadfile is not exist"
        sys.exit(2)

def getopts():

    global device, interface, speed, address, loadfile
    try:
        opts, args = getopt.getopt(sys.argv[1:],"vhd:i:s:f:p:",["version", "help", "device=","if=","speed=","loadfile=","addr="])
    except getopt.GetoptError as err:
        # print help information and exit:
        print str(err)  # will print something like "option -a not recognized"
        usage()
        sys.exit(2)

    verbose = False
    #print opts
    for o, a in opts:
        #print o, a
        if o in ("-v", "--version"):
            print VERSION
        elif o in ("-h", "--help"):
            usage()
            sys.exit()
        elif o in ("-d", "--device"):
            device = a
            print "set device: %s" % device
        elif o in ("-i", "--if"):
            interface = a
            print "set interface: %s" % interface
        elif o in ("-s", "--speed"):
            speed = a
            print "set speed: %s" % speed
        elif o in ("-f", "--loadfile"):
            loadfile = a
            print "set loadfile: %s" % loadfile
        elif o in ("-p", "--addr"):
            address = a
            print "set address: %s" % address
        else:
            assert False, "unhandled option"
    
def main():

    getopts()
    check_args()
    jlink_run()

if __name__ == "__main__":
    main()
