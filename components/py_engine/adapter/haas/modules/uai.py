from minicv import ML

AI_ENGINE_ALIYUN = 1
AI_ENGINE_NATIVE = 2

class AI:
    def __init__(self, type=AI_ENGINE_NATIVE, accessKey=None, accessSecret=None, ossEndpoint=None, ossBucket=None):
        self.type = type
        self.ml = ML()
        if (self.type == AI_ENGINE_ALIYUN):
            self.ml.open(self.ml.ML_ENGINE_CLOUD)
            if not accessKey or not accessSecret:
                print('access key can not be null')
                return
            else:
                self.ml.config(accessKey, accessSecret, ossEndpoint, ossBucket)
        else:
            print('now only support cloud ai, not support nativate ai yet')
            print("Please use example: ai = AI(AI.AI_ENGINE_CLOUD, 'Your-Access-Key', 'Your-Access-Secret')")

    # 人脸比对
    def compareFace(self, imagePath, compareFacePath):
        self.ml.setInputData(imagePath, compareFacePath)
        self.ml.loadNet("FacebodyComparing")
        self.ml.predict()
        resp = self.ml.getPredictResponses(None)
        self.ml.unLoadNet()
        return resp

    # 人体检测
    def detectPedestrian(self, imagePath):
        self.ml.setInputData(imagePath)
        self.ml.loadNet("DetectPedestrian")
        self.ml.predict()
        resp = self.ml.getPredictResponses(None)
        self.ml.unLoadNet()
        return resp

    # 水果检测
    def detectFruits(self, imagePath):
        self.ml.setInputData(imagePath, None)
        self.ml.loadNet("DetectFruits")
        self.ml.predict()
        resp = self.ml.getPredictResponses(None)
        self.ml.unLoadNet()
        return resp

    # 车牌识别
    def recognizeLicensePlate(self, imagePath):
        self.ml.setInputData(imagePath)
        self.ml.loadNet("RecognizeLicensePlate")
        self.ml.predict()
        resp = self.ml.getPredictResponses(None)
        self.ml.unLoadNet()
        return resp

    def __del__(self):
        try:
            self.ml.close()
            del self.type
            del self.ml
        except Exception:
            pass
