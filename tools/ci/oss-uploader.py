#!/usr/bin/env python

import sys
from subprocess import call
import base64
import oss2

argc = len(sys.argv)

def msg_info():
	print 'Usage:\n\t', sys.argv[0], 'src dst'

if argc != 3:
	msg_info()
	exit()

src = sys.argv[1]
dst = sys.argv[2]

print src + ' -> oss:' + dst

endpoint = 'oss-cn-shanghai.aliyuncs.com'

# you won't break it, so we put it here ;)
auth = oss2.Auth(base64.b64decode('TFRBSUpxTmhheVJaS3FrTg=='), base64.b64decode('V3czY2JLeGZtcmFvc3RQb0xGYnRoSkVRUTZNV2Vq'))
bucket = oss2.Bucket(auth, endpoint, 'binaries')
bucket.put_object_from_file(dst, src, headers=None, progress_callback=None)
