#!/usr/bin/env python

#
# This is a simple script to generate and convert weights
# from regular ordering to specific ordering used by
# q7_x4 or q7_q15_x4
#


import numpy as np

def convert_to_x4_weights(weights):
    [r, h, w, c] = weights.shape
    weights = np.reshape(weights, (r, h*w*c))
    num_of_rows = r
    num_of_cols = h*w*c
    new_weights = np.copy(weights)
    new_weights = np.reshape(new_weights, (r*h*w*c))
    counter = 0
    for i in range(int(num_of_rows)/4):
      # we only need to do the re-ordering for every 4 rows
      row_base = 4*i
      for j in range (int(num_of_cols)/4):
        # for each 4 entries
        column_base = 4*j
        new_weights[counter]   =  weights[row_base  ][column_base  ]
        new_weights[counter+1] =  weights[row_base+1][column_base  ]
        new_weights[counter+2] =  weights[row_base  ][column_base+2]
        new_weights[counter+3] =  weights[row_base+1][column_base+2]
        new_weights[counter+4] =  weights[row_base+2][column_base  ]
        new_weights[counter+5] =  weights[row_base+3][column_base  ]
        new_weights[counter+6] =  weights[row_base+2][column_base+2]
        new_weights[counter+7] =  weights[row_base+3][column_base+2]

        new_weights[counter+8] =  weights[row_base  ][column_base+1]
        new_weights[counter+9] =  weights[row_base+1][column_base+1]
        new_weights[counter+10] = weights[row_base  ][column_base+3]
        new_weights[counter+11] = weights[row_base+1][column_base+3]
        new_weights[counter+12] = weights[row_base+2][column_base+1]
        new_weights[counter+13] = weights[row_base+3][column_base+1]
        new_weights[counter+14] = weights[row_base+2][column_base+3]
        new_weights[counter+15] = weights[row_base+3][column_base+3]
        counter = counter + 16
      # the remaining ones are in order
      for j in range((int)(num_of_cols-num_of_cols%4), int(num_of_cols)):
        new_weights[counter] = weights[row_base][j]
        new_weights[counter+1] = weights[row_base+1][j]
        new_weights[counter+2] = weights[row_base+2][j]
        new_weights[counter+3] = weights[row_base+3][j]
        counter = counter + 4
    return new_weights

def convert_q7_q15_weights(weights):
    [r, h, w, c] = weights.shape
    weights = np.reshape(weights, (r, h*w*c))
    num_of_rows = r
    num_of_cols = h*w*c
    new_weights = np.copy(weights)
    new_weights = np.reshape(new_weights, (r*h*w*c))
    counter = 0
    for i in range(int(num_of_rows)/4):
      # we only need to do the re-ordering for every 4 rows
      row_base = 4*i
      for j in range (int(num_of_cols)/2):
        # for each 2 entries
        column_base = 2*j
        new_weights[counter]   =  weights[row_base  ][column_base  ]
        new_weights[counter+1] =  weights[row_base+1][column_base  ]
        new_weights[counter+2] =  weights[row_base  ][column_base+1]
        new_weights[counter+3] =  weights[row_base+1][column_base+1]
        new_weights[counter+4] =  weights[row_base+2][column_base  ]
        new_weights[counter+5] =  weights[row_base+3][column_base  ]
        new_weights[counter+6] =  weights[row_base+2][column_base+1]
        new_weights[counter+7] =  weights[row_base+3][column_base+1]

        counter = counter + 8
      # the remaining ones are in order
      for j in range((int)(num_of_cols-num_of_cols%2), int(num_of_cols)):
        new_weights[counter] = weights[row_base][j]
        new_weights[counter+1] = weights[row_base+1][j]
        new_weights[counter+2] = weights[row_base+2][j]
        new_weights[counter+3] = weights[row_base+3][j]
        counter = counter + 4
    return new_weights


vec_dim = 64
row_dim = 32

update_weight = np.zeros((row_dim,vec_dim), dtype=int)
reset_weight = np.zeros((row_dim,vec_dim), dtype=int)
hidden_weight = np.zeros((row_dim,vec_dim), dtype=int)

update_bias = np.zeros((row_dim), dtype=int)
reset_bias = np.zeros((row_dim), dtype=int)
hidden_bias = np.zeros((row_dim), dtype=int)

input_data1 = np.zeros((vec_dim-row_dim), dtype=int)
input_data2 = np.zeros((vec_dim-row_dim), dtype=int)
history_data = np.zeros((row_dim), dtype=int)

outfile = open("arm_nnexamples_gru_test_data.h", "w")

for i in range(row_dim):
  for j in range(vec_dim):
    update_weight[i][j] = np.random.randint(256)-128
    reset_weight[i][j] = np.random.randint(256)-128
    hidden_weight[i][j] = np.random.randint(256)-128

for i in range(row_dim):
  update_bias[i] =  np.random.randint(256)-128
  reset_bias[i] =  np.random.randint(256)-128
  hidden_bias[i] =  np.random.randint(256)-128
  history_data[i] = np.random.randint(2**10)-2**9

for i in range(vec_dim-row_dim):
  input_data1[i] = np.random.randint(2**10)-2**9
  input_data2[i] = np.random.randint(2**10)-2**9

weight = np.reshape(update_weight, (row_dim, vec_dim, 1, 1))
new_weight = convert_to_x4_weights(weight)

outfile.write("#define UPDATE_GATE_WEIGHT_X2 {")
weight.tofile(outfile,sep=",",format="%d")
outfile.write("}\n\n")

new_weight = convert_q7_q15_weights(weight)
outfile.write("#define UPDATE_GATE_WEIGHT_X4 {")
new_weight.tofile(outfile,sep=",",format="%d")
outfile.write("}\n\n")

weight = np.reshape(reset_weight, (row_dim, vec_dim, 1, 1))
new_weight = convert_to_x4_weights(weight)

outfile.write("#define RESET_GATE_WEIGHT_X2 {")
weight.tofile(outfile,sep=",",format="%d")
outfile.write("}\n\n")

new_weight = convert_q7_q15_weights(weight)
outfile.write("#define RESET_GATE_WEIGHT_X4 {")
new_weight.tofile(outfile,sep=",",format="%d")
outfile.write("}\n\n")

weight = np.reshape(hidden_weight, (row_dim, vec_dim, 1, 1))
new_weight = convert_to_x4_weights(weight)

outfile.write("#define HIDDEN_STATE_WEIGHT_X2 {")
weight.tofile(outfile,sep=",",format="%d")
outfile.write("}\n\n")

new_weight = convert_q7_q15_weights(weight)
outfile.write("#define HIDDEN_STATE_WEIGHT_X4 {")
new_weight.tofile(outfile,sep=",",format="%d")
outfile.write("}\n\n")

outfile.write("#define UPDATE_GATE_BIAS {")
update_bias.tofile(outfile,sep=",",format="%d")
outfile.write("}\n\n")

outfile.write("#define RESET_GATE_BIAS {")
reset_bias.tofile(outfile,sep=",",format="%d")
outfile.write("}\n\n")

outfile.write("#define HIDDEN_STATE_BIAS {")
update_bias.tofile(outfile,sep=",",format="%d")
outfile.write("}\n\n")

outfile.write("#define INPUT_DATA1 {")
input_data1.tofile(outfile,sep=",",format="%d")
outfile.write("}\n\n")

outfile.write("#define INPUT_DATA2 {")
input_data2.tofile(outfile,sep=",",format="%d")
outfile.write("}\n\n")


outfile.write("#define HISTORY_DATA {")
history_data.tofile(outfile,sep=",",format="%d")
outfile.write("}\n\n")



outfile.close()
