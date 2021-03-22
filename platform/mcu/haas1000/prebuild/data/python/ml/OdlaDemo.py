
from minicv import ML
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
ml = ML()
ml.open(ml.ML_ENGINE_ODLA)
ml.setInputData("/data/python/resource/odla/test4.png")
ml.loadNet("default")
ml.predict()
responses_value = bytearray(10)
ml.getPredictResponses(responses_value)
print(responses_value)
ml.unLoadNet()
ml.close()
print("-----ml odla demo end-----")
