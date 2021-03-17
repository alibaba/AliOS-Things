
import ml
print("-------------------Welcome HaasAI MicroPython--------------------")

print("-----ml ucloud FacebodyComparing demo start-----")
OSS_ACCESS_KEY = "xxx"
OSS_ACCESS_SECRET = "xxx"
OSS_ENDPOINT = "xxx"
OSS_BUCKET = "xxx"
ML_MYFACE_PATH = "xxx"

mlobj = ml.open(ml.ML_ENGINE_CLOUD);
ml.config(mlobj, OSS_ACCESS_KEY, OSS_ACCESS_SECRET, OSS_ENDPOINT, OSS_BUCKET, ML_MYFACE_PATH);
ml.setInputData(mlobj, "/data/python/resource/capture.jpg");
ml.loadNet(mlobj, "FacebodyComparing");
ml.predict(mlobj);
responses_value = bytearray(10)
ml.getPredictResponses(mlobj,responses_value);
print(responses_value);
ml.unLoadNet(mlobj);
ml.close(mlobj);
print("-----ml ucloud FacebodyComparing demo end-----")
