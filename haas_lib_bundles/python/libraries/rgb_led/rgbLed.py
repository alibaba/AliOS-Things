from driver import PWM

class RgbLed():
    def __init__(self, pwmRObj, pwmGObj, pwmBObj):
        if not isinstance(pwmRObj, PWM):
            raise ValueError("parameter pwmRObj is not an PWM object")
        if not isinstance(pwmGObj, PWM):
            raise ValueError("parameter pwmGObj is not an PWM object")
        if not isinstance(pwmBObj, PWM):
            raise ValueError("parameter pwmBObj is not an PWM object")
        self.pwmR = pwmRObj
        self.pwmG = pwmGObj
        self.pwmB = pwmBObj

        self.pwmR.setOption({'freq': 255, 'duty': 0})
        self.pwmG.setOption({'freq': 255, 'duty': 0})
        self.pwmB.setOption({'freq': 255, 'duty': 0})

    def setColor(self, color):
        if isinstance(color, tuple) and len(color) is 3:
            red = color[0]
            green = color[1]
            blue = color[2]

            self.pwmR.setOption({'freq': 255, 'duty': 255 // red})
            self.pwmG.setOption({'freq': 255, 'duty': 255 // green})
            self.pwmB.setOption({'freq': 255, 'duty': 255 // blue})

        elif isinstance(color, int) and color >= 0 and color <= 0xffffff:
            red = color >> 12
            green = (color >> 8) & 0x00ff
            blue = color & 0x0000ff

            self.pwmR.setOption({'freq': 255, 'duty': 255 // red})
            self.pwmG.setOption({'freq': 255, 'duty': 255 // green})
            self.pwmB.setOption({'freq': 255, 'duty': 255 // blue})

        else:
            raise ValueError("color type error! color should be like (255, 255, 255) or 0xffffff")
