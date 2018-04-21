src = Split('''
''')

component = aos_component('activation', src)

component.add_global_includes(".")

if aos_global_config.arch == "ARM968E":
    component.add_prebuilt_libs("./ARM968E-S/activation.a")


if aos_global_config.arch == "Cortex-M4":
    if aos_global_config.compiler == "armcc":
        component.add_prebuilt_libs("./Cortex-M4/KEIL/activation.a")
    elif aos_global_config.compiler == "iar":
        component.add_prebuilt_libs("./Cortex-M4/IAR/activation.a")
    elif aos_global_config.enable_vfp == 1:
        component.add_prebuilt_libs("./Cortex-M4/VFP/activation.a")
    else:
        component.add_prebuilt_libs("./Cortex-M4/activation.a")
    
if aos_global_config.arch == "linux":
    component.add_prebuilt_libs("./linux/activation.a")    
    
#scons not support yet, to do
if aos_global_config.arch == "xtensa":
    if aos_global_config.mcu_family == "esp32":
        component.add_prebuilt_libs("./xtensa/esp32/activation.a")
    elif aos_global_config.mcu_family == "esp8266":
        component.add_prebuilt_libs("./xtensa/esp8266/activation.a")
        
        
        
        
 
    
    