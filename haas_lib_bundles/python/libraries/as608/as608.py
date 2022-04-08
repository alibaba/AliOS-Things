"""
Copyright (C) 2015-2021 Alibaba Group Holding Limited
`HaaS Python as608`
====================================================

A driver for the AS608 finger print module

* Author(s): HaaS Group

Implementation Notes
--------------------

**Hardware:**

* HaaS Python as608
  https://haas.iot.aliyun.com/solution/detail/hardware

**Software and Dependencies:**

* HaaS Python API documents:
  https://haas.iot.aliyun.com/haasapi/index.html#/

* HaaS Python Driver Libraries:
  https://github.com/alibaba/AliOS-Things/tree/master/haas_lib_bundles/python/libraries

"""

import utime
import ustruct
from micropython import const
from driver import UART

# check passwoard: 4 byte 0000
cmdVerifyPasswoard = b'\xEF\x01\xFF\xFF\xFF\xFF\x01\x00\x07\x13\x00\x00\x00\x00\x00\x1B'

# 采集图片
cmdGetImage = b'\xEF\x01\xFF\xFF\xFF\xFF\x01\x00\x03\x01\x00\x05'

# 生成指纹图片对应的特征值
cmdImage2Char = b'\xEF\x01\xFF\xFF\xFF\xFF'
cmdSaveimage2Char = b'\x01\x00\x04\x02'

# 创建指纹模板
cmdCreateModel = b'\xEF\x01\xFF\xFF\xFF\xFF\x01\x00\x03\x05\x00\x09'

# 保存指纹模板
cmdStoreModel = b'\xEF\x01\xFF\xFF\xFF\xFF'
cmdSaveStoreModel = b'\x01\x00\x06\x06\x01'

#指纹匹配指令
cmdMatch = b'\xEF\x01\xFF\xFF\xFF\xFF\x01\x00\x03\x03\x00\x07'

#指纹搜索指令
cmdSearch = b'\xEF\x01\xFF\xFF\xFF\xFF\x01\x00\x08\x04\x01\x00\x00\x00\x7F\x00\x8D'

# 读取索引表
cmdReadIndexTable = b'\xEF\x01\xFF\xFF\xFF\xFF\x01\x00\x04\x1F\x00\x00\x24'

#删除指纹记录
cmdDeleteModel = b'\xEF\x01\xFF\xFF\xFF\xFF'
cmdSaveDeleteModel = b'\x01\x00\x07\x0c\x00'

# 清除数据库中的指纹信息
cmdEmptyDatabase = b'\xEF\x01\xFF\xFF\xFF\xFF\x01\x00\x03\x0D\x00\x11'

# 获取指纹图片信息
cmdGetFPImage = b'\xEF\x01\xFF\xFF\xFF\xFF\x01\x00\x03\x0a\x00\x0e'

# 获取指纹特征值信息
cmdGetFPChar = b'\xEF\x01\xFF\xFF\xFF\xFF\x01\x00\x04\x08\x00\x00\x00'


SUCCESS = const(0)
FAIL = const(-1)
NO_FINGER = const(2)

DATABASE_CAPABILITY = const(300)

CMD_RSP_TIMEOUT_MS = const(500) # 单条指令超时时间
CMD_RSP_WAIT_TIME_MS = const(10) # 目前指令的response最多44个byte，44*(1+8+2+1)/57600 ~= 9.9ms

class AS608:
    def __init__(self, *args, **kargs):
        self._uartDev = None

        if not isinstance(args[0], UART):
            raise ValueError("parameter is not an UART object")

        #实例化和AS608通信所用串口
        self._uartDev=args[0]

    #接收指令执行结果
    def getCmdResult(self):
        cnt = 0
        len = 12
        rx = bytearray(len * b'5')

        # 检查UART接收缓冲区中是否有足够的数据
        l = self._uartDev.any()
        while(l < len):
            # print('uart.any:', l)
            cnt += 1
            if cnt > CMD_RSP_TIMEOUT_MS/CMD_RSP_WAIT_TIME_MS: # 等待超时时间后退出
                break
            utime.sleep_ms(CMD_RSP_WAIT_TIME_MS)
            l = self._uartDev.any()

        self._uartDev.read(rx)
        return rx

    def matchCmdResult(self):
        cnt = 0
        len = 14
        rx = bytearray(len * b'5')

        # 检查UART接收缓冲区中是否有足够的数据
        l = self._uartDev.any()
        while(l < len):
            # print('uart.any:', l)
            cnt += 1
            if cnt > CMD_RSP_TIMEOUT_MS/CMD_RSP_WAIT_TIME_MS: # 等待超时时间后退出
                break
            utime.sleep_ms(CMD_RSP_WAIT_TIME_MS)
            l = self._uartDev.any()

        self._uartDev.read(rx)
        return rx

    #接收指纹图像数据
    def getLongResult(self):
        cnt = 0
        rx = bytearray(0)

        # 检查UART接收缓冲区中是否有足够的数据
        while(cnt < 5):
            utime.sleep_ms(30)
            buf = bytearray(512)
            l = self._uartDev.any()
            # 检查UART中是否有数据
            if l > 0:
                # 从UART读取数据
                l = self._uartDev.read(buf)
            #print(l)

            if l > 0:
                # 合并UART返回结果
                rx += buf[0:l]
                cnt = 0
            else:
                cnt += 1

        return rx

    # 接收搜索指令结果
    def searchCmdResult(self):
        cnt = 0
        len = 16
        rx = bytearray(len * b'5')

        l = self._uartDev.any()
        while(l < len):
            # print('uart.any:', l)
            cnt += 1
            if cnt > CMD_RSP_TIMEOUT_MS/CMD_RSP_WAIT_TIME_MS: # 等待超时时间后退出
                break
            utime.sleep_ms(CMD_RSP_WAIT_TIME_MS)
            l = self._uartDev.any()
        self._uartDev.read(rx)
        return rx

    # 接收索引表结果
    def readIndexCmdResult(self):
        cnt = 0
        len = 44
        rx = bytearray(len * b'5')

        l = self._uartDev.any()
        while(l < len):
            # print('uart.any:', l)
            cnt += 1
            if cnt > CMD_RSP_TIMEOUT_MS/CMD_RSP_WAIT_TIME_MS: # 等待超时时间后退出
                break
            utime.sleep_ms(CMD_RSP_WAIT_TIME_MS)
            l = self._uartDev.any()
        self._uartDev.read(rx)
        return rx

    # 验证密码
    def verifyPassword(self):
        self._uartDev.write(cmdVerifyPasswoard)

        rsp = self.getCmdResult()

        # 检查命令是否执行成功
        if rsp[-3] == 0:
            return SUCCESS
        else:
            return FAIL
        '''
        Confirm code=00H shows OK;
        Confirm Code=01H shows receiving packet error;
        Confirm Code=13H shows password incorrect;
        Confirm Code=21H shows Must verify password first;
        '''

    # 录入指纹图像
    def getImage(self):
        self._uartDev.write(cmdGetImage)
        rsp = self.getCmdResult()
        # print(rsp)
        # 检查命令是否执行成功
        if rsp[9] == 0:
            return SUCCESS
        elif rsp[9] == 2:
            return NO_FINGER
        else:
            return FAIL
        '''
        Confirm Code=00H - 录入成功
        Confirm Code=01H - 收包错误
        Confirm Code=02H - 传感器上无手指
        Confirm Code=03H - 指纹录入失败
        '''

    # 生成指纹对应的特征值， slot代表Buffer缓冲区ID
    def image2Character(self, slot = 1):
        sumImage2Char = cmdSaveimage2Char + bytearray([slot, 0, slot + 0x7])
        self._uartDev.write(cmdImage2Char)
        self._uartDev.write(sumImage2Char)

        rsp = self.getCmdResult()

        # 检查命令是否执行成功
        if rsp[9] == 0:
            return SUCCESS
        else:
            return FAIL
        '''
        Confirm Code=00H - 生成特征值成功
        Confirm Code=01H - 收包错误
        Confirm Code=06H - 指纹图像太乱，生成特征值失败
        Confirm Code=07H - 特征点太少，生成特征值失败
        feature;
        Confirm Code=15H - 图像缓冲区内没有有效原始图，生成特征值失败
        '''

    # 合并特征并生成模板
    def createModel(self):
        self._uartDev.write(cmdCreateModel)

        rsp = self.getCmdResult()

        # 检查命令是否执行成功
        if rsp[9] == 0:
            return SUCCESS
        else:
            return FAIL
        '''
        Confirm Code=00H - 合并成功
        Confirm Code=01H - 收包错误
        Confirm Code=0aH - 合并失败（两枚指纹不属于同一手指）
        '''

    # 将模板文件存储到PageID中，默认存储缓冲区1中的模板
    def storeModel(self, id):
        #sumStoreModel = cmdSaveStoreModel + bytearray([id, 0, id + 0x0E])
        payload = cmdSaveStoreModel + bytearray([(id >> 8) & 0xff, id & 0xff])
        s = sum(payload)
        sumStoreModel = cmdStoreModel + payload + bytearray([(s >> 8) & 0xff, s & 0xff])

        self._uartDev.write(sumStoreModel)

        rsp = self.getCmdResult()

        # 检查命令是否执行成功
        if rsp[9] == 0:
            return SUCCESS
        else:
            return FAIL
        '''
        Confirm Code=00H - 储存成功
        Confirm Code=01H - 收包错误
        Confirm Code=0bH - pageID超出范围
        Confirm Code=18H - 写Flash操作出错
        '''
    # 精确比对两枚指纹特征
    def match(self):
        self._uartDev.write(cmdMatch)

        rsp = self.matchCmdResult()

        # 检查命令是否执行成功
        if rsp[9] == 0:
            return SUCCESS
        else:
            return FAIL
        '''
        Confirm Code=00H - 指纹匹配
        Confirm Code=01H - 收包错误
        Confirm Code=08H - 指纹不匹配
        '''
    # 以缓冲区1或缓冲区2中的特征文件搜索整个或部分指纹库，若搜索到，返回页码
    def search(self):
        result = FAIL
        fingerId = -1
        confidence = 0
        self._uartDev.write(cmdSearch)

        rsp = self.searchCmdResult()
        # print(rsp)
        # 检查命令是否执行成功
        if rsp[9] == 0:
            result = SUCCESS
            fingerId, confidence = ustruct.unpack(">HH", bytes(rsp[10:14]))
        else:
            fingerId, confidence = -1, 0
        # print (result, fingerId, confidence)

        return result, fingerId, confidence
        '''
        Confirm Code=00H - 搜索成功
        Confirm Code=01H - 收包错误
        Confirm Code=09H - 没有搜索到，此时fingerId和confidence均为0
        '''
    # 删除Flash指纹库中的一个特征文件
    def deleteModel(self, id):
        if id >= DATABASE_CAPABILITY or id < 0:
            return FAIL

        deleteModel = cmdSaveDeleteModel + bytearray([id, 0, 1, 0, id + 0x15])
        self._uartDev.write(cmdDeleteModel)
        self._uartDev.write(deleteModel)

        rsp = self.getCmdResult()

        # 检查命令是否执行成功
        if rsp[9] == 0:
            return SUCCESS
        else:
            return FAIL
        '''
        Confirm Code=00H - 删除模板成功
        Confirm Code=01H - 收包错误
        Confirm Code=10H - 删除模板失败
        '''

    # 删除flash数据库中的所有指纹模板
    def emptyDatabase(self):
        self._uartDev.write(cmdEmptyDatabase)

        rsp = self.getCmdResult()
        # 检查命令是否执行成功
        if rsp[9] == 0:
            return SUCCESS
        else:
            return FAIL
        '''
        Confirm Code=00H - 清空指纹模板成功
        Confirm Code=01H - 收包错误
        Confirm Code=11H - 清空指纹模板失败
        '''

    # 获取指纹特征值
    def getFPCharacter(self, slot = 1):
        # 获取指纹特征值信息
        cmd = bytearray(cmdGetFPChar)
        cmd[10] = slot

        s = sum(cmd[6:11])
        cmd[11] = (s >> 8) & 0xff
        cmd[12] = s & 0xff

        self._uartDev.write(cmd)

        rsp = self.getLongResult()
        # 检查命令是否执行成功
        if rsp[9] == 0:
            return SUCCESS, rsp[12:len(rsp)]
        else:
            return FAIL, []
        '''
        Confirm Code=00H - 清空指纹模板成功
        Confirm Code=01H - 收包错误
        Confirm Code=0dH - 指纹执行失败
        '''

    # 获取指纹图像
    def getFPImage(self):
        self._uartDev.write(cmdGetFPImage)

        rsp = self.getLongResult()
        # 检查命令是否执行成功
        if rsp[9] == 0:
            return SUCCESS, rsp[12:len(rsp)]
        else:
            return FAIL, []

        '''
        Confirm Code=00H - 清空指纹模板成功
        Confirm Code=01H - 收包错误
        Confirm Code=0fH - bu不能发送后续数据包
        '''

    def getEmptyPosition(self):
        for i in range(4):
            cmdReadIndexTable = b'\xEF\x01\xFF\xFF\xFF\xFF\x01\x00\x04\x1F\x00\x00\x24'

            s = sum(cmdReadIndexTable[6:10]) + i
            cmd = cmdReadIndexTable[0:10] + bytearray([i]) + bytearray([(s >> 8) & 0xff, s & 0xff])
            self._uartDev.write(cmd)
            rsp = self.readIndexCmdResult()
            # print(rsp)
            # 检查命令是否执行成功
            if rsp[9] == 0:
                index = rsp[10:41]
                for j in range(len(index)):
                    for m in range(8):
                        if not (index[j] & (1 << m)):
                            return i * 32 + j * 8 + m

        return FAIL

    # 指纹录入
    def fingerEnroll(self, id):
        p = FAIL

        if id >= DATABASE_CAPABILITY or id < 0:
            return FAIL

        print('wait for finger print on the pannel')

        while p != NO_FINGER:
            p = self.getImage()

        # 开始采集指纹
        while p != SUCCESS:
            p = self.getImage()

        print('finger detected')
        # 指纹图片转化为特征值
        p = self.image2Character(1)
        if p != SUCCESS:
            print('image to text failed, exit')
            return 0

        # 再录制一次
        print('take off your finger, please')

        #Take off your finger
        p = 0

        while p != NO_FINGER:
            p = self.getImage()

        # put on again
        # Get image again
        print('put on your finger again, please')

        while p != SUCCESS:
            p = self.getImage()

        # 指纹图片转化为特征值
        p = self.image2Character(2)
        if p != SUCCESS:
            return 0

        print('creating finger model')
        # 创建指纹模板
        p = self.createModel()
        if p != SUCCESS:
            print('creating model failed')
            return 0

        print('store finger model')
        # 存储指纹模板
        p = self.storeModel(id)
        if p != SUCCESS:
            # fingerrecordfail
            print('store finger model failed')
            return FAIL
        print('store finger model success')
        return SUCCESS

    # 指纹识别
    def fingerSearch(self):
        p = FAIL
        print('search finger')
        print('wait for finger print on the pannel')
        while p != NO_FINGER:
            p = self.getImage()

        while p != SUCCESS:
            p = self.getImage()

        print('finger detected')

        p = self.image2Character(1)
        if p != SUCCESS:
            # 指纹图片转换为特征值失败
            print('image to text failed, exit')
            return -1

        # 在指纹库中搜索指纹
        p, id, confidence = self.search()
        if p == SUCCESS:
            # 搜索成功
            print('finger id:', id, ' confidence:', confidence)
            return SUCCESS, id
        else:
            # 搜索失败
            print('no match finger found')
            return FAIL, -1


