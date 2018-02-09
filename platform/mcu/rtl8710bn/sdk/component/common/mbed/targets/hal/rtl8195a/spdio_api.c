#include <spdio_api.h>

struct spdio_t *g_spdio_priv = NULL;

s8 spdio_rx_done_cb(void *padapter, u8 *data, u16 offset, u16 pktsize, u8 type){
	struct spdio_buf_t *buf = (struct spdio_buf_t *)data;
	struct spdio_t *obj = (struct spdio_t *)padapter;

	if(obj)
		return obj->rx_done_cb(obj, buf, (u8 *)(buf->buf_addr+offset), pktsize, type);
	else
		SPDIO_API_PRINTK("spdio rx done callback function is null!");
	return SUCCESS;
}

s8 spdio_tx_done_cb(void *padapter, u8 *data, u16 offset, u16 pktsize, u8 type){
	struct spdio_t *obj = (struct spdio_t *)padapter;
	struct spdio_buf_t *buf = (struct spdio_buf_t *)data;
	if(obj)
		return obj->tx_done_cb(obj, buf);
	else
		SPDIO_API_PRINTK("spdio tx done callback function is null!");
	return SUCCESS;		
}

s8 spdio_tx(struct spdio_t *obj, struct spdio_buf_t *pbuf){
	return HalSdioRxCallback((u8 *)pbuf, 0, pbuf->buf_size, pbuf->type);
}

void spdio_structinit(struct spdio_t *obj){
	obj->rx_bd_bufsz = SPDIO_RX_BUFSZ_ALIGN(2048+24); //extra 24 bytes for sdio header
	obj->rx_bd_num = 24;
	obj->tx_bd_num = 24;
	obj->priv = NULL;
	obj->rx_buf = NULL;
	obj->rx_done_cb = NULL;
	obj->tx_done_cb = NULL;
}

void spdio_init(struct spdio_t *obj)
{
	if(obj == NULL){
		SPDIO_API_PRINTK("spdio obj is NULL, spdio init failed!");
		return;
	}
	if((obj->rx_bd_num == 0) ||(obj->rx_bd_bufsz == 0) ||  (obj->rx_bd_bufsz%64)
		||(obj->tx_bd_num == 0) ||(obj->tx_bd_num%2)||(obj->rx_buf == NULL))
	{
		SPDIO_API_PRINTK("spdio obj resource isn't correctly inited, spdio init failed!");
		return;
	}
	g_spdio_priv = obj;
	HalSdioInit();
	HalSdioRegisterTxCallback(spdio_rx_done_cb, (void *)obj);
	HalSdioRegisterRxDoneCallback(spdio_tx_done_cb, (void *)obj);
}

void spdio_deinit(struct spdio_t *obj)
{	
	if(obj == NULL){
		SPDIO_API_PRINTK("spdio obj is NULL, spdio deinit failed");
		return;
	}
	HalSdioDeInit();
	g_spdio_priv = NULL;
}
