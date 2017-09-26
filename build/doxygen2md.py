#!/usr/bin/env python

import os
import re

def annotation_get(f):

    fd = open(f, 'r')
    code = fd.read()
    fd.close()

    return re.findall(r'/\*\*.*?\);', code, re.DOTALL)

def annotation_analyse(num, annotation, templet):

    templet = templet.replace("FUNC_NUM", str(num))

    func = re.findall(r'\*/.*?\);', annotation, re.DOTALL)
    func_name = (func[0].replace("const ", "").replace("long long", "long").partition(" "))[2].partition("(")[0].replace("*", "")
    templet = templet.replace("FUNC_NAME", func_name)

    function = (re.findall(r'\*\/\n(.*?);', func[0], re.DOTALL))[0]
    templet = templet.replace("FUNCTION", function)

    if annotation.find("@") == -1:
        func_desc = ((re.findall(r'(/\*\*\n \* )(.*?)(\n \*/)', annotation, re.DOTALL))[0][1]).replace(" *", " ")
        templet = templet.replace("FUNC_DESC", func_desc)
        templet = templet.replace("PARAM_DESC", "None.\n")
        templet = templet.replace("RETURN_DESC", "None.\n")
    else:
        func_desc = ((re.findall(r'(/\*\*\n \* )(.*?)(\n \*\n \* @)', annotation, re.DOTALL))[0][1]).replace(" *", " ")
        templet = templet.replace("FUNC_DESC", func_desc)

        if annotation.find("@param") == -1:
            templet = templet.replace("PARAM_DESC", "None.\n")
            if annotation.find("@ret") == -1:
                templet = templet.replace("RETURN_DESC", "None.\n")
            else:
                func_return = ((re.findall(r'(@return  )(.*?)(\n \*/)', annotation, re.DOTALL))[0][1]).replace(" *           ", "")
                templet = templet.replace("RETURN_DESC", func_return)
        else:
            templet_head = (templet.partition("  PARAM_DESC"))[0]
            templet_mid  = "  | IN/OUT |  NAME  |  DESC  |\n  |--------|--------|--------|\n"
            templet_tail = (templet.partition("  PARAM_DESC"))[2]

            if annotation.find("@ret") == -1:
                func_params = (re.findall(r'(@param.*?)\n \*/', annotation, re.DOTALL))[0].replace("\n *", " ").replace("           ", "").split("@param")
            else:
                func_params = (re.findall(r'(@param.*?)\n \*\n \* @', annotation, re.DOTALL))[0].replace("\n *", " ").replace("           ", "").split("@param")

            del func_params[0]
            for param in func_params:
                params = "  | " + param.replace("  ", " | ", 2) + " |"
                templet_mid = templet_mid + params + "\n"

            templet = templet_head + templet_mid + templet_tail

            if annotation.find("@ret") == -1:
                templet = templet.replace("RETURN_DESC", "None.\n")
            else:
                func_return = ((re.findall(r'(@return  )(.*?)(\n \*/)', annotation, re.DOTALL))[0][1]).replace(" *           ", "")
                templet = templet.replace("RETURN_DESC", func_return)

    return templet

def doxygen2md(f, templet):
    index = ""

    fd = open("./out/" + os.path.basename(f) + ".md", 'w+')

    num = 0
    for annotation in annotation_get(f):
        num += 1
        md = annotation_analyse(num, annotation, templet)
        fd.write(md)
        index = index + "  * [" + ((((md.split("\n\n"))[0]).split(" "))[1]).replace("-", " ") + "]" + "(#" + (((md.split("\n\n"))[0]).split(" "))[1] + ")\n"

    fd.seek(0)
    md = fd.read()
    fd.seek(0)
    fd.write("# API INDEX\n\n")
    fd.write(index)
    fd.write("\n------\n\n")
    fd.write(md)

    fd.close()

def main():
    fs = []

    for root, dirs, files in os.walk('./include'):
        for f in files:
            if (os.path.splitext(f)[1] == ".h"):
                fs.append(os.path.join(root, f))

    fd = open("./build/MD.templet", 'r')
    templet = fd.read()
    fd.close()

    print "------------------------"

    for f in fs:
        print f
        doxygen2md(f, templet)

    print "------------------------"

if __name__ == '__main__':
    main()

