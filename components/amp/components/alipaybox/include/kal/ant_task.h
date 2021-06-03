#ifndef __ANT_TASK_H__
#define __ANT_TASK_H__
#ifdef __cplusplus
extern "C" {
#endif

#define TASK_ID_INVALID -1
typedef ant_u32 ant_task_t;

typedef void (* ant_task_function_t)(void *);

#ifdef __cplusplus
}
#endif
#endif

