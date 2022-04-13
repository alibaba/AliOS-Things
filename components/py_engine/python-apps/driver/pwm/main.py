
from driver import PWM

print("-------------------pwm test--------------------")
pwm = PWM()
pwm.open("pwm3")
duty = pwm.getDuty()
freq = pwm.getFreq()
print(duty)
print(freq)
pwm.setConfig(3250000, 0.5)
duty = pwm.getDuty()
freq = pwm.getFreq()
print(duty)
print(freq)
pwm.close()
print("-------------------pwm end--------------------")
