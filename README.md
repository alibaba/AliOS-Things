This is the specific release of AliOS Things to custome-tailor for qualcomm.

### How to use:
-  Step1:
-  Step2:
-  Step3:


### aos2.0.0_qcom Release Notes:

Clone Command: ```git clone https://github.com/alibaba/AliOS-Things.git -b aos2.0.0_qcom```

#### Change Highlights (comparing with rel_2.0.0):
- armv7a CPU supported, MMU enablbed.
- Runnning permission separated: kernel in SVC while application in non-SVC.
- Kernel image and user image split. Kernel loads app via filesystem while app gets kernel services by system call.
- Kernel can run callback function in user space.
- 1 kernel and 15 user apps simultaneously run, and apps isolated.
- Apps installed or uninstalled dinamically.
- Linkkit can connnect the aliyun cloud in user space.
- Footprint optimized.
