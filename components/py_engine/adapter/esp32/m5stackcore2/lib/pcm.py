# -*- encoding: utf-8 -*-
'''
@File    :   Player.py
@Time    :   2021/12/08 20:32:10
@Author  :   zeta.zz
@License :   (C)Copyright 2015-2021, M5STACK
@Desc    :   Player I2S driver.
'''


from machine import I2S
from machine import Pin
from axp192 import Axp192
from micropython import const
import io
import math
import struct
import gc
import urequests

BCK_PIN = Pin(12)
WS_PIN = Pin(0)
SDOUT_PIN = Pin(2)

PI = 3.141592653

I2S0 = const(0)
F16B = const(16)
F24B = const(24)
F32B = const(32)


class Player:

    def __init__(self):
        self.power = Axp192()
        self.power.powerAll()

    def open(self):
        pass

    def play(self, wav_file, rate=None, data_format=None, channel=None):
        """
        Parameter:
            wav_file
        Return:
            False Not WAV format file
        """
        if type(wav_file) is str:
            try:
                wav = open(wav_file, 'rb')
            except Exception as e:
                print('Audio file open caught exception: {} {}'.format(
                    type(e).__name__, e))
                return
        elif type(wav_file) is bytes:
            wav = io.BytesIO(len(wav_file))
            wav.write(wav_file)
            wav.seek(0)
        else:
            return "Unknow file type"

        wav_head = wav.read(44)
        if wav_head[0:4] != b"RIFF" and wav_head[8:12] != b"WAVE":
            return "Wrong WAV format file"

        if rate and data_format and channel:
            channels = channel
            samplerate = rate
            dataformat = data_format
        else:
            channels = (wav_head[0x17] << 8) | (wav_head[0x16])
            if channels == 1:
                channels = I2S.MONO
            elif channels == 2:
                channels = I2S.STEREO
            samplerate = (wav_head[0x1B] << 24) | (wav_head[0x1A] << 16) | (wav_head[0x19] << 8) | (wav_head[0x18])
            dataformat = (wav_head[0x23] << 8) | (wav_head[0x22])

        audio_out = I2S(
            I2S0,
            sck=BCK_PIN, ws=WS_PIN, sd=SDOUT_PIN,
            mode=I2S.TX,
            bits=dataformat,
            format=channels,
            rate=samplerate,
            ibuf=3*1024)

        # advance to first byte of Data section in WAV file
        # wav.seek(44)

        # allocate sample arrays
        # memoryview used to reduce heap allocation in while loop
        wav_samples = bytearray(1024)
        wav_samples_mv = memoryview(wav_samples)

        # continuously read audio samples from the WAV file
        # and write them to an I2S DAC
        self.power.setSpkEnable(True)
        try:
            while True:
                # try:
                num_read = wav.readinto(wav_samples_mv)
                num_written = 0
                if num_read == 0:
                    # pos = wav.seek(44)
                    # exit
                    break
                else:
                    while num_written < num_read:
                        num_written += audio_out.write(
                            wav_samples_mv[num_written:num_read])
        except (KeyboardInterrupt, Exception) as e:
            print('Player caught exception: {} {}'.format(type(e).__name__, e))
            self.power.setSpkEnable(False)
            raise
        finally:
            self.power.setSpkEnable(False)
            audio_out.deinit()
            wav.close()
            del wav
            del wav_samples_mv
            del wav_samples
            gc.collect()

    def playCloudWAV(self, url):
        """
        Parameter:
            url: WAV format file URL
        Return:
            False
            None
        """
        request = urequests.get(url)
        if (request.status_code) == 200:
            self.playWAV(request.content)
        else:
            return "Request WAV file fail"

    def playTone(self, freq, beta, rate=44100, data_format=F16B, channel=I2S.STEREO):
        """
        Parameter:
            freq = frequency
            duration = time in secods
        Return:
        """
        wave_data = io.BytesIO()
        freq_rate = (freq / rate)
        # Calculate a period of sine wave
        cycle = rate / freq
        for i in range(0, cycle):
            # 6.283185 = 2 * PI
            x = 6.283185 * freq_rate * i
            data = int(32767 * math.sin(x))
            wave_data.write(bytes(struct.pack('h', data)))

        audio_out = I2S(
            I2S0,
            sck=BCK_PIN, ws=WS_PIN, sd=SDOUT_PIN,
            mode=I2S.TX,
            bits=data_format,
            format=channel,
            rate=rate,
            ibuf=3 * 1024)

        wave_data.seek(0)
        # One cycle sine wave data length
        length = (len(wave_data.read()))
        wave_data.seek(0)
        # Calculate how many cycles
        cycles = int((rate * beta) / cycle)

        wave_samples = bytearray(length)
        wave_samples_mv = memoryview(wave_samples)

        self.power.setSpkEnable(True)
        try:
            for i in range(0, cycles):
                num_read = wave_data.readinto(wave_samples_mv)
                num_written = 0
                if num_read == 0:
                    wave_data.seek(0)
                else:
                    while num_written < num_read:
                        num_written += audio_out.write(wave_samples_mv[num_written:num_read])
                        # print(num_written)
        except (KeyboardInterrupt, Exception) as e:
            print('Player caught exception: {} {}'.format(type(e).__name__, e))
            self.power.setSpkEnable(False)
            raise
        finally:
            self.power.setSpkEnable(False)
            audio_out.deinit()
            wave_data.close()
            del wave_data
            del wave_samples_mv
            del wave_samples
            gc.collect()
