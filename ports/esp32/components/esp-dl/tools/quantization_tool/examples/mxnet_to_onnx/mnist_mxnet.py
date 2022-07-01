# training refer to https://github.com/apache/incubator-mxnet/blob/master/example/gluon/mnist/mnist.py

import numpy as np
from mxnet.contrib import onnx as onnx_mxnet
import onnx
from onnx import checker


# define net
# from mxnet import gluon
# from mxnet.gluon import nn
# net = gluon.nn.HybridSequential()
# net.hybridize()
# net.add(nn.Dense(128, activation='relu'))
# net.add(nn.Dense(64, activation='relu'))
# net.add(nn.Dense(10))


if __name__ == "__main__":
    sym = 'mnist_mxnet-symbol.json'
    params = 'mnist_mxnet-0010.params'

    input_shape = (1, 28, 28)

    # Path of output file
    onnx_file_path = 'mnist_model_mxnet.onnx'

    # Invoke exort model API
    converted_model_path = onnx_mxnet.export_model(sym, params, [input_shape], np.float32, onnx_file_path)

    model_proto = onnx.load_model(converted_model_path)
    checker.check_graph(model_proto.graph)
