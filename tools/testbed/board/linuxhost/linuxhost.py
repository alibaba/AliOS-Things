import os, sys, time, serial, subprocess, traceback, pty

def list_devices(os):
    return []
    return ['/dev/tty20', '/dev/tty21']

def new_device(port):
    return open(port, 'rb+')

def erase(port):
    error = 'fail'
    return error

def program(port, address, file):
    error = 'fail'
    return error

def control(port, operation):
    ret = 'fail'
    return ret
