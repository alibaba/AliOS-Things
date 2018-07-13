EN | [中文](LinkVoice-Application.zh)

# LinkVoice Application

With the progress of AI technology, intelligent voice is no longer a rigid tool, but more like a life assistant, bringing people more convenient and interesting experience. "Help me open the air conditioner.", "Do I need an umbrella for work tomorrow?", "Where is my express?"... In IoT era, all your needs can be realized by your sentence. The LinkVoice SDK integrated in AliOS Things can achieve intelligent voice interaction.

# Alibaba intelligent voice service

Alibaba intelligent voice service contains rich music, can enable voice interaction and smart home control, and can customize device skills (for example, it can manipulate running machines, massage chairs and other devices by voice). Services provided by us Include :

- general services: search for songs, columns, radio stations, weather; refer to encyclopedia, four arithmetic operation table, and etc.
- services provided by Alibaba: smart home control, recharge mobile phone, Tmall shopping,  and etc. (You need to connect to account system, which can refer to SDS access);
- private service: equipment control, customer service inquiry, and etc. (If customized skills are needed, please provide product requirements when signing the contract).

# Functions integration 

To use Alibaba linkvoice service, you need to integrate Alink SDK and Link-Voice SDK. Alink SDK provides devices with access to Alibaba IoT platform, account system, network distribution, OTA and etc. Link-Voice SDK provides devices with Alibaba intelligent voice service. Devices should first integrate Alink SDK to be a device in SDS platform and then integrate Link-Voice SDK to use Alibaba intelligent voice service. In addition to relying on Alink to manage devices in the platform, Link-Voice SDK needs modules as listed in the table to complete its work. 

WebSockets is used to complete voice data interaction; opus is used to transform voice recording from PCM format to opus format (the server can only receive opus format); Cjson is used for JSON analysis; Mbedtls is used to encrypt the connection in underlying layer of alink and WebSockets, securing its data transmission.

![依赖组件](https://camo.githubusercontent.com/337e6a420ff7d661c8902fc763d2e566577ccb05/68747470733a2f2f696d672e616c6963646e2e636f6d2f7466732f54423157734777614b4f5342754e6a79304664585862446e5658612d3634382d3239302e706e67)

AliOS Things has integrated all these modules in the list, so you can directly use AliOS Things to develop intelligent voice. Suggestion for mcu capability :

- Flash>=512KB
- RAM>=200KB
- CPU>=180Mhz

# Preparation for development platform

You need development boards that meet the performance requirements mentioned in the previous section, together with audio recording and playing functions. Also, you need to complete AliOS Tings migration adaptation. This article takes xr871evb as an example  (OS matching has been completed). The resources of the platform:

- cpu:200Mhz cortext-M4f
- RAM：448KB（some code related to hardwares need to be operated in ram; about 280KB can actually be used）
- FLASH：2MB SPI FLASH

# Environment setup and code compilation 

You should first set up AliOS development environment ( take linux as an example）: AliOS-Things-Linux-Environment-Setup

Then download the latest version of source code of AliOS Things in github （[https://github.com/alibaba/AliOS-Things）：](https://github.com/alibaba/AliOS-Things%EF%BC%89%EF%BC%9A) also take linux as an example :

```
`git clone git@github.com:alibaba/AliOS-Things.git`

```

switch to master branch：

```
`git checkout master`

```

We suggest you to add your development branch based on the master one : 

```
`git checkout –b dev-xxx(yourname):`

```

Till now on, environment has been set up, code has been prepared and you can start compilation and programming test. Example procedure of link-voice test :

```
`aos make linkvoiceapp@xr871evb xr871=1`

```

Program your code：

```
`cd platform/mcu/xr871/tools/`

```

Modify serial port configuration ：

```
`vim settings.ini`  

```

Replace the serial poet number with your board's, you can check it in ls /dev/tty*. Reserve and exit. ![uart setting](https://camo.githubusercontent.com/586142cb67fd425fb6f9c44f31e8c4fb32567188/68747470733a2f2f696d672e616c6963646e2e636f6d2f7466732f5442314852756f614c395442754e6a7930466358586265694658612d3836352d3732382e706e67)Burn your code in. You should first turn the switch to ON position as is shown in the picture :![flash setting](https://camo.githubusercontent.com/f061b3c1d38fe54ca40a88aefe0a3dc2386aa5a0/68747470733a2f2f696d672e616c6963646e2e636f6d2f7466732f54423170324b6b614d6d5442754e6a793158625858614d725658612d3836352d3435342e706e67)Then execute :

```
`./phoenixMC_linux`  to start code burning. After burning,  set the switch back to the serial port position and restart.
```

Open minicom or other serial port supervision device to input information, with baud rate as 115200. Operation example: start network distribution after first power on :

```
`netmgr connect ssid psswd`

```

Replace ssid and psswd with the name and password of your wireless network. When connecting to network, the following notice will appear in your terminal :![press key](https://camo.githubusercontent.com/2d3d09cb35fe14521a87b20f1dc4058aa8b8eb3d/68747470733a2f2f696d672e616c6963646e2e636f6d2f7466732f5442316f7461746151795742754e6a7930467058586173735858612d3836352d3233312e706e67)Press button 2 (AK2) to enable speech recognition function. ![talking](https://camo.githubusercontent.com/f0511cb3bf687f604b2c4099e767fc80896ca913/68747470733a2f2f696d672e616c6963646e2e636f6d2f7466732f5442316f3061746151795742754e6a7930467058586173735858612d3836352d3139392e706e67)At this time, speak to the development board, and your voice data will be encoded, uploaded to the cloud, and then corresponding feedback will return no matter the identification has succeeded or failed. Example: 1: Do you need an umbrella for work tomorrow? 2. Tell me a ghost story. 3. Recommend me a suspense movie. 4. Open the air conditioner.

# Introduction to API

## 1）Initiate

`int pal_init(const struct pal_config *config);`

- Description： SDK initiation. You only need to call it once.
- Parameters：`config` transfer necessary parameters to SDK.
- Returns：0 on success, negative error on failure.

## 2）Destroy

`void pal_destroy();`

- Description : destroy SDK and release the resources.
- Parameters：no
- Returns：no

## 3）Get SDK version number

`int pal_version();`

- Description : return SDK version number
- Parameters : no
- Returns : SDK version number

## 4）Set log level 

`void pal_set_log_level(int level);`

- Description：set SDK log level. In testing stage, you can set it as PAL_LOG_LEVEL_DEBUG to make testing easier. After testing, set it as PAL_LOG_LEVEL_ERROR.
- Parameters：`level` is the log level of SDK.
- Returns：no

## 5）Environment setup

`void pal_set_env(int env);`

- Description：set up SDK environment, and it is PAL_ENV_RELEASE by fault. You can connect to Alibaba online environment in vendors' environment. There is no need for vendors to use this interface. 
- Parameters：`env` is the environment of SDK
- Returns : no

## 6）Vendor player notify SDK of message

`int pal_notify_msg(const char *msg);`

- Description：vendor players notify SDK of its status or key events in json format as defined in  `Link_Voice_SDK_播控协议_v1.0.0.xlsx`.
- Parameters：`msg` is the message that vendors want to report to SDK
- Returns：0 on success, negative error on failure.

## 7）Vendor player post ALink message through SDK

`int pal_post_alink_msg(const char *msg);`

- Description : ALINK will be initiated during SDK initiation, and keeps the connection to Alibaba platform. Vendors can post message to ALink through this interface, where SDK will post it to ALink server side.
- Parameters：`msg` is the message that vendors want to post to ALink through SDK. The format is the message format defined by ALink.
- Returns：0 on success, negative error on failure.

## 8）Start speech recognition

`int pal_asr_start();`

- Description : It will be called when speech recognition function is triggered through button or far field awakening. 
- Parameters：no
- Returns：0 on success, negative error on failure.

## 9）Send out voice data 

`int pal_asr_send_buffer(const char *buffer, int buffer_len);`

- Description：This interface should be called when calling of pal_asr_start has been completed. When calling, voice data will be sent. If the data is in PCM format, it should be 640 bytes. If return is PAL_VAD_STATUS_STOP, which means the voice is detected by the cloud to end, vendors can call pal_asr_stop or pal_asr_stop_async to obtain the recognition result.
- Parameters：`buffer` is voice data. `buffer_len` is the length and byte of this voice data.
- Returns：the VAD status detected by the cloud

## 10）End voice recognition (synchro interface)

`struct pal_rec_result* pal_asr_stop();`

- Description : As for synchro interface, pal_rec_result will return the result of voice recognition. "Status" shows the statues of voice recognition; "should_restore_player_status" shows whether the vendor player recover to its former status when completing voice recognition, where 0 means not recover and 1 means recover; "asr_result" shows the text recognized by ASR; "task_status" shows the status of the task; "PAL_REC_TASK_STATUS_END" shows the end of this task; "PAL_REC_TASK_STATUS_WAITING" shows it's a multi - wheel dialogue.
- Parameters：no
- Returns：the result of voice recognition

## 11）End voice recognition（Asynchronous interface）

`void pal_asr_stop_async(pal_asr_callback callback, void *user);`

- Description：Interface for asynchronous return, whose function is similar to pal_asr_stop.
- Parameters：`callback` is a registered callback function, which is used to return recognition result;  `user` is user-defined pointer, which will be returned to users in `callback`.
- Returns：no

## 12）Cancel voice recognition 

`void pal_asr_cancel();`

- Description：Cancel voice recognition 
- Parameters：no
- Returns：no

## 13）Destroy the result of voice recognition 

`void pal_rec_result_destroy(struct pal_rec_result *result);`

- Description：pal_asr_stop and pal_asr_stop_async can return the result of voice recognition. You can use this interface to release resources.
- Parameters：`result` is the result that need to be destroyed. 
- Returns：no

## 14）Transform text to voice (synchronous interface) 

`struct pal_rec_result* pal_get_tts(const char *text);`

- Description：Transform text to voice. The return result, pal_rec_result, needs to be destroyed by pal_rec_result_destroy.
- Parameters：`text` is the text that needs to be transformed 
- Returns：The returned structure. The voice transformed from text is a playable url in tts field. 
