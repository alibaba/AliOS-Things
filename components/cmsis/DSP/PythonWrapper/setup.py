from distutils.core import setup, Extension
import glob
import numpy
import config
import sys
import os
from config import ROOT

includes = [os.path.join(ROOT,"Include"),os.path.join("cmsisdsp_pkg","src")]

if sys.platform == 'win32':
  cflags = ["-DWIN",config.cflags,"-DUNALIGNED_SUPPORT_DISABLE"] 
  # Custom because a customized arm_math.h is required to build on windows
  # since the visual compiler and the win platform are
  # not supported by default in arm_math.h
else:
  cflags = ["-Wno-unused-variable","-Wno-implicit-function-declaration",config.cflags]

transform = glob.glob(os.path.join(ROOT,"Source","TransformFunctions","*.c"))
#transform.remove(os.path.join(ROOT,"Source","TransformFunctions","arm_dct4_init_q15.c"))
#transform.remove(os.path.join(ROOT,"Source","TransformFunctions","arm_rfft_init_q15.c"))
transform.remove(os.path.join(ROOT,"Source","TransformFunctions","TransformFunctions.c"))

support = glob.glob(os.path.join(ROOT,"Source","SupportFunctions","*.c"))
support.remove(os.path.join(ROOT,"Source","SupportFunctions","SupportFunctions.c"))

fastmath = glob.glob(os.path.join(ROOT,"Source","FastMathFunctions","*.c"))
fastmath.remove(os.path.join(ROOT,"Source","FastMathFunctions","FastMathFunctions.c"))

filtering = glob.glob(os.path.join(ROOT,"Source","FilteringFunctions","*.c"))
filtering.remove(os.path.join(ROOT,"Source","FilteringFunctions","FilteringFunctions.c"))

matrix = glob.glob(os.path.join(ROOT,"Source","MatrixFunctions","*.c"))
matrix.remove(os.path.join(ROOT,"Source","MatrixFunctions","MatrixFunctions.c"))

statistics = glob.glob(os.path.join(ROOT,"Source","StatisticsFunctions","*.c"))
statistics.remove(os.path.join(ROOT,"Source","StatisticsFunctions","StatisticsFunctions.c"))

complexf = glob.glob(os.path.join(ROOT,"Source","ComplexMathFunctions","*.c"))
complexf.remove(os.path.join(ROOT,"Source","ComplexMathFunctions","ComplexMathFunctions.c"))

basic = glob.glob(os.path.join(ROOT,"Source","BasicMathFunctions","*.c"))
basic.remove(os.path.join(ROOT,"Source","BasicMathFunctions","BasicMathFunctions.c"))

controller = glob.glob(os.path.join(ROOT,"Source","ControllerFunctions","*.c"))
controller.remove(os.path.join(ROOT,"Source","ControllerFunctions","ControllerFunctions.c"))

common = glob.glob(os.path.join(ROOT,"Source","CommonTables","*.c"))
common.remove(os.path.join(ROOT,"Source","CommonTables","CommonTables.c"))

#modulesrc = glob.glob(os.path.join("cmsisdsp_pkg","src","*.c"))
modulesrc = []
modulesrc.append(os.path.join("cmsisdsp_pkg","src","fftinit.c"))
modulesrc.append(os.path.join("cmsisdsp_pkg","src","cmsismodule.c"))

module1 = Extension(config.extensionName,
                    sources = (support
                              + fastmath
                              + filtering
                              + matrix
                              + statistics
                              + complexf
                              + basic
                              + controller
                              + transform
                              + modulesrc
                              + common
                              )
                              ,
                    include_dirs =  includes + [numpy.get_include()],
                    #extra_compile_args = ["-Wno-unused-variable","-Wno-implicit-function-declaration",config.cflags]
                    extra_compile_args = cflags
                              )

setup (name = config.setupName,
       version = '0.0.1',
       description = config.setupDescription,
       ext_modules = [module1],
       author = 'Copyright (C) 2010-2019 ARM Limited or its affiliates. All rights reserved.',
       url="https://github.com/ARM-software/CMSIS_5",
       classifiers=[
        "Programming Language :: Python",
        "License :: OSI Approved :: Apache Software License",
        "Operating System :: OS Independent",
    ])