from machine import Pin

def getChipName():
    return "stm32"

def getDataPath():
    return "/"

def getPinMap():
    pinMap = {
        "SPI2" : {
            "PORT": 2,
            "MOSI" : "B15",
            "SCLK" : "B13",
            "MOSO" : "C2",
            "CS": "B12",
        }
    }
    return pinMap

def getSupportedPinMode():
    pinMode = {
        'input': Pin.IN,            # 0
        'output': Pin.OUT,          # 1
        'alt': Pin.ALT,             # 2
        'analog': Pin.ANALOG,       # 3
        'opendrain': Pin.OUT_OD,    # 17
        'altopendrain': Pin.ALT_OPEN_DRAIN # 18
    }
    return pinMode