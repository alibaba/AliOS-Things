#!/usr/bin/env python3
# -*- encoding: utf-8 -*-
'''
@File       :    cloudAI.py
@Description:    云端AI
@Author     :    jiangyu
@version    :    1.0
'''

from aliyunIoT import Device
import utime        # 延时函数在utime库中
import ujson as json

class CloudAI :
    def __gesture_cb(self, dict) :
        '''
        Reply list :
            handGestureReply : 手势识别
        '''
        gesture = 'NA'
        if dict != None:
            ext = dict['ext']
            ext_dict = json.loads(ext)
            result = ext_dict['result']
            if result == 'success':
                score = ext_dict['score']
                if score > 0.4 :
                    gesture = ext_dict['type']
                    print("recognize hand gesture : " + gesture)
        self.__cb('handGestureReply', gesture)

    def __license_plate_cb(self, dict) :
        plateNumber = 'NA'
        if dict != None:
            ext = dict['ext']
            ext_dict = json.loads(ext)
            result = ext_dict['result']
            if result == 'success':
                g_confidence = ext_dict['confidence']
                if g_confidence > 0.7 :
                    plateNumber = ext_dict['plateNumber']
                    print('detect: ' + plateNumber)
        self.__cb('ocrCarNoReply', plateNumber)

    def __fruits_cb(self, dict) :
        fruit_name = 'NA'
        if dict != None:
            ext = dict['ext']
            ext_dict = json.loads(ext)
            result = ext_dict['result']
            if result == 'success':
                i = 0
                fruits_list = ext_dict['fruitList']
                while (i < len(fruits_list)) :
                    g_score = fruits_list[i]['score']
                    fruit_name = fruits_list[i]['name']
                    if g_score > 0.6:
                        print('detect: ' + fruit_name)
                    i += 1
        self.__cb('detectFruitsReply', fruit_name)

    def __pedestrian_cb(self, dict) :
        detected = False
        if dict != None:
            ext = dict['ext']
            ext_dict = json.loads(ext)
            result = ext_dict['result']
            if result == 'success':
                i = 0
                data = ext_dict['data']
                data_dict = json.loads(data)
                elements_list = data_dict['elements']
                while (i < len(elements_list)) :
                    g_score = elements_list[i]['score']
                    if g_score > 0.6:
                        print('Pedestrian Detected')
                        detected = True
                    i += 1
        self.__cb('DetectPedestrianReply', detected)

    def __businesscard_cb(self, dict) :
        card_info = {}
        if dict != None:
            ext = dict['ext']
            ext_dict = json.loads(ext)
            result = ext_dict['result']
            if result == 'success':
                card_info['name'] = ext_dict['name']
                print("name : " + card_info['name'])
                if card_info['name'] == '' :
                    card_info['name'] = 'unknown'
                phoneNumbers_list = ext_dict['cellPhoneNumbers']
                print("phoneNumbers : ")
                print(phoneNumbers_list)
                if len(phoneNumbers_list) :
                    card_info['phoneNumbers'] = phoneNumbers_list[0]
                else :
                    card_info['phoneNumbers'] = 'unknown'
                email_list = ext_dict['emails']
                print("email_list: ")
                print(email_list)
                if len(email_list) :
                    card_info['email'] = email_list[0]
                else :
                    card_info['email'] = 'unknown'
        self.__cb('recognizeBusinessCardReply', card_info)

    def __rubblish_cb(self, dict) :
        name = 'NA'
        if dict != None:
            ext = dict['ext']
            extDict = json.loads(ext)
            result = extDict['result']
            if result == 'success':
                i = 0
                elements = extDict['elements']
                while (i < len(elements)) :
                    gScore = elements[i]['categoryScore']
                    if gScore > 0.8:
                        name = elements[i]['category']
                        print('detect: ' + name)
                        break
                    i += 1
        self.__cb('classifyingRubbishReply', name)

    def __object_cb(self, dict) :
        name = 'NA'
        if dict != None:
            ext = dict['ext']
            extDict = json.loads(ext)
            result = extDict['result']
            if result == 'success':
                i = 0
                elements = extDict['elements']
                while (i < len(elements)) :
                    gScore = elements[i]['score']
                    if gScore > 0.25:
                        name = elements[i]['type']
                        print('detect: ' + name)
                        break
                    i += 1
        self.__cb('detectObjectReply', name)

    def __vehicletype_cb(self, dict) :
        name = 'NA'
        detect = False
        if dict != None:
            ext = dict['ext']
            ext_dict = json.loads(ext)
            result = ext_dict['result']
            if result == 'success':
                i = 0
                item_list = ext_dict['items']
                name = 'NA'
                while (i < len(item_list)) :
                    g_score = item_list[i]['score']
                    name = item_list[i]['name']
                    # 这里可以修改识别的可信度，目前设置返回可信度大于85%才认为识别正确
                    if g_score > 0.85 and name != 'others':
                        print('detect: ' + name)
                        detect = True
                        self.__cb('recognizeVehicleReply', name)

                        break
                    i += 1
        if detect == False:
            self.__cb('recognizeVehicleReply', 'NA')

    def __vehiclelogo_cb(self, dict) :
        num = 0
        if dict != None:
            ext = dict['ext']
            ext_dict = json.loads(ext)
            result = ext_dict['result']
            if result == 'success':
                item_list = ext_dict['elements']
                num = len(item_list)
                if num > 0:
                    print('detect: ' + str(num) + ' vehicle')
                    detected = True
                if detected == False:
                    print('do not detect!')
        self.__cb('recognizeLogoReply', num)

    def __cb_lk_service(self, data):
        self.g_lk_service = True
        print('download <----' + str(data))
        if data != None :
            params = data['params']
            params_dict = json.loads(params)
            command = params_dict['commandName']
            if command == 'handGestureReply' :
                self.__gesture_cb(params_dict)
            elif command == 'ocrCarNoReply' :
                self.__license_plate_cb(params_dict)
            elif command == 'DetectPedestrianReply' :
                self.__pedestrian_cb(params_dict)
            elif command == 'detectFruitsReply' :
                self.__fruits_cb(params_dict)
            elif command == 'recognizeBusinessCardReply' :
                self.__businesscard_cb(params_dict)
            elif command == 'classifyingRubbishReply' :
                self.__rubblish_cb(params_dict)
            elif command == 'detectObjectReply' :
                self.__object_cb(params_dict)
            elif command == 'recognizeVehicleReply' :
                self.__vehicletype_cb(params_dict)
            elif command == 'recognizeLogoReply' :
                self.__vehiclelogo_cb(params_dict)
            else :
                print('unknown command reply')

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

    def __init__(self, dev_info, callback) :
        self.__dev_info = dev_info
        self.__cb = callback
        self.g_lk_connect = False
        self.g_lk_service = False
        self.__connect_iot()

    def getDevice(self) :
        return self.device

    def __upload_request(self, command, frame) :
            # 上传图片到LP
        fileName = 'test.jpg'
        start = utime.ticks_ms()
        fileid = self.device.uploadContent(fileName, frame, None)

        if fileid != None:
            ext = { 'filePosition':'lp', 'fileName': fileName, 'fileId': fileid }
            ext_str = json.dumps(ext)
            all_params = {'id': 1, 'version': '1.0', 'params': { 'eventType': 'haas.faas', 'eventName': command, 'argInt': 1, 'ext': ext_str }}
            all_params_str = json.dumps(all_params)
            #print(all_params_str)
            upload_file = {
                'topic': '/sys/' + self.__dev_info['productKey'] + '/' + self.__dev_info['deviceName'] + '/thing/event/hli_event/post',
                'qos': 1,
                'payload': all_params_str
            }

            # 上传完成通知HaaS聚合平台
            print('upload--->' + str(upload_file))
            self.g_lk_service = False
            self.device.publish(upload_file)
            i = 0
            while (self.g_lk_service == False and i < 200) :
                utime.sleep_ms(10)
                i = i + 1
                continue
        else:
            print('filedid is none, upload content fail')
        time_diff = utime.ticks_diff(utime.ticks_ms(), start)
        print('recognize time : %d' % time_diff)

    def recognizeGesture(self, frame) :
        self.__upload_request('handGesture', frame)

    def recognizeLicensePlate(self, frame) :
        self.__upload_request('ocrCarNo', frame)

    def detectPedestrian(self, frame) :
        self.__upload_request('detectPedestrian', frame)

    def detectFruits(self, frame) :
        self.__upload_request('detectFruits', frame)

    def recognizeBussinessCard(self, frame) :
        self.__upload_request('recognizeBusinessCard', frame)

    def recognizeVehicleType(self, frame) :
        self.__upload_request('recognizeVehicle', frame)

    def detectVehicleCongestion(self, frame) :
        self.__upload_request('vehicleCongestionDetect', frame)

    def classifyRubbish(self, frame) :
        self.__upload_request('classifyingRubbish', frame)

    def detectObject(self, frame) :
        self.__upload_request('detectObject', frame)
