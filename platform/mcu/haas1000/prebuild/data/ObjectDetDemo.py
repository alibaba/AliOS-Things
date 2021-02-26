
import haascv
from haascv import ml
print("-------------------Welcome HaasAI MicroPython--------------------")

print("-----ml ucloud ObjectDet demo start-----")
OSS_ACCESS_KEY = "xxxx"
OSS_ACCESS_SECRET = "xxxx"
OSS_ENDPOINT = "xxxx"
OSS_BUCKET = "xxxx"

mlobj = ml();
mlobj.open(ml.ML_ENGINE_CLOUD);
mlobj.config(OSS_ACCESS_KEY, OSS_ACCESS_SECRET, OSS_ENDPOINT, OSS_BUCKET, "NULL");
mlobj.setInputData("/data/capture.jpg");
mlobj.loadNet("ObjectDet");
mlobj.predict();
responses_value = bytearray(10)
mlobj.getPredictResponses(responses_value);
print(responses_value);
mlobj.unLoadNet();
mlobj.close();
print("-----ml ucloud ObjectDet demo end-----")
