from machine import Pin

def getChipName():
    return "esp32"

def getDataPath():
    return "/"

def getSupportedPinMode():
    pinMode = {
        'input': Pin.IN,
        'output': Pin.OUT,
        'opendrain': Pin.OPEN_DRAIN
    }
    return pinMode

def getPinMap():
    pinMap = {
        I2C0 : {
            PORT_NUM : 0,
            I2C0_SCL : 2,
            I2C0_SDA : 1,
        },
        SPI2 : {
            PORT_NUM : 2,
            SPI3_MOSI : 11,
            SPI3_SCLK : 12,
            SPI3_MOSO : 13,
            SPI3_CS: 10,
        },
        UART1 : {
            PORT_NUM : 1,
            UART_TXD : 41,
            UART_RXD : 42,
        },
        ADC0: 36,
        ADC1: 39,
        ADC2: 34, 
        ADC3: 35,
        ADC4: 32,
        ADC5: 33
    }
    return pinMap