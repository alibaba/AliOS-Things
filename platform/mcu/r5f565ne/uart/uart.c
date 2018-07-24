#include "rx_platform.h"
#include "uart\uart.h"

/* Defines for SCI support */
#include "r_sci_rx_private.h"

/* Include specifics for chosen MCU.  */
#include "r_sci_rx_platform.h"


_uart_drv uart_drv[13];

void SCI_PUT_CHAR(sci_hdl_t const hdl,long ch)
{
	char ca;
	ca = (uint8_t )(ch);
	while (0 == hdl->rom->regs->SSR.BIT.TEND);
	hdl->rom->regs->TDR = ca;
}


/**
 * Initialises a UART interface
 *
 *
 * @param[in]  uart  the interface which should be initialised
 *
 * @return  0 : on success, EIO : if an error occurred with any step
 */
int32_t hal_uart_init(uart_dev_t *uart)
{
	uint8_t chan;
	sci_cfg_t p_cfg;
    int32_t ret = -1;
    _uart_drv *pdrv;
    if(NULL!=uart){
    	chan = (*uart).port;
    	pdrv = &(uart_drv[chan]);
    	aos_sem_new(&pdrv->rx_semphr, 0);
    	
    	p_cfg.async.baud_rate = (*uart).config.baud_rate;      // ie 9600, 19200, 115200
    	
    	p_cfg.async.clk_src			= SCI_CLK_INT;        // use SCI_CLK_INT/EXT8X/EXT16X
    	
    	switch((*uart).config.data_width)
    	{
    		case DATA_WIDTH_5BIT:
    			p_cfg.async.data_size = 5;
    			break;
    			
    		case DATA_WIDTH_6BIT:
    			p_cfg.async.data_size = 6;
    			break;
    			
    		case DATA_WIDTH_7BIT:
    			p_cfg.async.data_size = SCI_DATA_7BIT;
    			break;
    			
    		case DATA_WIDTH_8BIT:
    			p_cfg.async.data_size = SCI_DATA_8BIT;
    			break;
    			
    		case DATA_WIDTH_9BIT:
    			p_cfg.async.data_size = 9;
    			break;
    			
    		default:
    			break;
    	}
    	
    	if((*uart).config.parity==NO_PARITY){
    		p_cfg.async.parity_en 	= SCI_PARITY_OFF;      	// use SCI_PARITY_ON/OFF
    		p_cfg.async.parity_type	= SCI_ODD_PARITY;    	// use SCI_ODD/EVEN_PARITY
    	}
    	else if((*uart).config.parity==ODD_PARITY){
    		p_cfg.async.parity_en 	= SCI_PARITY_ON;      	// use SCI_PARITY_ON/OFF
    		p_cfg.async.parity_type	= SCI_ODD_PARITY;    	// use SCI_ODD/EVEN_PARITY
    	}
    	else if((*uart).config.parity==EVEN_PARITY){
    		p_cfg.async.parity_en 	= SCI_PARITY_ON;      	// use SCI_PARITY_ON/OFF
    		p_cfg.async.parity_type	= SCI_EVEN_PARITY;    	// use SCI_ODD/EVEN_PARITY
    	}
    	
    	
    	if((*uart).config.stop_bits==STOP_BITS_1){
    		p_cfg.async.stop_bits		=SCI_STOPBITS_1;	// use SCI_STOPBITS_1/2
    	}
    	else if((*uart).config.stop_bits==STOP_BITS_2){
    		p_cfg.async.stop_bits		=SCI_STOPBITS_2;	// use SCI_STOPBITS_1/2
    	}
    	
    	p_cfg.async.int_priority	= 2;   					// interrupt priority; 1=low, 15=high
    	
		ret = R_SCI_Open(chan,SCI_MODE_ASYNC,&p_cfg,NULL,&(uart_drv[chan].hdl));
	}
	
	return ret;
}


/**
 * Transmit data on a UART interface
 *
 * @param[in]  uart  the UART interface
 * @param[in]  data  pointer to the start of data
 * @param[in]  size  number of bytes to transmit
 *
 * @return  0 : on success, EIO : if an error occurred with any step
 */
int32_t hal_uart_send(uart_dev_t *uart, const void *data, uint32_t size, uint32_t timeout)
{
	int32_t		ret = -1;
	uint8_t		*p_src;
	uint16_t	length;
	uint8_t		chan;
	
	if(NULL!=uart && NULL!=data && 0<size){
		chan = (*uart).port;

		p_src = data;
		length = (uint16_t)size;


		R_SCI_Send(uart_drv[chan].hdl,p_src,length);

//	    for (int i = 0; i < length; i++) {
//	    	SCI_PUT_CHAR(uart_drv[chan].hdl, *p_src++);
//	    }



	    return 0;

		
		}
	
}

/**
 * Receive data on a UART interface
 *
 * @param[in]   uart         the UART interface
 * @param[out]  data         pointer to the buffer which will store incoming data
 * @param[in]   expect_size  number of bytes to receive
 * @param[out]  recv_size    number of bytes received
 * @param[in]   timeout      timeout in milisecond
 *
 * @return  0 : on success, EIO : if an error occurred with any step
 */
int32_t hal_uart_recv_II(uart_dev_t *uart, void *data, uint32_t expect_size,
					  uint32_t *recv_size, uint32_t timeout)
{
	uint8_t		*p_dst;
	uint16_t 	length;
	uint8_t		chan;
	int32_t 	ret = -1;
	_uart_drv *pdrv;
	uint32_t start_time, expired_time;
	start_time = aos_now_ms();
	expired_time = 0;
	
	if(NULL!=uart && NULL!=data && 0<expect_size){
		chan = (*uart).port;
		pdrv = &(uart_drv[chan]);
		p_dst = (uint8_t*)data;
		length = (uint16_t)expect_size;
		*recv_size = 0;

		for(;;)
		{
			ret = R_SCI_Receive(uart_drv[chan].hdl,p_dst,1);
			if (ret == 0)
			{
				(*recv_size)+=1;
				p_dst++;
				if(*recv_size>=length)
					{

						break;
					}

			}
			else
			{
				if(*recv_size != 0)
				{
					return 0;
				}

			}
			if ( aos_sem_wait( &pdrv->rx_semphr, timeout - expired_time) != 0)
			{
				*recv_size = 0;
				return -1;
			}
				if (chan==2)
				{
					__nop();
				}

	        expired_time = aos_now_ms() - start_time;
	        if(expired_time > timeout)
	        {
	            return -1;
	        }
		}
	}

	return ret;
}

/**
 * Deinitialises a UART interface
 *
 * @param[in]  uart  the interface which should be deinitialised
 *
 * @return  0 : on success, EIO : if an error occurred with any step
 */
int32_t hal_uart_finalize(uart_dev_t *uart)
{
	return(R_SCI_Close(uart_drv[(*uart).port].hdl));
}


