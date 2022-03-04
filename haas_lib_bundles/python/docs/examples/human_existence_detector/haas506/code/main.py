from driver import GPIO
import utime as time
import audio
import radar

#创建并打开一个GPIO实例
gpiorDev=GPIO()
gpiorDev.open('radar')
#创建一个radar实例
radarDev=radar.RADAR(gpiorDev)

#创建并打开一个Audio实例
aud=audio.Audio()
#开启音频播放使能
aud.set_pa()
#设置音量
aud.setVolume(5)
#循环检测是否有人经过
while True:
    if radarDev.detect()==1:
        print("detect body")
        aud.play('/data/pyamp/detect_body.mp3')
    else:
        print("no body")
    time.sleep(2)        


