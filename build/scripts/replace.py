#!/usr/local/bin/python
#coding:gbk
import re

def replace_ip():
        old_file='./out/export/linkkit-sdk-c/src/infra/system/guider.c'
        w_str=""
        with open(old_file, 'r') as f:
            target1='10.125.3.189'
            target2='100.67.80.75'
            target3='106.15.166.168'
            target4='10.125.7.82'
            targetList = (target1, target2, target3, target4)
            new_ip='10.10.10.10'

            for line in f:
                for k, v in enumerate(targetList):
                    if re.search(v, line):
                        line=re.sub(v, new_ip, line)
                w_str+=line
            f.close()

        with open(old_file, 'w') as of:
            of.write(w_str)
            of.close()
