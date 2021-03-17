
import ml
print("-------------------Welcome HaasAI MicroPython--------------------")

print("-----ml odla demo start-----")
#test data
#"/data/python/resource/odla/test0.png"
#"/data/python/resource/odla/test1.png"
#"/data/python/resource/odla/test2.png"
#"/data/python/resource/odla/test3.png"
#"/data/python/resource/odla/test4.png"
#"/data/python/resource/odla/test5.png"
#"/data/python/resource/odla/test6.png"
#"/data/python/resource/odla/test7.png"
#"/data/python/resource/odla/test8.png"
#"/data/python/resource/odla/test9.png"
mlobj = ml.open(ml.ML_ENGINE_ODLA);
ml.setInputData(mlobj, "/data/python/resource/odla/test4.png");
ml.loadNet(mlobj, "default");
ml.predict(mlobj);
responses_value = bytearray(10)
ml.getPredictResponses(mlobj, responses_value);
print(responses_value);
ml.unLoadNet(mlobj);
ml.close(mlobj);
print("-----ml odla demo end-----")
