import utime   # 延时函数在utime
from .detections.fall_detection import fall_detection
from .detections.tap_detection import tap_detection

class Motion:
    def __init__(self, action, getData, onActionDetected):
        self.action = action
        if (action == "fall"):
            self.detectAction = fall_detection(getData)
        elif (action == "single_tap"):
            self.detectAction = tap_detection(1, getData)
        elif (action == "double_tap"):
            self.detectAction = tap_detection(2, getData)
        self.onActionDetected = onActionDetected

    # 使能action检测，若用户不指定灵敏度，则使用默认灵敏度
    def enable(self, sensitivity=''):
        self.enableDetection = True
        if (sensitivity != ''):
            self.detectAction.sensitivity = sensitivity
        self.detect_action()

    # 关闭action检测
    def disable(self):
        self.enableDetection = False

    def detect_action(self):
        while(self.enableDetection == True):
            isDetected = self.detectAction.detect()                  # 检测Action是否产生
            if (isDetected == True):                                 # Action被检测到
                print(self.action, "detected!")
                if (hasattr(self, 'onActionDetected')):
                    self.onActionDetected()
            utime.sleep_us(10)
