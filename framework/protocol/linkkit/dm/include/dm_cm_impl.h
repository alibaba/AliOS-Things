#ifndef CM_CM_IMPL_H
#define CM_CM_IMPL_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define DM_CM_IMPL_CLASS get_dm_cm_impl_class()

typedef struct {
    const void* _;
    int         cm_inited;
} dm_cm_impl_t;

extern const void* get_dm_cm_impl_class();

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* CM_CM_IMPL_H */
