
import haascv
from haascv import ml
print("-------------------Welcome HaasAI MicroPython--------------------")

print("-----ml ucloud demo start-----")
mlobj = ml();
mlobj.open(ml.ML_ENGINE_CLOUD);
mlobj.setInputData("/data/capture.jpg");
mlobj.loadNet("default");
mlobj.predict();
responses_value = bytearray(10)
mlobj.getPredictResponses(responses_value);
print(responses_value);
mlobj.unLoadNet();
mlobj.close();
print("-----ml ucloud demo end-----")
