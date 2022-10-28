#!/usr/bin/python3

import sys
import os

error          = 1


def parseLog(filename):

    inFileName  = filename
    outFileName = os.path.splitext(inFileName)[0] + '_parsed' + os.path.splitext(inFileName)[1]

    infile = open(inFileName).read()
    infile = infile.split('\n')

    outfile = open(outFileName, 'w')

    count = 0
    strName = ""
    strNr = -1
    strFUT = ""
    coverageInfo = 0

    for line in infile:
        if line.find("==================================================") != -1:
            continue
        if line.find("--------------------------------------------------") != -1:
            continue
        if line.find("Start: Group") != -1:
            outfile.write("\n")
            continue
        if line.find("End: Group") != -1:
            outfile.write("\n")
            continue
        if line.find("Start: Test") != -1:
            outfile.write("\n")
            continue
        if line.find("End: Test") != -1:
            outfile.write("\n")
            continue
        if line.find("Start Dump: String") != -1:
            continue
        if line.find("End Dump: String")   != -1:
            strName = strName.rstrip("\n")
            outfile.write(strName)
            if strNr == 3:
                strFUT = strName
#            else:
#                strFUT == ""
            if   strName == "Group Name:":
                strNr = 1
                outfile.write("  ")
            elif strName == "Test Name:":
                strNr = 2
                outfile.write("  ")
            elif strName == "Function Under Test:":
                strNr = 3
                outfile.write("  ")
            else:
                strNr = 4
                if len(strName) < 128:
                    outfile.write("\n")
            strName = ""
            continue
        if line.find("Start: Coverage Information") != -1:
            coverageInfo = 1
            outfile.write(line)
            outfile.write("\n")
        if line.find("End: Coverage Information")   != -1:
            strFUT == ""
            coverageInfo = 0
        if coverageInfo == 1:
#            if line.find(strFUT) == -1: #this line contains no relevant coverage info
#                continue
            if line.find("- 0%") == -1 and line.find("src") == -1 and line.find("Functions") != -1:
                outfile.write(line + "\n")
            continue
        if line.find("0x") == 0: #this is a line to translate
            line = line[12:35] + line[37:61]
            nums = line.split(' ')
            for num in nums:
                intNum = int(num, base=16)
#                if intNum == 10:
#                    continue
                if intNum == 0:
                    continue
                strName += str(chr(intNum))
            continue
        outfile.write(line)
        outfile.write("\n")

def print_usage(sys_argv):
    script_name    = sys_argv[0]
    usage_str      = "Syntax: {0} filename\n"

    print (usage_str)

def exit_on_error(sys_argv):
    print_usage(sys_argv)
    exit(1)

if __name__ == '__main__':
    arg_len = len(sys.argv)

    if arg_len != 2:
        exit_on_error(sys.argv)

    if error == parseLog(sys.argv[1]):
        exit_on_error(sys.argv)
