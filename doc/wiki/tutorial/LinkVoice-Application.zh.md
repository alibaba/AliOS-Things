[EN](LinkVoice-Application) | 中文

  随着AI技术的进步，智能语音开始将人机交互从手+眼睛的传统模式中解放出来。带给人们更便捷、更风趣、更有人情味的体验，让被操作对象变得不再只是一个死板的工具，而更像是一个有生命的助理。“帮我打开空调”，“明天上班需要带伞吗”，“快递到哪了”…在万物互联的时代，你的所有需求只需要一句话便能实现。
AliOS Things 集成的Link Voice SDK即可实现智能语音交互。

# 关于阿里智能语音服务
阿里智能语音服务为设备提供语音交互能力、丰富的音乐内容、智能家居控制等，并可进行专有设备技能定制（如：语音操控跑步机、按摩椅等设备）。包括：

* 通用服务：搜歌、搜栏目、搜电台、问天气、百科、四则运算等；
* 阿里服务：控制智能家居、充值手机费、天猫超市购物、查询电费等 （需接入账号体系，可参考SDS接入）；
* 私有服务：操控设备、售后电话查询等 （需要技能定制，签约时请提供产品需求）。

# 功能集成
设备接入阿里语音服务，需要集成Alink SDK和Link-Voice SDK，其中Alink SDK为设备提供接入阿里IoT平台的连接、账号体系、配网、OTA等能力，而Link-Voice SDK为设备提供阿里智能语音服务。设备首先要集成了Alink SDK成为SDS平台的一个设备，才能通过集成Link-Voice SDK使用阿里智能语音服务。
Link-Voice SDK除了依赖Alink为设备完成平台接入设备管理外，还需要表格所列模块完成相应工作。其中websockets用来进行语音数据的交互；opus完成语音录制的PCM格式到opus格式的转换（服务端只接收opus格式）；cjson用来做json解析；mbedtls为alink和websockets的底层连接进行加密，为其数据传输提供安全保障。
![依赖组件](https://img.alicdn.com/tfs/TB1WsGwaKOSBuNjy0FdXXbDnVXa-648-290.png)

而目前AliOS Things已完成以上表格所有模块的移植适配工作并将其集成进来，所以我们直接使用AliOS Things便可完成愉快的智能语音开发。主mcu性能建议：
* Flash>=512KB
* RAM>=200KB
* CPU>=180Mhz

# 开发平台准备
按理满足上一章节性能要求，并带音频录制及播放功能的开发板即可。前提是需要完成AliOS Tings的移植适配工作。本文以全志xr871evb（已完成OS适配）为例进行介绍。
本平台资源：
* cpu:200Mhz cortext-M4f
* RAM：448KB（部分硬件相关code需要加载到ram中运行，实际可用约280KB）
* FLASH：2MB SPI FLASH

# 环境搭建及代码编译
先搭建AliOS开发环境（以linux为例）：
AliOS-Things-Linux-Environment-Setup

然后从github阿里官方开源库下载最新版本的AliOS Things源码（https://github.com/alibaba/AliOS-Things）：
以linux下开发为例：

    `git clone git@github.com:alibaba/AliOS-Things.git`

切换到主分支：

    `git checkout master`

建议再在主分支上新建一个自己的开发分支：

    `git checkout –b dev-xxx(yourname):`

到目前为止环境也安装好了，代码也准备完毕，只待编译及烧录测试。
编译link-voice测试例程：

    `aos make linkvoiceapp@xr871evb xr871=1`

代码烧入：

    `cd platform/mcu/xr871/tools/`

修改串口配置：

    `vim settings.ini`  

将串口改成你板子的串口号，可ls /dev/tty*查看，保存退出。
![uart setting](https://img.alicdn.com/tfs/TB1HRuoaL9TBuNjy0FcXXbeiFXa-865-728.png)
代码烧写，先将全志开发板启动选择拨码开关拨至NO位置，如同所示：
![flash setting](https://img.alicdn.com/tfs/TB1p2KkaMmTBuNjy1XbXXaMrVXa-865-454.png)
然后执行

    `./phoenixMC_linux`  开始进行代码烧写，烧写完成后再将拨码开关拨回靠串口位置，重启。

打开minicom或其他串口工具监视设备输入信息，波特率115200。
功能演示：
初次上电后先进行配网：

    `netmgr connect ssid psswd`

其中ssid和psswd分别替换为你无线网络名及密码。
根据终端提示，待网络连接后，出现以下提示时：
![press key](https://img.alicdn.com/tfs/TB1otataQyWBuNjy0FpXXassXXa-865-231.png)
按按键2（AK2）触发语音识别功能。
![talking](https://img.alicdn.com/tfs/TB1o0ataQyWBuNjy0FpXXassXXa-865-199.png)
此时对着开发板说话，语音数据被编码、上传到云端、识别成功后返回相应信息，识别失败亦有相应提示。
示例：
1:明天上班需要带伞吗？
2.给我讲个鬼故事。
3.推荐一部悬疑电影。
4.把空调打开。

# API介绍
## 1）初始化
`int pal_init(const struct pal_config *config);`
* 说明： SDK初始化，只需调用一次。
* 入参：`config`结构体向SDK传递必须的参数
* 返回：0成功；-1失败
## 2）销毁
`void pal_destroy();`
* 说明：SDK销毁，释放资源。
* 入参：无
* 返回：无
## 3）获取SDK版本
`int pal_version();`
* 说明：返回SDK的版本号
* 入参：无
* 返回：SDK版本号
## 4）设置日志级别
`void pal_set_log_level(int level);`
* 说明：设置SDK的日志级别。调试阶段可设置为PAL_LOG_LEVEL_DEBUG方便调试问题，调试稳定之后上线前把日志级别设置为PAL_LOG_LEVEL_ERROR。
* 入参：`level`为SDK的日志级别
* 返回：无
## 5）设置环境
`void pal_set_env(int env);`
* 说明：设置SDK的环境，默认是PAL_ENV_RELEASE，在厂商外部环境下可以连接阿里线上环境。厂商无需使用这个接口。
* 入参：`env`为SDK的环境
* 返回： 无
## 6）厂商播放器向SDK上报消息
`int pal_notify_msg(const char *msg);`
* 说明：厂商的播放器的状态或者按键事件需要按照`Link_Voice_SDK_播控协议_v1.0.0.xlsx`定义的json格式上报给SDK与上层应用同步状态。
* 入参：`msg`为厂商播放器需要向SDK传递的事件消息
* 返回：0成功；-1失败
## 7）厂商播放器通过SDK透传ALink消息
`int pal_post_alink_msg(const char *msg);`
* 说明：SDK初始化过程会把ALink进行初始化，与阿里平台保持物联长连接，厂商设备需要上报给ALink的消息可通过该接口上报，SDK对该消息进行透传上报给ALink服务端。
* 入参：`msg`为厂商需要通过SDK透传给ALink进行上报的消息，格式按照ALink定义的消息格式
* 返回：0成功；-1失败
## 8）开始一次语音识别
`int pal_asr_start();`
* 说明：设备通过按键或者远场唤醒触发语音识别时调用。
* 入参：无
* 返回：0成功；-1失败；
## 9）发送语音数据
`int pal_asr_send_buffer(const char *buffer, int buffer_len);`
* 说明：该接口要在调用pal_asr_start成功了之后调用，发送语音数据。如果是PCM格式的数据，则要求每次640字节。如果返回PAL_VAD_STATUS_STOP则为云端检测到语音结束了，厂商这时候可以调用pal_asr_stop或pal_asr_stop_async来获本次识别结果。
* 入参：`buffer`语音数据，`buffer_len`语音数据长度，字节
* 返回：返回云端检测到的VAD状态
## 10）结束本次语音识别（同步接口）
`struct pal_rec_result* pal_asr_stop();`
* 说明：pal_rec_result结构体返回本次语音识别的结果，同步的接口。结构体里的字段status表示本次语音识别的状态；should_restore_player_status表示厂商播放器处理完本次语音识别事件之后是否恢复之前的状态，0是不恢复，1是恢复；asr_result表示ASR识别的文本；task_status表示语音识别任务状态，PAL_REC_TASK_STATUS_END表示单次语音识别会话结束，PAL_REC_TASK_STATUS_WAITING表示多轮对话，应当播完TTS之后自动进入拾音状态，开始一次新的语音识别。
* 入参：无
* 返回：语音识别结果的结构体
## 11）结束本次语音识别（异步接口）
`void pal_asr_stop_async(pal_asr_callback callback, void *user);`
* 说明：异步返回的接口，功能与pal_asr_stop一样。
* 入参：`callback`注册的回调函数，用于返回语音识别结果；`user`用户自定义指针，在`callback`中会回传给用户
* 返回：无
## 12）取消本次语音识别
`void pal_asr_cancel();`
* 说明：取消本次的语音识别。
* 入参：无
* 返回：无
## 13）销毁语音识别结果
`void pal_rec_result_destroy(struct pal_rec_result *result);`
* 说明：pal_asr_stop和pal_asr_stop_async返回的语音识别结果，需要通过该接口来释放资源。
* 入参：`result`需要销毁的结果
* 返回：无
## 14）文本转语音（同步接口）
`struct pal_rec_result* pal_get_tts(const char *text);`
* 说明：提供文本转语音的功能。返回的结果pal_rec_result需要通过pal_rec_result_destroy销毁。
* 入参：`text`需要转换的文本
* 返回：返回的结构体，文本转语音的结果在tts字段中，是一个可播放的url。


