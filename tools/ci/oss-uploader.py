#!/usr/bin/env python

import sys
import oss2

argc = len(sys.argv)

def msg_info():
	print 'Usage:\n\t', sys.argv[0], 'src dst'

if argc != 5:
	msg_info()
	exit()

k = sys.argv[1]
s = sys.argv[2]
src = sys.argv[3]
dst = sys.argv[4]

print src + ' -> oss:' + dst

endpoint = 'oss-cn-hangzhou.aliyuncs.com'

auth = oss2.Auth(k, s)
bucket = oss2.Bucket(auth, endpoint, 'alios-things-image')
bucket.put_object_from_file(dst, src, headers=None, progress_callback=None)
