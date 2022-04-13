#ifndef _USB_HOST_HCD_HAL_H
#define _USB_HOST_HCD_HAL_H

typedef void *usb_hcd_context_t;

typedef struct _usb_host_hcd_op {
    /*
     *  pointer to next host hcd
     */
    struct _usb_host_hcd_op *next;

    /*
     *  hcd driver version info
     */
    char *version;

    /*
     *  hcd driver name
     */
    char *name;

    /**
     *  Initialize hcd driver:
     *   1. Set hcd parameters accordingly.
     *       hcd->type                = EHCI_CONTROLLER;
     *       hcd->available_bandwidth = EHCI_AVAILABLE_BANDWIDTH;
     *       hcd->root_hub_port_num   = EHCI_PORT_NUM;
     *
     *   2. Bring up hardware ready.
     *
     *   3. If success, update hcd status.
     *       hcd->status = HCD_STATUS_OPER;
     *
     * @param[in]  hcd  pointer to hcd structure. See definition in usb_host_types.h
     * @return  0 - success
     *          Other error code see return values in usb_host_defines.h
     */
    int (*hcd_init)(usb_hcd_context_t hcd);

    /**
     * Deinitialize hcd driver
     *
     * @param[in]  hcd  pointer to hcd structure. See definition in usb_host_types.h
     * @return  0 - success, -1 - failure
     */
    int (*hcd_deinit)(usb_hcd_context_t hcd);

    /**
     *  Process commands from upper layer
     *
     *  HCD_DISABLE_CONTROLLER = 0,     // 禁用controller
     *  HCD_GET_PORT_STATUS = 1,        // 获取port状态
     *  HCD_ENABLE_PORT  = 2,           // 使能port
     *  HCD_DISABLE_PORT = 3,           // 禁用port
     *  HCD_POWER_ON_PORT = 4,          // 向port供电
     *  HCD_POWER_OFF_PORT = 5,         // 停止port供电
     *  HCD_RESUME_PORT = 6,            // 恢复port
     *  HCD_SUSPEND_PORT = 7,           // 挂起port
     *  HCD_RESET_PORT = 8,             // 重置port
     *  HCD_GET_FRAME_NUMBER = 9,       // 获取帧号
     *  HCD_SET_FRAME_NUMBER = 10,      // 设置帧号
     *  HCD_TRANSFER_REQUEST = 11,      // 请求发送
     *  HCD_TRANSFER_ABORT  = 12,       // 取消发送
     *  HCD_CREATE_ENDPOINT = 13,       // 创建端点
     *  HCD_DESTROY_ENDPOINT = 14,      // 销毁端点
     *  HCD_RESET_ENDPOINT = 15,        // 重置端点
     *  HCD_PROCESS_DONE_QUEUE = 16,    // 处理已发生的传输
     *
     * @param[in]  hcd   -  pointer to hcd structure.
     * @param[in]  cmd   -  command value. See command definition in usb_host_types.h
     * @param[in]  param -  command parameter, Optional.
     *                      For example, HCD_TRANSFER_REQUEST passes usb_transfer_t along
     *
     * @return  0 - success
     *          Other error code see return values in usb_host_defines.h
     */
    int (*hcd_cmd)(usb_hcd_context_t hcd, unsigned int cmd, void *param);

    /**
     * obtain hcd interrupt status from register
     *
     * @return  1 - status change
     *          0 - no status change
     *          -1 - status error
     */
    int (*check_hcd_intrpt_status)(usb_hcd_context_t hcd);

    /**
     * obtain hcd port status from register
     *
     * @return  1 - port status change
     *          0 - no port status change
     *         -1 - port status error
     */
    int (*check_hcd_port_status)(usb_hcd_context_t hcd, unsigned int port);

    /**
     * Register usb hcd interrupt handle for host to react
     *
     * @param[in]  interrupt callback function pointer.
     *
     * @return  0 - success, -1 - failure
     */
    int (*register_hcd_intrpt_handle_cb)(void (*inrpt_handle)(void));
} usb_host_hcd_op_t;

#endif
