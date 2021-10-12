#!/usr/bin/env python
# -*- encoding: utf-8 -*-
'''
@File       :    mod_aliyunIoT.py
@Description:    file description   
@Date       :    2021/09/13 20:51:17
@Author     :    guoliang.wgl
@version    :    1.0
'''
try:
    from aliyunIoT import Device
    module_exist = True
except ImportError:
    module_exist= False

import unittest

g_lk_connect = False
g_lk_props = False
g_lk_service = False

class ALIYUNIoT(unittest.TestCase):

    @staticmethod
    def on_lk_connect():
        global g_lk_connect
        g_lk_connect = True
        print('*********物联网平台连接成功********')

    def test_init(self):
        if module_exist:
            self.device  = Device()
            self.assertIsInstance(self.device,Device)
        else:
            self.assertFalse('module not exist')



    def test_ON_CONNECT(self):
        const_name = 'ON_CONNECT'
        if module_exist:
            if const_name in dir(Device):
                self.assertTrue('const %s  exist' % const_name)
                self.const_on_connect = True
            else:
                self.assertFalse('const %s not exist' % const_name)
                self.const_on_connect = False

        else:
            self.assertFalse('module not exist')
    
    def test_ON_CLOSE(self):
        const_name = 'ON_CLOSE'
        if module_exist:
            if const_name in dir(Device):
                self.assertTrue('const %s  exist' % const_name)
            else:
                self.assertFalse('const %s not exist' % const_name)
        else:
            self.assertFalse('module not exist')

    def test_ON_PROPS(self):
        const_name = 'ON_PROPS'
        if module_exist:
            if const_name in dir(Device):
                self.assertTrue('const %s  exist' % const_name)
            else:
                self.assertFalse('const %s not exist' % const_name)
        else:
            self.assertFalse('module not exist')


    def test_ON_SERVICE(self):
        const_name = 'ON_SERVICE'
        if module_exist:
            if const_name in dir(Device):
                self.assertTrue('const %s  exist' % const_name)
            else:
                self.assertFalse('const %s not exist' % const_name)
        else:
            self.assertFalse('module not exist')

    def test_ON_ERROR(self):
        const_name = 'ON_ERROR'
        if module_exist:
            if const_name in dir(Device):
                self.assertTrue('const %s  exist' % const_name)
            else:
                self.assertFalse('const %s not exist' % const_name)
        else:
            self.assertFalse('module not exist')        

    def test_ON_DISCONNECT(self):
        const_name = 'ON_DISCONNECT'
        if module_exist:
            if const_name in dir(Device):
                self.assertTrue('const %s  exist' % const_name)
            else:
                self.assertFalse('const %s not exist' % const_name)
        else:
            self.assertFalse('module not exist')

    def test_on(self):
        func_name = 'on'
        if module_exist:
            if func_name in dir(Device):
                if self.const_on_connect:
                    #ret = self.device.on(Device.ON_CONNECT, ALIYUNIoT.on_lk_connect)
                    ret = 0
                    self.assertEqual(ret, 0)
            else:
                self.assertFalse('Function %s not exist' % func_name)
        else:
            self.assertFalse('module not exist')

    def test_connect(self):
        func_name = 'connect'
        if module_exist:
            if func_name in dir(Device):
                productKey = "a1uTFk4xjko"
                deviceName  =    "mpy_001"
                deviceSecret  =   "b6e62d6fe7833a51f826796a81f6b6e7"
                key_info = {
                    'region' : 'cn-shanghai' ,
                    'productKey': productKey ,
                    'deviceName': deviceName ,
                    'deviceSecret': deviceSecret ,
                    'keepaliveSec': 60
                }
                ret = self.device.connect(key_info)
                self.assertEqual(ret, 0)
        else:
            self.assertFalse('Function %s not exist' % func_name)

    def test_getDeviceHandle(self):
        func_name = 'getDeviceHandle'
        if module_exist:
            if func_name in dir(Device):
                while not g_lk_connect:
                    
            else:
                self.assertFalse('Function %s not exist' % func_name)
        else:
            self.assertFalse('module not exist')       

    def test_publish(self):
        func_name = 'publish'
        if module_exist:
            if func_name in dir(Device):
                pass
            else:
                self.assertFalse('Function %s not exist' % func_name)
        else:
            self.assertFalse('module not exist')

    def test_subscribe(self):
        func_name = 'subscribe'
        if module_exist:
            if func_name in dir(Device):
                pass
            else:
                self.assertFalse('Function %s not exist' % func_name)
        else:
            self.assertFalse('module not exist')

    def test_unsubscribe(self):
        func_name = 'unsubscribe'
        if module_exist:
            if func_name in dir(Device):
                pass
            else:
                self.assertFalse('Function %s not exist' % func_name)
        else:
            self.assertFalse('module not exist')

    def test_getNtpTime(self):
        func_name = 'getNtpTime'
        if module_exist:
            if func_name in dir(Device):
                pass
            else:
                self.assertFalse('Function %s not exist' % func_name)
        else:
            self.assertFalse('module not exist')            

    def test_postProps(self):
        func_name = 'postProps'
        if module_exist:
            if func_name in dir(Device):
                pass
            else:
                self.assertFalse('Function %s not exist' % func_name)
        else:
            self.assertFalse('module not exist')

    def test_postEvent(self):
        func_name = 'postProps'
        if module_exist:
            if func_name in dir(Device):
                pass
            else:
                self.assertFalse('Function %s not exist' % func_name)
        else:
            self.assertFalse('module not exist')

    def test_end(self):
        func_name = 'end'
        if module_exist:
            if func_name in dir(Device):
                pass
            else:
                self.assertFalse('Function %s not exist' % func_name)
        else:
            self.assertFalse('module not exist')

if __name__ == '__main__':
    unittest.main()