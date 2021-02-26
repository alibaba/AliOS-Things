
import haascv
from haascv import ml
print("-------------------Welcome HaasAI MicroPython--------------------")

print("-----ml odla demo start-----")
#test data
#"/data/odla/test0.bmp"
#"/data/odla/test1.bmp"
#"/data/odla/test2.bmp"
#"/data/odla/test3.bmp"
#"/data/odla/test4.bmp"
#"/data/odla/test5.bmp"
#"/data/odla/test6.bmp"
#"/data/odla/test7.bmp"
#"/data/odla/test8.bmp"
#"/data/odla/test9.bmp"
mlobj = ml();
mlobj.open(ml.ML_ENGINE_ODLA);
mlobj.setInputData("/data/odla/test4.bmp");
mlobj.loadNet("default");
mlobj.predict();
responses_value = bytearray(10)
mlobj.getPredictResponses(responses_value);
print(responses_value);
mlobj.unLoadNet();
mlobj.close();
print("-----ml odla demo end-----")
