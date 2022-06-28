import utime
from micropython import const
from driver import UART
import sh1106              # SH1106 OLED驱动库
from driver import SPI     # 引入SPI总线库
from driver import GPIO    # 引入GPIO库
import framebuf            # framebuf基类，用于设置字体库
import _thread             # 线程库
import as608               # AS608指纹识别模块库

fig = None
p= -1
SaveResult = -1

from audio import Player, Snd

resDir = "/data/pyamp/resource/"
PUT_ON_FINGER = "putonfinger.wav"                    # 请将手指按压在指纹识别面板上
PUT_OFF_FINGER = "putofffinger.wav"                  # 请移开手指
FINGER_RECORD_SUCCESS = "fingerrecordsuccess.wav"    # 指纹录制成功
FINGER_SEARCH_FAIL = "searchfail.wav"                # 指纹识别失败 请再试一次
FINGER_SEARCH_SUCCESS = "searchfingersuccess.wav"    # 欢迎回家
FINGER_NOT_MATCH = "fingernotmatch.wav"              # 您的指纹不在指纹库中
PUT_ON_FINGER_AGAIN = "putonagain.wav"               # 请再次将手指放在指纹识别面板上
FINGER_RECORD_FAIL = "fingerrecordfail"              # 指纹录入失败 请重新录入
AUDIO_HEADER = 'fs:'

player = None
oled = None

K1 = None
K2 = None
K3 = None
K4 = None
keyEvents = {'K1':False, 'K2':False, 'K3':False, 'K4':False}

SYS_INIT = const(1)
SYS_ENROLLING = const(2)
SYS_DETECTING = const(3)

promptName = None

# OLED初始化
def oledInit():
    global oled

    # 字库文件存放于项目目录 font, 注意若用到了中英文字库则都需要放置
    framebuf.set_font_path(framebuf.FONT_HZK12, '/data/font/HZK12')
    framebuf.set_font_path(framebuf.FONT_HZK16, '/data/font/HZK16')
    framebuf.set_font_path(framebuf.FONT_HZK24, '/data/font/HZK24')
    framebuf.set_font_path(framebuf.FONT_HZK32, '/data/font/HZK32')
    framebuf.set_font_path(framebuf.FONT_ASC12_8, '/data/font/ASC12_8')
    framebuf.set_font_path(framebuf.FONT_ASC16_8, '/data/font/ASC16_8')
    framebuf.set_font_path(framebuf.FONT_ASC24_12, '/data/font/ASC24_12')
    framebuf.set_font_path(framebuf.FONT_ASC32_16, '/data/font/ASC32_16')

    oled_spi = SPI()
    oled_spi.open("oled_spi")

    oled_res = GPIO()
    oled_res.open("oled_res")

    oled_dc = GPIO()
    oled_dc.open("oled_dc")

    #oled像素132*64
    oled = sh1106.SH1106_SPI(132, 64, oled_spi, oled_dc, oled_res)

# 按键K1~K4的初始化
def keyInit():
    global K1, K2, K3, K4
    K1 = GPIO()
    K1.open("KEY_1")
    K1.on(k1Handler)

    K2 = GPIO()
    K2.open("KEY_2")
    K2.on(k2Handler)

    K3 = GPIO()
    K3.open("KEY_3")
    K3.on(k3Handler)

    K4 = GPIO()
    K4.open("KEY_4")
    K4.on(k4Handler)

def k1Handler(obj):
    print('K1 pressed')
    keyEvents['K1'] = True


def k2Handler(obj):
    print('K2 pressed')
    keyEvents['K2'] = True

def k3Handler(obj):
    print('K3 pressed')
    keyEvents['K3'] = True

def k4Handler(obj):
    print('K4 pressed')
    keyEvents['K4'] = True

# 检查是否有按键按下事件处于pending状态
def keyEventsPending():
    if all(value == False for value in keyEvents.values()):
        return False
    return True

# 清除按键事件
def clearKeyEvents():
    keyEvents['K1'] = False
    keyEvents['K2'] = False
    keyEvents['K3'] = False
    keyEvents['K4'] = False
    pass

# OLED显示
# text:显示的文本
# x:水平坐标 y:垂直坐标 
# color:颜色
# clear: True-清屏显示 False-不清屏显示
# sz:字体大小
def oledShowText(text, x, y, color, clear, sz):
    global oled
    if clear:
        oled.fill(0) # 清屏
    oled.text(text, x, y, color, size = sz)
    oled.show()

# 清除屏幕
def oledClear():
    oled.fill(0) # 清屏
    oled.show()

# 播放器初始化
def playerInit():
    global player
    Snd.init()
    player = Player()
    player.open()
    player.setVolume(8)

# 播放提示音
def playVoicePrompt(prompt):

    global promptName
    promptName = prompt
    '''
    global player

    if player.getState():
        player.stop()

    # 音频文件路径格式：‘fs:/data/pyamp/resource/xxx.wav’
    player.play(AUDIO_HEADER + resDir + prompt)
    '''

def playVoicePromptSync(prompt):
    '''
    global promptName
    promptName = prompt
    '''

    global player

    if player.getState():
        player.stop()

    # 音频文件路径格式：‘fs:/data/pyamp/resource/xxx.wav’
    player.play(AUDIO_HEADER + resDir + prompt)
    player.waitComplete()


# 指纹录入
def fingerEnroll(id):
    p = as608.FAIL
    global SaveResult

    # 如果在识别指纹过程中被返回初始页面操作打断，则退出此循环
    if keyEvents['K4'] or keyEvents['K2']:
        return
    playVoicePrompt(PUT_ON_FINGER)
    oledShowText('请按压触控板', 32, 28, 1, True, 12)
    print('wait for finger print on the pannel...')

    while p != as608.NO_FINGER:
        p = fig.getImage()

    print('invalid, please put your finger on the pannel again...')
    # 开始采集指纹
    while p != as608.SUCCESS:
        p = fig.getImage()
        # 如果在识别指纹过程中被返回初始页面操作打断，则退出此循环
        if keyEvents['K4'] or keyEvents['K2']:
            return

    print('finger detected')
    # 指纹图片转化为特征值
    p = fig.image2Character(1)
    if p != as608.SUCCESS:
        SaveResult = 0
        print('image to text failed, exit...')
        playVoicePrompt(FINGER_RECORD_FAIL)
        return 0

    # putofffinger
    print('take off your finger please')
    oledShowText('请移开手指', 32, 28, 1, True, 12)
    playVoicePrompt(PUT_OFF_FINGER)

    #Take off your finger
    p = 0

    while p != as608.NO_FINGER:
        p = fig.getImage()

    # put on again
    #Get image again
    print('put on your finger again, please...')
    oledShowText('请再次按压触控板', 17, 28, 1, True, 12)
    playVoicePrompt(PUT_ON_FINGER_AGAIN)

    while p != as608.SUCCESS:
        p = fig.getImage()

    # 指纹图片转化为特征值
    p = fig.image2Character(2)
    if p != as608.SUCCESS:
        SaveResult = 0
        print('image to text failed, exit...')
        playVoicePrompt(FINGER_RECORD_FAIL)
        return 0
    
    print('creating finger model...')
    # 创建指纹模板
    p = fig.createModel()
    if p != as608.SUCCESS:
        SaveResult = 0
        print('creating model failed')
        return 0

    print('store finger model...')
    # 存储指纹模板
    p = fig.storeModel(id)
    if p != as608.SUCCESS:
        SaveResult = 0
        # fingerrecordfail
        oledShowText('指纹录入失败', 25, 28, 1, True, 12)
        playVoicePromptSync(FINGER_RECORD_FAIL)
        return 0
    SaveResult = 1
    # fingerrecordsuccess
    oledShowText('指纹录入成功', 25, 28, 1, True, 12)
    playVoicePromptSync(FINGER_RECORD_SUCCESS)

    return 1

# 指纹识别
def fingerSearch():
    p = as608.FAIL
    print('search finger...')
    print('wait for finger print on the pannel...')
    while p != as608.NO_FINGER:
        p = fig.getImage()

    while p != as608.SUCCESS:
        p = fig.getImage()
        # 如果在识别指纹过程中被返回初始页面操作打断，则退出此循环
        if keyEvents['K4'] or keyEvents['K2']:
            return

    print('finger detected')

    p = fig.image2Character(1)
    if p != as608.SUCCESS:
        # 指纹图片转换为特征值失败
        print('image to text failed, exit...')
        playVoicePrompt(FINGER_SEARCH_FAIL)

        return -1

    # 在指纹库中搜索指纹
    p, id, confidence = fig.search() 
    if p == as608.SUCCESS:
        print('finger id:', id, ' confidence:', confidence)
        # searchfingersuccess
        oledShowText('指纹识别成功', 32, 28, 1, True, 12)
        playVoicePrompt(FINGER_SEARCH_SUCCESS)
        return id
    else:
        print('no match finger found')
        # fingernotmatch
        oledShowText('指纹未注册', 32, 28, 1, True, 12)
        playVoicePrompt(FINGER_NOT_MATCH)
        return -1

def promptThread(arg):
    global player, promptName

    while True:
        if promptName:
            prompt = promptName
            promptName = None
            # 如果播放器处于播放状态，先停止播放当前提示音
            if player.getState():
                player.stop()

            # 音频文件路径格式：‘fs:/data/pyamp/resource/xxx.wav’
            player.play(AUDIO_HEADER + resDir + prompt)
            player.waitComplete()
        else:
            utime.sleep(0.1)
            #print('promptThread')
            pass

# 主函数
if __name__ == '__main__':
    # 系统状态机初始化
    systemState = SYS_INIT
    # OLED屏幕初始化
    oledInit()

    # 播放器初始化
    playerInit()

    # 串口设备初始化
    uartDev = UART()
    uartDev.open('as608')
    # AS608指纹识别装置初始化
    fig = as608.AS608(uartDev)
    # 按键初始化
    keyInit()

    try:
        # 启动播放声音线程
        _thread.stack_size(10 * 1024)
        _thread.start_new_thread(promptThread, ['promptThread'])
    except:
       print("Error: unable to start thread")


    # 清除按键状态
    clearKeyEvents()
    while True:

        # 对按键时键进行处理
        if keyEvents['K1']:
            if systemState == SYS_INIT:
                systemState = SYS_ENROLLING

        if keyEvents['K3']:
            if systemState == SYS_INIT:
                systemState = SYS_DETECTING

        if keyEvents['K4'] or keyEvents['K2']:
            # 返回按键
            systemState = SYS_INIT

        # 清除按键中断事件
        clearKeyEvents()

        # 系统状态机切换
        if systemState == SYS_INIT:
            oledShowText('<              >', 2, 28, 1, True, 16)
            oledShowText('指纹注册  指纹识别', 10, 30, 1, False, 12)
            # 等待用户操作
            while not keyEventsPending():
                utime.sleep(0.1)
                pass

        elif systemState == SYS_ENROLLING:
            oledShowText('指纹注册中', 32, 28, 1, True, 12)
            # 查找空闲可用id
            id = fig.getEmptyPosition()

            # 指纹注册
            fingerEnroll(id)
            systemState = SYS_INIT
        elif systemState == SYS_DETECTING:
            # 开始指纹识别
            oledShowText('指纹识别中', 38, 28, 1, True, 12)
            fingerSearch()
    
    uartDev.close()
    del fig