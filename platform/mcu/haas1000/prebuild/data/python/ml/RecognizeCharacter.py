
import ml
print("-------------------Welcome HaasAI MicroPython--------------------")

print("-----ml ucloud RecognizeCharacter demo start-----")
OSS_ACCESS_KEY = "xxx"
OSS_ACCESS_SECRET = "xxx"
OSS_ENDPOINT = "xxx"
OSS_BUCKET = "xxx"

mlobj = ml.open(ml.ML_ENGINE_CLOUD);
ml.config(mlobj, OSS_ACCESS_KEY, OSS_ACCESS_SECRET, OSS_ENDPOINT, OSS_BUCKET, "NULL");
ml.setInputData(mlobj, "/data/python/resource/test.jpg");
ml.loadNet(mlobj, "RecognizeCharacter");
ml.predict(mlobj);
responses_value = bytearray(10)
ml.getPredictResponses(mlobj, responses_value);
print(responses_value);
ml.unLoadNet(mlobj);
ml.close(mlobj);
print("-----ml ucloud RecognizeCharacter demo end-----")
