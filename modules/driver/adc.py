# Adapter for machine driver

from boardparser import BoardConfigParser
from machine import Pin as mach_Pin
from machine import ADC as mach_ADC


"""
ADC APIs:

"adc_test": {
    "type": "ADC",
    "port": 6,
    "sampling": 12000000,
    "atten": 0,
    "width": 3
}

const pin_obj_t * const pin_adc1[16] = {
  [0] = &pin_A0_obj,
  [1] = &pin_A1_obj,
  [2] = &pin_A2_obj,
  [3] = &pin_A3_obj,
  [4] = &pin_A4_obj,
  [5] = &pin_A5_obj,
  [6] = &pin_A6_obj,
  [7] = &pin_A7_obj,
  [8] = &pin_B0_obj,
  [9] = &pin_B1_obj,
  [10] = &pin_C0_obj,
  [11] = &pin_C1_obj,
  [12] = &pin_C2_obj,
  [13] = &pin_C3_obj,
  [14] = &pin_C4_obj,
  [15] = &pin_C5_obj,
};

const pin_obj_t * const pin_adc2[16] = {
  [0] = &pin_A0_obj,
  [1] = &pin_A1_obj,
  [2] = &pin_A2_obj,
  [3] = &pin_A3_obj,
  [4] = &pin_A4_obj,
  [5] = &pin_A5_obj,
  [6] = &pin_A6_obj,
  [7] = &pin_A7_obj,
  [8] = &pin_B0_obj,
  [9] = &pin_B1_obj,
  [10] = &pin_C0_obj,
  [11] = &pin_C1_obj,
  [12] = &pin_C2_obj,
  [13] = &pin_C3_obj,
  [14] = &pin_C4_obj,
  [15] = &pin_C5_obj,
};

const pin_obj_t * const pin_adc3[16] = {
  [0] = &pin_A0_obj,
  [1] = &pin_A1_obj,
  [2] = &pin_A2_obj,
  [3] = &pin_A3_obj,
  [4] = &pin_F6_obj,
  [5] = &pin_F7_obj,
  [6] = &pin_F8_obj,
  [7] = &pin_F9_obj,
  [8] = &pin_F10_obj,
  [9] = &pin_F3_obj,
  [10] = &pin_C0_obj,
  [11] = &pin_C1_obj,
  [12] = &pin_C2_obj,
  [13] = &pin_C3_obj,
  [14] = &pin_F4_obj,
  [15] = &pin_F5_obj,
};

"""
class ADC:
    def __init__(self):
        self.adc = None
        
    def open(self, node):
        if type(node) is str:          
            parser = BoardConfigParser()
            try:
                item = parser.findItem(node, 'ADC')
            except Exception as e:
                print(e)
                return BoardConfigParser.NODE_NOT_EXIST

            self.port = item['port']   
            self.sampling = item['sampling']
            
            pin = mach_Pin(self.port)
            self.adc = mach_ADC(pin)
            return 0
        else:
            raise ValueError('Node type should be str')

    def close(self):
        self.adc = None
        return 0
         
    def readRaw(self):
        adc = self.adc
        if adc is not None:
            return adc.read_u16()
        else:
            raise Exception('readRaw disabled when ADC closed')
    
    def readVoltage(self):
        adc = self.adc
        if adc is not None:
            return adc.read_uv(buf)
        else:
            raise Exception('readVoltage disabled when ADC closed')
