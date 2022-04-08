# -*- encoding: utf-8 -*-
'''
@File       :    heartbeat.py
@Description:    心率传感器驱动
@Author     :    victor.wang
@version    :    1.0
'''
from driver import ADC                           # ADC类，通过微处理器的ADC模块读取ADC通道输入电压
from driver import TIMER                         # 定时器类，用于定时调用心率传感器
from driver import GPIO

class HEARTBEAT (object):
    def __init__(self, adcdev=None, rled=None, gled=None, bled=None, highBPM=150, lowBPM=100):

        if (not isinstance(adcdev, ADC)):
            raise ValueError("parameter is wrong")

        self.adcObj = adcdev
        self.rled=rled
        self.bled=bled
        self.gled=gled
        self.highBPM=highBPM
        self.lowBPM=lowBPM

        self.ledexist= (rled!=None and gled!=None and bled!=None)

        self.rate=[0 for _ in range(10)]

        self.sampleCounter = 0     # 心跳时间
        self.lastBeatTime = 0      # IBIT
        self.P = 1800              # 用于确定心跳波形的最大值
        self.T = 1800              # 用于确定心跳波形的最小值
        self.thresh = 1800         # 用于确定心跳的瞬态值
        self.amp = 150             # 心率波形的幅度
        self.firstBeat = True      # 第一次心跳
        self.secondBeat = False    # 第二次心跳

        self.BPM=0;                # 心率
        self.Signal = 0            # 心跳信号
        self.IBI = 600             # 心跳间隔
        self.Pulse = False         # 心跳波心是否为高
        self.QS = False            # 找到心跳

        self.timerobj = TIMER(0)  # 定时器
        self.timerobj.open(period=2, mode=self.timerobj.PERIODIC, callback=self.getHeartbeat)

    def getBPM(self):
        return self.BPM

    def getHeartbeat(self, args):
        self.timerobj.stop()
        self.Signal = self.adcObj.readVoltage()           # 读心跳传感器
        self.sampleCounter +=2.5                          # 记录时间 单位（ms）(理论值为2ms  2.5ms 为校准值)
        NUM = self.sampleCounter - self.lastBeatTime      # 距上次心跳时间

        # 确定心跳波形的波峰和波谷
        if(self.Signal < self.thresh and NUM > (self.IBI/5)*3):   # 去除dichrotic噪音
            if (self.Signal < self.T):                            # self.T  是波谷值
                self.T = self.Signal                              # 记录波谷值

        if (self.Signal > self.thresh and self.Signal > self.P):
            self.P = self.Signal                                  # self.P 是波峰


        if (NUM > 250):
            if ((self.Signal > self.thresh) and (self.Pulse == False)):
                self.Pulse = True
                if self.ledexist:
                    if self.BPM >= self.highBPM:                     # 如果心率过快，点亮红灯
                        self.rled.write(1)
                    elif self.BPM <= self.lowBPM:                    # 如果心率过慢，点亮蓝灯
                        self.bled.write(1)
                    else:                                            # 如果心率合适，点亮绿灯
                        self.gled.write(1)

                self.IBI = self.sampleCounter - self.lastBeatTime            # 心跳间隔
                self.lastBeatTime = self.sampleCounter                       # 更新心跳间隔起始点

                if(self.secondBeat):                                         # 第二次心跳
                    self.secondBeat = False                                  # 清除第二次心跳标识
                    for i in range(10):
                        self.rate[i] = self.IBI

                if(self.firstBeat):                                          # 第一次心跳
                    self.firstBeat = False                                   # 清除第一次心跳标识
                    self.secondBeat = True                                   # 置位第二次心跳标识
                    self.timerobj.start()
                    return

                runningTotal = 0
                for i in range(9):
                    self.rate[i] = self.rate[i+1]                        # 丢弃最老的IBI值
                    runningTotal = runningTotal + self.rate[i]           # 求和最老的9个IBI值

                self.rate[9] = self.IBI
                runningTotal = runningTotal+self.rate[9]
                runningTotal = runningTotal/10
                self.BPM = 60000/runningTotal                             # 计算心率
                self.QS = True

        if ((self.Signal < self.thresh) and (self.Pulse == True)):
            if self.ledexist:
                self.rled.write(0)
                self.bled.write(0)
                self.gled.write(0)
            self.Pulse = False
            self.amp = self.P - self.T
            self.thresh = self.amp/2 + self.T
            self.P = self.thresh
            self.T = self.thresh

        if (NUM > 2500):                                                  # 如果2.5秒后还没检测到心跳
            self.thresh = 1800                                            # thresh 设置为默认值
            self.P = 1800                                                 # P 设置为默认值
            self.T = 1800                                                 # T 设置为默认值
            self.lastBeatTime = self.sampleCounter                        # 更新lastBeatTime
            self.firstBeat = True                                         # 置位第一次心跳标识
            self.secondBeat = False                                       # 清除第二次心跳标识

        self.timerobj.start()
        return

    def start(self):
        self.timerobj.start()

    def stop(self):
        self.timerobj.stop()
        self.timerobj.close()
