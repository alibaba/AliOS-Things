import numpy as np
import os
import collections

import onnx
from onnx import helper, numpy_helper
from onnx import TensorProto
from onnxruntime.transformers.onnx_model import OnnxModel

import onnxoptimizer

# check model format
def check_model_extension(model_path):
    # Split the extension from the path and normalise it to lowercase.
    ext = os.path.splitext(model_path)[-1].lower()

    # Now we can simply use != to check for inequality, no need for wildcards.
    if (ext != ".onnx"):
        raise Exception(model_path, "is an unknown file format. Use the model ending with .onnx format")

    if not os.path.exists(model_path):
        raise Exception("[ ERROR ] Path of the onnx model file is Invalid")

# Fuse batch normalization into Conv and Gemm node
def fuse_bn(model_proto):
    onnx_model = OnnxModel(model_proto)
    nodes = model_proto.graph.node

    for node in nodes:
        if node.op_type == 'BatchNormalization':
            node1 = onnx_model.get_parent(node, 0)
            if node1.op_type in ['Conv', 'Gemm']: # fuse bn with conv or gemm
                children = onnx_model.get_children(node1)
                if len(children) != 1: # cannot fuse if used by other nodes
                    continue

                weights = numpy_helper.to_array(onnx_model.get_initializer(node1.input[1]))
                if len(node1.input) > 2:
                    bias = numpy_helper.to_array(onnx_model.get_initializer(node1.input[2]))
                else:
                    bias = np.array(0, dtype=weights.dtype)

                scale = numpy_helper.to_array(onnx_model.get_initializer(node.input[1]))
                offset = numpy_helper.to_array(onnx_model.get_initializer(node.input[2]))
                mean = numpy_helper.to_array(onnx_model.get_initializer(node.input[3]))
                var = numpy_helper.to_array(onnx_model.get_initializer(node.input[4]))

                epsilon = 1e-5
                for att in node.attribute:
                    if att.name == 'epsilon':
                        epsilon = att.f

                scale_new = scale / np.sqrt(var + epsilon)
                if len(weights.shape) == 4:
                    weights = weights.transpose(2, 3, 1, 0)
                    weights_new = weights * scale_new
                    weights_new = weights_new.transpose(3, 2, 0, 1)
                if len(weights.shape) == 2:
                    weights = weights.transpose(1, 0)
                    weights_new = weights * scale_new
                    weights_new = weights_new.transpose(1, 0)

                bias_new = (bias - mean) * scale_new + offset

                initializer = onnx_model.get_initializer(node1.input[1])
                initializer.CopyFrom(
                    numpy_helper.from_array(weights_new.astype(np.float32), node1.input[1]))

                if len(node1.input) > 2:
                    initializer = onnx_model.get_initializer(node1.input[2])
                    initializer.CopyFrom(
                        numpy_helper.from_array(bias_new.astype(np.float32), node1.input[2]))
                else:
                    ini = onnx.helper.make_tensor(node1.name + '_bias', TensorProto.FLOAT, bias_new.shape,
                                                  bias_new.astype(bias.dtype))
                    model_proto.graph.initializer.append(ini)
                    node1.input.extend([node1.name + '_bias'])

                node1.output[0] = node.output[0]
                model_proto.graph.node.remove(node)

    onnx_model.update_graph()


# For 'Attempting to broadcast an axis by a dimension other than 1'
def handle_prelu(model_proto):
    onnx_model = OnnxModel(model_proto)
    for node in model_proto.graph.node:
        if node.op_type == 'PRelu':
            initializer = onnx_model.get_initializer(node.input[1])
            new_array = numpy_helper.to_array(initializer)[:, np.newaxis, np.newaxis]
            new_initializer = onnx.helper.make_tensor(initializer.name, TensorProto.FLOAT, new_array.shape, new_array)
            model_proto.graph.initializer.remove(initializer)
            model_proto.graph.initializer.append(new_initializer)
    onnx_model.update_graph()



# name of GEMM node missing if 'fuse_matmul_add_bias_into_gemm' happens
def add_fused_gemm_name(model_proto):
    nodes = [n for n in model_proto.graph.node if n.op_type == 'Gemm']
    node_name_counter = 0
    for node in nodes:
        if node.name == '':
            node.name = 'fused_gemm_' + str(node_name_counter)
            node_name_counter += 1


def rename_node(model_proto):
    node_name_counter = collections.Counter()
    for node in model_proto.graph.node:
        node_name_counter[node.op_type] += 1
        # rename node name
        node.name = str(node.op_type) + '_' + str(node_name_counter[node.op_type])


# Set input batch size to dynamic and run shape inference
def convert_model_batch_to_dynamic(model):
    initializers =  [node.name for node in model.graph.initializer]
    inputs = []

    for node in model.graph.input:
        inputs.append(node)

    for node in inputs:
        if node.name in initializers:
            # remove initializer from inputs
            model.graph.input.remove(node)

    shape = inputs[0].type.tensor_type.shape
    inputs[0].type.tensor_type.elem_type = TensorProto.FLOAT
    model.graph.output[0].type.tensor_type.elem_type = TensorProto.FLOAT
    dim = shape.dim

    if model.opset_import[0].version > 13:
        model.opset_import[0].version = 13
    if not dim[0].dim_param:
        dim[0].dim_param = 'N'
    model = onnx.shape_inference.infer_shapes(model)

    return model


def optimize_fp_model(model_path):
    check_model_extension(model_path)
    model_proto = onnx.load(model_path)

    # Convert static batch to dynamic batch
    model_proto = convert_model_batch_to_dynamic(model_proto)

    # Optimizer
    model_proto = onnxoptimizer.optimize(model_proto,
                                       ['fuse_bn_into_conv', 'fuse_add_bias_into_conv', 'fuse_pad_into_conv',
                                        'fuse_matmul_add_bias_into_gemm', 'fuse_transpose_into_gemm',
                                        'eliminate_nop_flatten', 'eliminate_nop_pad', 'eliminate_nop_transpose',
                                        'eliminate_unused_initializer', 'eliminate_duplicate_initializer'])
    # Add Gemm name
    add_fused_gemm_name(model_proto)

    # Fuse bn into conv or gemm
    fuse_bn(model_proto)

    # Save optimized model
    model_name = model_path.split(".")
    model_path = model_name[0] + "_optimized.onnx"
    onnx.save(model_proto, model_path)

    return model_path

