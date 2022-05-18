#!/usr/bin/env python
import json
import sys

if len(sys.argv) > 1:
    filename = sys.argv[1]
else:
    filename = './result.json'


def get_key(item):
    ds = item[0].split('_')
    no = int(ds[1]) * 10000 + int(ds[2]) * 100 + int(ds[3])
    ro = ds[0]
    return '%d-%s' % (no, ro)


result = json.load(open(filename, 'r'))
o = open('./result.csv', 'w')
o.write('Case,Status,Started,Stopped,Reason\n')
for k, v in sorted(result.items(), key=get_key):
    o.write('%s,%s,%s,%s,%s\n' % (
        k,
        (v['passed'] and 'Pass') or 'Fail',
        v['started'],
        v['stopped'],
        (v['error'] or '').replace('\n', ' '),
    ))
