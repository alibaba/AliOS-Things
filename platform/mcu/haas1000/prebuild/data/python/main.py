
import driver
import datainput
import imagecodec
import imageproc
import ml
import ui
import videocodec
import utime

print("-------------------Welcome HaasAI MicroPython--------------------")
print("-----datainput test case start-----")

datainputobj = datainput.open(datainput.DATA_INPUT_SOURCE_IMAGE_JPG, "/data/datainput.img");
print(datainput.DATA_INPUT_CAMERA_NUMBER_NONE)
print(datainput.DATA_INPUT_CAMERA_NUMBER_FRONT)
print(datainput.DATA_INPUT_CAMERA_NUMBER_BACK)
print(datainput.DATA_INPUT_CAMERA_NUMBER_MAX)
print(datainput.DATA_INPUT_SOURCE_NONE)
print(datainput.DATA_INPUT_SOURCE_IMAGE_BMP)
print(datainput.DATA_INPUT_SOURCE_IMAGE_GIF)
print(datainput.DATA_INPUT_SOURCE_IMAGE_PNG)
print(datainput.DATA_INPUT_SOURCE_IMAGE_JPG)
print(datainput.DATA_INPUT_SOURCE_VIDEO_FILE)
print(datainput.DATA_INPUT_SOURCE_VIDEO_RTMP)
print(datainput.DATA_INPUT_SOURCE_VIDEO_RTSP)
print(datainput.DATA_INPUT_SOURCE_CAMERA_USB)
print(datainput.DATA_INPUT_SOURCE_CAMERA_LOCAL)
print(datainput.DATA_INPUT_SOURCE_CAMERA_IP)
print(datainput.DATA_INPUT_SOURCE_MAX)
ret = datainput.checkDataReady(datainputobj);
print(ret);
request_data = datainput.requestData(datainputobj);
print(request_data);
ret = datainput.releaseData(datainputobj, request_data);
print(ret);
datainput.close(datainputobj);

datainputobj2 = datainput.openCamera(datainput.DATA_INPUT_SOURCE_CAMERA_USB, datainput.DATA_INPUT_CAMERA_NUMBER_FRONT);
ret = datainput.checkDataReady(datainputobj2);
print(ret);
request_data = datainput.requestData(datainputobj2);
print(request_data);
ret = datainput.releaseData(datainputobj2, request_data);
print(ret);
datainput.close(datainputobj2);
print("-----datainput test case end-----")

print("-----imagecodec test case start-----")

print(imagecodec.CODEC_IMAGE_SOURCE_NONE);
print(imagecodec.CODEC_IMAG_SOURCE_IMAGE_PNG);
print(imagecodec.CODEC_IMAG_SOURCE_IMAGE_JPG);
print(imagecodec.CODEC_IMAG_SOURCE_IMAGE_BMP);
print(imagecodec.CODEC_IMAG_SOURCE_IMAGE_GIF);
print(imagecodec.CODEC_IMAG_SOURCE_MAX);
imagecodecobj = imagecodec.open(2);
img_buffer = imagecodec.imgRead(imagecodecobj, "/data/python/resource/test.img");
print(img_buffer);
img_buffer = imagecodec.imgReadMulti(imagecodecobj, "/data/python/resource/test.img");
print(img_buffer);
imagecodec.imgWrite(imagecodecobj, 0, "/data/python/resource/test.img");
imagecodec.imgWriteMulti(imagecodecobj, 0, "/data/python/resource/test.img");
imagecodec.imgDecode(imagecodecobj, 0, 0);
decode_image = imagecodec.imgDecode2(imagecodecobj, "/data/python/resource/test.img");
print(decode_image);
imagecodec.imgEncode(imagecodecobj, 0, 0);
imagecodec.close(imagecodecobj);

print("-----imagecodec test case end-----")
print("-----imageproc test case start-----")
print(imageproc.IMAGE_PROC_NONE)
print(imageproc.IMAGE_PROC_CPU)
print(imageproc.IMAGE_PROC_OPENCV)
print(imageproc.IMAGE_PROC_HAL)
print(imageproc.IMAGE_PROC_MAX)
imageporcobj = imageproc.open(imageproc.IMAGE_PROC_HAL);
print("-----imageproc cvt-----")

imageproc.cvtColor(imageporcobj,0,1,0);
imageproc.resize(imageporcobj, 0, 480, 800, 0);
imageproc.imgCopy(imageporcobj, 0, 0);
imageproc.rectangle(imageporcobj, 0, 10, 30, 100, 300);
imageproc.circle(imageporcobj, 0, 10, 50, 15);
imageproc.drawMarker(imageporcobj, 0, 50, 100, 2);
imageproc.fillPoly(imageporcobj, 0, 50, 200, 0xdd);
imageproc.putText(imageporcobj, 0, "text-test", 50, 100);
imageproc.dilateErode(imageporcobj, 0, 0, 3, 10);
imageproc.imageAdd(imageporcobj, 0, 0, 0);
imageproc.imageMinus(imageporcobj, 0, 0, 0);
imageproc.imageElementMultiply(imageporcobj, 0, 0, 0);
imageproc.imageElementDivide(imageporcobj, 0, 0, 0);
imageproc.writeImageToFile(imageporcobj, 0, "/data/python/resource/test.img");
imageproc.close(imageporcobj);
print("-----imageproc test case end-----")
print("-----ml test case start-----")
print(ml.ML_ENGINE_NONE);
print(ml.ML_ENGINE_MNN);
print(ml.ML_ENGINE_CLOUD);
print(ml.ML_ENGINE_ODLA);
print(ml.ML_ENGINE_MAX);
mlobj = ml.open(ml.ML_ENGINE_MNN);
ml.setInputData(mlobj, "/data/datainput.img");
ml.loadNet(mlobj, "/data/modeobj");
ml.predict(mlobj);
ml_store = bytearray(50)
ml_value = ml.getPredictResponses(mlobj, ml_store);
print(ml_value);
ml.unLoadNet(mlobj);
ml.close(mlobj);
print("-----ml test case end-----")
print("-----ui test case start-----")
print(ui.UI_FRAMEWORK_NONE)
print(ui.UI_FRAMEWORK_GREENUI)
print(ui.UI_FRAMEWORK_FB)
print(ui.UI_FRAMEWORK_LVGL)
print(ui.UI_FRAMEWORK_SDL)
print(ui.UI_FRAMEWORK_MAX)
uiobj = ui.open(ui.UI_FRAMEWORK_SDL);
width = ui.getDisplayWidth(uiobj);
print(width)
height = ui.getDisplayHeight(uiobj);
print(height)
backlight = ui.getDisplayBacklight(uiobj);
print(backlight)
ui.setDisplayBacklight(uiobj, 50);
displayType = ui.getDisplayType(uiobj);
print(displayType)
displayFreq = ui.getDisplayFreq(uiobj);
print(displayFreq)
ui.setDisplayFreq(uiobj, 1000);
ui.drawPoint(uiobj, 10, 30, 0xdd);
ui.drawLine(uiobj, 10, 30, 20, 60, 0xdd);
ui.drawRectangle(uiobj, 10, 30, 80, 200, 0xdd);
ui.drawCircle(uiobj, 10, 30, 15, 0xdd);
ui.drawText(uiobj, "text-test", 30, 0xdd, 10, 30, 80, 200);
ui.drawPixels(uiobj, 0, 10, 20, 480, 800);
ui.drawImage(uiobj, "/data/python/resource/test.img", 10, 30, 480, 800);
ui.updateDisplay(uiobj);
ui.close(uiobj);
print("-----ui test case end-----")
print("-----videocodec test case start-----")
print(videocodec.VIDEO_CODEC_NONE)
print(videocodec.VIDEO_CODEC_FFMPEG)
print(videocodec.VIDEO_CODEC_HAL)
print(videocodec.VIDEO_CODEC_MAX)
videocodecobj = videocodec.open(videocodec.VIDEO_CODEC_HAL);
videocodec.decodeConfig(videocodecobj, 0);
videocodec.startDecode(videocodecobj);
videocodec.stopDecode(videocodecobj);
video_image = videocodec.getDecodeImageData(videocodecobj, 0);
print(video_image)
videocodec.encodeConfig(videocodecobj, 0);
videocodec.startEncode(videocodecobj);
videocodec.stopEncode(videocodecobj);
encode_data = videocodec.getEncodePacketData(videocodecobj);
print(encode_data)
videocodec.close(videocodecobj)
print("-----videocodec test case end-----")
print("-----driver test case-----")
adcobj     = driver.adc()
batteryobj = driver.battery()
chargerobj = driver.charger()
#dacobj     = driver.dac()
#gnssobj    = driver.gnss()
httpobj    = driver.http()
mqttobj    = driver.mqtt()
#networkobj = driver.network()
pmobj      = driver.pm()
rtcobj     = driver.rtc()
systemobj  = driver.system()
tcpobj     = driver.tcp()
tempobj    = driver.timer()
udpobj     = driver.udp()
wdgobj     = driver.wdg()
#pwm test case
print("-------------------Welcome HaasAI MicroPython--------------------")
