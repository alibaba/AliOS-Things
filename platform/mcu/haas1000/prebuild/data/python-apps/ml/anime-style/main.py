
from minicv import ML
print("-------------------Welcome HaasAI MicroPython--------------------")

print("-----ml ucloud AnimeStyle demo start-----")
OSS_ACCESS_KEY = "xxxx"
OSS_ACCESS_SECRET = "xxxx"
OSS_ENDPOINT = "xxxx"
OSS_BUCKET = "xxxx"

ml = ML()
ml.open(ml.ML_ENGINE_CLOUD)
ml.config(OSS_ACCESS_KEY, OSS_ACCESS_SECRET, OSS_ENDPOINT, OSS_BUCKET, "NULL")
ml.setInputData("/data/python-apps/ml/anime-style/res/test.jpg")
ml.loadNet("AnimeStyle")
ml.predict()
responses_value = bytearray(10)
ml.getPredictResponses(responses_value)
print(responses_value)
ml.unLoadNet()
ml.close()
print("-----ml ucloud AnimeStyle demo end-----")
