#!/usr/bin/env python
# -*- coding: UTF-8 -*-
import sys, os

compdirs = [
    ['middleware/linkkit/sdk-c', ''],
    ['utility/libc/compilers/armlibc', 'prebuilt/extra/stdc/armlibc'],
    ['utility/libc/compilers/iar', 'prebuilt/extra/stdc/iarlibc'],
]

# Example file list
examples = ['linkkitapp', 'linkkit_gateway', 'mqttapp', 'coapapp', 'http2app']
example_files = []

for example in examples:
    if example.startswith('linkkit'):
        dist_example_dir = "linkkit"
    elif example.startswith('mqtt'):
        dist_example_dir = "mqtt"
    elif example.startswith('coap'):
        dist_example_dir = "coap"
    elif example.startswith('http2'):
        dist_example_dir = "http2"


    for filename in os.listdir('app/example/' + example):
        if (filename.find('example') != -1 and filename.endswith('.c')):
            src = os.path.join('app/example', example, filename)
            dest = os.path.join('examples', dist_example_dir, filename)
            example_files += [[src, dest]]
        elif filename == "deprecated" or filename == "data":
	    for filename2 in os.listdir('app/example/' + example +"/" +filename):
                src = os.path.join('app/example', example, filename, filename2)
                dest = os.path.join('examples', dist_example_dir, filename, filename2)
                example_files += [[src, dest]]

copy_map =  compdirs + example_files

