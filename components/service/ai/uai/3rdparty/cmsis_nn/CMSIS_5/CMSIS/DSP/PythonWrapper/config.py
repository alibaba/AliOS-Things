CMSISDSP = 1

ROOT=".."

config = CMSISDSP

if config == CMSISDSP:
    extensionName = 'cmsisdsp' 
    setupName = 'CMSISDSP'
    setupDescription = 'CMSIS-DSP Python API'
    cflags="-DCMSISDSP"

