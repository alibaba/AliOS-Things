#
# Copyright (c) 2014-2018 Alibaba Group. All rights reserved.
# License-Identifier: Apache-2.0
#
# Licensed under the Apache License, Version 2.0 (the "License"); you may
# not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#   http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
# WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#
#

# import os
# #import threading
# import ujson
# import utime
# import sys
import _linkkit as lk

class LinkKit(object):
    def __init__(self, host_name, product_key, device_name, device_secret,product_secret=None, user_data=None):
        print('this is LinkKit init function') 

        def __str_is_empty(value):
            if value is None or value == "":
                return True
            else:
                return False
        # param check
        if __str_is_empty(host_name):
            raise ValueError("host_name wrong")
        if __str_is_empty(product_key):
            raise ValueError("product key wrong")
        if __str_is_empty(device_name):
            raise ValueError("device name wrong")
        if __str_is_empty(device_secret) and __str_is_empty(product_secret):
            raise ValueError("device secret & product secret are both empty")
        print('this is LinkKit param check end ') 

    #             # config internal property
        self.__host_name = host_name
        self.__product_key = product_key
        self.__device_name = device_name
        self.__device_secret = device_secret
        self.__product_secret = product_secret
        self.__on_device_dynamic_register = None
        self.__on_service_request = None
        self.__on_prop_set = None
    #     # mqtt callbacks
        self.__on_connect = None
        self.__on_disconnect = None
        self.__on_thing_prop_post = None
        self.__on_thing_event_post = None
        lk.init(self.__host_name,self.__product_key,self.__device_name,self.__device_secret,self.__product_secret)
        print("end init for LinkKit")

    def __register_callback(self):
        lk.register_call_back(1,self.__on_connect)
        lk.register_call_back(3,self.__on_disconnect)
        lk.register_call_back(5,self.__on_service_request)
        lk.register_call_back(7,self.__on_prop_set)
        lk.register_call_back(9,self.__on_thing_prop_post)
        lk.register_call_back(10,self.__on_thing_event_post)

    def connect_async(self):
        self.__register_callback();
        ret = lk.register_dyn_dev()
        if(ret == True):
            print("register_dyn_dev success")
        else:
            print("register_dyn_dev failed")

        if self.on_device_dynamic_register is None:
            raise Exception("user not give on_device_dynamic_register")
        try:
            self.on_device_dynamic_register(0,"succeed",None)
            
        except Exception as e:
                print('callback on_device_dynamic_register failed ')
                return 2
        lk.connect()

    def thing_post_property(self, property_data):
        return lk.post_property(property_data)    

    def thing_trigger_event(self, event_tuple):
        print(event_tuple)
        if isinstance(event_tuple, tuple):
            event, params = event_tuple
        return lk.post_event(event,params)


    def do_yield(self,timeout):
        return lk.do_yield(timeout)    

    @property
    def on_connect(self):
        return self.__on_connect

    @on_connect.setter
    def on_connect(self, value):
        self.__on_connect = value
        pass

    @property
    def on_disconnect(self):
        return self.__on_disconnect

    @on_disconnect.setter
    def on_disconnect(self, value):
        self.__on_disconnect = value

    @property
    def on_thing_call_service(self):
        return None

    @on_thing_call_service.setter
    def on_thing_call_service(self, value):
        self.__on_thing_call_service = value

    @property
    def on_prop_set(self):
        return self.on_prop_set 

    @on_prop_set.setter
    def on_prop_set(self, value):
        self.__on_prop_set = value

    @property
    def on_service_request(self):
        return self.__on_service_request 

    @on_service_request.setter
    def on_service_request(self, value):
        self.__on_service_request = value    

    @property
    def on_thing_event_post(self):
        return self.__on_thing_event_post

    @on_thing_event_post.setter
    def on_thing_event_post(self, value):
        self.__on_thing_event_post = value

    @property
    def on_thing_prop_post(self):
        return self.__on_thing_prop_post

    @on_thing_prop_post.setter
    def on_thing_prop_post(self, value):
        self.__on_thing_prop_post = value
        





  