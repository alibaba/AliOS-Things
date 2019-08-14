/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#include "module_fs.h"
#include <be_jse_module.h>
#include <stdio.h>
#include <string.h>
#include "be_jse_api.h"

static int convert_fs_mode_to_oflag(const char *mode)
{
    if (strcmp(mode, "w") == 0) return (O_WRONLY | O_CREAT | O_TRUNC);
    if (strcmp(mode, "r") == 0) return (O_RDONLY);
    if (strcmp(mode, "w+") == 0) return (O_RDWR | O_CREAT | O_TRUNC);
    if (strcmp(mode, "r+") == 0) return (O_RDWR);
    if (strcmp(mode, "a") == 0) return (O_RDWR | O_CREAT | O_APPEND);
    if (strcmp(mode, "a+") == 0) return (O_RDWR | O_CREAT | O_APPEND);

    return (O_WRONLY | O_CREAT);
}

static int check_fs_is_support()
{
    int ret, fd;
    const char *string = "test if fs mount ok";
    char testfile[64]  = {0};

    snprintf(testfile, sizeof(testfile), "%s/%s", JSE_FS_ROOT_DIR,
             "testfile.txt");
    fd = jse_open(testfile, O_RDWR | O_CREAT | O_TRUNC);
    if (fd <= 0) {
        jse_warn("check_fs_is_support open fail\n\r");
        return 0;
    }

    ret = jse_write(fd, string, strlen(string));
    if (ret <= 0) {
        jse_warn("check_fs_is_support write fail\n\r");
        return 0;
    }

    ret = jse_unlink(testfile);
    if (ret) {
        jse_warn("check_fs_is_support sync fail\n\r");
        return 0;
    }

    jse_close(fd);
    return 1;
}

/*****************************************************************************
*Function:    module_fs_issupport
*Description: js native addon for FILE.issupport()
                          check if the js FILE object is support
*Called by:   js api
*Input:       none
*Output:      1 :support ,0 :not support
*****************************************************************************/
static be_jse_symbol_t *module_fs_issupport()
{
    int ret = 0;

    be_jse_handle_function(0, 0, 0, 0, 0);
    ret = check_fs_is_support();
    return new_int_symbol(ret);
}

/*****************************************************************************
*Function:    module_fs_read
*Description: js native addon for FILE.read(filepath)
                          read the file content
*Called by:   js api
*Input:       filepath : string
*Output:      a String object which the file content is
*****************************************************************************/
static be_jse_symbol_t *module_fs_read()
{
    int ret                         = -1;
    int fd                          = 0;
    int len                         = 0;
    int size                        = 0;
    be_jse_symbol_t *arg0           = NULL;
    be_jse_symbol_t *ret_jse_symbol = NULL;
    char path[BE_FS_PATH_LEN_MAX]   = {0};
    char *p_read_buff               = NULL;

    be_jse_handle_function(0, &arg0, NULL, NULL, NULL);
    if (arg0 == NULL || !symbol_is_string(arg0)) {
        jse_error("%s arg invalid\n\r", __FUNCTION__);
        goto done;
    }

    symbol_to_str(arg0, path, symbol_str_len(arg0));

    fd = jse_open(path, O_RDONLY);
    if (fd < 0) goto done;

    size = be_lseek(fd, 0, SEEK_END);
    be_lseek(fd, 0, SEEK_SET);

    p_read_buff = jse_calloc(1, size + 1);
    if (!p_read_buff) {
        jse_debug("calloc fail");
        goto done;
    }

    len = jse_read(fd, p_read_buff, size);
    if (len > 0) {
        p_read_buff[len] = 0;
    }
    jse_close(fd);

done:
    symbol_unlock(arg0);
    if (len > 0) {
        ret_jse_symbol = new_str_symbol(p_read_buff);
    } else {
        ret_jse_symbol = new_str_symbol("");
    }
    jse_free(p_read_buff);
    return ret_jse_symbol;
}

/*****************************************************************************
*Function:    module_fs_write
*Description: js native addon for FILE.write(filepath,buffer,mode)
                          write the content to the file
*Called by:   js api
*Input:       filepath : string ;
                          buffer: the content buffer which is a String object
                          mode: could be "w","r","w+","r+","a","a+"
*Output:      0 write ok ;other write fail
*****************************************************************************/
static be_jse_symbol_t *module_fs_write()
{
    int ret                       = -1;
    be_jse_symbol_t *arg0         = NULL;
    be_jse_symbol_t *arg1         = NULL;
    be_jse_symbol_t *arg2         = NULL;
    char path[BE_FS_PATH_LEN_MAX] = {0};
    char mode[8]                  = {0};

    be_jse_handle_function(0, &arg0, &arg1, &arg2, NULL);
    if (arg0 == NULL || !symbol_is_string(arg0) || arg1 == NULL ||
        !symbol_is_string(arg1) || arg2 == NULL || !symbol_is_string(arg2) ||
        (symbol_str_len(arg2) > sizeof(mode) - 1)) {
        jse_error("%s arg invalid\n\r", __FUNCTION__);
        goto done;
    }

    int len   = symbol_str_len(arg1);
    char *str = jse_calloc(1, len + 1);
    if (!str) goto done;

    symbol_to_str(arg0, path, symbol_str_len(arg0));
    symbol_to_str(arg1, str, symbol_str_len(arg1));
    symbol_to_str(arg2, mode, symbol_str_len(arg2));

    jse_debug("FS.write(%s,%s,%s);\n\r", path, str, mode);

    int fd = jse_open(path, convert_fs_mode_to_oflag(mode));
    if (fd < 0) {
        jse_error("be_osal_open fail\n\r");
        goto done;
    }

    len = jse_write(fd, str, strlen(str));
    if (len <= 0) {
        jse_error("be_osal_write fail\n\r");
        goto done;
    }

    jse_sync(fd);
    ret = jse_close(fd);

done:
    jse_free(str);
    symbol_unlock(arg0);
    symbol_unlock(arg1);
    symbol_unlock(arg2);
    return new_int_symbol(ret);
}

/*****************************************************************************
*Function:    module_fs_delete
*Description: js native addon for FILE.delete(filepath)
                          delete the file
*Called by:   js api
*Input:       filepath : string
*Output:      0 delete ok ;other delete fail
*****************************************************************************/
static be_jse_symbol_t *module_fs_delete()
{
    be_jse_symbol_t *arg0         = NULL;
    int ret                       = -1;
    char path[BE_FS_PATH_LEN_MAX] = {0};

    be_jse_handle_function(0, &arg0, NULL, NULL, NULL);
    if (arg0 == NULL || !symbol_is_string(arg0)) {
        jse_error("module_fs_delete param invalid\n\r");
        goto done;
    }

    symbol_to_str(arg0, path, symbol_str_len(arg0));
    ret = jse_unlink(path);

done:
    symbol_unlock(arg0);
    return new_int_symbol(ret);
}

static be_jse_symbol_t *module_handle_cb(be_jse_vm_ctx_t *execInfo,
                                         be_jse_symbol_t *var, const char *name)
{
    jse_debug("%s Enter: name=%s\n\r", __FUNCTION__, name);

    if (strcmp(name, "issupport") == 0) return module_fs_issupport();
    if (strcmp(name, "read") == 0) return module_fs_read();
    if (strcmp(name, "delete") == 0) return module_fs_delete();
    if (strcmp(name, "write") == 0) return module_fs_write();

    return BE_JSE_FUNC_UNHANDLED;
}

void module_fs_register(void)
{
    int ret = -1;

    be_jse_module_load("FS", module_handle_cb);
}
