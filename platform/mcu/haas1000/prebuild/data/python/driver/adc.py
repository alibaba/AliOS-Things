
import driver

print("-------------------adc test--------------------")
adcobj = driver.adc()
adcobj.open("/data/python/config/adc.json", "ADC0");
value = adcobj.read();
print(value);
adcobj.close();
print("-------------------adc test--------------------")
