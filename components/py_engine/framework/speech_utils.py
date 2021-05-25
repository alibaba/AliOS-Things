#!/usr/bin/env python
# -*- encoding: utf-8 -*-
'''
@File       :    speech_utils.py
@Description:    file description   
@Date       :    2021/05/17 11:43:06
@Author     :    guoliang.wgl
@version    :    1.0
'''

import math
import http
from audio import Player,Snd

toneDir = "/sdcard/resource/"
tonenameSuffix = [".wav", ".mp3"]
tonenameNumb = ["SYS_TONE_0", "SYS_TONE_1", "SYS_TONE_2", "SYS_TONE_3", "SYS_TONE_4", "SYS_TONE_5", "SYS_TONE_6", "SYS_TONE_7", "SYS_TONE_8", "SYS_TONE_9"]
tonenameNumb1 = "SYS_TONE_yao"
tonenameDot = "SYS_TONE_dian"
tonenameUnit = ["SYS_TONE_MEASURE_WORD_ge", "SYS_TONE_MEASURE_WORD_shi", "SYS_TONE_MEASURE_WORD_bai", "SYS_TONE_MEASURE_WORD_qian"]
tonenameHunit = ["SYS_TONE_MEASURE_WORD_wan", "SYS_TONE_MEASURE_WORD_yi", "SYS_TONE_MEASURE_WORD_sw", "SYS_TONE_MEASURE_WORD_bw", "SYS_TONE_MEASURE_WORD_qw"]

player = None
def init_audio():
    global player
    Snd.install_codec_driver()
    Snd.init()
    player = Player()
    player.create()
    player.set_volume(8)

def play(path):
    global player
    player.set_source(path)
    player.start()
    player.wait_complete()

def playlist(pathlist):
    for path in pathlist:
        play('fs:'+path)

    print('********end playlist*******')

def play_voice(data,dir):
    global toneDir, tonenameSuffix, playlist
    toneDir = dir
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
            toneList = add_amount(speech_num,toneList,audioResFormat)
        else:
            toneList.append(toneDir + speech + tonenameSuffix[audioResFormat])
        print(toneList)
    playlist(toneList)



def add_amount(num_str, toneList, formatFlag):
    global toneDir,tonenameSuffix,tonenameNumb,tonenameNumb1,tonenameDot,tonenameUnit,tonenameHunit 
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
    
    if (numb < 0 or numb >= 1000000000000):
        print('amount overrange')
        return toneIndex

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
                toneList.append(toneDir + tonenameNumb[slot] + tonenameSuffix[formatFlag])
                count += 1
                if (slot == 0 and prevSlotZero == False):
                    prevSlotZero = True
                elif (prevSlotZero == True and slot != 0):
                    prevSlotZero = False

            if (slot > 0 and depth > 0) :
                toneList.append(toneDir + tonenameUnit[depth] + tonenameSuffix[formatFlag])
                count += 1
            depth = depth - 1
        if (i > 0):
            toneList.append(toneDir + tonenameHunit[i - 1] + tonenameSuffix[formatFlag])
            count += 1
        i = i - 1

    if (count == 0 and numb == 0):
        toneList.append(toneDir + tonenameNumb[0] + tonenameSuffix[formatFlag])
    if (deci >= 0.0001) :
        toneList.append(toneDir + tonenameDot + tonenameSuffix[formatFlag])

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
                toneList.append(toneDir + tonenameNumb[int(item)] + tonenameSuffix[formatFlag])
    return toneList


def download_resource_file(on_request,resDir):
    global toneDir
    toneDir = resDir
    client=http.client()
    client.set_header("Accept: */*\r\n")
    client.get(on_request['url'])
    response = json.loads(client.get_response())
    #print(response)
    format = response['format']
    size = response['size']
    format = response['format']
    audio = response['audios'][0]
    id = audio['id']
    path = toneDir +id+'.'+format
    print('************ begin to download: ' + path)
    ret  = client.download(audio['url'],path)
    if ret == 0 :
        print('************ download: ' + path + ' succeed')
    else:
        print('************ download: ' + path + ' failed')