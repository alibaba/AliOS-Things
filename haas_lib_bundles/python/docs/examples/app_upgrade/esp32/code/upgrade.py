#!/usr/bin/env python3
# -*- encoding: utf-8 -*-
'''
@File       :    upgrade.py
@Description:    python应用升级能力实现
@Author     :    杭漂
@version    :    1.0
'''

from aliyunIoT import Device
import ujson as json

class Upgrade :
    def compareVersion(self,oldVersion,newVersion):
        old_list = oldVersion.split('.')
        new_list = newVersion.split('.')
        if (len(new_list) == 3) and (len(old_list) ==3) :
            if int(new_list[0]) > int(old_list[0]):
                return 1
            elif (int(new_list[0]) == int(old_list[0])) and (int(new_list[1]) > int(old_list[1])):
                return 1
            elif (int(new_list[0]) == int(old_list[0])) and (int(new_list[1]) == int(old_list[1])) and (int(new_list[2]) > int(old_list[2])):
                return 1
            else:
                return 0
        else:
            print("oldVersion(%s) or newVersion(%s) format mismatch,must x.x.x" % (oldVersion,newVersion))
        return 0

    def __cb_lk_service(self, data):
        print('service payload data ---->\n' + str(data))
        if data != None:
            params = data['params']
            if (data['service_id'].find("hli_service_upgrade_push") != -1):
                params_dict = json.loads(params)
                if params_dict['errorCode'] == '200':
                    app_list = params_dict['app']
                    i = 0
                    while (i < len(app_list)) :
                        appId = app_list[i]['appId']
                        if(appId == self.__app_info['appId']):
                            version = app_list[i]['version']
                            if app_list[i]['rollingBack'] == 0 :
                                url = app_list[i]['url']
                                version = app_list[i]['version']
                                needUpgrade = self.compareVersion(self.__app_info['localAppVersion'],version)
                                if needUpgrade == 1:
                                    print("local version[%s] low,upgrade to [%s]." % (self.__app_info['localAppVersion'],version))
                                    data_info = {'appId':appId,'url':url,'version':version,'localAppVersion':self.__app_info['localAppVersion']}
                                    self.__cb(data_info)
                        i += 1
                else :
                    print("receive upgrade notice failed[%s]." % (params['errorCode']))

    def __pub_query_upgrade_event(self,app_info) :
        self.__app_info = app_info
        appInfo1 = {'appId':app_info['appId'],'version':app_info['localAppVersion']}
        allParams = {'id': 1, 'version': '1.0', 'params': { 'mac':app_info['mac'], 'ip': app_info['ip'],'userScenario':'online', 'vendorId': 'HaaSPython','buildType':'eng','isSecurityOn':0 ,'firmwareVersion':'0.0.1','app':[appInfo1]}}
        all_params_str = json.dumps(allParams)

        query_upgrade_topic = '/sys/' + self.__dev_info['productKey'] + '/' + self.__dev_info['deviceName'] + '/thing/event/hli_event_upgrade_query/post'
        topic_info = {
            'topic': query_upgrade_topic,
            'qos': 1,
            'payload': all_params_str
        }
        self.device.publish(topic_info)
        print('Topic发布成功:%s' % (query_upgrade_topic))

    def __pub_upgrade_result_event(self,app_info,argInt) :
        self.__app_info = app_info
        ext = {'appId':app_info['appId'],'appNewVersion':app_info['appNewVersion'],'appOldVersion':app_info['localAppVersion']}
        ext_str = json.dumps(ext)
        # argInt，200：成功。404：包下载失败。405：包安装失败
        allParams = {'id': 1, 'version': '1.0', 'params': \
            { 'eventType':'haas.upgrade', 'eventName': 'app.result','argInt':argInt, 'ext': ext_str}}
        all_params_str = json.dumps(allParams)

        query_upgrade_topic = '/sys/' + self.__dev_info['productKey'] + '/' + self.__dev_info['deviceName'] + '/thing/event/hli_event/post'
        topic_info = {
            'topic': query_upgrade_topic,
            'qos': 1,
            'payload': all_params_str
        }
        self.device.publish(topic_info)
        print('Topic发布成功:%s' % (query_upgrade_topic))

    def __sub_upgrade_push_service(self) :
        upgrade_topic = '/sys/' + self.__dev_info['productKey'] + '/' + self.__dev_info['deviceName'] + '/thing/service/hli_service_upgrade_push'
        sub_topic = {
            'topic': upgrade_topic,
            'qos': 1
        }
        ret = self.device.subscribe(sub_topic)
        if ret == 0 :
            print("subscribed topic success: %s" % (upgrade_topic))
        else :
            print("subscribed topic failed: %s" % (upgrade_topic))

    def __cb_lk_connect(self, data):
        print('link platform connected')
        self.g_lk_connect = True

    def __connect_iot(self) :
        self.device = Device()
        self.device.on(Device.ON_CONNECT, self.__cb_lk_connect)
        self.device.on(Device.ON_SERVICE, self.__cb_lk_service)
        self.device.connect(self.__dev_info)

        while True:
            if self.g_lk_connect:
                break

    def __init__(self, dev_info,app_info,callback) :
        self.__dev_info = dev_info
        self.__app_info = app_info
        self.__cb = callback
        self.g_lk_connect = False
        self.__connect_iot()
        #订阅应用升级服务
        self.__sub_upgrade_push_service()
