#-*- coding: utf-8 -*-

"""
Copyright (C) 2015-2021 Alibaba Group Holding Limited

    MicroPython's driver for CHT8305

    Licensed under the Apache License, Version 2.0 (the "License");
    you may not use this file except in compliance with the License.
    You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

    Author: HaaS
"""

'''!
  @file DFRobot_PAJ7620U2.h
  @brief Define the basic structure of the class DFRobot_PAJ7620 gesture sensor
  @n The PAC7620 integrates gesture recognition function with general I2C interface into a single chip forming an image analytic sensor
  @n system. It can recognize 9 human hand gesticulations such as moving up, down, left, right, forward, backward, circle-clockwise,
  @n circle-counter Key Parameters clockwise, and waving. It also offers built-in proximity detection in sensing approaching or
  @n departing object from the sensor. The PAC7620 is designed with great flexibility in power-saving mechanism, well suit for low
  @n power battery operated HMI devices. The PAJ7620 is packaged into module form in-built with IR LED and optics lens as a complete
  @n sensor solution.
  @copyright   Copyright (c) 2010 DFRobot Co.Ltd (http://www.dfrobot.com)
  @license     The MIT License (MIT)
  @author      Alexander(ouki.wang@dfrobot.com)
  @maintainer  [fary](feng.yang@dfrobot.com)
  @version  V1.0
  @date  2019-07-16
  @url https://github.com/DFRobot/DFRobot_PAJ7620U2
'''

import utime
import json
from driver import I2C

## DEVICE ID
PAJ7620_IIC_ADDR = 0x73
PAJ7620_PARTID = 0x7620

## REGISTER BANK SELECT
PAJ7620_REGITER_BANK_SEL = 0xEF

## REGISTER BANK 0
PAJ7620_ADDR_PART_ID_LOW                 = 0x00
PAJ7620_ADDR_PART_ID_HIGH                = 0x01
PAJ7620_ADDR_VERSION_ID                  = 0x01
PAJ7620_ADDR_SUSPEND_CMD                 = 0x03
PAJ7620_ADDR_GES_PS_DET_MASK_0           = 0x41
PAJ7620_ADDR_GES_PS_DET_MASK_1           = 0x42
PAJ7620_ADDR_GES_PS_DET_FLAG_0           = 0x43
PAJ7620_ADDR_GES_PS_DET_FLAG_1           = 0x44
PAJ7620_ADDR_STATE_INDICATOR             = 0x45
PAJ7620_ADDR_PS_HIGH_THRESHOLD           = 0x69
PAJ7620_ADDR_PS_LOW_THRESHOLD            = 0x6A
PAJ7620_ADDR_PS_APPROACH_STATE           = 0x6B
PAJ7620_ADDR_PS_RAW_DATA                 = 0x6C

## REGISTER BANK 1
PAJ7620_ADDR_PS_GAIN                     = 0x44
PAJ7620_ADDR_IDLE_S1_STEP_0              = 0x67
PAJ7620_ADDR_IDLE_S1_STEP_1              = 0x68
PAJ7620_ADDR_IDLE_S2_STEP_0              = 0x69
PAJ7620_ADDR_IDLE_S2_STEP_1              = 0x6A
PAJ7620_ADDR_OP_TO_S1_STEP_0             = 0x6B
PAJ7620_ADDR_OP_TO_S1_STEP_1             = 0x6C
PAJ7620_ADDR_OP_TO_S2_STEP_0             = 0x6D
PAJ7620_ADDR_OP_TO_S2_STEP_1             = 0x6E
PAJ7620_ADDR_OPERATION_ENABLE            = 0x72

PAJ7620_BANK0                            = 0
PAJ7620_BANK1                            = 1

## PAJ7620_ADDR_SUSPEND_CMD
PAJ7620_I2C_WAKEUP                       = 0x01
PAJ7620_I2C_SUSPEND                      = 0x00

## PAJ7620_ADDR_OPERATION_ENABLE
PAJ7620_ENABLE                           = 0x01
PAJ7620_DISABLE                          = 0x00

## You can adjust the reaction time according to the actual circumstance.
GES_REACTION_TIME                        = 0.05
## When you want to recognize the Forward/Backward gestures, your gestures' reaction time must less than GES_ENTRY_TIME(0.8s).
GES_ENTRY_TIME                           = 2
GES_QUIT_TIME                            = 1

GESTURE = json.loads('{\
 "0": "None",\
 "1": "Right",\
 "2": "Left",\
 "4": "Up",\
 "8": "Down",\
 "16": "Forward",\
 "32": "Backward",\
 "64":"Clockwise",\
 "128": "Anti-Clockwise",\
 "256":"Wave",\
 "512":"WaveSlowlyDisorder",\
 "3": "WaveSlowlyLeftRight",\
 "12": "WaveSlowlyUpDown",\
 "48": "WaveSlowlyForwardBackward"}')

class PAJ7620:
   ## OK
   ERR_OK                               = 0
   ## Error in I2C Bus
   ERR_I2C_BUS                         = -1
   ## IC version mismatch
   ERR_IC_VERSION                       = -2
   ## no gestures detected
   GESTURE_NONE  = 0x00
   ## move from left to right
   GESTURE_RIGHT = 0x01 << 0
   ## move from right to left
   GESTURE_LEFT = 0x01 << 1
   ## move from down to up
   GESTURE_UP = 0x01 << 2
   ## move form up to down
   GESTURE_DOWN = 0x01 << 3
   ## starts far, move close to sensor
   GESTURE_FORWARD = 0x01 << 4
   ## starts near, move far to sensor
   GESTURE_BACKWARD = 0x01 << 5
   ## clockwise
   GESTURE_CLOCKWISE = 0x01 << 6
   ## anti - clockwise
   GESTURE_ANTI_CLOCKWISE = 0x01 << 7
   ## wave quickly
   GESTURE_WAVE = 0x01 << 8
   ##wave randomly and slowly
   GESTURE_WAVE_SLOWLY_DISORDER = 0x01 << 9
   ## slowly move left and right
   GESTURE_WAVE_SLOWLY_LEFT_RIGHT = GESTURE_LEFT + GESTURE_RIGHT
   ## slowly move up and down
   GESTURE_WAVE_SLOWLY_UP_DOWN = GESTURE_UP + GESTURE_DOWN
   ##slowly move forward and backward
   GESTURE_WAVE_SLOWLY_FORWARD_BACKWARD = GESTURE_FORWARD + GESTURE_BACKWARD
   ## support all gestures, no practical meaning, only suitable for writing abstract program logic.
   GESTURE_ALL = 0xff
   ## some registers are located in Bank0
   BANK_0 = 0
   ## some registers are located in Bank1
   BANK_1 = 1
   ## Gesture Update Rate is 120HZ, Gesture speed is 60°/s - 600°/s
   NORMAL_RATE = 0
   ## Gesture Update Rate is 240HZ,Gesture speed is 60°/s - 1200°/s
   GAMING_RATE = 1
   _gesture_high_rate = True

   _init_register_array = [
      [0xEF,0x00],
      [0x32,0x29],
      [0x33,0x01],
      [0x34,0x00],
      [0x35,0x01],
      [0x36,0x00],
      [0x37,0x07],
      [0x38,0x17],
      [0x39,0x06],
      [0x3A,0x12],
      [0x3F,0x00],
      [0x40,0x02],
      [0x41,0xFF],
      [0x42,0x01],
      [0x46,0x2D],
      [0x47,0x0F],
      [0x48,0x3C],
      [0x49,0x00],
      [0x4A,0x1E],
      [0x4B,0x00],
      [0x4C,0x20],
      [0x4D,0x00],
      [0x4E,0x1A],
      [0x4F,0x14],
      [0x50,0x00],
      [0x51,0x10],
      [0x52,0x00],
      [0x5C,0x02],
      [0x5D,0x00],
      [0x5E,0x10],
      [0x5F,0x3F],
      [0x60,0x27],
      [0x61,0x28],
      [0x62,0x00],
      [0x63,0x03],
      [0x64,0xF7],
      [0x65,0x03],
      [0x66,0xD9],
      [0x67,0x03],
      [0x68,0x01],
      [0x69,0xC8],
      [0x6A,0x40],
      [0x6D,0x04],
      [0x6E,0x00],
      [0x6F,0x00],
      [0x70,0x80],
      [0x71,0x00],
      [0x72,0x00],
      [0x73,0x00],
      [0x74,0xF0],
      [0x75,0x00],
      [0x80,0x42],
      [0x81,0x44],
      [0x82,0x04],
      [0x83,0x20],
      [0x84,0x20],
      [0x85,0x00],
      [0x86,0x10],
      [0x87,0x00],
      [0x88,0x05],
      [0x89,0x18],
      [0x8A,0x10],
      [0x8B,0x01],
      [0x8C,0x37],
      [0x8D,0x00],
      [0x8E,0xF0],
      [0x8F,0x81],
      [0x90,0x06],
      [0x91,0x06],
      [0x92,0x1E],
      [0x93,0x0D],
      [0x94,0x0A],
      [0x95,0x0A],
      [0x96,0x0C],
      [0x97,0x05],
      [0x98,0x0A],
      [0x99,0x41],
      [0x9A,0x14],
      [0x9B,0x0A],
      [0x9C,0x3F],
      [0x9D,0x33],
      [0x9E,0xAE],
      [0x9F,0xF9],
      [0xA0,0x48],
      [0xA1,0x13],
      [0xA2,0x10],
      [0xA3,0x08],
      [0xA4,0x30],
      [0xA5,0x19],
      [0xA6,0x10],
      [0xA7,0x08],
      [0xA8,0x24],
      [0xA9,0x04],
      [0xAA,0x1E],
      [0xAB,0x1E],
      [0xCC,0x19],
      [0xCD,0x0B],
      [0xCE,0x13],
      [0xCF,0x64],
      [0xD0,0x21],
      [0xD1,0x0F],
      [0xD2,0x88],
      [0xE0,0x01],
      [0xE1,0x04],
      [0xE2,0x41],
      [0xE3,0xD6],
      [0xE4,0x00],
      [0xE5,0x0C],
      [0xE6,0x0A],
      [0xE7,0x00],
      [0xE8,0x00],
      [0xE9,0x00],
      [0xEE,0x07],
      [0xEF,0x01],
      [0x00,0x1E],
      [0x01,0x1E],
      [0x02,0x0F],
      [0x03,0x10],
      [0x04,0x02],
      [0x05,0x00],
      [0x06,0xB0],
      [0x07,0x04],
      [0x08,0x0D],
      [0x09,0x0E],
      [0x0A,0x9C],
      [0x0B,0x04],
      [0x0C,0x05],
      [0x0D,0x0F],
      [0x0E,0x02],
      [0x0F,0x12],
      [0x10,0x02],
      [0x11,0x02],
      [0x12,0x00],
      [0x13,0x01],
      [0x14,0x05],
      [0x15,0x07],
      [0x16,0x05],
      [0x17,0x07],
      [0x18,0x01],
      [0x19,0x04],
      [0x1A,0x05],
      [0x1B,0x0C],
      [0x1C,0x2A],
      [0x1D,0x01],
      [0x1E,0x00],
      [0x21,0x00],
      [0x22,0x00],
      [0x23,0x00],
      [0x25,0x01],
      [0x26,0x00],
      [0x27,0x39],
      [0x28,0x7F],
      [0x29,0x08],
      [0x30,0x03],
      [0x31,0x00],
      [0x32,0x1A],
      [0x33,0x1A],
      [0x34,0x07],
      [0x35,0x07],
      [0x36,0x01],
      [0x37,0xFF],
      [0x38,0x36],
      [0x39,0x07],
      [0x3A,0x00],
      [0x3E,0xFF],
      [0x3F,0x00],
      [0x40,0x77],
      [0x41,0x40],
      [0x42,0x00],
      [0x43,0x30],
      [0x44,0xA0],
      [0x45,0x5C],
      [0x46,0x00],
      [0x47,0x00],
      [0x48,0x58],
      [0x4A,0x1E],
      [0x4B,0x1E],
      [0x4C,0x00],
      [0x4D,0x00],
      [0x4E,0xA0],
      [0x4F,0x80],
      [0x50,0x00],
      [0x51,0x00],
      [0x52,0x00],
      [0x53,0x00],
      [0x54,0x00],
      [0x57,0x80],
      [0x59,0x10],
      [0x5A,0x08],
      [0x5B,0x94],
      [0x5C,0xE8],
      [0x5D,0x08],
      [0x5E,0x3D],
      [0x5F,0x99],
      [0x60,0x45],
      [0x61,0x40],
      [0x63,0x2D],
      [0x64,0x02],
      [0x65,0x96],
      [0x66,0x00],
      [0x67,0x97],
      [0x68,0x01],
      [0x69,0xCD],
      [0x6A,0x01],
      [0x6B,0xB0],
      [0x6C,0x04],
      [0x6D,0x2C],
      [0x6E,0x01],
      [0x6F,0x32],
      [0x71,0x00],
      [0x72,0x01],
      [0x73,0x35],
      [0x74,0x00],
      [0x75,0x33],
      [0x76,0x31],
      [0x77,0x01],
      [0x7C,0x84],
      [0x7D,0x03],
      [0x7E,0x01]]

   def __init__(self, i2cObj):
       self.i2cobj = None
       if not isinstance(i2cObj, I2C):
          raise ValueError("parameter is not an I2C object")

       self.i2cobj = i2cObj
       while self._begin() != 0:
         print("initial PAJ7620 error!! Please check if the wire sequence is correct?")
         utime.sleep(0.5)

   def _setReg(self,reg,value):
      buf = bytearray(value)
      self.i2cobj.memWrite(buf,reg,8)

   def _getReg(self, reg):
      buf = bytearray(8)
      self.i2cobj.memRead(buf,reg,8)
      return buf

   def _selBank(self,data):
      if data == self.BANK_0 or data == self.BANK_1:
         self._setReg(PAJ7620_REGITER_BANK_SEL, [data])

   def _begin(self):
      self._selBank(self.BANK_0)
      partid = self._getReg(PAJ7620_ADDR_PART_ID_LOW)
      if partid == None:
        return self.ERR_I2C_BUS
      utime.sleep(0.1)
      if (partid[1]<<8|partid[0]) != PAJ7620_PARTID:
        return self.ERR_IC_VERSION
      for i in range(0,len(self._init_register_array)):
        self._setReg(self._init_register_array[i][0], [self._init_register_array[i][1]])
      self._selBank(self.BANK_0)
      return self.ERR_OK

   def setGestureHighrate(self,status):
      self._gesture_highrate = status

   def gestureDescription(self,gesture):
      ges = str(gesture)
      return GESTURE.get(ges, "")

   def getGesture(self):
      buf = self._getReg(PAJ7620_ADDR_GES_PS_DET_FLAG_1)
      gesture = buf[0] << 8
      if gesture == self.GESTURE_WAVE:
         utime.sleep(GES_QUIT_TIME)
      else :
         gesture = self.GESTURE_NONE
         buf = self._getReg(PAJ7620_ADDR_GES_PS_DET_FLAG_0)
         gesture = buf[0] & 0x00ff
         if not self._gesture_highrate:
            utime.sleep(GES_ENTRY_TIME)
            tmp = self._getReg(PAJ7620_ADDR_GES_PS_DET_FLAG_0)
            utime.sleep(0.2)
            gesture = gesture|tmp[0]
         if gesture != self.GESTURE_NONE:
            utime.sleep(0.1)
         elif gesture == self.GESTURE_FORWARD:
            if not self._gesture_highrate:
               utime.sleep(GES_QUIT_TIME)
            else:
               utime.sleep(GES_QUIT_TIME / 5)
         elif gesture == self.GESTURE_BACKWARD:
            print("Backward Event Detected")
            if not self._gesture_high_rate:
               utime.sleep(GES_QUIT_TIME)
            else:
               utime.sleep(GES_QUIT_TIME / 5)
         else:
            tmp = self._getReg(PAJ7620_ADDR_GES_PS_DET_FLAG_1)
            if tmp[0]:
               gesture = self.GESTURE_WAVE
               print("Wave Event Detected")
      return gesture
