
from driver import ADC

print("-------------------start adc test--------------------")
adc = ADC()
adc.open("ADC0")
value = adc.read()
print(value)
adc.close()
print("-------------------end adc test--------------------")
