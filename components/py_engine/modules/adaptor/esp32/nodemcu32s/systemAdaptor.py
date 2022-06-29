from machine import Pin

def getSupportedPinMode():
    pinMode = {
        'input': Pin.IN,
        'output': Pin.OUT,
        'irq': Pin.IN,
        'opendrain' : Pin.OPEN_DRAIN,
        'rising' : Pin.IRQ_RISING,
        'falling' : Pin.IRQ_FALLING
    }
    return pinMode

def getChipName():
    return "esp32"

def getDataPath():
    return "/"


def getPinMap():
    pinMap = {
        "I2C0" : {
            "PORT": 0,
            "SCL" : 22,
            "SDA" : 21,
        },
        "SPI3" : {
            "PORT": 2,
            "MOSI" : 23,
            "SCLK" : 18,
            "MOSO" : 19,
            "CS": 5,
        },
        "UART2" : {
            "PORT": 2,
            "TX" : 17,
            "RX" : 16,
        },
        "ADC0": 36,
        "ADC3": 39,
        "ADC4": 32, 
        "ADC5": 33,
        "ADC6": 34,
        "ADC7": 35
    }
    return pinMap