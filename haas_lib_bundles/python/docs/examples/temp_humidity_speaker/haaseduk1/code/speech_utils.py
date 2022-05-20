#!/usr/bin/env python3
# -*- encoding: utf-8 -*-
'''
@File       :    speech_utils.py
@Description:    播报音箱库
@Date       :    2022/02/11 14:34:45
@Author     :    guoliang.wgl
@version    :    1.0
'''
import time


import math
import http
import json
import time

AUDIO_HEADER = 'fs:'
from audio import Player, Snd


on_callback = False
on_download = False
cb_data = None
class Speaker:
    tonenameSuffix = [".wav", ".mp3"]
    tonenameNumb = ["SYS_TONE_0", "SYS_TONE_1", "SYS_TONE_2", "SYS_TONE_3", "SYS_TONE_4",
        "SYS_TONE_5", "SYS_TONE_6", "SYS_TONE_7", "SYS_TONE_8", "SYS_TONE_9"]
    tonenameDot = "SYS_TONE_dian"
    tonenameUnit = ["SYS_TONE_MEASURE_WORD_ge", "SYS_TONE_MEASURE_WORD_shi",
        "SYS_TONE_MEASURE_WORD_bai", "SYS_TONE_MEASURE_WORD_qian"]
    tonenameHunit = ["SYS_TONE_MEASURE_WORD_wan", "SYS_TONE_MEASURE_WORD_yi",
        "SYS_TONE_MEASURE_WORD_sw", "SYS_TONE_MEASURE_WORD_bw", "SYS_TONE_MEASURE_WORD_qw"]
    tonenameTempUnit = "centigrade.wav"
    tonenameTempPrefix = "temperature.wav"
    tonenameHumidityPrefix = "humidity.wav"
    tonenameNegativePrefix = "nagative.wav"

    def __init__(self,res_dir):
        self.toneDir = res_dir
        self._create_player()

    def _create_player(self):
        Snd.init()
        player = Player()
        player.open()
        player.setVolume(8)
        self._player = player


    def play(self,path):
        self._player.play(path)
        self._player.waitComplete()

    def playlist(self,pathlist):
        for path in pathlist:
            self.play(AUDIO_HEADER + path)

    def play_voice(self,data,dir_info):
        format =  data['format']
        audioResFormat = 0
        if (format == 'mp3'):
            audioResFormat = 1
        speechs =  data['speechs']
        toneList = []
        for speech in speechs:
            print(speech)
            # length = len(speech)
            if speech.endswith('}') and speech.startswith('{') and (speech[1] == '$'):
                speech_num = speech.strip('{').strip('$').strip('}')
                toneList = self.add_amount(speech_num,toneList,audioResFormat)
            else:
                toneList.append(self.toneDir + speech + self.tonenameSuffix[audioResFormat])
        print(toneList)
        self.playlist(toneList)

    def add_amount(self,num_str, toneList, formatFlag):
        num_f = float(num_str)
        numb = int(num_f)
        deci = num_f - numb
        target = numb
        subTarget = 0
        subNumber = None
        slot = 0
        factor = 0
        count = 0
        prevSlotZero = False
        hundredMillionExist = False
        tenThousandExist = False
        if (numb < 0 or numb >= 100):
            print('amount overrange')
            return toneList
        if (deci < 0.0001 and deci > 0.0):
            deci = 0.0001
        i = 2
        while(i >= 0):
            factor = math.pow(10000,i)
            if target < factor:
                i = i -1
                continue
            subTarget = int(target / factor)
            target %= factor
            if (subTarget == 0):
                i = i -1
                continue

            if (i == 2):
                hundredMillionExist = True
            elif (i == 1):
                tenThousandExist = True
            subNumber = subTarget
            prevSlotZero = False

            depth = 3
            while(depth >= 0):
                if(subNumber == 0):
                    break
                factor = math.pow(10, depth)
                if ((hundredMillionExist == True or tenThousandExist == True) and i == 0):
                    pass
                elif (hundredMillionExist == True and tenThousandExist == True and depth > 0 and subTarget < factor):
                    pass
                elif (subTarget < factor):
                    depth = depth - 1
                    continue

                slot = int(subNumber / factor)
                subNumber %= factor
                if (slot == 0 and depth == 0):
                    depth = depth - 1
                    continue

                if ((subTarget < 20 and depth == 1) or (slot == 0 and prevSlotZero) or (slot == 0 and depth == 0)):
                    pass
                else:
                    toneList.append(self.toneDir + self.tonenameNumb[slot] + self.tonenameSuffix[formatFlag])
                    count += 1
                    if (slot == 0 and prevSlotZero == False):
                        prevSlotZero = True
                    elif (prevSlotZero == True and slot != 0):
                        prevSlotZero = False

                if (slot > 0 and depth > 0) :
                    toneList.append(self.toneDir + self.tonenameUnit[depth] + self.tonenameSuffix[formatFlag])
                    count += 1
                depth = depth - 1
            if (i > 0):
                toneList.append(self.toneDir + self.tonenameHunit[i - 1] + self.tonenameSuffix[formatFlag])
                count += 1
            i = i - 1

        if (count == 0 and numb == 0):
            toneList.append(self.toneDir + self.tonenameNumb[0] + self.tonenameSuffix[formatFlag])
        if (deci >= 0.0001) :
            toneList.append(self.toneDir + self.tonenameDot + self.tonenameSuffix[formatFlag])

            deci ="{:.4f}".format(deci)
            deci_tmp = str(deci).strip().rstrip('0')
            deci_str = ''
            got_dot = False
            for j in range(len(deci_tmp)):
                if(got_dot):
                    deci_str = deci_str + deci_tmp[j]
                elif deci_tmp[j] == '.':
                    got_dot = True
            deciArray = deci_str
            for item in deciArray:
                if (item >= '0' and item <= '9'):
                    print(self.tonenameNumb[int(item)])
                    toneList.append(self.toneDir + self.tonenameNumb[int(item)] + self.tonenameSuffix[formatFlag])
        return toneList


    def download_resource_file(self,on_request,resDir):
        global on_callback,on_download,cb_data
        data = {
            'url':on_request['url'],
            'method': 'GET',
            'headers': {
            },
            'timeout': 30000,
            'params' : ''
        }
        def cb(data):
            global on_callback,cb_data
            on_callback = True
            cb_data = data
        http.request(data,cb)
        while True:
            if  on_callback:
                on_callback = False
                break
            else:
                time.sleep(1)
        response = json.loads(cb_data['body'])
        audio = response['audios'][0]
        format = audio['format']
        id = audio['id']
        size = audio['size']
        path = self.toneDir +id+'.'+format
        print('************ begin to download: ' + path)
        d_data = {
            'url': audio['url'],
            'filepath': path
        }
        def d_cb(data):
            global on_download
            on_download = True
        http.download(d_data,d_cb)
        while True:
            if on_download:
                on_download = False
                break
            else:
                time.sleep(1)
        print('download succeed :' + path)
