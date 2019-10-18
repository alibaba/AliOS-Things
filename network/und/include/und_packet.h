/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef UND_PACKET_H
#define UND_PACKET_H

#ifdef __cplusplus
extern "C" {
#endif

#define UND_TOPIC_REPORT_REPLY             "/sys/%s/%s/_thing/diag/post_reply"
#define UND_TOPIC_REPORT                   "/sys/%s/%s/_thing/diag/post"
#define UND_METHOD_REPORT                  "thing.log.post"

#define UND_ALINK_VER                      "1.0"
#define UND_REPORT_FMT                     "{\"id\":\"%u\",\"version\":\"%s\",\"params\":%s}"
#define UND_REPORT_PARAM_FMT               "{\"p\":[%s],\"model\":\"format=simple|quantity=batch|time=history\"}"

int und_packet_ctx_init();

int und_packet_ctx_deinit();

int und_build_topic(const char *fmt, char *topic_buf, int topic_buf_len);

int und_build_packet_param(const char *content, char *param, int param_len);

int und_build_packet(const char *ver, const char *param, char *buf, int buf_len);

#ifdef __cplusplus
}
#endif

#endif  /* end of UND_PACKET_H */
