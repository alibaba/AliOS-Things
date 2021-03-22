#! python

import os
import shutil
import sys

from datetime import datetime
from buildutils.builder import Device, Compiler, Axis, Step, BuildStep, RunModelStep, Builder, Filter

OPTIMIZATION = [ 'O1', 'O2', 'Ofast', 'Os', 'Oz' ]

CC_OPT = {
  Compiler.AC5: {
    'O1': 'O0', 
    'O2': 'O1', 
    'Ofast': 'Otime', 
    'Os': 'O2', 
    'Oz': 'O3'
  },
  Compiler.AC6: {
    'O1': 'O1', 
    'O2': 'O2', 
    'Ofast': 'Ofast', 
    'Os': 'Os', 
    'Oz': 'Oz'
  },
  Compiler.AC6LTM: {
    'O1': 'O1', 
    'O2': 'O2', 
    'Ofast': 'Ofast', 
    'Os': 'Os', 
    'Oz': 'Oz'
  },
  Compiler.AC6STBL: {
    'O1': 'O1', 
    'O2': 'O2', 
    'Ofast': 'Ofast', 
    'Os': 'Os', 
    'Oz': 'Oz'
  },
  Compiler.GCC: {
    'O1': 'O1', 
    'O2': 'O2', 
    'Ofast': 'Ofast', 
    'Os': 'Os', 
    'Oz': 'O3'
  },
}

CORTEX_M = [
  Device.CM0,
  Device.CM0PLUS,
  Device.CM3,
  Device.CM4,
  Device.CM4FP,
  Device.CM7,
  Device.CM7SP,
  Device.CM7DP,
  Device.CM23,
  Device.CM33,
  Device.CM23NS,
  Device.CM33NS,
  Device.CM23S,
  Device.CM33S,
  Device.CM35P,
  Device.CM35PS,
  Device.CM35PNS
]

BOOTLOADER = [
  Device.CM23NS,
  Device.CM33NS,
  Device.CM35PNS
]

FVP_MODELS = {
  Device.CM0         : { 'cmd': "FVP_MPS2_Cortex-M0",      'args': { 'timeout': 120, 'limit': "1000000000", 'config': "config/ARMCM0_config.txt"     } },
  Device.CM0PLUS     : { 'cmd': "FVP_MPS2_Cortex-M0plus",  'args': { 'timeout': 120, 'limit': "1000000000", 'config': "config/ARMCM0plus_config.txt" } },
  Device.CM3         : { 'cmd': "FVP_MPS2_Cortex-M3",      'args': { 'timeout': 120, 'limit': "1000000000", 'config': "config/ARMCM3_config.txt"     } },
  Device.CM4         : { 'cmd': "FVP_MPS2_Cortex-M4",      'args': { 'timeout': 120, 'limit': "1000000000", 'config': "config/ARMCM4_config.txt"     } },
  Device.CM4FP       : { 'cmd': "FVP_MPS2_Cortex-M4",      'args': { 'timeout': 120, 'limit': "1000000000", 'config': "config/ARMCM4FP_config.txt"   } },
  Device.CM7         : { 'cmd': "FVP_MPS2_Cortex-M7",      'args': { 'timeout': 120, 'limit': "1000000000", 'config': "config/ARMCM7_config.txt"     } },
  Device.CM7SP       : { 'cmd': "FVP_MPS2_Cortex-M7",      'args': { 'timeout': 120, 'limit': "1000000000", 'config': "config/ARMCM7SP_config.txt"   } },
  Device.CM7DP       : { 'cmd': "FVP_MPS2_Cortex-M7",      'args': { 'timeout': 120, 'limit': "1000000000", 'config': "config/ARMCM7DP_config.txt"   } },
  Device.CM23        : { 'cmd': "FVP_MPS2_Cortex-M23",     'args': { 'timeout': 120, 'limit': "1000000000", 'config': "config/ARMCM23_config.txt",            'target': "cpu0" } },
  Device.CM33        : { 'cmd': "FVP_MPS2_Cortex-M33",     'args': { 'timeout': 120, 'limit': "1000000000", 'config': "config/ARMCM33_config.txt",            'target': "cpu0" } },
  Device.CM23NS      : { 'cmd': "FVP_MPS2_Cortex-M23",     'args': { 'timeout': 120, 'limit': "1000000000", 'config': "config/ARMCM23_TZ_config.txt",         'target': "cpu0" } },
  Device.CM33NS      : { 'cmd': "FVP_MPS2_Cortex-M33",     'args': { 'timeout': 120, 'limit': "1000000000", 'config': "config/ARMCM33_DSP_FP_TZ_config.txt",  'target': "cpu0" } },
  Device.CM23S       : { 'cmd': "FVP_MPS2_Cortex-M23",     'args': { 'timeout': 120, 'limit': "1000000000", 'config': "config/ARMCM23_TZ_config.txt",         'target': "cpu0" } },
  Device.CM33S       : { 'cmd': "FVP_MPS2_Cortex-M33",     'args': { 'timeout': 120, 'limit': "1000000000", 'config': "config/ARMCM33_DSP_FP_TZ_config.txt",  'target': "cpu0" } },
  Device.CM35P       : { 'cmd': "FVP_MPS2_Cortex-M35P",    'args': { 'timeout': 120, 'limit': "1000000000", 'config': "config/ARMCM35P_config.txt",           'target': "cpu0" } },
  Device.CM35PS      : { 'cmd': "FVP_MPS2_Cortex-M35P",    'args': { 'timeout': 120, 'limit': "1000000000", 'config': "config/ARMCM35P_DSP_FP_TZ_config.txt", 'target': "cpu0" } },
  Device.CM35PNS     : { 'cmd': "FVP_MPS2_Cortex-M35P",    'args': { 'timeout': 120, 'limit': "1000000000", 'config': "config/ARMCM35P_DSP_FP_TZ_config.txt", 'target': "cpu0" } },
  Device.CA5         : { 'cmd': "FVP_VE_Cortex-A5x1",      'args': { 'timeout': 120, 'limit': "1000000000", 'config': "config/ARMCA5_config.txt" } },
  Device.CA7         : { 'cmd': "FVP_VE_Cortex-A7x1",      'args': { 'timeout': 120, 'limit': "1000000000", 'config': "config/ARMCA7_config.txt" } },
  Device.CA9         : { 'cmd': "FVP_VE_Cortex-A9x1",      'args': { 'timeout': 120, 'limit': "1000000000", 'config': "config/ARMCA9_config.txt" } },
  Device.CA5NEON     : { 'cmd': "FVP_VE_Cortex-A5x1",      'args': { 'timeout': 120, 'limit': "1000000000", 'config': "config/ARMCA5neon_config.txt" } },
  Device.CA7NEON     : { 'cmd': "FVP_VE_Cortex-A7x1",      'args': { 'timeout': 120, 'limit': "1000000000", 'config': "config/ARMCA7neon_config.txt" } },
  Device.CA9NEON     : { 'cmd': "FVP_VE_Cortex-A9x1",      'args': { 'timeout': 120, 'limit': "1000000000", 'config': "config/ARMCA9neon_config.txt" } }
}

def projects(step, config):
  result = [ str(config['compiler']).lower()+".rtebuild" ]
  if config['device'] in BOOTLOADER:
    result += [ "bootloader/"+str(config['compiler']).lower()+".rtebuild" ]
  return result

def images(step, config):
  result = [ "build/arm{dev}/arm{dev}.elf".format(dev=config['device'].value[1].lower()) ]
  if config['device'] in BOOTLOADER:
    result += [ "bootloader/build/arm{dev}/arm{dev}.elf".format(dev=config['device'].value[1].lower()) ]
  return result

def storeResult(step, config, cmd):
  result = "result_{cc}_{dev}_{opt}_{now}.junit".format(dev=config['device'], cc=config['compiler'], opt=config['optimize'],now=datetime.now().strftime("%Y%m%d%H%M%S"))
  resultfile = step.storeJunitResult(cmd, result, "{cc}.{dev}.{opt}".format(dev=config['device'], cc=config['compiler'], opt=config['optimize']))
  if not resultfile:
    cmd.appendOutput("Storing results failed!");
    cmd.forceResult(1)

def add_options(step, config, cmd):
  cmd._options['optimize']  = CC_OPT[config['compiler']][config['optimize']]

def create():
  deviceAxis = Axis("device", abbrev="d", values=Device, desc="Device(s) to be considered.")
  compilerAxis = Axis("compiler", abbrev="c", values=Compiler, desc="Compiler(s) to be considered.")
  optimizeAxis = Axis("optimize", abbrev="o", values=OPTIMIZATION , desc="Optimization level(s) to be considered.")

  buildStep = BuildStep("build", abbrev="b", desc="Build the selected configurations.")
  buildStep.projects = projects
  buildStep.target = lambda step, config: "arm"+config['device'].value[1].lower()
  buildStep.pre = add_options

  runStep = RunModelStep("run", abbrev="r", desc="Run the selected configurations.")
  runStep.images = images
  runStep.model = lambda step, config: FVP_MODELS[config['device']]
  runStep.post = storeResult

  debugStep = RunModelStep("debug", abbrev="d", desc="Debug the selected configurations.")
  debugStep.images = images
  debugStep.args = lambda step, config: { 'cadi' : True, 'timeout': None }
  debugStep.model = lambda step, config: FVP_MODELS[config['device']]
  
  filterAC5 = Filter().addAxis(compilerAxis, Compiler.AC5).addAxis(deviceAxis, "CM[23]3*")
  filterAC6LTM = Filter().addAxis(compilerAxis, Compiler.AC6LTM).addAxis(deviceAxis, "CM[23]3*")

  builder = Builder()
  builder.addAxis([ compilerAxis, deviceAxis, optimizeAxis ])
  builder.addStep([ buildStep, runStep, debugStep ])
  builder.addFilter([ filterAC5, filterAC6LTM ])

  return builder

def complete(builder, success):
  builder.saveJunitResult("build_{now}.junit".format(now = datetime.now().strftime("%Y%m%d%H%M%S")))
