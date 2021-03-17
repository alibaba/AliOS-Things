
import netmgr as nm
import sys

nm.init()
if(len(sys.argv) == 3):
    nm.connect(sys.argv[1],sys.argv[2])
else:
    nm.connect("linklab-wifi-1","eagle402")

