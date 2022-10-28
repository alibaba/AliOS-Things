#!/usr/bin/python3

import sys

toolchain_list = ["ARM", "GCC", "ARMCLANG"]
core_list      = ["cortexM0l", "cortexM3l", "cortexM4l", "cortexM4lf", "cortexM7l", "cortexM7lfsp", "cortexM7lfdp", 
                  "ARMv8MBLl", "ARMv8MMLl", "ARMv8MMLlfsp", "ARMv8MMLlfdp", "ARMv8MMLld", "ARMv8MMLldfsp", "ARMv8MMLldfdp" ]
test_list      = ["MPS2", "FVP", "Simulator"]
error          = 1


def parseLog(toolchain, core, test):
    if toolchain not in toolchain_list:
        print ("Error: Unkown toolchain '{0}'".format(toolchain))
        return error

    if core not in core_list:
        print ("Error: Unkown core '{0}'".format(core))
        return error

    if test not in test_list:
        print ("Error: Unkown test '{0}'".format(test))
        return error

    inFileName  = ".\DspLibTest_{2}\{0}\Logs\DspLibTest_{2}_{1}.log".format(toolchain, core, test)
    outFileName = ".\DspLibTest_{2}\{0}\Logs\DspLibTest_{2}_{1}_parsed.log".format(toolchain, core, test)

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
    usage_str      = "Syntax: {0} toolchain core test\n".format(sys.argv[0])
    argument_desc  = "\n  toolchain: {0}".format(" ".join(toolchain_list))
    argument_desc += "\n  core:      {0}".format(" ".join(core_list))
    argument_desc += "\n  test:      {0}".format(" ".join(test_list))
    argument_desc += "\n\ne.g.: parseLog ARM cortexM3l FVP"

    print (usage_str + argument_desc)

def exit_on_error(sys_argv):
    print_usage(sys_argv)
    exit(1)

if __name__ == '__main__':
    arg_len = len(sys.argv)

    if arg_len != 4:
        exit_on_error(sys.argv)

    if error == parseLog(sys.argv[1], sys.argv[2], sys.argv[3]):
        exit_on_error(sys.argv)
