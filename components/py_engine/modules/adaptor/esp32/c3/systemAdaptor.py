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
            I2C0_SCL : 4,
            I2C0_SDA : 5,
        },
        SPI3 : {
            PORT_NUM : 3,
            SPI3_MOSI : 7,
            SPI3_SCLK : 6,
            SPI3_MOSO : 2,
            SPI3_CS: 10,
        },
        UART1 : {
            PORT_NUM : 1,
            UART_TXD : 9,
            UART_RXD : 3,
        },
        ADC0: 36,
        ADC1: 39,
        ADC2: 34, 
        ADC3: 35,
        ADC4: 32,
        ADC5: 33
    }
    return pinMap