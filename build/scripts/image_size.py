#! /usr/bin/env python

import os, sys, getopt

def print_usage():
    print ""
    print "Usage:"
    print sys.argv[0]
    print " Usage: return size of the input file"
    sys.stdout.flush()

def main():
    
    if not len(sys.argv)==2:
        print_usage()
        sys.exit(2)

    IMAGE_FILE = sys.argv[1]

    if not os.path.isfile(IMAGE_FILE):
        print "Input a iamge file!"
        sys.stdout.flush()
        sys.exit()


    if not os.path.exists(IMAGE_FILE):
        print "Input image file not exist!"
        sys.stdout.flush()
        sys.exit()

    image_size = os.path.getsize(IMAGE_FILE)
    print image_size
    sys.stdout.flush()
    sys.exit(0)

if __name__ == "__main__":
    try:
        main()
    except KeyboardInterrupt:
        pass
