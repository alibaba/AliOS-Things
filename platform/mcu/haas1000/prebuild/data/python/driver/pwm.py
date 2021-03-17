
import driver

print("-------------------pwm test--------------------")
pwmObj = driver.pwm();
pwmObj.open("/data/python/config/pwm.json", "pwm3");

duty = pwmObj.getDuty()
freq = pwmObj.getFreq()
print(duty)
print(freq)

pwmObj.setConfig(3250000, 0.5)

duty = pwmObj.getDuty()
freq = pwmObj.getFreq()
print(duty)
print(freq)

pwmObj.close()
