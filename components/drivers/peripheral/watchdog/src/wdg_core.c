#include <aos/list.h>
#include <drv/wdt.h>

#include <devicevfs/devicevfs.h>
#include <drivers/ddkc_log.h>
#include "wdg_core.h"

// suppose there's only one watchdog exist in the system
wdg_controller_t g_system_wdg;


static void wdt_event_cb_fun(csi_wdt_t *wdt, void *arg)
{
    wdg_controller_t *wdg = arg;

    ddkc_err("watchdog%d timeout happens\r\n", wdg->id);

    return;
}


/**
 * start a new watchdog
 *
 * @param[in]  id  watchdog controller's id
 * @param[in]  ms  watchdog's timeout value, in unit of ms
 *
 * @return  handle of the watchdog device, used for later watchdog operations, when open operation success;
            NULL when open operation fails
 */
wdg_dev_handle_t aos_wdg_open (uint32_t id, uint32_t ms) {
    aos_status_t status = 0;
    csi_error_t ret = CSI_OK;
    wdg_controller_t *wdg = NULL;
    wdg_dev_t *user = NULL;

    // if id is not 0, print warning message and force assign it to 0
    if (id != 0) {
        ddkc_warn("id:%d is not valid, force set to 0\r\n", id);
        id = 0;
    }

    // if ms is less than 20, print warning message and force assign it to 20
    if (ms < 20) {
        ddkc_warn("too smaller timeout value, force set to 20ms\r\n", ms);
        ms = 20;
    }

    wdg = &g_system_wdg;

    status = aos_mutex_lock(&wdg->lock, AOS_WAIT_FOREVER);
    if (status) {
        ddkc_warn("lock wdg%d failed, status:%d\r\n", wdg->id, status);
        return NULL;
    }

    // check and make sure watchdog controller is initialized
    if (!wdg->init) {
        ret = csi_wdt_init(&wdg->csi_dev, wdg->id);
        if (ret != CSI_OK) {
            ddkc_err("wdt%d init failed, ret:%d\r\n", wdg->id, ret);
            goto err;
        }
        csi_wdt_attach_callback(&wdg->csi_dev, wdt_event_cb_fun, wdg);
        wdg->init = true;
    }

    user = malloc(sizeof(wdg_dev_t));
    if (!user) {
        ddkc_err("malloc for wdg_dev_t failed\r\n");
        goto err;
    }
    user->id = id;
    user->controller = wdg;
    dlist_init(&user->node);
    user->wdg_dev_handle = AOS_WDG_MAGIC;
    user->started = false;
    user->timeout = ms;

    ret = aos_mutex_new(&user->lock);
    if (ret) {
        ddkc_assert(1, "aos_mutex_new for watchdog user lock failed, ret:%d\r\n", ret);
        free(user);
        user = NULL;

        goto err;
    }

    dlist_add(&user->node, &wdg->user);

    status = aos_mutex_unlock(&wdg->lock);
    if (status) {
        ddkc_warn("unlock wdg%d failed, status:%d\r\n", wdg->id, status);
    }

    return &user->wdg_dev_handle;

err:
    status = aos_mutex_unlock(&wdg->lock);
    if (status) {
        ddkc_warn("unlock wdg%d failed, status:%d\r\n", wdg->id, status);
    }

    return NULL;
}

aos_status_t _aos_wdg_auto_config(wdg_controller_t *wdg)
{
    int32_t ret =  0;
    aos_status_t status = 0;
    bool b = false;
    uint32_t tmo = 0xffffffff;
    dlist_t *head = &wdg->user;
    dlist_t *pos = NULL, *n = NULL;
    wdg_dev_t *w = NULL;

    // go through watchdog's user list and pick one with shortest tmo setting from the started users
    dlist_for_each_safe(pos, n, head)
    {
        w = aos_container_of(pos, wdg_dev_t, node);
        if (w->started && tmo > w->timeout)
        {
            tmo = w->timeout;
            b = true;
        }
    }
    if (b) {
        // reconfig watchdog's timeout value
        /// TODO: need to check the return value
        csi_wdt_stop(&wdg->csi_dev);
        ret += csi_wdt_set_timeout(&wdg->csi_dev, tmo);
        ret += csi_wdt_start(&wdg->csi_dev);
    } else {
        // stop the watchdog
        /// TODO: need to check the return value
        csi_wdt_stop(&wdg->csi_dev);
        wdg->started = false;
    }

    if (ret) {
        ddkc_assert(1, "reconfig watchdog controller failed, ret:%d\r\n", ret);
    }

    return ret;
}

/**
 * close target watchdog handle
 *
 * @param[in]  dev  handle of the watchdog device to be operated, must be the return value of aos_wdg_open
 *
 * @return  0 if operation success
            negative error code if operation fails
 */
aos_status_t aos_wdg_close (wdg_dev_handle_t dev_h) {
    int ret = 0;
    aos_status_t status = 0;
    wdg_dev_t *user = NULL;
    wdg_controller_t *wdg = NULL;

    if (!dev_h || (*dev_h != AOS_WDG_MAGIC)) {
        ddkc_err("invalid dev_h:%p, *dev_h:0x%x", dev_h, dev_h ? *dev_h : 0);
        return -EINVAL;
    }

    user = aos_container_of(dev_h, wdg_dev_t, wdg_dev_handle);
    wdg = (wdg_controller_t *)(user->controller);

    status = aos_mutex_lock(&wdg->lock, AOS_WAIT_FOREVER);
    if (status) {
        /// TODO: how to handle this case? cannot recover from this case for the moment
        ddkc_assert(1, "lock wdg%d failed, status:%d\r\n", wdg->id, status);
        return -EIO;
    }

    dlist_del(&user->node);
    if (dlist_empty(&wdg->user)) {
        // stop watchdog if it is started
        csi_wdt_stop(&wdg->csi_dev);

        csi_wdt_detach_callback(&wdg->csi_dev);

        csi_wdt_uninit(&wdg->csi_dev);
        wdg->tmo = 0;
        wdg->init = false;
    } else {
        // check whether tmo equals to current watchdog or not, reset tmo to shortst in the user list
        if (wdg->tmo == user->timeout)
        {
            _aos_wdg_auto_config(wdg);
        }
    }

    status = aos_mutex_unlock(&wdg->lock);
    if (status) {
        ddkc_warn("unlock wdg%d failed, status:%d\r\n", wdg->id, status);
    }

    return 0;
}

/**
 * start/stop target watchdog
 *
 * @param[in]  dev  handle of the watchdog device to be operated, must be the return value of aos_wdg_open
 *
 * @return  0 if operation success
            negative error code if operation fails
 */
aos_status_t aos_wdg_control (wdg_dev_handle_t dev_h, bool enable) {
    int ret = 0;
    aos_status_t status = 0;
    wdg_dev_t *user = NULL;
    wdg_controller_t *wdg = NULL;

    if (!dev_h || (*dev_h != AOS_WDG_MAGIC)) {
        ddkc_err("invalid dev_h:%p, *dev_h:0x%x", dev_h, dev_h ? *dev_h : 0);
        return -EINVAL;
    }

    user = aos_container_of(dev_h, wdg_dev_t, wdg_dev_handle);
    wdg = (wdg_controller_t *)(user->controller);

    status = aos_mutex_lock(&wdg->lock, AOS_WAIT_FOREVER);
    if (status) {
        ddkc_warn("lock wdg%d failed, status:%d\r\n", wdg->id, status);
        return -EIO;
    }
    user->started = enable;
    // if current watchdog user's tmo is shorter than watchdog controller's setting, reconfig the watchdog
    // if current watchdog user's is disabled, reconfig the watchdog
    if (((wdg->tmo > user->timeout) && (enable)) || (!enable))
    {
        _aos_wdg_auto_config(wdg);
    }

    status = aos_mutex_unlock(&wdg->lock);
    if (status) {
        ddkc_warn("unlock wdg%d failed, status:%d\r\n", wdg->id, status);
    }
    return 0;
}


/**
 * start target watchdog
 *
 * @param[in]  dev  handle of the watchdog device to be operated, must be the return value of aos_wdg_open
 *
 * @return  0 if operation success
            negative error code if operation fails
 */
aos_status_t aos_wdg_start (wdg_dev_handle_t dev_h) {
    return aos_wdg_control(dev_h, true);
}

/**
 * stop target watchdog
 *
 * @param[in]  dev  handle of the watchdog device to be operated, must be the return value of aos_wdg_open
 *
 * @return  0 if operation success
            negative error code if operation fails
 */
aos_status_t aos_wdg_stop (wdg_dev_handle_t dev_h) {
    return aos_wdg_control(dev_h, false);
}

/**
 * set timeout value of target watchdog
 *
 * @param[in]  dev  handle of the watchdog device to be operated, must be the return value of aos_wdg_open
 * @param[in]  clk  watchdog's new timeout value, in unit of ms
 *
 * @return  0 if operation success
            negative error code if operation fails
 */
aos_status_t aos_wdg_timeout_set (wdg_dev_handle_t dev_h,  uint32_t ms) {
    aos_status_t status = 0;
    wdg_dev_t *user = NULL;

    if (!dev_h || (*dev_h != AOS_WDG_MAGIC)) {
        ddkc_err("invalid dev_h:%p, *dev_h:0x%x", dev_h, dev_h ? *dev_h : 0);
        return -EINVAL;
    }

    user = aos_container_of(dev_h, wdg_dev_t, wdg_dev_handle);

    /// TODO: shall protect this procedure
    user->timeout = ms;

    if (user->started)
        status = aos_wdg_control(dev_h, true);

    return status;
}

/**
 * feed target watchdog
 *
 * @param[in]  dev  handle of the watchdog device to be operated, must be the return value of aos_wdg_open
 *
 * @return  0 if operation success
            negative error code if operation fails
 */
aos_status_t aos_wdg_feed (wdg_dev_handle_t dev_h) {
    int ret = 0;
    aos_status_t status = 0;
    wdg_dev_t *user = NULL;
    wdg_controller_t *wdg = NULL;

    if (!dev_h || (*dev_h != AOS_WDG_MAGIC)) {
        ddkc_err("invalid dev_h:%p, *dev_h:0x%x", dev_h, dev_h ? *dev_h : 0);
        return -EINVAL;
    }

    user = aos_container_of(dev_h, wdg_dev_t, wdg_dev_handle);
    wdg = (wdg_controller_t *)(user->controller);

    status = aos_mutex_lock(&wdg->lock, AOS_WAIT_FOREVER);
    if (status) {
        ddkc_warn("lock wdg%d failed, status:%d\r\n", wdg->id, status);
        return -EIO;
    }

    ret = csi_wdt_feed(&wdg->csi_dev);
    if (ret) {
        ddkc_warn("feed watchdog failed, ret:%d\r\n", ret);
        return -EIO;
    }

    status = aos_mutex_unlock(&wdg->lock);
    if (status) {
        ddkc_warn("unlock wdg%d failed, status:%d\r\n", wdg->id, status);
    }
    return 0;
}

uint32_t aos_wdg_get_num (void) {
    return 1;
}


int32_t aos_wdg_init (void) {
    aos_status_t ret = 0;
 
    // init global watchdog controller's variable
    g_system_wdg.id = 0;
    g_system_wdg.tmo = 0;
    g_system_wdg.init = false;
    INIT_AOS_DLIST_HEAD(&g_system_wdg.user);

    // init global i2c controller's lock
    ret = aos_mutex_new(&g_system_wdg.lock);
    if (ret) {
        ddkc_assert(1, "aos_mutex_new for system watchdog lock failed, ret:%d\r\n", ret);
    }

    ddkc_warn("enter %s\r\n", __func__);

    return 0;
}

BUS_DRIVER_ENTRY(aos_wdg_init)
