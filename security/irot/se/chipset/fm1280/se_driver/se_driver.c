/*
 * Copyright (C) 2018 Alibaba Group Holding Limited
 */

#include "se_driver.h"
#include "se_cmd.h"
#include "stm32f10x.h"
#include "bitband.h"

//soft delayus when system clk @72Mhz
void delayus(uint32_t cnt)
{
	volatile uint32_t i,j;

	for(i=cnt; i>0; i--){
		for(j=14; j>0; j--);
	}
}

void delayms(uint32_t cnt)
{
    // aos_msleep(cnt);
	delayus(cnt*1000);
}

void init_timeout_ms(uint32_t timeout_ms)
{
	//enable tmr3 clk
	RCC->APB1ENR |= 2;
	TIM3->CR1 = 0;
	//TIM_CLK=72MHz,72000/72M=1ms
	TIM3->ARR = timeout_ms+timeout_ms/5-1;
	TIM3->PSC = 60000-1;
	TIM3->EGR |= 1;
	TIM3->SR = 0;
	//enable timer3
	TIM3->CR1 |= 1<<7 | 1;
}

uint8_t check_timeout_ms(void)
{
	if(TIM3->SR&1){
		TIM3->CR1 = 0;
		return 1;
	}else{
		return 0;
	}
}

/***********************I2C inf******************************/
static uint8_t gfm_I2CAddr;
static StSeFunc *pgfm_SeFunc = NULL;
static StSeI2CDriver *pgfm_I2CDrv = NULL;
static StSeSPIDriver *pgfm_SPIDrv = NULL;



void user_i2c_dev_power_on(void)
{
	//SE_VCC_EN--PE4,SE_IRQ--PD9
	GPIOE->CRL &= 0xFFF0FFFF;
	GPIOE->CRL |= 0x00030000;

	//power on
	PEout(4) = 1;
	delayms(I2C_PWR_ON_DELAY);
}

void user_i2c_dev_power_off(void)
{
	//SE_VCC_EN--PE4,SE_IRQ--PD9
	GPIOE->CRL &= 0xFFF0FFFF;
	GPIOE->CRL |= 0x00030000;

	//power off
	PEout(4) = 0;
	delayms(I2C_PWR_OFF_DELAY);
}

void user_i2c_init(void)
{
	//enable I2C2 clk
	RCC->APB1ENR |= 1<<22;

	//hard_i2c io init
	GPIOB->CRH &= 0xFFFF00FF;
	GPIOB->CRH |= 0x0000EE00;

	//i2c reg init
	I2C2->CR1 |= 1<<15;
	I2C2->CR1 &= ~(1<<15);
	I2C2->CR2 |= 30;
	#ifndef I2C_CLK_400K
	//100Kbps
	I2C2->CCR |= 180;
	#else
	//400K
	I2C2->CCR |= 1<<15 | 34;
	#endif
	I2C2->TRISE = 31;
	I2C2->OAR1 |= 1<<14;

	//enable ack,enable i2c module
	I2C2->CR1 |= 1<<10 | 1;

	delayms(200);
}

void user_i2c_start(void)
{
	uint32_t count;

	//enable ack
	I2C2->CR1 |= 1<<10;
	//start
	I2C2->CR1 |= 1<<8;

	count = CHECK_TIMES_I2C;
	do{
		count--;
		if((I2C2->SR1&1)&&(I2C2->SR2&3))
		{
			break;
		}
	}while(count);
}

void user_i2c_stop(void)
{
	//disable ack
	I2C2->CR1 &= ~(1<<10);
	//stop
	I2C2->CR1 |= 1<<9;
}

uint8_t user_i2c_send_char(uint8_t ch)
{
	uint32_t count;

	I2C2->DR = ch;
	count = CHECK_TIMES_I2C;
	do{
		count--;
		if((I2C2->SR1&0x84)&&(I2C2->SR2&7))
		{
			break;
		}
	}while(count);

	delayus(BYTE_DELAY_I2C);

	if(count)
		return 0;
	else
		return 12;	//timeout err
}

uint8_t user_i2c_recv_char(void)
{
	uint32_t count;

	count = CHECK_TIMES_I2C;
	do{
		count--;
		if((I2C2->SR1&0x40)&&(I2C2->SR2&3))
		{
			break;
		}
	}while(count);

	delayus(BYTE_DELAY_I2C);

    return I2C2->DR;
}

uint8_t user_i2c_send_addr(uint8_t ch)
{
	uint32_t count;

	I2C2->DR = ch;
	count = CHECK_TIMES_I2C;
	do{
		count--;
		if((I2C2->SR1&2)&&(I2C2->SR2&3))
		{
			break;
		}
	}while(count);

	delayus(BYTE_DELAY_I2C);

	if(count){
		PCout(9) = 0;
		return 0;
	}else{
		PCout(9) = 1;
		return 12;	//timeout err
	}
}

StSeI2CDriver gusr_i2c_drv =
{
	SE_ADDR,
	user_i2c_dev_power_on,
	user_i2c_dev_power_off,
	user_i2c_init,
	user_i2c_start,
	user_i2c_stop,
	user_i2c_send_char,
	user_i2c_recv_char,
	user_i2c_send_addr
};

//void fm_i2c_drv_reg(StSeI2CDriver *fm_i2c_drv)
void fm_i2c_drv_reg(void *fm_i2c_drv)
{
	pgfm_I2CDrv = fm_i2c_drv;
}

void fm_i2c_drv_unreg(void)
{
	pgfm_I2CDrv = NULL;
}

void fm_i2c_dev_init(void)
{
	if(pgfm_I2CDrv)
		pgfm_I2CDrv->fm_i2c_init();
}

void fm_i2c_open_device(void)
{
	if(pgfm_I2CDrv)
	{
		gfm_I2CAddr = pgfm_I2CDrv->se_i2c_addr;
		pgfm_I2CDrv->fm_i2c_power_on();
	}
}

void fm_i2c_close_device(void)
{
	if(pgfm_I2CDrv)
	{
		gfm_I2CAddr = 0;
		pgfm_I2CDrv->fm_i2c_power_off();
	}
}

uint8_t fm_i2c_send_frame(uint8_t cmd, uint8_t *sbuf, uint16_t slen)
{
	FM_I2C_HEAD fm_i2c_hd;
	uint16_t i;
	uint8_t bcc;
	uint8_t ret;

	fm_i2c_hd.lenlo = slen+3;
	fm_i2c_hd.lenhi = (slen+3)>>8;
	fm_i2c_hd.nad = 0;
	fm_i2c_hd.flag.cmd = cmd;

	if(!pgfm_I2CDrv)
		return 11;

	pgfm_I2CDrv->fm_i2c_start();

	//send se addr,write slaver
	ret = pgfm_I2CDrv->fm_i2c_send_addr(gfm_I2CAddr);
	if(ret) goto END;

	//send pack head
	for(i=0; i<4; i++)
	{
		ret = pgfm_I2CDrv->fm_i2c_send_char(*(&fm_i2c_hd.lenlo+i));
		if(ret) goto END;
	}

	//calc bcc
	bcc = fm_i2c_hd.lenlo ^ fm_i2c_hd.lenhi ^ fm_i2c_hd.nad ^ fm_i2c_hd.flag.cmd;

	//send data
	for(i=0; i<slen; i++)
	{
		ret = pgfm_I2CDrv->fm_i2c_send_char(*(sbuf+i));
		bcc ^= sbuf[i];
		if(ret) goto END;
	}

	//send bcc
	ret = pgfm_I2CDrv->fm_i2c_send_char(bcc);

	pgfm_I2CDrv->fm_i2c_stop();

	//debug
	#ifdef DEBUG_I2C
	//ls_osa_print("HEAD:%02x,%02x,LRC:%02x\r\n",fm_i2c_hd.lenhi,fm_i2c_hd.lenlo,bcc);
	#endif

	delayms(FRAME_DELAY_I2C);

END:
	return ret;
}

uint8_t fm_i2c_recv_frame(uint8_t *rbuf, uint16_t *rlen)
{
	uint16_t i;
	uint16_t recvLen;
	uint8_t bcc;
	uint8_t ret;
	FM_I2C_HEAD fm_i2c_hd;

	*rlen = 0;

	if(!pgfm_I2CDrv)
		return 11;

	pgfm_I2CDrv->fm_i2c_start();

	//send se addr,read slaver
	ret = pgfm_I2CDrv->fm_i2c_send_addr(gfm_I2CAddr+1);
	if(ret)
	{
		return ret;
	}

	//recv length
	fm_i2c_hd.lenlo = pgfm_I2CDrv->fm_i2c_recv_char();
	fm_i2c_hd.lenhi = pgfm_I2CDrv->fm_i2c_recv_char();

	recvLen  = (fm_i2c_hd.lenhi << 8) + fm_i2c_hd.lenlo;

	if(recvLen < I2C_MIN_LEN || recvLen > I2C_MAX_LEN)
	{
		//ls_osa_print("err_recvLen=%04x\r\n",recvLen);
		*rlen = 0;
		return 13;
	}

	//recv nad and sta
	fm_i2c_hd.nad = pgfm_I2CDrv->fm_i2c_recv_char();
	fm_i2c_hd.flag.sta = pgfm_I2CDrv->fm_i2c_recv_char();

	bcc = fm_i2c_hd.lenlo ^ fm_i2c_hd.lenhi ^ fm_i2c_hd.nad ^ fm_i2c_hd.flag.sta;

	//recv data length
	*rlen = recvLen - 3;

	//recv data
	for(i=0; i<*rlen; i++)
	{
		rbuf[i] = pgfm_I2CDrv->fm_i2c_recv_char();
		bcc ^= rbuf[i];
	}

	//stop
	pgfm_I2CDrv->fm_i2c_stop();

	//recv and check bcc
	bcc ^= pgfm_I2CDrv->fm_i2c_recv_char();

	if(bcc)
	{
		*rlen = 0;
		return 14;
	}

	delayms(FRAME_DELAY_I2C);

	return fm_i2c_hd.flag.sta;
}

uint8_t fm_i2c_get_atr(uint8_t *rbuf, uint16_t *rlen)
{
	uint8_t ret;

	ret = fm_i2c_send_frame(I2C_CMD_GET_ATR, 0, 0);
	if(ret) return ret;

	init_timeout_ms(POLL_TIMEOUT);
	do{
		ret = fm_i2c_recv_frame(rbuf, rlen);
		if(!ret) break;
	}while(!check_timeout_ms());

	return ret;
}

uint8_t fm_i2c_transceive(uint8_t *sbuf, uint16_t slen, uint8_t *rbuf, uint16_t *rlen,
						uint16_t poll_inv, uint32_t poll_timeout)
{
	uint8_t ret;

#ifdef DEBUG_I2C_SEND
	uint16_t i;
	for(i=0; i<slen; i++)
		ls_osa_print("%02x,",sbuf[i]);
	ls_osa_print("\r\n");
#endif

	*rlen = 0;
	ret = fm_i2c_send_frame(I2C_CMD_IBLOCK, sbuf, slen);
#ifdef DEBUG_I2C
	ls_osa_print("fm_i2c_send_frame_ret=%02x\r\n",ret);
#endif
	if(ret) return ret;

	init_timeout_ms(poll_timeout);
	do{
		ret = fm_i2c_recv_frame(rbuf, rlen);

		if(ret == 12){	//timeout
			delayms(poll_inv);
		}else if(ret==13||ret==14){	//bcc err
			fm_i2c_send_frame(I2C_CMD_NAK, 0, 0);
			init_timeout_ms(poll_timeout);
		}else if(ret == 0xF2){	//WTX
			init_timeout_ms(poll_timeout);
		}else
			break;
	}while(!check_timeout_ms());

#ifdef DEBUG_I2C
	for(i=0; i<*rlen; i++)
	    ls_osa_print("%02x,",rbuf[i]);
	ls_osa_print("\r\n");
#endif

	return ret;
}

StSeFunc gfm_se_i2c =
{
	SE_IF_I2C,
	fm_i2c_drv_reg,
	fm_i2c_dev_init,
	fm_i2c_open_device,
	fm_i2c_close_device,
	fm_i2c_get_atr,
	fm_i2c_transceive,
	fm_i2c_drv_unreg
};
/*************************I2C inf end**************************************/
/****************************SPI inf******************************************/
void user_spi_init(void)
{
	//gpio init for SPI2
	//SPI_CS--PB12
	GPIOB->CRH &= 0xFFF0FFFF;
	GPIOB->CRH |= 0x00030000;
	PBout(12) = 1;

	//enable SPI2 clk
	RCC->APB1ENR |= 1<<14;
	//SPI_CLK--PB13,SPI_MISO--PB14,SPI_MOSI--PB15
	GPIOB->CRH &= 0x000FFFFF;
	GPIOB->CRH |= 0xBBB00000;

	//SPI2 init,36MHz/2=18MHz,master,CPOL=1,CPHA=1,
	SPI2->CR1 |= 3<<8 | 1<<6 | 7;
}

void user_spi_dev_power_on(void)
{
	//SE_VCC_EN--PE4,
	GPIOE->CRL &= 0xFFF0FFFF;
	GPIOE->CRL |= 0x00030000;

	//power on
	PEout(4) = 1;
	delayms(SPI_PWR_ON_DELAY);
}

void user_spi_dev_power_off(void)
{
	//SE_VCC_EN--PE4,
	GPIOE->CRL &= 0xFFF0FFFF;
	GPIOE->CRL |= 0x00030000;

	//power off
	PEout(4) = 0;
	delayms(SPI_PWR_OFF_DELAY);
}

void user_spi_chip_select(void)
{
	delayus(SPI_FRAME_DELAY);
	PBout(12) = 0;
	delayus(SPI_SEL_DELAY);
}

void user_spi_chip_deselect(void)
{
	delayus(SPI_DESEL_DELAY);
	PBout(12) = 1;
}

uint8_t user_spi_transceive_char(uint8_t ch)
{
	uint32_t count;

	count = CHECK_TIMES_SPI;
	//tx
	SPI2->DR = ch;
	do{
		count--;
	}while( !(SPI2->SR & 2) && count );

	count = CHECK_TIMES_SPI;
	//rx
	do{
		count--;
	}while( !(SPI2->SR & 1) && count );

	ch = SPI2->DR;

	delayus(SPI_CWT_DELAY);

	return ch;
}

StSeSPIDriver gusr_spi_drv =
{
	SE_SPI_NUM_1,
	user_spi_dev_power_on,
	user_spi_dev_power_off,
	user_spi_init,
	user_spi_chip_select,
	user_spi_chip_deselect,
	user_spi_transceive_char
};

//void fm_spi_drv_reg(StSeSPIDriver *fm_spi_drv)
void fm_spi_drv_reg(void *fm_spi_drv)
{
	pgfm_SPIDrv = fm_spi_drv;
}

void fm_spi_drv_unreg(void)
{
	pgfm_SPIDrv = NULL;
}

void fm_spi_dev_init(void)
{
	if(pgfm_SPIDrv)
		pgfm_SPIDrv->fm_spi_init();
}

void fm_spi_open_device(void)
{
	if(pgfm_SPIDrv)
		pgfm_SPIDrv->fm_spi_power_on();
}

void fm_spi_close_device(void)
{
	if(pgfm_SPIDrv)
		pgfm_SPIDrv->fm_spi_power_off();
}

uint8_t fm_spi_chk_state(uint16_t inv, uint32_t timeout)
{
	uint8_t state = 1;

	init_timeout_ms(timeout);
	//chk state and wait se ok
	do{
		pgfm_SPIDrv->fm_spi_select();
		pgfm_SPIDrv->fm_spi_transceive_char(SPI_CMD_CHK_STATE);
		delayus(SPI_CMD_DELAY);
		state = pgfm_SPIDrv->fm_spi_transceive_char(0xFF);
		pgfm_SPIDrv->fm_spi_deselect();
		if(state) delayms(inv);
		else break;
	}while(!check_timeout_ms());

	return state;
}

uint8_t fm_spi_get_devid(uint8_t *devid, uint16_t *rlen)
{
	uint8_t i;
	uint8_t devid_len = 7;

	*rlen = 0;

	if(!pgfm_SPIDrv)
		return 11;

	pgfm_SPIDrv->fm_spi_select();

	//send spi cmd header
	pgfm_SPIDrv->fm_spi_transceive_char(SPI_CMD_GET_ID);
	delayus(SPI_CMD_DELAY);

	for(i=0; i<devid_len; i++)
	{
		devid[i] = pgfm_SPIDrv->fm_spi_transceive_char(0xFF);
	}

	pgfm_SPIDrv->fm_spi_deselect();

	*rlen = devid_len;

	return 0;
}

uint8_t fm_spi_transceive(uint8_t *sbuf, uint16_t slen, uint8_t *rbuf, uint16_t *rlen,
						uint16_t poll_inv, uint32_t poll_timeout)
{
	uint8_t LenHi;
	uint8_t LenLo;
	uint8_t Lrc = 0;
	uint16_t i;

	//debug
	#ifdef DEBUG_SPI
	for(i=0; i<slen; i++)
		ls_osa_print("%02x,",sbuf[i]);
	ls_osa_print("\r\n");
	#endif

	*rlen = 0;

	if(!pgfm_SPIDrv)
		return 11;

	pgfm_SPIDrv->fm_spi_select();

	//send spi cmd header
	pgfm_SPIDrv->fm_spi_transceive_char(SPI_CMD_SEND_DATA);
	delayus(SPI_CMD_DELAY);

	//send length
	LenHi = slen>>8;
	LenLo = slen;
	pgfm_SPIDrv->fm_spi_transceive_char(LenHi);
	pgfm_SPIDrv->fm_spi_transceive_char(LenLo);

	//calc lrc
	Lrc = 0xFF^LenHi^LenLo;

	//send frame
	for(i=0;i<slen;i++)
	{
		pgfm_SPIDrv->fm_spi_transceive_char(sbuf[i]);
		Lrc ^= sbuf[i];
	}

	//send lrc
	pgfm_SPIDrv->fm_spi_transceive_char(Lrc);

	pgfm_SPIDrv->fm_spi_deselect();

	//debug
	#ifdef DEBUG_SPI
	ls_osa_print("HEAD:%02x,%02x,LRC:%02x\r\n",LenHi,LenLo,Lrc);
	#endif

	//chk se state
	if(fm_spi_chk_state(poll_inv, poll_timeout))
		return 12;

	pgfm_SPIDrv->fm_spi_select();
	//send spi receive cmd header
	pgfm_SPIDrv->fm_spi_transceive_char(SPI_CMD_RECV_DATA);
	delayus(SPI_CMD_DELAY);

	//read 2bytes length
	LenHi = pgfm_SPIDrv->fm_spi_transceive_char(0xFF);
	LenLo = pgfm_SPIDrv->fm_spi_transceive_char(0xFF);
	*rlen = LenHi<<8 | LenLo;

	if(*rlen < SPI_MIN_LEN || *rlen > SPI_MAX_LEN)
	{
		*rlen = 0;
		pgfm_SPIDrv->fm_spi_deselect();
		return 13;
	}

	//calc lrc
	Lrc = 0xFF^LenHi^LenLo;

	//receive frame
	for(i=0; i<*rlen; i++)
	{
		rbuf[i] = pgfm_SPIDrv->fm_spi_transceive_char(0xFF);
		Lrc ^= rbuf[i];
	}

	//receive lrc byte
	Lrc ^= pgfm_SPIDrv->fm_spi_transceive_char(0xFF);

	pgfm_SPIDrv->fm_spi_deselect();

	if(Lrc)
	{
		*rlen = 0;
		return 14;
	}

	return 0;
}

StSeFunc gfm_se_spi =
{
	SE_IF_SPI,
	fm_spi_drv_reg,
	fm_spi_dev_init,
	fm_spi_open_device,
	fm_spi_close_device,
	fm_spi_get_devid,
	fm_spi_transceive,
	fm_spi_drv_unreg
};

/****************************SPI inf end**************************************/
StSeFunc *fm_se_register(StSeFunc *fm_se)
{
	StSeFunc *pfm_SeFunc = NULL;

	pfm_SeFunc = fm_se;
	if(pfm_SeFunc)
	{
		if(pfm_SeFunc->se_name == SE_IF_I2C){
			pfm_SeFunc->fm_driver_register(&gusr_i2c_drv);

		}else if(pfm_SeFunc->se_name == SE_IF_SPI){
			pfm_SeFunc->fm_driver_register(&gusr_spi_drv);

		}
	}

	return pfm_SeFunc;
}

irot_result_t se_open_session(void** handle)
{
	uint8_t result = 0;
	uint8_t rbuf[32];
	uint16_t rlen = 0;
	uint8_t i;

	#ifdef SE_TYPE_I2C
	pgfm_SeFunc = fm_se_register(&gfm_se_i2c);
	#else
	pgfm_SeFunc = fm_se_register(&gfm_se_spi);
	#endif

	if(pgfm_SeFunc == NULL) return IROT_ERROR_GENERIC;

	*handle = pgfm_SeFunc;

	pgfm_SeFunc->fm_open_device();

	pgfm_SeFunc->fm_device_init();

	result = pgfm_SeFunc->fm_dev_power_on(rbuf, &rlen);

	#ifdef SE_DEBUG
	ls_osa_print("PowerOn:ret=%x,rlen=%d\r\n",result,rlen);
	for(i=0;i<rlen;i++)
	{
		ls_osa_print("%02x,",rbuf[i]);
	}
	ls_osa_print("\r\n");
	#endif

	if(result) return IROT_ERROR_TIMEOUT;

    return IROT_SUCCESS;
}

irot_result_t se_transmit(void* handle, const uint8_t* cmd_apdu, const uint32_t cmd_len, uint8_t* rsp_buf, uint32_t* rsp_len)
{
	uint8_t result = 0;
	StSeFunc *pfm_SeFunc = handle;
	uint16_t i;

    memset(rsp_buf, 0x00, *rsp_len);

	if (handle == NULL) {
        ls_osa_print("error: handle is NULL\n");
        return IROT_ERROR_GENERIC;
    }

	#ifdef SE_DEBUG
	ls_osa_print("Send:slen=%d\r\n",cmd_len);
	for(i=0;i<cmd_len;i++)
	{
		ls_osa_print("%02x,",cmd_apdu[i]);
	}
	ls_osa_print("\r\n");
	#endif

	result = pfm_SeFunc->fm_apdu_transceive((uint8_t*)cmd_apdu, cmd_len, rsp_buf, (uint16_t *)rsp_len, POLL_INTERVAL, POLL_TIMEOUT);

	#ifdef SE_DEBUG
	ls_osa_print("Recv:ret=%x,rlen=%d\r\n",result,*rsp_len);
	for(i=0;i<*rsp_len;i++)
	{
		ls_osa_print("%02x,",rsp_buf[i]);
	}
	ls_osa_print("\r\n");
	#endif

	if(result) return IROT_ERROR_TIMEOUT;

    return IROT_SUCCESS;
}

irot_result_t se_close_session(void* handle)
{
	StSeFunc *pfm_SeFunc = handle;

	if(pfm_SeFunc == NULL) return IROT_ERROR_GENERIC;

	pfm_SeFunc->fm_close_device();

	handle = NULL;

	#ifdef SE_DEBUG
	ls_osa_print("PowerOff!\r\n");
	#endif

    return IROT_SUCCESS;
}

