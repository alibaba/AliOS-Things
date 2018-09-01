//
//  pal.h
//  pal_sdk
//

#ifndef pal_h
#define pal_h

#ifdef __cplusplus
extern "C"
{
#endif
    
/*
 * Link-Voice SDK回调。不要在回调里面做耗时操作，阻塞callback线程。
 */
enum pal_callback_cmd_type {
    PAL_CALLBACK_CMD_NOTIFY,                              //SDK事件通知给设备
    PAL_CALLBACK_CMD_QUERY,                               //SDK向设备查询状态，设备的状态拷贝到buffer中传给SDK
    PAL_CALLBACK_CMD_ALINK                                //SDK透传的Alink的ALINK_CLOUD_CONNECTED，ALINK_CLOUD_DISCONNECTED，ALINK_GET_DEVICE_STATUS，ALINK_SET_DEVICE_STATUS四个事件给设备
};

/*
 * cmd: SDK事件通知设备/状态查询的json字符串，参考Link_Voice_SDK_protocal_v1.0.0.xlsx。
 * cmd_type: pal_callback_cmd_type类型
 */
    
typedef void (*pal_callback)(const char *cmd, int cmd_type, char *buffer, int buffer_capacity, void *user);

/*
 * 目前支持两种格式：PCM，Opus；采样率必须是16000Hz，单声道，16 bits。
 */
enum pal_audio_format {
    PAL_AUDIO_FORMAT_PCM,
    PAL_AUDIO_FORMAT_OPUS
};
    
/*
 * Link-Voice SDK日志级别
 */
enum pal_log_level {
    PAL_LOG_LEVEL_DEBUG,
    PAL_LOG_LEVEL_INFO,
    PAL_LOG_LEVEL_ERROR
};
    
/*
 * Link-Voice SDK环境切换
 */
enum pal_env {
    PAL_ENV_RELEASE,
    PAL_ENV_PRE_RELEASE,
    PAL_ENV_DAILY
};
    
/*
 * ASR模型：
 * PAL_ASR_MODEL_NEAR_FIELD为近场模型，建议Push-To-Talk/Tap-To-Talk的音箱选择该模型；
 * PAL_ASR_MODEL_FAR_FIELD为远场模型，建议远场唤醒的音箱选择该模型。
 */
enum pal_asr_model {
    PAL_ASR_MODEL_NEAR_FIELD,
    PAL_ASR_MODEL_FAR_FIELD
};

/*
 * 配置信息
 */
struct pal_config
{
    int asr_model;                          //PAL_ASR_MODEL_NEAR_FIELD 或 PAL_ASR_MODEL_FAR_FIELD
    int format;                             //AUDIO_FORMAT_PCM 或 AUDIO_FORMAT_OPUS
    unsigned int sample_rate;               //目前仅支持16000Hz
    unsigned int channels;                  //目前仅支持单声道，channels = 1
    unsigned int bits_per_sample;           //目前仅支持16 bits每采样点
    char *ca_file_path;                     //CA证书文件路径，如: /tmp/ca.pem。RTOS填NULL
    void *user;                             //用户自定义指针，在SDK回调里面会回传，可以NULL
    pal_callback callback;                  //SDK回调
};

/*
 * Link-Voice SDK初始化
 * config  configuraion information
 * return  0: success
 *        -1: failed
 */
int pal_init(const struct pal_config *config);

/*
 * 厂商播放器向Link-Voice SDK上报消息。
 */
int pal_notify_msg(const char *msg);
    
/*
 * 厂商设备通过Link-Voice SDK上报ALink消息
 */
int pal_post_alink_msg(const char *msg);

/*
 * 设备解绑。设备在恢复出厂设置前需要调用，将设备与ALink服务端解绑。
 * return  0: 解绑成功
 -1: 解绑失败，但下次设备配网ALink会先进行解绑再重新注册
 */
int pal_factory_reset();

/*
 * Link-Voice SDK销毁
 */
void pal_destroy();

/*
 * Link-Voice SDK version
 */
int pal_version();
    
/*
 * Link-Voice SDK 切换环境
 * 线上 PAL_ENV_RELEASE  (default)
 * 预发 PAL_ENV_PRE_RELEASE
 * 日常 PAL_ENV_DAILY
 */
void pal_set_env(int env);

/*
 * Link-Voice SDK 日志级别: PAL_LOG_LEVEL_DEBUG, PAL_LOG_LEVEL_INFO, PAL_LOG_LEVEL_ERROR
 */
void pal_set_log_level(int level);

/*
 * 语音识别状态
 */
enum pal_rec_result_status
{
    PAL_REC_RESULT_STATUS_NONE = -1,
    PAL_REC_RESULT_STATUS_SUCCESS = 0,
    PAL_REC_RESULT_STATUS_NETWORKING_ERROR,
    PAL_REC_RESULT_STATUS_NETWORKING_TIMEOUT
};

/*
 * 语音识别任务状态
 * RECOGNIZE_TASK_STATUS_END本轮对话结束；RECOGNIZE_TASK_STATUS_WAITING多轮对话。
 */
enum pal_rec_task_status {
    PAL_REC_TASK_STATUS_END = 0,
    PAL_REC_TASK_STATUS_WAITING
};

/*
 * 语音识别结果
 */
struct pal_rec_result
{
    int status;                       //本次语音识别是否成功 pal_rec_result_status
    int should_restore_player_status; //控制播放器是否处理完语音识别事件之后恢复之前的状态，0是不恢复；1是恢复。
    char *asr_result;                 //ASR结果
    int task_status;                  //语音识别任务状态，pal_rec_task_status。PAL_REC_TASK_STATUS_END表示单次会话结束；PAL_REC_TASK_STATUS_WAITING表示多轮会话，应当播完TTS之后进入拾音状态。
    char *tts;                        //TTS
    char *raw;                        //包含语音识别的结果，透传给设备自己解析并执行相关控制指令
};

enum pal_vad_status
{
    PAL_VAD_STATUS_NONE = -1,
    PAL_VAD_STATUS_VOICE,
    PAL_VAD_STATUS_SILENCE,
    PAL_VAD_STATUS_STOP
};

/*
 * 开始一次语音识别。
 * return 0: success
 *       -1: failed
 */
int pal_asr_start();

/*
 * 向Link-Voice SDK发送语音数据。返回VAD的状态，vad_status。If use pcm format, buffer_len must be 640
 */
int pal_asr_send_buffer(const char *buffer, int buffer_len);

/*
 * 结束本次语音识别，同步接口。请调用pal_rec_result_destroy销毁pal_rec_result。
 */
struct pal_rec_result* pal_asr_stop();
    
/*
 * 结束本次语音识别，异步接口。请调用pal_rec_result_destroy销毁pal_rec_result。
 */
typedef void (*pal_asr_callback)(struct pal_rec_result *result, void *user);
void pal_asr_stop_async(pal_asr_callback callback, void *user);
    
/*
 * 取消本次语音识别。
 */
void pal_asr_cancel();

/*
 * 文本转语音，同步接口。请调用pal_rec_result_destroy销毁pal_rec_result。
 *
 */
struct pal_rec_result* pal_get_tts(const char *text);
    
/*
 * 销毁pal_rec_result。
 */
void pal_rec_result_destroy(struct pal_rec_result *result);
    
#ifdef __cplusplus
}
#endif

#endif /* pal_h */
