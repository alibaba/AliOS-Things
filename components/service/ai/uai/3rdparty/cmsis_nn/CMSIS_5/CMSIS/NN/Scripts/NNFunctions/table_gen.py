#!/usr/bin/python

import math

class Table(object):

  def __init__(self, table_entry=256, table_range=8):
    self.table_entry = table_entry
    self.table_range = table_range
    pass

  def sigmoid(self, x):
    return 1 / (1 + math.exp(-1*x))
  
  def tanh(self, x):
    return (math.exp(2*x)-1) / (math.exp(2*x)+1)
  
  def fp2q7(self, x):
    x_int = math.floor(x*(2**7)+0.5)
    if x_int >= 128 :
      x_int = 127
    if x_int < -128 :
      x_int = -128
    if x_int >= 0 :
      return x_int
    else :
      return 0x100 + x_int
  
  def fp2q15(self, x):
    x_int = math.floor(x*(2**15)+0.5)
    if x_int >= 2**15 :
      x_int = 2**15-1
    if x_int < -1*2**15 :
      x_int = -1*2**15
    if x_int >= 0 :
      return x_int
    else :
      return 0x10000 + x_int

  def table_gen(self):
    outfile = open("NNCommonTable.c", "wb")

    outfile.write("/*\n * Common tables for NN\n *\n *\n *\n *\n */\n\n#include \"arm_math.h\"\n#include \"NNCommonTable.h\"\n\n/*\n * Table for sigmoid\n */\n")
  
    for function_type in ["sigmoid", "tanh"]:
      for data_type in [7, 15]:
        out_type = "q"+str(data_type)+"_t"
        act_func = getattr(self, function_type)
        quan_func = getattr(self, 'fp2q'+str(data_type))

        # unified table
        outfile.write('const %s %sTable_q%d[%d] = {\n' % (out_type, function_type, data_type, self.table_entry) )
        for i in range(self.table_entry):
          # convert into actual value
          if i < self.table_entry/2:
            value_q7 = self.table_range * (i)
          else:
            value_q7 = self.table_range * (i - self.table_entry)

          if data_type == 7:
            #outfile.write('%f, ' % (act_func(float(value_q7)/256)))
            outfile.write('0x%02x, ' % (quan_func(act_func(float(value_q7)/self.table_entry))))
          else:
            #outfile.write('%f, ' % (act_func(float(value_q7)/256)))
            outfile.write('0x%04x, ' % (quan_func(act_func(float(value_q7)/self.table_entry))))
          if i % 8 == 7:
            outfile.write("\n")
        outfile.write("};\n\n")

      for data_type in [15]:
        out_type = "q"+str(data_type)+"_t"
        act_func = getattr(self, function_type)
        quan_func = getattr(self, 'fp2q'+str(data_type))

        # H-L tables
        outfile.write('const %s %sLTable_q%d[%d] = {\n' % (out_type, function_type, data_type, self.table_entry/2))
        for i in range(self.table_entry/2):
          # convert into actual value, max value is 16*self.table_entry/4 / 4
          # which is equivalent to self.table_entry / self.table_entry/2 = 2, i.e., 1/4 of 8
          if i < self.table_entry/4:
            value_q7 = self.table_range * i / 4
          else:
            value_q7 = self.table_range * (i - self.table_entry/2) / 4
          if data_type == 7:
            #outfile.write('%f, ' % (act_func(float(value_q7)/256)))
            outfile.write('0x%02x, ' % (quan_func(act_func(float(value_q7)/(self.table_entry/2)))))
          else:
            #outfile.write('%f, ' % (act_func(float(value_q7)/256)))
            outfile.write('0x%04x, ' % (quan_func(act_func(float(value_q7)/(self.table_entry/2)))))
          if i % 8 == 7:
            outfile.write("\n")
        outfile.write("};\n\n")

        outfile.write('const %s %sHTable_q%d[%d] = {\n' % (out_type, function_type, data_type, 3*self.table_entry/4))
        for i in range(3 * self.table_entry/4):
          # convert into actual value, tageting range (2, 8)
          if i < 3*self.table_entry/8 :
            value_q7 = self.table_range * ( i + self.table_entry/8 )
          else:
            value_q7 = self.table_range * ( i + self.table_entry/8 - self.table_entry)
          if data_type == 7:
            #outfile.write('%f, ' % (act_func(float(value_q7)/256)))
            outfile.write('0x%02x, ' % (quan_func(act_func(float(value_q7)/self.table_entry))))
          else:
            #outfile.write('%f, ' % (act_func(float(value_q7)/256)))
            outfile.write('0x%04x, ' % (quan_func(act_func(float(value_q7)/self.table_entry))))
          if i % 8 == 7:
            outfile.write("\n")
        outfile.write("};\n\n")
    
    outfile.close()
  
  
mytable = Table(table_entry=256, table_range=16)

mytable.table_gen()
