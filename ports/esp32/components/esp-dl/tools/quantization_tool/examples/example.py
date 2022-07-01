import numpy as np
import onnx
import onnxruntime as rt

import os
import sys
import platform
system_type = platform.system()
path = f'../{system_type.lower()}'
if system_type == 'Windows':
    path = path.replace('/', '\\')
sys.path.append(path)
sys.path.append("..")

from optimizer import *
from calibrator import *
from evaluator import *

if __name__ == "__main__":
    # Optimize the onnx model
    model_path = 'mnist_model_example.onnx'
    optimized_model_path = optimize_fp_model(model_path)

    # Calibration
    with open('mnist_test_data.pickle', 'rb') as f:
        (test_images, test_labels) = pickle.load(f)
    test_images = test_images / 255.0

    # Prepare the calibration dataset
    calib_dataset = test_images[0:5000:50]
    pickle_file_path = 'mnist_calib.pickle'
    model_proto = onnx.load(optimized_model_path)

    print('Generating the quantization table:')
    calib = Calibrator('int16', 'per-tensor', 'minmax')
    calib.set_providers(['CPUExecutionProvider'])
    calib.generate_quantization_table(model_proto, calib_dataset, pickle_file_path)
    calib.export_coefficient_to_cpp(model_proto, pickle_file_path, 'esp32s3', '.', 'test_mnist', True)

    # Evaluate the performance
    print('Evaluating the performance on esp32s3:')
    eva = Evaluator('int16', 'per-tensor', 'esp32s3')
    eva.set_providers(['CPUExecutionProvider'])
    eva.generate_quantized_model(model_proto, pickle_file_path)

    output_names = [n.name for n in model_proto.graph.output]
    providers = ['CPUExecutionProvider']
    m = rt.InferenceSession(optimized_model_path, providers=providers)

    batch_size = 100
    batch_num = int(len(test_images) / batch_size)
    res = 0
    fp_res = 0
    input_name = m.get_inputs()[0].name
    for i in range(batch_num):
        # int8_model
        [outputs, _] = eva.evalute_quantized_model(test_images[i * batch_size:(i + 1) * batch_size], False)
        res = res + sum(np.argmax(outputs[0], axis=1) == test_labels[i * batch_size:(i + 1) * batch_size])

        # floating-point model
        fp_outputs = m.run(output_names, {input_name: test_images[i * batch_size:(i + 1) * batch_size].astype(np.float32)})
        fp_res = fp_res + sum(np.argmax(fp_outputs[0], axis=1) == test_labels[i * batch_size:(i + 1) * batch_size])

    print('accuracy of int8 model is: %f' % (res / len(test_images)))
    print('accuracy of fp32 model is: %f' % (fp_res / len(test_images)))
