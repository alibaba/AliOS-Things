#ifndef _DEV_TIMER_H
#define _DEV_TIMER_H

#ifdef __cplusplus
extern "C"{
#endif /* __cplusplus */

#ifdef __LINUX_UB2__
#include <stdint.h>
#endif

#include "joylink.h"

#define SUPPORT_TIMER 0
#define NOT_SUPPORT_TIMER 1   

#pragma pack(1)

typedef struct {
    int len;
    char cmd[1];   //streamid + value
}JLCmd_t;

typedef struct {
    int len;
    char num;
    JLCmd_t cmds[1];
}JLCmdTrank_t;

typedef struct {
    char len;
    char exp[1];
}JLTimeExp_t;

typedef struct {
    char feedid[32];
    char name[128];
    int id;
    char state;  //0: start 1:stop
    char type;   //0：joylink timer task  1：dev self time task
    JLTimeExp_t time_exp;
    JLCmdTrank_t cmd_trank;
}JLTimeTask_base_t;

typedef struct {
    char feedid[32];
    char name[128];
    int id;
    char state;  //0: start 1:stop
    char type;   //0：joylink timer task  1：dev self time task
    JLTimeExp_t start_time_exp;
    JLTimeExp_t stop_time_exp;
    char repeat_flag;
    char period_type;
    char period[31];
    JLCmdTrank_t start_cmd_trank;
    JLCmdTrank_t stop_cmd_trank;
}JLTimeTask_period_t;

typedef struct {
    char feedid[32];
    char name[128];
    int id;
    char state;  //0: start 1:stop
    char type;   //0：joylink timer task  1：dev self time task
    int action_interval;
    int sleep_interval;
    int times;
    JLCmdTrank_t action_c_trank;
    JLCmdTrank_t sleep_c_trank;
}JLTimeTask_cycle_t;

typedef struct {
    char feedid[32];
    char name[128];
    int id;
    char state;  //0: start 1:stop
    char type;   //0：joylink timer task  1：dev self time task
    int interval;
    JLCmdTrank_t action_c_trank;
    JLCmdTrank_t stop_c_trank;
}JLTimeTask_countdown_t;

typedef union {
    JLTimeTask_base_t base_task;
    JLTimeTask_period_t period_task;
    JLTimeTask_cycle_t cycle_task;
    JLTimeTask_countdown_t contdown_task;
}JLTimeTask_u;

typedef struct {
    int mode;   //0:base 1:period 2:cycle 3:countdown
    JLTimeTask_u type;
}JLTimeTask_t;

typedef struct {
    unsigned int timestamp;
    unsigned int biz_code;    
    unsigned int serial; 
}JLTimeTaskComHead_t;

typedef struct {
    JLTimeTaskComHead_t ch;
    int task_num;
    JLTimeTask_t tasks[1];
}   JLTimeTaskReq_t;

typedef struct {
    JLTimeTaskComHead_t ch;
    int code;
}JLTimeTaskRsp_t;

typedef struct {
    JLTimeTaskComHead_t ch;
    int ids_num;
    int ids[1];
}JLTimeTaskIdsReq_t;

typedef struct {
    JLTimeTaskComHead_t ch;
    int code;
    int ids_num;
    int ids[1];
}JLTimeTaskIdsRsp_t;

typedef struct {
    int id;
    char step;
    int code;
}result_t;

typedef struct {
    JLTimeTaskComHead_t ch;
    int num;
    result_t results[1];
}JLTimeTaskResultReportReq_t;

typedef struct {
    int id;
    char step;
}received_t;

typedef struct {
    JLTimeTaskComHead_t ch;
    int num;
    received_t received[1];
}JLTimeTaskResultReportRsp_t;

typedef struct {
    JLTimeTaskComHead_t ch;
    int code;
    int id;
}JLTimeTaskNewReportRsp_t;

#pragma pack()

typedef enum{
    TIME_TASK_ST_START = 0,
    TIME_TASK_ST_STOP
}E_TimeTaskState;

typedef enum{
    TIME_TASK_TYPE_JOYLINK = 0,
    TIME_TASK_TYPE_DEV_SELF
}E_TimeTaskType;

#define BASE_MODE 0
#define PERIOD_MODE 1
#define CYCLE_MODE 2
#define COUNTDOWN_MODE 3

int joylink_proc_time_task(char *src, int src_len);

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif
