
from driver import ADC

print("-------------------start adc test--------------------")
adc = ADC()
adc.open("ADC2")
value = adc.read()
print(value)
adc.close()
print("-------------------end adc test--------------------")
