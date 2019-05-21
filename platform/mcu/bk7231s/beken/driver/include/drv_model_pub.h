#ifndef _DRV_MODEL_PUB_H_
#define _DRV_MODEL_PUB_H_

#define DRV_FAILURE                  ((UINT32)-5)
#define DRV_SUCCESS                  (0)

#define DD_HANDLE_MAGIC_WORD         (0xA5A50000)
#define DD_HANDLE_MAGIC_MASK         (0xFFFF0000)
#define DD_HANDLE_ID_MASK            (0x0000FFFF)

#define DD_HANDLE_UNVALID            ((UINT32)-1)
#define DD_ID_UNVALID                ((UINT32)-1)

typedef UINT32                       DD_HANDLE;

typedef struct _dd_operations_
{
    UINT32 (*open) (UINT32 op_flag);
    UINT32 (*close) (void);
    UINT32 (*read) (char *user_buf, UINT32 count, UINT32 op_flag);
    UINT32 (*write) (char *user_buf, UINT32 count, UINT32 op_flag);
    UINT32 (*control) (UINT32 cmd, void *parm);
} DD_OPERATIONS;

typedef struct _sdd_operations_
{
    UINT32 (*control) (UINT32 cmd, void *parm);
} SDD_OPERATIONS;

typedef enum _DD_OPEN_METHOD_
{
    DD_OPEN_METHOD_ONE_TIME = 0,	// open one time only
    DD_OPEN_METHOD_MUTI_TIME	    // open multi times
} DD_OPEN_METHOD;

/*******************************************************************************
* Function Declarations
*******************************************************************************/
extern UINT32 drv_model_init(void);
extern UINT32 drv_model_uninit(void);
extern DD_HANDLE ddev_open(char *dev_name, UINT32 *status, UINT32 op_flag);
extern UINT32 ddev_close(DD_HANDLE handle);
extern UINT32 ddev_read(DD_HANDLE handle, char *user_buf , UINT32 count, UINT32 op_flag);
extern UINT32 ddev_write(DD_HANDLE handle, char *user_buf , UINT32 count, UINT32 op_flag);
extern UINT32 ddev_control(DD_HANDLE handle, UINT32 cmd, VOID *param);
extern UINT32 sddev_control(char *dev_name, UINT32 cmd, VOID *param);
extern UINT32 ddev_register_dev(char *dev_name, DD_OPERATIONS *optr);
extern UINT32 sddev_register_dev(char *dev_name, SDD_OPERATIONS *optr);
extern UINT32 ddev_unregister_dev(char *dev_name);
extern UINT32 sddev_unregister_dev(char *dev_name);

#endif //_DRV_MODEL_PUB_H_

