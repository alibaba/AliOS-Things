#include "include.h"
#include "arm_arch.h"

#include "drv_model_pub.h"
#include "drv_model.h"
#include "mem_pub.h"
#include "str_pub.h"
#if CFG_SUPPORT_ALIOS
#include "ll.h"
#endif

static DRV_SDEV_S drv_sdev_tbl[DD_MAX_SDEV] =
{
    {0},
};

static DRV_DEV_S drv_dev_tbl[DD_MAX_DEV] =
{
    {0},
};

UINT32 drv_model_init(void)
{
    os_memset(drv_dev_tbl, 0, sizeof(drv_dev_tbl));
    os_memset(drv_sdev_tbl, 0, sizeof(drv_sdev_tbl));

    return DRV_SUCCESS;
}

UINT32 drv_model_uninit(void)
{
    os_memset(drv_dev_tbl, 0, sizeof(drv_dev_tbl));
    os_memset(drv_sdev_tbl, 0, sizeof(drv_sdev_tbl));

    return DRV_SUCCESS;
}

UINT32 ddev_check_handle(DD_HANDLE handle)
{
    UINT32 magic;
    UINT32 id;

    magic = handle & DD_HANDLE_MAGIC_MASK;
    id    = handle & DD_HANDLE_ID_MASK;
    if((DD_HANDLE_MAGIC_WORD == magic)
            && (id < DD_MAX_DEV))
    {
        return DRV_SUCCESS;
    }
    else
    {
        return DRV_FAILURE;
    }
}

DD_HANDLE ddev_make_handle(UINT32 id)
{
    UINT32 handle = DD_HANDLE_UNVALID;

    if(id >= DD_MAX_DEV)
    {
        goto make_exit;
    }

    handle = id + DD_HANDLE_MAGIC_WORD;

make_exit:
    return handle;
}

UINT32 ddev_get_id_from_handle(DD_HANDLE handle)
{
    UINT32 magic;
    UINT32 id;

    magic = handle & DD_HANDLE_MAGIC_MASK;
    id    = handle & DD_HANDLE_ID_MASK;

    if(magic != DD_HANDLE_MAGIC_WORD)
    {
        return DD_ID_UNVALID;
    }

    return id;
}

DD_HANDLE ddev_open(char *dev_name, UINT32 *status, UINT32 op_flag)
{
    UINT32 i;
    UINT32 handle;
    UINT32 name_len;
    DRV_DEV_PTR dev_ptr;
    DD_OPERATIONS *operation;
    GLOBAL_INT_DECLARATION();

    handle = DD_HANDLE_UNVALID;
    name_len = os_strlen(dev_name);
    if((!(dev_name && status)) || (name_len > DD_MAX_NAME_LEN))
    {
        goto open_exit;
    }

    *status = DRV_FAILURE;

    for(i = 0; i < DD_MAX_DEV; i ++)
    {
        dev_ptr = &drv_dev_tbl[i];
        if((dev_ptr)
                && (0 == os_strncmp(dev_ptr->name, dev_name, name_len)))
        {
            if(DD_STATE_OPENED == dev_ptr->state)
            {
                handle = ddev_make_handle(i);
            }
            else if(DD_STATE_CLOSED == dev_ptr->state)
            {
                handle = ddev_make_handle(i);

                operation = dev_ptr->op;
                if(operation && (operation->open))
                {
                    *status = (operation->open)(op_flag);
                }
                
                GLOBAL_INT_DISABLE();
                dev_ptr->state = DD_STATE_OPENED;
                dev_ptr->use_cnt = 0;
                GLOBAL_INT_RESTORE();
            }
            else
            {
            }

            GLOBAL_INT_DISABLE();
            dev_ptr->use_cnt ++;
            GLOBAL_INT_RESTORE();
            break;
        }
    }

    ASSERT(DD_HANDLE_UNVALID != handle);

open_exit:
    return handle;
}

UINT32 ddev_close(DD_HANDLE handle)
{
    UINT32 id;
    DRV_DEV_PTR dev_ptr;
    DD_OPERATIONS *operation;
    GLOBAL_INT_DECLARATION();

    id = ddev_get_id_from_handle(handle);
    if(DD_ID_UNVALID == id)
    {
        return DRV_FAILURE;
    }

    dev_ptr = &drv_dev_tbl[id];

    GLOBAL_INT_DISABLE();
    dev_ptr->use_cnt --;
    GLOBAL_INT_RESTORE();

    if(0 == dev_ptr->use_cnt)
    {
        operation = dev_ptr->op;
        if(operation && (operation->close))
        {
            (operation->close)();
        }

        ASSERT(dev_ptr);
        
        GLOBAL_INT_DISABLE();
        dev_ptr->state = DD_STATE_CLOSED;
        GLOBAL_INT_RESTORE();
    }

    return DRV_SUCCESS;
}

UINT32 ddev_read(DD_HANDLE handle, char *user_buf, UINT32 count, UINT32 op_flag)
{
    UINT32 id;
    UINT32 status;
    DRV_DEV_PTR dev_ptr;
    DD_OPERATIONS *operation;

    id = ddev_get_id_from_handle(handle);
    if(DD_ID_UNVALID == id)
    {
        return DRV_FAILURE;
    }

    status = DRV_FAILURE;
    dev_ptr = &drv_dev_tbl[id];
    ASSERT(dev_ptr);
    operation = dev_ptr->op;
    if(operation && (operation->read))
    {
        status = (operation->read)(user_buf, count, op_flag);
    }

    return status;
}

UINT32 ddev_write(DD_HANDLE handle, char *user_buf, UINT32 count, UINT32 op_flag)
{
    UINT32 id;
    UINT32 status;
    DRV_DEV_PTR dev_ptr;
    DD_OPERATIONS *operation;

    id = ddev_get_id_from_handle(handle);
    if(DD_ID_UNVALID == id)
    {
        return DRV_FAILURE;
    }

    status = DRV_FAILURE;
    dev_ptr = &drv_dev_tbl[id];
    ASSERT(dev_ptr);
    operation = dev_ptr->op;
    if(operation && (operation->write))
    {
        status = (operation->write)(user_buf, count, op_flag);
    }

    return status;
}

UINT32 ddev_control(DD_HANDLE handle, UINT32 cmd, VOID *param)
{
    UINT32 id;
    UINT32 status;
    DRV_DEV_PTR dev_ptr;
    DD_OPERATIONS *operation;

    id = ddev_get_id_from_handle(handle);
    if(DD_ID_UNVALID == id)
    {
        return DRV_FAILURE;
    }

    status = DRV_FAILURE;
    dev_ptr = &drv_dev_tbl[id];
    ASSERT(dev_ptr);
    operation = dev_ptr->op;
    if(operation && (operation->control))
    {
        status = (operation->control)(cmd, param);
    }

    return status;
}

UINT32 sddev_control(char *dev_name, UINT32 cmd, VOID *param)
{
    UINT32 i;
    UINT32 status;
    UINT32 name_len;
    DRV_SDEV_PTR dev_ptr;
    SDD_OPERATIONS *operation = NULLPTR;

    ASSERT(dev_name);
    status = DRV_FAILURE;
    name_len = os_strlen(dev_name);
    for(i = 0; i < DD_MAX_SDEV; i ++)
    {
        dev_ptr = &drv_sdev_tbl[i];
        if((dev_ptr)
                && (0 == os_strncmp(dev_ptr->name, dev_name, name_len)))
        {
            operation = dev_ptr->op;
            if(operation && (operation->control))
            {
                status = (operation->control)(cmd, param);
            }

            break;
        }
    }

    ASSERT(operation);

    return status;
}

UINT32 ddev_register_dev(char *dev_name, DD_OPERATIONS *optr)
{
    UINT32 i;
    DRV_DEV_PTR dev_ptr;

    if(!(dev_name && optr))
    {
        return DRV_FAILURE;
    }

    dev_ptr = NULLPTR;
    for(i = 0; i < DD_MAX_DEV; i ++)
    {
        dev_ptr = &drv_dev_tbl[i];
        if( (NULLPTR == dev_ptr->name)
                && (DD_STATE_NODEVICE == dev_ptr->state))
        {
            dev_ptr->name  = dev_name;
            dev_ptr->op    = optr;
            dev_ptr->state = DD_STATE_CLOSED;

            break;
        }
    }

    ASSERT(DD_MAX_DEV != i);
    ASSERT(NULLPTR != dev_ptr->op);

    return DRV_SUCCESS;
}

UINT32 sddev_register_dev(char *dev_name, SDD_OPERATIONS *optr)
{
    UINT32 i;
    DRV_SDEV_PTR dev_ptr;

    if(!(dev_name && optr))
    {
        return DRV_FAILURE;
    }

    dev_ptr = NULLPTR;
    for(i = 0; i < DD_MAX_SDEV; i ++)
    {
        dev_ptr = &drv_sdev_tbl[i];
        if( (NULLPTR == dev_ptr->name)
                && (DD_STATE_NODEVICE == dev_ptr->state))
        {
            dev_ptr->name  = dev_name;
            dev_ptr->op    = optr;
            dev_ptr->state = DD_STATE_CLOSED;

            break;
        }
    }

    ASSERT(DD_MAX_SDEV != i);
    ASSERT(NULLPTR != dev_ptr->op);

    return DRV_SUCCESS;
}

UINT32 ddev_unregister_dev(char *dev_name)
{
    UINT32 i;
    UINT32 name_len;
    DRV_DEV_PTR dev_ptr;

    if(!dev_name)
    {
        return DRV_FAILURE;
    }

    dev_ptr = NULLPTR;
    name_len = os_strlen(dev_name);
    if((!dev_name) || (name_len > DD_MAX_NAME_LEN))
    {
        return DRV_FAILURE;
    }

    for(i = 0; i < DD_MAX_SDEV; i ++)
    {
        dev_ptr = &drv_dev_tbl[i];
        if(0 == os_strncmp(dev_ptr->name, dev_name, name_len))
        {
            dev_ptr->name  = 0;
            dev_ptr->op    = 0;
            dev_ptr->state = DD_STATE_NODEVICE;

            dev_ptr        = NULLPTR;

            break;
        }
    }

    ASSERT(NULLPTR == dev_ptr);

    return DRV_SUCCESS;
}

UINT32 sddev_unregister_dev(char *dev_name)
{
    UINT32 i;
    UINT32 name_len;
    DRV_SDEV_PTR dev_ptr;

    if(!dev_name)
    {
        return DRV_FAILURE;
    }

    dev_ptr = NULLPTR;
    name_len = os_strlen(dev_name);
    if((!dev_name) || (name_len > DD_MAX_NAME_LEN))
    {
        return DRV_FAILURE;
    }

    for(i = 0; i < DD_MAX_DEV; i ++)
    {
        dev_ptr = &drv_sdev_tbl[i];
        if(0 == os_strncmp(dev_ptr->name, dev_name, name_len))
        {
            dev_ptr->name  = 0;
            dev_ptr->op    = 0;
            dev_ptr->state = DD_STATE_NODEVICE;

            dev_ptr        = NULLPTR;

            break;
        }
    }

    ASSERT(NULLPTR == dev_ptr);

    return DRV_SUCCESS;
}

// EOF
