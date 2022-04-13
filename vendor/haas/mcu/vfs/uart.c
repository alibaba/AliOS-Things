#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cmsis_os.h"
#include "cmsis.h"
#include "plat_types.h"

#include "aos/vfs/uart.h"
#include "hal_uart.h"

#include "aos/errno.h"
#include "aos/kernel.h"
#include "aos/vfs.h"
#include "ulog/ulog.h"


#define _CLI_RING_BUFFER_SIZE 512 // mast be 2^n
static __SRAMBSS unsigned char _hal_uart_buf[_CLI_RING_BUFFER_SIZE];
static __SRAMBSS unsigned char _hal_uart2_buf[_CLI_RING_BUFFER_SIZE];

#define UART_DRV_TAG "vfs_uart"


#ifdef LOG
#undef LOG
#define LOG printf
#endif


static int vfs_uart_ioctl(file_t *f, int cmd, unsigned long arg);
static ssize_t vfs_uart_write(file_t *f, const void *buf, size_t len);
static ssize_t vfs_uart_read(file_t *f, void *buf, size_t len);
static int vfs_uart_open(inode_t *node,file_t *file);
static int vfs_uart_poll(file_t *f, int flag, poll_notify_t notify, void *fd, void *arg);
static int vfs_uart_close(file_t *file);

static int _hal_uart_size = 0;
static int _hal_uart2_size = 0;
static int _hal_uart_read_offset = 0;
static int _hal_uart2_read_offset = 0;
static int panic_uart_open = 0;

typedef struct console_sema
{
    osSemaphoreId sema_id;
    osSemaphoreDef_t def;
    ksem_t sem;
} _cli_sema_t;

static _cli_sema_t cli_sema;
static _cli_sema_t cli2_sema_rx;
static _cli_sema_t cli2_sema_tx;
static osSemaphoreId write_sem = NULL;
static osSemaphoreId read_sem = NULL;

typedef struct
{
    aos_mutex_t mutex;
    _cli_sema_t tx_sem;
    _cli_sema_t rx_sem;
    enum HAL_UART_ID_T uart_port;
    struct HAL_UART_CFG_T uart_cfg;
} vfs_uart_obj_t;

file_ops_t uart_fops = {
    .open = vfs_uart_open,
    .read = vfs_uart_read,
    .write = vfs_uart_write,
    .close = vfs_uart_close,
    .poll  = vfs_uart_poll,
	.ioctl = vfs_uart_ioctl,
};

static int uart2_id = -1;
static void _hal_uart_rx_start(void)
{
    struct HAL_DMA_DESC_T dma_desc_rx;
    unsigned int desc_cnt = 1;
    union HAL_UART_IRQ_T mask;

    mask.reg = 0;
    mask.BE = 0;
    mask.FE = 0;
    mask.OE = 0;
    mask.PE = 0;
    mask.RT = 1;

    hal_uart_dma_recv_mask(hal_trace_get_id(), _hal_uart_buf, _CLI_RING_BUFFER_SIZE, &dma_desc_rx, &desc_cnt, &mask);
}

extern void panicNmiInputFilter(uint8_t ch);
static void _uart_dma_rx_handler(uint32_t xfer_size, int dma_error, union HAL_UART_IRQ_T status)
{
    _hal_uart_read_offset = 0;
    _hal_uart_size = xfer_size;

    if (_hal_uart_size == 5) {
        panicNmiInputFilter(_hal_uart_buf[0]);
        panicNmiInputFilter(_hal_uart_buf[1]);
        panicNmiInputFilter(_hal_uart_buf[2]);
        panicNmiInputFilter(_hal_uart_buf[3]);
    } else if (_hal_uart_size == 2) {
        panicNmiInputFilter(_hal_uart_buf[0]);
    }

    // if (_hal_uart_buf[xfer_size-2] == '\r' && _hal_uart_buf[xfer_size-1] == '\n')
    //     xfer_size--;
    // else if (_hal_uart_buf[xfer_size-2] == '\n' && _hal_uart_buf[xfer_size-1] == '\r')
    //     xfer_size--;
    while (xfer_size > 0) {
        xfer_size--;
        osSemaphoreRelease(cli_sema.sema_id);
    }
    _hal_uart_rx_start();
}

static void _hal_uart2_rx_start(void)
{
    struct HAL_DMA_DESC_T dma_desc_rx;
    unsigned int desc_cnt = 1;
    union HAL_UART_IRQ_T mask;

    mask.reg = 0;
    mask.BE = 0;
    mask.FE = 0;
    mask.OE = 0;
    mask.PE = 0;
    mask.RT = 1;

    hal_uart_dma_recv_mask(uart2_id, _hal_uart2_buf, _CLI_RING_BUFFER_SIZE, &dma_desc_rx, &desc_cnt, &mask);
}
static void _uart2_dma_rx_handler(uint32_t xfer_size, int dma_error, union HAL_UART_IRQ_T status)
{
    _hal_uart2_read_offset = 0;
    _hal_uart2_size = xfer_size;

    if (_hal_uart2_size == 5)
    {
        panicNmiInputFilter(_hal_uart2_buf[0]);
        panicNmiInputFilter(_hal_uart2_buf[1]);
        panicNmiInputFilter(_hal_uart2_buf[2]);
        panicNmiInputFilter(_hal_uart2_buf[3]);
    }
    else if (_hal_uart2_size == 2)
    {
        panicNmiInputFilter(_hal_uart2_buf[0]);
    }
    if (_hal_uart2_buf[xfer_size - 2] == '\r' && _hal_uart2_buf[xfer_size - 1] == '\n')
        xfer_size--;
    else if (_hal_uart2_buf[xfer_size - 2] == '\n' && _hal_uart2_buf[xfer_size - 1] == '\r')
        xfer_size--;

    while (xfer_size > 0)
    {
        xfer_size--;
        osSemaphoreRelease(cli2_sema_rx.sema_id);
    }
    _hal_uart2_rx_start();
}

static void _uart2_dma_tx_handler(uint32_t xfer_size, int dma_error)
{
    osSemaphoreRelease(cli2_sema_tx.sema_id);
}

static void uart_irq_handler(enum HAL_UART_ID_T id, union HAL_UART_IRQ_T status)
{
    osStatus ret;

    if (status.TX)
    {
        ret = osSemaphoreRelease(write_sem);
        ASSERT(ret == osOK, "%s: Failed to release write_sem: %d", __func__, ret);
    }
    if (status.RX || status.RT)
    {
        ret = osSemaphoreRelease(read_sem);
        ASSERT(ret == osOK, "%s: Failed to release read_sem: %d", __func__, ret);
    }
}

int vfs_uart_open(inode_t *node, file_t *file)
{
    int ret = 0;
    vfs_uart_obj_t *uart_obj = NULL;
    enum HAL_UART_ID_T uart_port = 0;
    if (!(node && file))
    {
        LOGE(UART_DRV_TAG,"%s invalid node[%p] or file[%p]\n", __func__, node, file);
        return -EINVAL;
    }

    /* just open the /dev/vfs0 node here */
    if ((strncmp(UART_VFS_PATCH_0, node->i_name, strlen(node->i_name))) == 0 )
    {
        uart_port = (enum HAL_UART_ID_T)node->i_arg;
        LOGD(UART_DRV_TAG,"%s port0 %d\n",__func__,uart_port);
    }
    else if ((strncmp(UART_VFS_PATCH_2, node->i_name, strlen(node->i_name))) == 0)
    {
        uart_port = (enum HAL_UART_ID_T)node->i_arg;
        LOGD(UART_DRV_TAG, "%s port2 %d\n",__func__,uart_port);
    }
    else
    {
        LOGE(UART_DRV_TAG, "%s incorrect device node path:%s\n", __func__, node->i_name);
        return -EINVAL;
    }

    LOGD(UART_DRV_TAG, "%s start to open %s\n", __func__, node->i_name);
    uart_obj = (vfs_uart_obj_t *)malloc(sizeof(vfs_uart_obj_t));
    if (!uart_obj)
    {
        LOGE(UART_DRV_TAG, "%s malloc for vfs_sample_obj_t failed\n", __func__);
        return -ENOMEM;
    }
    file->f_arg = (void *)uart_obj;
    uart_obj->uart_port = uart_port;
    uart_obj->uart_cfg.baud = hal_trace_get_baudrate();
    //uart_obj->uart_cfg->mode = MODE_TX_RX;
    uart_obj->uart_cfg.parity = HAL_UART_PARITY_NONE;
    uart_obj->uart_cfg.stop = HAL_UART_STOP_BITS_1;
    uart_obj->uart_cfg.data = HAL_UART_DATA_BITS_8;
    uart_obj->uart_cfg.flow = HAL_UART_FLOW_CONTROL_NONE;
    uart_obj->uart_cfg.tx_level = HAL_UART_FIFO_LEVEL_1_2;
    uart_obj->uart_cfg.rx_level = HAL_UART_FIFO_LEVEL_1_2;
    uart_obj->uart_cfg.dma_rx = true;
    uart_obj->uart_cfg.dma_tx = true;
    uart_obj->uart_cfg.dma_rx_stop_on_err = false;
    if (uart_port == hal_trace_get_id())
    {
        //osSemaphoreDelete(cli_sema.sema_id);
        if(hal_uart_opened(uart_port)){
            hal_uart_reopen(uart_port, &uart_obj->uart_cfg);
        }else{
            hal_uart_open(uart_port, &uart_obj->uart_cfg);
        }
        if (uart_port == HAL_UART_ID_0)
        {
            hal_iomux_set_uart0();
        }
        else if (uart_port == HAL_UART_ID_1)
        {
            hal_iomux_set_uart1();
        }
        else
        {
            hal_iomux_set_uart2();
        }
        hal_uart_irq_set_dma_handler(uart_obj->uart_port, _uart_dma_rx_handler, NULL, NULL);
        _hal_uart_rx_start();
        cli_sema.def.sem = &cli_sema.sem;
        cli_sema.def.name = "cli_uart";
        cli_sema.sema_id = osSemaphoreCreate(&cli_sema.def, 0);
    }
    else
    {
        //uart_obj->uart_cfg.baud = hal_trace_get_baudrate();
        uart2_id = uart_port;
        if(hal_uart_opened(uart2_id)){
            hal_uart_reopen(uart2_id, &uart_obj->uart_cfg);
        }else{
            hal_uart_open(uart2_id, &uart_obj->uart_cfg);
        }
        
        printf("hal_uart_open id %d\n",uart2_id);
        if (uart2_id == HAL_UART_ID_0)
        {
            hal_iomux_set_uart0();
        }
        else if (uart2_id == HAL_UART_ID_1)
        {
            hal_iomux_set_uart1();
        }
        else
        {
            hal_iomux_set_uart2();
        }
        //hal_uart_irq_set_handler(uart_id, uart_irq_handler);
        hal_uart_irq_set_dma_handler(uart2_id, _uart2_dma_rx_handler, _uart2_dma_tx_handler, NULL);
        _hal_uart2_rx_start();
        cli2_sema_rx.def.sem = &cli2_sema_rx.sem;
        cli2_sema_rx.def.name = "cli_uart2_rx";
        cli2_sema_rx.sema_id = osSemaphoreCreate(&cli2_sema_rx.def, 0);
        cli2_sema_tx.def.sem = &cli2_sema_tx.sem;
        cli2_sema_tx.def.name = "cli_uart2_tx";
        cli2_sema_tx.sema_id = osSemaphoreCreate(&cli2_sema_tx.def, 0);
    }
    return 0;
}

//char uart_input_read(uart_dev_t *uart)
//{
    //return hal_uart_blocked_getc(uart->port);
//}
/**
 *
 * @param f
 * @param buf
 * @param len
 * @return bytes number read if operation success; negative error code if operation fails
 *
 */
ssize_t vfs_uart_read(file_t *file,void *buf, size_t len)
{
    vfs_uart_obj_t *uart_obj = (vfs_uart_obj_t *)file->f_arg;
    size_t _len = 0;

    if (uart_obj->uart_port == hal_trace_get_id())
    {
        osSemaphoreWait(cli_sema.sema_id, osWaitForever);
        _len = (_hal_uart_size > UART_MAX_SIZE) ? UART_MAX_SIZE : _hal_uart_size;
        _len = (_len > len) ? len : _len;
        memcpy(buf, _hal_uart_buf + _hal_uart_read_offset, _len);
        _hal_uart_read_offset += _len;
    }
    else
    {
        osSemaphoreWait(cli2_sema_rx.sema_id, osWaitForever);
        _len = (_hal_uart2_size > UART_MAX_SIZE) ? UART_MAX_SIZE : _hal_uart2_size;
        _len = (_len > len) ? len : _len;
        memcpy(buf, _hal_uart2_buf + _hal_uart2_read_offset, _len);
        _hal_uart2_read_offset += _len;
    }
    return _len;
}
/**
 *
 * @param f
 * @param buf
 * @param len
 * @return bytes number written if operation success; negative error code if operation fails
 *
 */
ssize_t vfs_uart_write(file_t *file, const void *buf,size_t len)
{
    vfs_uart_obj_t *uart = (vfs_uart_obj_t *)file->f_arg;
    int trace_id = hal_trace_get_id();
    LOGD(UART_DRV_TAG, "%s trace id %d,write port %d\r",__func__,trace_id,uart->uart_port);
    if (uart->uart_port == trace_id)
    {
        if (panic_uart_open)
        {
            for (int i = 0; i < len; i++)
                hal_uart_blocked_putc(uart->uart_port, *((char *)buf + i));
        }
        else
        {
            hal_trace_output_block(buf, len);
        }
    }
    else
    {
        enum HAL_UART_ID_T uart_id = uart2_id;
        LOGD(UART_DRV_TAG, "%s---%d--buf--%p-len-%d\r\n",__func__,uart_id,buf,len);
        hal_uart_dma_send_sync_cache(uart_id, buf, len, NULL, NULL);
        osSemaphoreWait(cli2_sema_tx.sema_id, osWaitForever);
    }
    return 0;
}

static int get_baud_from_arg(unsigned long arg)
{
    int baud = 0;
    int type = arg & CBAUD;
    switch (type)
    {
    case B115200:
        baud = 115200;
        break;
    case B921600:
        baud = 921600;
        break;
    case B1500000:
        baud = 1500000;
        break;
    default:
        break;
    }
    return baud;
}

static int get_csize_from_arg(unsigned long arg)
{
    int csize;
    int type = arg & CSIZE;
    switch (type)
    {
    case CS5:
        csize = HAL_UART_DATA_BITS_5;
        break;
    case CS6:
        csize = HAL_UART_DATA_BITS_6;
        break;
    case CS7:
        csize = HAL_UART_DATA_BITS_7;
        break;
    case CS8:
        csize = HAL_UART_DATA_BITS_8;
        break;
    default:
        break;
    }
    return csize;
}

static int get_stopb_from_arg(unsigned long arg)
{
    int stopb;
    int type = arg & CSTOPB;
    switch (type)
    {
    case PARODD:
        stopb = HAL_UART_STOP_BITS_1;
        break;
    case PARENB:
        stopb = HAL_UART_STOP_BITS_2;
        break;
    default:
        break;
    }
    return stopb;
}

static int get_cfg_baud(struct HAL_UART_CFG_T *cfg )
{
    int type = cfg->baud;
    int baud = 0;
    switch (type)
    {
    case 115200:
        baud = B115200;
        break;
    case 921600:
        baud = B921600;
        break;
    case 1500000:
        baud = B1500000;
        break;
    default:
        break;
    }
    return baud;
}

static int get_cfg_data(struct HAL_UART_CFG_T *cfg )
{
    int type = cfg->data;
    int data = 0;
    switch (type)
    {
    case HAL_UART_DATA_BITS_5:
        data = CS5;
        break;
    case HAL_UART_DATA_BITS_6:
        data = CS6;
        break;
    case HAL_UART_DATA_BITS_7:
        data = CS7;
        break;
    case HAL_UART_DATA_BITS_8:
        data = CS8;
        break;
    default:
        break;
    }
    return data;
}

static int get_cfg_stop(struct HAL_UART_CFG_T *cfg )
{
    int type = cfg->stop;
    int stopb = 0;
    switch (type)
    {
    case HAL_UART_STOP_BITS_1:
        stopb = PARODD;
        break;
    case HAL_UART_STOP_BITS_2:
        stopb = PARENB;
        break;
    default:
        break;
    }
    return stopb;
}

/**
 *
 * @param file
 * @param cmd
 * @param arg
 * @return 0 if operation success; negative error code if operation fails
 */
int vfs_uart_ioctl(file_t *f,int cmd, unsigned long arg)
{
    vfs_uart_obj_t *uart_obj = (vfs_uart_obj_t *)f->f_arg;
    struct HAL_UART_CFG_T *cfg = &uart_obj->uart_cfg;
    int mode = (int)arg;
    if (cmd == SET_CFLAG)
    {   
        cfg->baud = get_baud_from_arg(mode);
        cfg->data = get_csize_from_arg(mode);
        cfg->stop = get_stopb_from_arg(mode);
        hal_uart_reopen(uart_obj->uart_port, cfg);
    }
    else if (cmd == GET_CFLAG)
    {
        arg &= (~CBAUD);
        arg |= get_cfg_baud(cfg);
        arg &= (~CSIZE);
        arg |= get_cfg_data(cfg);
        arg &= (~CSTOPB);
        arg |= get_cfg_stop(cfg);
    }
    return 0;
}

/**
 *
 * @param f
 * @param flag
 * @param notify
 * @param fd
 * @param arg
 * @return
 *
 */
static int vfs_uart_poll(file_t *file, int flag, poll_notify_t notify, void *fd, void *arg) 
{
	vfs_uart_obj_t *obj = NULL;

	if (!file) {
		LOGE(UART_DRV_TAG, "%s invalid file[%p]\n", __func__, file);
		return -EIO;
	}

	obj = (vfs_uart_obj_t *)file->f_arg;
	if (aos_mutex_lock(&obj->mutex, AOS_WAIT_FOREVER) != 0) {
		LOGE(UART_DRV_TAG, "Failed to lock mutex (%s).", __func__);
		return -1;
	}
	aos_mutex_unlock(&obj->mutex);
}
/**
 *
 * @param file
 * @return 0 if operation success; negative error code if operation fails
 *
 */
int vfs_uart_close(file_t *file)
{
    vfs_uart_obj_t *uart_obj = (vfs_uart_obj_t *)file->f_arg;
    int32_t ret = 0;
    //ENTER_FUNCTION();

    osDelay(100); // make sure last dma tx done!
    hal_uart_close(uart_obj->uart_port);

    if (uart_obj->uart_port == hal_trace_get_id())
    {
        if (cli_sema.sema_id) {
            osSemaphoreDelete(cli_sema.sema_id);
            cli_sema.sema_id = NULL;
        }
    }
    else
    {
        if (cli2_sema_tx.sema_id) {
            osSemaphoreDelete(cli2_sema_tx.sema_id);
            cli2_sema_tx.sema_id = NULL;
        }

        if (cli2_sema_rx.sema_id) {
            osSemaphoreDelete(cli2_sema_rx.sema_id);
            cli2_sema_rx.sema_id = NULL;
        }

        if (write_sem)
        {
            osSemaphoreDelete(write_sem);
            write_sem = NULL;
        }
        if (read_sem)
        {
            osSemaphoreDelete(read_sem);
            read_sem = NULL;
        }
    }
    if (uart_obj) {
		free(uart_obj);
		file->f_arg = NULL;
	}
    //LEAVE_FUNCTION();
    return ret;
}

static int vfs_uart_drv_register(void)
{
    int ret;
    ret = aos_register_driver(UART_VFS_PATCH_0, &uart_fops, HAL_UART_ID_0);
    LOGD(UART_DRV_TAG, "%s %d\r\n",__func__,HAL_UART_ID_0);
#if (CHIP_HAS_UART > 2)
    ret = aos_register_driver(UART_VFS_PATCH_2, &uart_fops, HAL_UART_ID_2);
    LOGD(UART_DRV_TAG, "%s %d\r\n",__func__,HAL_UART_ID_2);
#endif
    return 0;
}
void vfs_uart_drv_init(void)
{
    int ret = 0;
    LOGD(UART_DRV_TAG, "%s\r\n",__func__);
    ret = vfs_uart_drv_register();

    if (ret)
    {
        LOGD(UART_DRV_TAG, "%s success \n", __func__);
    }

    return ret;
}


void _vfs_uart_test(void)
{
    int fd;
    int mode = 0;
    vfs_uart_drv_init();
    char buf[128] = "help\r\n";
    int len = 128;
    char *dev_name = UART_VFS_PATCH_2;
    for (size_t i = 0; i < 15; i++)
    {
        /* code */
        LOGD(UART_DRV_TAG, "%s %d\r\n",__func__,i);
        fd = open(dev_name,O_RDWR);
        //ioctl(fd,GET_CFLAG,&mode);
        LOGD(UART_DRV_TAG, "%s open %d\r\n",__func__,fd);
        //ioctl(fd,SET_CFLAG,mode);
        len = read(fd,buf,128);
        write(fd,buf,len);
        //LOGD(SAMPLE_DRV_TAG,"%s read %s\n",__FUNCTION__,buf);
        close(fd);
        osDelay(1000);
    } 
}