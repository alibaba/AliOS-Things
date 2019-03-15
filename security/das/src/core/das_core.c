/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#include <das/platform.h>
#include <das/service.h>
#include <das/hardware.h>

#include "proto/pb_encode.h"
#include "proto/pb_decode.h"
#include "proto/lsoc.pb.h"

#include <das.h>

static const char DAS_VERSION[] = "das:1.1.0,firmware:";

static const char DAS_PLATFORM[] = "os:rtos,arch:" DAS_ARCH;

static const char DAS_FIL_TAG[] = "file:";
static const char DAS_KEY_DEVICE_ID[] = "uid:";
static const char DAS_KEY_SEPARATOR = ',';

#ifdef DAS_DEBUG

#define DAS_LOG_COMMAND(m)                                          \
        DAS_LOG("ats_cmd - domain: %d\n", m.domain);                \
        DAS_LOG("ats_cmd - code: %d\n", m.code);                    \
        DAS_LOG("ats_cmd - time_ms: 0x%lx\n", (long)m.timestamp);   \
        DAS_LOG("ats_cmd - which_data: %d\n", m.which_data);

#define DAS_LOG_INFO(m)                                              \
        DAS_LOG("dev_inf - domain: %d\n", m->domain);                \
        DAS_LOG("dev_inf - code: %d\n", m->code);                    \
        DAS_LOG("dev_inf - time_ms: 0x%lx\n", (long)m->timestamp);   \
        DAS_LOG("dev_inf - which_data: %d\n", m->which_data);        \
        DAS_LOG("dev_inf - version: %s\n", m->data.infData.version); \
        DAS_LOG("dev_inf - device: %s\n", m->data.infData.device);   \
        DAS_LOG("dev_inf - state: %s\n", m->data.infData.state);

#define DAS_LOG_ATTEST_FILE(m)                                           \
        DAS_LOG("ats_fil - domain: %d\n", m->domain);                    \
        DAS_LOG("ats_fil - code: %d\n", m->code);                        \
        DAS_LOG("ats_fil - time_ms: 0x%lx\n", (long)m->timestamp);       \
        DAS_LOG("ats_fil - which_data: %d\n", m->which_data);            \
        DAS_LOG("ats_fil - cmd: 0x%lx\n", (long)m->data.atsFilData.cmd); \
        DAS_LOG("ats_fil - path: %s\n", m->data.atsFilData.path);        \
        DAS_LOG("ats_fil - sum: %s\n", m->data.atsFilData.signature);


#define DAS_LOG_ATTEST_END(m)                                            \
        DAS_LOG("ats_end - domain: %d\n", m->domain);                    \
        DAS_LOG("ats_end - code: %d\n", m->code);                        \
        DAS_LOG("ats_end - time_ms: 0x%lx\n", (long)m->timestamp);       \
        DAS_LOG("ats_end - which_data: %d\n", m->which_data);            \
        DAS_LOG("ats_end - cmd: 0x%lx\n", (long)m->data.atsEndData.cmd); \
        DAS_LOG("ats_end - total: %d\n", m->data.atsEndData.total);      \
        DAS_LOG("ats_end - sum: %s\n", m->data.atsEndData.sum);

#define DAS_LOG_MEASURE_FILE(m)                                          \
        DAS_LOG("mes_fil - domain: %d\n", m->domain);                    \
        DAS_LOG("mes_fil - code: %d\n", m->code);                        \
        DAS_LOG("mes_fil - time_ms: 0x%lx\n", (long)m->timestamp);       \
        DAS_LOG("mes_fil - which_data: %d\n", m->which_data);            \
        DAS_LOG("mes_fil - cmd: 0x%lx\n", (long)m->data.mesFilData.cmd); \
        DAS_LOG("mes_fil - sum: %s\n", m->data.mesFilData.sum);          \
        DAS_LOG("mes_fil - mac: %s\n", m->data.mesFilData.mac);

#define DAS_LOG_MEASURE_END(m)                                           \
        DAS_LOG("mes_end - domain: %d\n", m->domain);                    \
        DAS_LOG("mes_end - code: %d\n", m->code);                        \
        DAS_LOG("mes_end - time_ms: 0x%lx\n", (long)m->timestamp);       \
        DAS_LOG("mes_end - which_data: %d\n", m->which_data);            \
        DAS_LOG("mes_end - cmd: 0x%lx\n", (long)m->data.mesEndData.cmd); \
        DAS_LOG("mes_end - total: %d\n", m->data.mesEndData.total);

#define DAS_LOG_STEPPING(m)                                          \
        DAS_LOG("stepping - step: %d\n", m->step);                   \
        DAS_LOG("stepping - service_index: %d\n", m->service_index);

#define DAS_CORE_ASSERT(_x)                             \
    do {                                                \
        if (!(_x)) {                                    \
            printf("ASSERT FAILURE:\n");                \
            printf("%s (%d): %s\n",                     \
                    __FILE__, __LINE__, __FUNCTION__);  \
            while (1) /* loop */;                       \
        }                                               \
    } while (0)

#else

#define DAS_LOG_COMMAND(m)
#define DAS_LOG_INFO(m)         
#define DAS_LOG_ATTEST_FILE(m)  
#define DAS_LOG_ATTEST_END(m)   
#define DAS_LOG_MEASURE_FILE(m) 
#define DAS_LOG_MEASURE_END(m)  
#define DAS_LOG_STEPPING(m)

#define DAS_CORE_ASSERT(_x)

#endif

#define DAS_SESSION_MAGIC     0x73736164

#define DAS_PUB_TOPIC(s,p,d)  { \
                                char *_o=s->pub_topic;  \
                                int _l=strlen("/sys/"); \
                                memcpy(_o,"/sys/",_l);  \
                                _o+=_l;_l=strlen(p);    \
                                memcpy(_o,p,_l);        \
                                _o+=_l;*_o='/';         \
                                _o++;_l=strlen(d);      \
                                memcpy(_o,d,_l);_o+=_l; \
                                _l=strlen("/security/upstream"); \
                                memcpy(_o,"/security/upstream",_l); \
                             }
#define DAS_SUB_TOPIC(s,p,d) { \
                                char *_o=s->sub_topic;  \
                                int _l=strlen("/sys/"); \
                                memcpy(_o,"/sys/",_l);  \
                                _o+=_l;_l=strlen(p);    \
                                memcpy(_o,p,_l);        \
                                _o+=_l;*_o='/';         \
                                _o++;_l=strlen(d);      \
                                memcpy(_o,d,_l);_o+=_l; \
                                _l=strlen("/security/downstream"); \
                                memcpy(_o,"/security/downstream",_l); \
                            }

typedef enum {
    DAS_CMD_NONE = 0,
    DAS_CMD_ATTEST = 1,
    DAS_CMD_MEASURE = 2,
} das_cmd_t;

typedef enum {
    DAS_STEP_IDLE = 0,
    DAS_STEP_INFO = 1,
    DAS_STEP_ATTEST = 2,
    DAS_STEP_MEASURE = 3,
} das_step_t;

typedef struct _das_session {
    uint32_t magic;         /* magic number */

    char pub_topic[DAS_TOPIC_LENGTH];
    char sub_topic[DAS_TOPIC_LENGTH];

    void *channel;          /* communication channel handle */
    bool connected;
    publish_handle_t publish_handle;

    das_cmd_t cmd;
    uint64_t cmd_id;

    uint64_t info_time;

    Message report;
    uint32_t report_number;

    das_step_t step;
    uint64_t stepping_now;
    das_service_t *stepping_service;

    das_sha1_context_t stepping_sum_context;
    das_sha1_context_t stepping_mac_context;
    uint8_t stepping_buffer[DAS_BUFFER_LENGTH];

    uint32_t service_index;
    das_service_state_t service_state;
} das_session_t;

static das_session_t _g_session = {0};

void *das_init(const char *product_name, const char *device_name)
{
    das_session_t *ss = &_g_session;
    das_service_t *service;
    int i;

    if (product_name == NULL || device_name == NULL) {
        DAS_LOG("invalid input args\n");
        return NULL;
    }

    ss->magic = DAS_SESSION_MAGIC;

    DAS_PUB_TOPIC(ss, product_name, device_name);
    DAS_SUB_TOPIC(ss, product_name, device_name);

    for (i = 0; i < DAS_MAX_SERVICE_NUMBER; i++) {
        service = das_service_table[i];
        if (service == NULL) {
            break;
        }

        if (!service->name) {
            DAS_LOG("invalid service %d\n", i);
            return NULL;
        }
        if (service->init) {
            service->init();
        }
    }

    ss->channel = NULL;
    ss->publish_handle = NULL;

    ss->cmd = DAS_CMD_NONE;
    ss->info_time = 0;
    ss->step = DAS_STEP_IDLE;

    return ss;
}

void das_final(void *session)
{
    /* session is static, do nothing*/
    DAS_LOG("das session finalized.\n");
}

static const char *_get_sub_topic(das_session_t *session)
{
    return session->sub_topic;
}

const char *das_sub_topic(void *session, const char *topic)
{
    uint32_t topic_len;
    das_session_t *ss = (das_session_t *)session;

    if (topic == NULL)
        return _get_sub_topic(ss);

    if (session == NULL || topic == NULL) {
        return NULL;
    }

    topic_len = strlen(topic);
    if (topic_len > DAS_TOPIC_LENGTH - 1) {
        return NULL;
    }

    memcpy(ss->sub_topic, topic, topic_len);
    ss->sub_topic[topic_len] = '\0';

    return ss->sub_topic;
}

static const char *_get_pub_topic(das_session_t *session)
{
    return session->pub_topic;
}

const char *das_set_pub_topic(void *session, const char *topic)
{
    uint32_t topic_len;
    das_session_t *ss = (das_session_t *)session;

    if (topic == NULL)
        return _get_pub_topic(ss);

    if (session == NULL || topic == NULL) {
        DAS_LOG("invalid input args\n");
        return NULL;
    }

    topic_len = strlen(topic);
    if (topic_len > DAS_TOPIC_LENGTH - 1) {
        DAS_LOG("short buffer, %d\n", topic_len);
        return NULL;
    }

    memcpy(ss->pub_topic, topic, topic_len);
    ss->pub_topic[topic_len] = '\0';

    return ss->pub_topic;
}

void das_connection(void *session, publish_handle_t publish_handle, void *channel)
{
    das_session_t *ss = (das_session_t *)session;

    if (session == NULL || publish_handle == NULL || channel == NULL) {
        DAS_LOG("invalid input args\n");
        DAS_CORE_ASSERT(0);
        return;
    }

    if (ss->magic != DAS_SESSION_MAGIC) {
        DAS_LOG("invalid session magic, 0x%x\n", ss->magic);
        DAS_CORE_ASSERT(0);
        return;
    }

    ss->channel = channel;
    ss->publish_handle = publish_handle;
    ss->connected = false;

    return;
}

void das_on_connected( void *session)
{
    das_session_t *ss = (das_session_t *)session;

    DAS_CORE_ASSERT(session != NULL && ss->magic == DAS_SESSION_MAGIC);

    ss->connected = true;

    return;
}

void das_on_disconnected( void *session)
{
    das_session_t *ss = (das_session_t *)session;

    DAS_CORE_ASSERT(session != NULL && ss->magic == DAS_SESSION_MAGIC);

    ss->connected = false;

    return;
}

static int _decode_message(das_session_t *session, const uint8_t *message, size_t size)
{
    bool status;
    pb_istream_t stream;
    Message cmd = Message_init_zero;

    if (session == NULL || message == NULL || size == 0) {
        DAS_LOG("invalid input args\n");
        return DAS_ERROR_BAD_PARAMETERS;
    }

    stream = pb_istream_from_buffer(message, size);

    status = pb_decode(&stream, Message_fields, &cmd);
    if (!status) {
        DAS_LOG("decode failed: %s\n", PB_GET_ERROR(&stream));
        return DAS_ERROR_GENERIC;
    }

    if (cmd.which_data == Message_atsCmdData_tag) {
        if (AtsCmdData_Type_FULL != cmd.data.atsCmdData.type) {
            DAS_LOG("not support type: %d\n", cmd.data.atsCmdData.type);
            return DAS_ERROR_NOT_SUPPORTED;
        }

        DAS_LOG_COMMAND(cmd);

        session->cmd = DAS_CMD_ATTEST;
        session->cmd_id = cmd.timestamp;

    } else if (cmd.which_data == Message_mesCmdData_tag) {
        if (MesCmdData_Type_FULL != cmd.data.mesCmdData.type) {
            DAS_LOG("not support type: %d\n", cmd.data.mesCmdData.type);
            return DAS_ERROR_NOT_SUPPORTED;
        }

        DAS_LOG_COMMAND(cmd);

        session->cmd = DAS_CMD_MEASURE;
        session->cmd_id = cmd.timestamp;

    } else {
        DAS_LOG("unknown command: %d\n", cmd.which_data);
    }

    return DAS_SUCCESS;
}

void das_on_message(void *session, const uint8_t *message, size_t size)
{
    das_session_t *ss = (das_session_t *)session;

    if (session == NULL || ss->magic != DAS_SESSION_MAGIC ) {
        DAS_LOG("Invalid session 0x%lx\n", (long)ss);
        return;
    }

    if (message == NULL || size == 0) {
        DAS_LOG("Invalid message 0x%lx\n", (long)message);
        return;
    }

    if (ss->cmd != DAS_CMD_NONE) {
        DAS_LOG("DAS command is in processing, so a incoming command dropped.\n");
        return;
    }

    _decode_message(ss, message, size);
}

static size_t _encode_message(das_session_t *session, Message *message)
{
    bool status;
    size_t size;
    pb_ostream_t stream;

    status = pb_get_encoded_size(&size, Message_fields, message);
    if (!status) {
        DAS_LOG("get encoded size failed\n");
        return 0;
    }

    /* get buf size only */
    if (message == NULL) {
        return size;
    }

    if (size > DAS_BUFFER_LENGTH) {
        DAS_LOG("short buffer, %lx\n", (long)size);
        return 0;
    }

    stream = pb_ostream_from_buffer(session->stepping_buffer, size);

    status = pb_encode(&stream, Message_fields, message);
    if (!status) {
        DAS_LOG("encoding failed: %s\n", PB_GET_ERROR(&stream));
        return 0;
    }

    return size;
}

static void _publish_message(das_session_t *session)
{
    size_t size;

    if (session->connected) {
        size = _encode_message(session, &session->report);
        if (size > 0) {
            session->publish_handle(session->pub_topic,
                     session->stepping_buffer, size, session->channel);
        }
    }
}

static void _publish_info(das_session_t *session, bool final)
{
    Message *report = &(session->report);
    InfData *info = &(report->data.infData);
    char *offset;
    int length;

    if (true == final) {
        return;
    }

    report->domain = Domain_SYS;
    report->code = Code_INF;
    report->timestamp = session->stepping_now;
    report->which_data = Message_infData_tag;

    /* version & version.firmware */
    offset = info->version;
    length = strlen(DAS_VERSION);
    memcpy(offset, DAS_VERSION, length);
    offset += length;

    length = info->device - offset;
    length = das_hal_firmware_version(offset, length);
    /* truncate overflow */
    offset += (length > (info->device - offset))? (info->device - offset): length;
    *offset = '\0';

    /* device & device.arch */
    offset = info->device;
    length = strlen(DAS_PLATFORM);
    memcpy(offset, DAS_PLATFORM, length);
    offset += length;

    *offset = DAS_KEY_SEPARATOR;
    offset++;
    /* device.id */
    length = strlen(DAS_KEY_DEVICE_ID);
    memcpy(offset, DAS_KEY_DEVICE_ID, length);
    offset += length;
    length = info->state - offset;
    length = das_hal_device_id(offset, length);
    /* truncate overflow */
    offset += (length > (info->state - offset))? (info->state - offset): length;
    *offset = '\0';

    DAS_LOG_INFO(report);

    _publish_message(session);
}

static void _publish_attest(das_session_t *session, bool final)
{
    uint8_t hash[DAS_SHA1_LENGTH];
    Message *report = &(session->report);
    AtsFilData *fdata = &(report->data.atsFilData);
    AtsEndData *edata = &(report->data.atsEndData);
    das_sum_context_t *sum_ctx = (das_sum_context_t *)session->stepping_buffer;

    report->domain = Domain_SYS;
    report->code = Code_ATS;
    report->timestamp = session->stepping_now;

    /* recycle sum context if final */
    das_sum_finish(sum_ctx, hash);

    if (false == final) {
        report->which_data = Message_atsFilData_tag;
        fdata->cmd = session->cmd_id;
        das_hex2string(fdata->signature, sizeof(fdata->signature), hash, DAS_SHA1_LENGTH);

        das_sum_update(&session->stepping_sum_context, hash, DAS_SHA1_LENGTH);
        session->report_number++;

        DAS_LOG_ATTEST_FILE(report);

        _publish_message(session);

        /* re-start sum context */
        das_sum_start(sum_ctx);
    } else {
        report->which_data = Message_atsEndData_tag;
        edata->cmd = session->cmd_id;
        edata->total = session->report_number;

        das_sum_finish(&session->stepping_sum_context, hash);
        memcpy(edata->sum, DAS_FIL_TAG, strlen(DAS_FIL_TAG));
        das_hex2string((char*)edata->sum + strlen(DAS_FIL_TAG),
            sizeof(edata->sum) - strlen(DAS_FIL_TAG), hash, DAS_SHA1_LENGTH);

        DAS_LOG_ATTEST_END(report);

        _publish_message(session);

        session->cmd = DAS_CMD_NONE;
    }
}

static void _publish_measure(das_session_t *session, bool final)
{
    uint8_t hash[DAS_SHA1_LENGTH];
    Message *report = &(session->report);
    MesFilData *fdata = &(report->data.mesFilData);
    MesEndData *edata = &(report->data.mesEndData);
    das_sum_context_t *sum_ctx = (das_sum_context_t *)session->stepping_buffer;

    report->domain = Domain_SYS;
    report->code = Code_MES;
    report->timestamp = session->stepping_now;

    /* recycle sum context if final */
    das_sum_finish(sum_ctx, hash);

    if (false == final) {
        das_sum_update(&session->stepping_sum_context, hash, DAS_SHA1_LENGTH);

        /* re-start sum context */
        das_sum_start(sum_ctx);
    } else {
        report->which_data = Message_mesFilData_tag;
        fdata->cmd = session->cmd_id;

        das_sum_finish(&session->stepping_sum_context, hash);
        das_hex2string(fdata->sum, sizeof(fdata->sum), hash, DAS_SHA1_LENGTH);

        das_mac_finish(&session->stepping_mac_context, hash);
        das_hex2string(fdata->mac, sizeof(fdata->mac), hash, DAS_SHA1_LENGTH);

        DAS_LOG_MEASURE_FILE(report);
        _publish_message(session);

        report->which_data = Message_mesEndData_tag;
        edata->cmd = session->cmd_id;
        edata->total = 1;

        DAS_LOG_MEASURE_END(report);

        _publish_message(session);

        session->cmd = DAS_CMD_NONE;
    }
}

das_result_t das_stepping(void *session, uint64_t now)
{
    das_result_t ret = DAS_SUCCESS;
    bool is_stepping = false;
    das_session_t *ss = (das_session_t *)session;
    uint8_t hash[DAS_SHA1_LENGTH] = {0};
    das_sum_context_t *sum_ctx = (das_sum_context_t *)ss->stepping_buffer;

    if (ss == NULL || ss->magic != DAS_SESSION_MAGIC) {
        DAS_LOG("invalid session, 0x%lx\n", (long)ss);
        return DAS_ERROR_BAD_PARAMETERS;
    }

    ss->stepping_now = now;

    if (ss->step == DAS_STEP_IDLE) {
        if (ss->cmd != DAS_CMD_NONE) {
            ss->report_number = 0;
            ss->step = (ss->cmd == DAS_CMD_ATTEST ? DAS_STEP_ATTEST : DAS_STEP_MEASURE);
            if (ss->cmd == DAS_CMD_ATTEST) {
                das_sum_start(sum_ctx);
                das_sum_start(&ss->stepping_sum_context);
            } else if (ss->cmd == DAS_CMD_MEASURE) {
                das_sum_start(sum_ctx);
                das_sum_start(&ss->stepping_sum_context);

                das_mac_start(&ss->stepping_mac_context);
                das_mac_update(&ss->stepping_mac_context, (uint8_t *)&ss->cmd_id, sizeof(uint64_t));
            }
        } else if (now - ss->info_time > DAS_HEARTBEAT_PERIOD) {
            ss->info_time = now;
            ss->step = DAS_STEP_INFO;
        }

        ss->service_index = 0;
        ss->stepping_service = das_service_table[0];
        ss->service_state.status = DAS_SRV_STATUS_START;
    }

    DAS_LOG_STEPPING(ss);

    if (ss->step != DAS_STEP_IDLE) {
        das_service_state_t *service_state = &ss->service_state;
        void (*publish_handle)(das_session_t *, bool) = NULL;

        memset(&ss->report, 0, sizeof(Message));

        switch (ss->step) {
            case DAS_STEP_INFO: {
                publish_handle = _publish_info;
                if (ss->stepping_service->info) {
                    ret = ss->stepping_service->info(
                              (char*)&(ss->report.data.infData.state),
                               sizeof(ss->report.data.infData.state), service_state);
                    if (ret != DAS_SUCCESS) {
                        DAS_LOG("service %s info error\n", ss->stepping_service->name);
                        goto _out;
                    }
                } else {
                    is_stepping = true;
                }
                break;
            }
            case DAS_STEP_ATTEST: {
                publish_handle = _publish_attest;
                if (ss->stepping_service->attest) {
                    ret = ss->stepping_service->attest(
                              (char*)&(ss->report.data.atsFilData.path),
                               sizeof(ss->report.data.atsFilData.path), sum_ctx, service_state);
                    if (ret != DAS_SUCCESS) {
                        DAS_LOG("service %s attest error, %d\n", ss->stepping_service->name, ret);
                        goto _out;
                    }
                } else {
                    is_stepping = true;
                }
                break;
            }
            case DAS_STEP_MEASURE: {
                publish_handle = _publish_measure;
                if (ss->stepping_service->measure) {
                    ret = ss->stepping_service->measure(sum_ctx, &ss->stepping_mac_context, service_state);
                    if (ret != DAS_SUCCESS) {
                        DAS_LOG("service %s attest error, %d\n", ss->stepping_service->name, ret);
                        goto _out;
                    }
                } else {
                    is_stepping = true;
                }
                break;
            }
            default:
                DAS_LOG("invalid das step status, %d\n", ss->step);
                ret = DAS_ERROR_GENERIC;
                goto _out;
        }

        if (is_stepping) {
             service_state->status = DAS_SRV_STATUS_START;
             ss->service_index++;
        } else {
             if (service_state->status == DAS_SRV_STATUS_PUBLISH) {
                 publish_handle(ss, false);
             } else if (service_state->status == DAS_SRV_STATUS_FINISH) {
                 publish_handle(ss, false);
                 service_state->status = DAS_SRV_STATUS_START;
                 ss->service_index++;
             }
        }

        ss->stepping_service = das_service_table[ss->service_index];
        if (ss->stepping_service == NULL) {
            publish_handle(ss, true);
            ss->step = DAS_STEP_IDLE;
        }
    }

_out:
    if (ret != DAS_SUCCESS) {
        das_sum_finish(sum_ctx, hash);
        das_sum_finish(&ss->stepping_sum_context, hash);

        if (ss->step == DAS_STEP_MEASURE) {
            das_mac_finish(&ss->stepping_mac_context, hash);
        }

        ss->step = DAS_STEP_IDLE;
    }

    return ret;
}

