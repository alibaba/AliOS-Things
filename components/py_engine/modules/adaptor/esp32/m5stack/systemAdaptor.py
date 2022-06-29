from machine import Pin

def getSupportedPinMode():
    pinMode = {
        'input': Pin.IN,
        'output': Pin.OUT,
        'opendrain': Pin.OPEN_DRAIN
    }
    return pinMode

def getChipName():
    return "esp32"

def getDataPath():
    return "/"

def getPinMap():
    pinMap = {
        I2C0 : {
            PORT_NUM : 0,
            I2C0_SCL : 22,
            I2C0_SDA : 21,
        },
        SPI3 : {
            PORT_NUM : 3,
            SPI3_MOSI : 23,
            SPI3_SCLK : 18,
            SPI3_MOSO : 19,
            SPI3_CS: 5,
        },
        UART2 : {
            PORT_NUM : 2,
            UART_TXD : 14,
            UART_RXD : 13,
        },
        ADC0: 36,
        ADC1: 39,
        ADC2: 34, 
        ADC3: 35,
        ADC4: 32,
        ADC5: 33
    }
    return pinMap