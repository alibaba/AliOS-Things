# -*- coding: utf-8 -*-
import os
import sys
import math
import wave
import struct
import numpy as np

max_volume = 0.85
max_short  = 32767
min_short = -32768
sample_rate = 16000
min_freq = 1700
max_freq = 1700 + 96 * 50 + 100
freq_inc = 50
time_inc = 0.1

def gen_voice_freq(freq, times):
    unit_sample = int(sample_rate*times)
    theta_inc = 2 * math.pi * freq / sample_rate
    df = sample_rate / (unit_sample - 1)
    theta = 0
    buffer = []
    for frame in range(unit_sample):
        tmp = math.pow(frame - unit_sample/2, 2) / math.pow(unit_sample/2, 2)
        vol = max_volume * math.sqrt(1 - tmp)
        fv_float = vol * math.cos(theta)
        fv_float_16 = int(max_short * fv_float)
        if fv_float_16 > max_short:
            fv_float_16 = max_short
        if fv_float_16 < min_short:
            fv_float_16 = min_short
        buffer.append(fv_float_16)
        theta += theta_inc
        if theta > 2.0 * math.pi:
            theta -= 2.0 * math.pi
    print('freq: %d' %freq)
    Y = np.fft.fft(buffer)*2/unit_sample
    absY = [np.abs(x) for x in Y]
    #print(absY)
    print(int(np.argmax(absY)*df))
    return buffer

'''
    i = 0
    while i < len(absY):
        if absY[i] > 0.01:            
            print("freq: %d, Y: %5.2f + %5.2f j, A:%3.2f, phi: %6.1f" % (i, Y[i].real, Y[i].imag, absY[i], math.atan2(Y[i].imag, Y[i].real)*180/math.pi))
        i+=1
'''
    

def gen_voice(text, wav_file):
    wave_data = []
    wave_data += gen_voice_freq(min_freq - 50, 0.128)
    for c in text:
        if c == '\n' :
            freq = max_freq
            print('max freq : %d' % freq)
        else :
            freq = min_freq + (ord(c) - 33)*freq_inc
        wave_data += gen_voice_freq(freq, 0.128)
    freq = min_freq + (ord('f') - 33)*freq_inc
    wave_data += gen_voice_freq(freq, 0.128)
    freq = min_freq + (ord('c') - 33)*freq_inc
    wave_data += gen_voice_freq(freq, 0.128)
    wave_data += gen_voice_freq(min_freq - 50, 0.128)

    f = wave.open(wav_file,"wb")
    f.setnchannels(1)
    f.setsampwidth(2)
    f.setframerate(sample_rate)    
    for data in wave_data:
        f.writeframes(struct.pack('h', int(data)))
    f.close()

def main(args):
    gen_voice("h3c_router01\n123456", "test.wav")
    #gen_voice_freq(5650, 0.1)

if __name__ == '__main__':
    main(sys.argv[1:])
