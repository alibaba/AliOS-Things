#git clone https://github.com/ARM-software/CMSIS_5.git
if [ ! -f "CMSIS_5/CMSIS/NN/Source/PoolingFunctions/arm_pool_q7_HWC_nonsquare.c" ]; then
    cp arm_pool_q7_HWC_nonsquare.c CMSIS_5/CMSIS/NN/Source/PoolingFunctions/arm_pool_q7_HWC_nonsquare.c
fi
python get_cmsis_nn.py

