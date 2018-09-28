#ifndef LINKEDGE_WS_H
#define LINKEDGE_WS_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

void websocket_local_link();
void websocket_loop();
int websocket_local_offline();
int websocket_report_event(const char * event, const char * param);
int websocket_report_prop(const char * prop, const char * value);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
