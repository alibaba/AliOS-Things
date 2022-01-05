# MultiNet Introduction [[中文]](./README_cn.md)

MultiNet is a lightweight model specially designed based on [CRNN](https://arxiv.org/pdf/1703.05390.pdf) and [CTC](http://citeseerx.ist.psu.edu/viewdoc/download?doi=10.1.1.75.6306&rep=rep1&type=pdf) for the implementation of multi-command recognization with ESP32. Now, up to 100 speech commands, including customized commands, are supported. 

## Overview

MultiNet uses the **MFCC features** of an audio clip as input, and the **phonemes** (Chinese or English) as output. By comparing the output phonemes, the relevant Chinese or English command is identified.

## Commands Recognition Process

1. Add customized commands to the speech command queue.
2. Prepare an audio clip of 30 ms (16 KHz, 16 bit, mono).
3. Input this audio to the MFCC model and get its **MFCC features**.
4. Input the obtained **MFCC features** to MultiNet and get the output **phoneme**.
5. Input the obtained **phoneme** to the Language model and get the output.
6. Compare the output against the existing speech commands one by one, and output the Command ID of the matching command (if any).

Please see the flow diagram below:

![speech_command-recognition-system](../img/multinet_workflow.png)


## User Guide

### User-defined Command

Currently, users can define their own speech commands by using the command `make menuconfig`. You can refer to the method of adding speech commands in `menuconfig->ESP Speech Recognition->Add speech commands`, there are already 20 chinese commands and 7 english commands pre-stored in sdkconfig.

**Chinese**

|Command ID|Command|Command ID|Command|Command ID|Command|Command ID|Command|
|:---:|:---:|:---:|:---:|:---:|:---:|:---:|:---:|
|0|打开空调 (Turn on the air conditioner)|5|降低一度 (Decrease by one degree)|10| 除湿模式 (Dehumidifying mode)|15| 播放歌曲 (Play a song)
|1|关闭空调 (Turn on the air conditioner)|6|制热模式 (Heating mode)|11| 健康模式 (Healthy mode)|16| 暂停播放 (Pause playing)
|2|增大风速 (Give me more wind)|7|制冷模式 (Cooling mode)|12| 睡眠模式 (Sleep mode)|17| 定时一小时 (Set timer to 1 hour)
|3|减少风速 (Give me less wind)|8|送风模式 (Ventilating mode)|13| 打开蓝牙 (Enable the Bluetooth)|18| 打开电灯 (Turn on the light)
|4| 升高一度 (Increase by one degree)|9|节能模式 (Power-saving mode)|10| 关闭蓝牙 (Disable the Bluetooth)|19| 关闭电灯 (Turn off the light)

**English**

|Command ID|Command|Command ID|Command|
|:---:|:---:|:---:|:---:|
|0|turn on the light|4|red mode|
|1|turn off the light|5|blue mode|
|2|lighting mode|6|yellow mode|
|3|reading mode|

MultiNet supports user-defined commands. You can add your own commands to MultiNet. Note that the newly added command should obtain its command ID before it can be recognized by MultiNet. 

### Add Speech Command

Now, the MultiNet model predifine some speech commands. Users also can define their own speech commands and the number of speech commands ID in the `menuconfig -> Component config -> ESP Speech Recognition -> Add speech commands` and `The number of speech commands`. 

##### Chinese Speech Command Recognition

The speech commands should be provided in Pinyin with spaces in between. For example, the command of “打开空调”, which means to turn on the air conditioner, should be provided as "da kai kong tiao".

##### English Speech Command Recognition

The speech commands should be provided in specific phonetic symbol with spaces in between. Please use the `general_label_EN/general_label_en.py` script in the tools directory of the skainet root directory to generate the phonetic symbols corresponding to the command words. For details, please refer to [the phonetic symbol generation method](https://github.com/espressif/esp-skainet/tree/master/tools/general_label_EN/README.md). 

**Note:**

- One speech commands ID can correspond to multiple speech command phrases;
- Up to 100 speech commands ID or speech command phrases, including customized commands, are supported;
- The corresponding multiple phrases for one Command ID need to be separated by ','.

### Basic Configuration

Define the following two variables before using the command recognition model:

1. Model version  
	The model version has been configured in `menuconfig` to facilitate your development. Please configure in `menuconfig` and add the following line in your code:  
	
	`static const esp_mn_iface_t *multinet = &MULTINET_MODEL;`
	
2. Model parameter  
	The language supported and the effectiveness of the model is determined by model parameters. Now only commands in Chinese are supported. Please configure the `MULTINET_COEFF` option in `menuconfig` and add the following line in your code to generate the model handle. The 6000 is the audio length for speech recognition, in ms, the range of sample_length is 0~6000.
	   
	`model_iface_data_t *model_data = multinet->create(&MULTINET_COEFF, 6000);`
		
### API Reference

#### Header   
- esp_mn_iface.h
- esp_mn_models.h

#### Function

- `typedef model_iface_data_t* (*esp_mn_iface_op_create_t)(const model_coeff_getter_t *coeff, int sample_length);`  

  **Definition**  
   
 	Easy function type to initialize a model instance with a coefficient.
    
  **Parameter**  
   
 	* coeff: The coefficient for speech commands recognition.  
 	* sample_length: Audio length for speech recognition, in ms. The range of sample_length is 0~6000.
    
  **Return**  
 	  
 	Handle to the model data.

- `typedef int (*esp_mn_iface_op_get_samp_chunksize_t)(model_iface_data_t *model);`

   **Definition**  
   
	 Callback function type to fetch the amount of samples that need to be passed to the detection function. Every speech recognition model processes a certain number of samples at the same time. This function can be used to query the amount. Note that the returned amount is in 16-bit samples, not in bytes.
       
  **Parameter**  
   
 	model: The model object to query.
  
  **Return**
  
    The amount of samples to feed the detection function.


- `typedef int (*esp_mn_iface_op_get_samp_chunknum_t)(model_iface_data_t *model);`

   **Definition**  
   
	 Callback function type to fetch the number of frames recognized by the speech command.
       
  **Parameter**  
   
 	model: The model object to query.
  
  **Return**
  
    The number of the frames recognized by the speech command.
    
- `typedef int (*esp_mn_iface_op_get_samp_rate_t)(model_iface_data_t *model);`

   **Definition**  
   
 	Get the sample rate of the samples to feed to the detection function.

  **Parameter**  
  
 	model: The model object to query.
 
  **Return**  
  
 	The sample rate, in Hz.

- `typedef float* (*esp_mn_iface_op_detect_t)(model_iface_data_t *model, int16_t *samples);`  

   **Definition**
 
    Easy function type to initialize a model instance with a coefficient.
    
  **Parameter**  

    coeff: The coefficient for speech commands recognition.  
    
  **Return**  
   
 	* The command id, if a matching command is found.
 	* -1, if no matching command is found.
 
- `typedef void (*esp_mn_iface_op_destroy_t)(model_iface_data_t *model);`  

   **Definition**  
  
   Destroy a voiceprint recognition model.
 
  **Parameters**  
  
  model: Model object to destroy.
