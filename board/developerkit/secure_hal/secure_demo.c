#include "stm32l4xx_hal.h"
#include <k_api.h>
#include "secure_demo.h"

SC_ATR g_card_atr;
uint8_t SC_ATR_Table[40] = {0};
static __IO uint8_t SCData = 0;
static uint32_t F_Table[16] = {372, 372, 558, 744, 1116, 1488, 1860, 0,
                               0, 512, 768, 1024, 1536, 2048, 0, 0};
static uint32_t D_Table[8] = {0, 1, 2, 4, 8, 16, 32, 64};
extern SMARTCARD_HandleTypeDef hsmartcard2;

void SC_Reset(int ResetState)
{
  if(ResetState)
  	hal_gpio_output_high(&brd_gpio_table[GPIO_SECURE_RST]);
  else
  	hal_gpio_output_low(&brd_gpio_table[GPIO_SECURE_RST]);
}

static int iso7816_get_atr(unsigned char *atr)
{
    unsigned char index = 0;
    unsigned char i = 0;
    unsigned char temp = 0;
    unsigned char h_len;
    unsigned char ret;
  
    /* get TS */
     ret = HAL_SMARTCARD_Receive(&hsmartcard2, &atr[i++], 1, SC_RECEIVE_TIMEOUT);
     //ret = HAL_SMARTCARD_Receive_IT(&hsmartcard2, &atr[i++], 1);
     if(ret != HAL_OK){
		LOG("HAL_SMARTCARD_Receive error\n");
		return ret;
     }
     //LOG("get Ts ok\n");
    /* get T0 */
     ret = HAL_SMARTCARD_Receive(&hsmartcard2, &atr[i], 1, SC_RECEIVE_TIMEOUT);
     //ret = HAL_SMARTCARD_Receive_IT(&hsmartcard2, &atr[i], 1);
     if(ret != HAL_OK){
		LOG("HAL_SMARTCARD_Receive error\n");
		return ret;
     }

    temp = atr[i] >> 4;
    h_len = atr[i] & 0xf;
    index = i;
    i++;
    while (temp) {
        if (temp & 0x1) {
            /* TAi */
            ret = HAL_SMARTCARD_Receive(&hsmartcard2, &atr[i++], 1, SC_RECEIVE_TIMEOUT);
            //ret = HAL_SMARTCARD_Receive_IT(&hsmartcard2, &atr[i++], 1);
	     if(ret != HAL_OK){
			LOG("HAL_SMARTCARD_Receive error\n");
			return ret;
	     }
        }
        if (temp & 0x2) {
            /* TBi */
            ret = HAL_SMARTCARD_Receive(&hsmartcard2, &atr[i++], 1, SC_RECEIVE_TIMEOUT);
             //ret = HAL_SMARTCARD_Receive_IT(&hsmartcard2, &atr[i++], 1);
	     if(ret != HAL_OK){
			LOG("HAL_SMARTCARD_Receive error\n");
			return ret;
	     }
        }
        if (temp & 0x4) {
            /* TCi */
            ret = HAL_SMARTCARD_Receive(&hsmartcard2, &atr[i++], 1, SC_RECEIVE_TIMEOUT);
	     //ret = HAL_SMARTCARD_Receive_IT(&hsmartcard2, &atr[i++], 1);	
	     if(ret != HAL_OK){
			LOG("HAL_SMARTCARD_Receive error\n");
			return ret;
	     }
        }
        if (temp & 0x8) {
            /* TDi */
            ret = HAL_SMARTCARD_Receive(&hsmartcard2, &atr[i], 1, SC_RECEIVE_TIMEOUT);
             //ret = HAL_SMARTCARD_Receive_IT(&hsmartcard2, &atr[i++], 1);
	     if(ret != HAL_OK){
			LOG("HAL_SMARTCARD_Receive error\n");
			return ret;
	     }
            temp = atr[i++] >> 4;
            continue;
        }
        temp = 0;
    }
    /* get historical bytes */
    while (h_len) {
        h_len--;
         ret = HAL_SMARTCARD_Receive(&hsmartcard2, &atr[i++], 1, SC_RECEIVE_TIMEOUT);
           // ret = HAL_SMARTCARD_Receive_IT(&hsmartcard2, &atr[i++], 1);
	     if(ret != HAL_OK){
			LOG("HAL_SMARTCARD_Receive error\n");
			return ret;
	     }
    }

#ifdef CONFIG_SE_GEMALTO_MTF008
    /* the last byte is checksum byte, it is existed in Gemalto's SE */
    ret = HAL_SMARTCARD_Receive(&hsmartcard2, &atr[i++], 1, SC_RECEIVE_TIMEOUT);
     //ret = HAL_SMARTCARD_Receive_IT(&hsmartcard2, &atr[i++], 1);
     if(ret != HAL_OK){
		LOG("HAL_SMARTCARD_Receive error\n");
		return ret;
     }
#endif

    return i;
}

static uint8_t iso7816_decode_atr(uint8_t *string)
{
    uint32_t i = 0;
    uint32_t flag = 0;
    uint32_t buf = 0;
    uint32_t protocol = 0;

    g_card_atr.TS = string[0];  /* Initial character */
    g_card_atr.T0 = string[1];  /* Format character */

    g_card_atr.Hlength = g_card_atr.T0 & (uint8_t)0x0F;

    if ((g_card_atr.T0 & (uint8_t)0x80) == 0x80) {
        flag = 1;
    }

    for (i = 0; i < 4; i++) {
        g_card_atr.Tlength = g_card_atr.Tlength +
                    (((g_card_atr.T0 & (uint8_t)0xF0) >> (4 + i)) & (uint8_t)0x1);
    }

    for (i = 0; i < g_card_atr.Tlength; i++) {
        g_card_atr.T[i] = string[i + 2];
    }

    protocol = g_card_atr.T[g_card_atr.Tlength - 1] & (uint8_t)0x0F;

    while (flag) {
        if ((g_card_atr.T[g_card_atr.Tlength - 1] & (uint8_t)0x80) == 0x80) {
            flag = 1;
        }
        else {
            flag = 0;
        }

        buf = g_card_atr.Tlength;
        g_card_atr.Tlength = 0;

        for (i = 0; i < 4; i++) {
            g_card_atr.Tlength = g_card_atr.Tlength + (((g_card_atr.T[buf - 1] & (uint8_t)0xF0) >> (4 + i)) & (uint8_t)0x1);
        }

        for (i = 0; i < g_card_atr.Tlength; i++) {
            g_card_atr.T[buf + i] = string[i + 2 + buf];
        }
        g_card_atr.Tlength += (uint8_t)buf;
    }

    for (i = 0; i < g_card_atr.Hlength; i++) {
        g_card_atr.H[i] = string[i + 2 + g_card_atr.Tlength];
    }

    return (uint8_t)protocol;
}

int t0_send_command_recv_status(SC_ADPU_Commands *apdu,
                                SC_ADPU_Response *response)
{
	int i = 0;
	int ret = 0;
	uint8_t locData;

	memset(response->data, 0, LC_MAX);
	response->SW1 = 0;
	response->SW2 = 0;

	/* send apdu header */
	ret = HAL_SMARTCARD_Transmit(&hsmartcard2, &(apdu->header.CLA), 1, SC_RECEIVE_TIMEOUT);
	if(ret != HAL_OK){
		LOG("HAL_SMARTCARD_Transmit CLA error\n");
		return ret;
	}
	ret = HAL_SMARTCARD_Transmit(&hsmartcard2, &(apdu->header.INS), 1, SC_RECEIVE_TIMEOUT);
	if(ret != HAL_OK){
		LOG("HAL_SMARTCARD_Transmit INS error\n");
		return ret;
	}
	ret = HAL_SMARTCARD_Transmit(&hsmartcard2, &(apdu->header.P1), 1, SC_RECEIVE_TIMEOUT);
	if(ret != HAL_OK){
		LOG("HAL_SMARTCARD_Transmit P1 error\n");
		return ret;
	}
	ret = HAL_SMARTCARD_Transmit(&hsmartcard2, &(apdu->header.P2), 1, SC_RECEIVE_TIMEOUT);
	if(ret != HAL_OK){
		LOG("HAL_SMARTCARD_Transmit P2 error\n");
		return ret;
	}
	
	if(apdu->body.LC > 0){
		//LOG("HAL_SMARTCARD_Transmit LC...\n");
		ret = HAL_SMARTCARD_Transmit(&hsmartcard2, &(apdu->body.LC), 1, SC_RECEIVE_TIMEOUT);
		if(ret != HAL_OK){
			LOG("HAL_SMARTCARD_Transmit LC error\n");
			return ret;
		}
	}
	else if (apdu->body.LE > 0){
		//LOG("HAL_SMARTCARD_Transmit LE...\n");
		ret = HAL_SMARTCARD_Transmit(&hsmartcard2, &(apdu->body.LE), 1, SC_RECEIVE_TIMEOUT);
		if(ret != HAL_OK){
			LOG("HAL_SMARTCARD_Transmit LE error\n");
			return ret;
		}
	}

	if(HAL_SMARTCARD_Receive(&hsmartcard2, &locData, 1, SC_RECEIVE_TIMEOUT) == HAL_OK){
		//LOG("SE:locData = 0x%x\n", locData);
		if(((locData & 0xF0) == 0x60) || ((locData & 0xF0) == 0x90))
		{
			/* SW1 received */
			response->SW1 = locData;

			if(HAL_SMARTCARD_Receive(&hsmartcard2, &locData, 1, SC_RECEIVE_TIMEOUT) == HAL_OK)
			{
				/* SW2 received */
				response->SW2 = locData;
			}
		}
		else if (((locData & 0xFE) == (((uint8_t)~(apdu->header.INS)) & 0xFE))
			||((locData & 0xFE) == (apdu->header.INS & 0xFE)))
		{
			response->data[0] = locData;/* ACK received */
		}
	}
	else
		return HAL_ERROR;

	/* if no status bytes received */
	if (response->SW1 == 0) {
		if (apdu->body.LC > 0) {
			//send data
			ret =  HAL_SMARTCARD_Transmit(&hsmartcard2,apdu->body.Data, apdu->body.LC, SC_RECEIVE_TIMEOUT);
			if(ret != HAL_OK){
				LOG("HAL_SMARTCARD_Transmit LC Data error\n");
				return ret;
			}	
		} 
		else if (apdu->body.LE > 0) {
			//receive data
			ret = HAL_SMARTCARD_Receive(&hsmartcard2, response->data, apdu->body.LE + 2, SC_RECEIVE_TIMEOUT);
			if (HAL_OK == ret)
			{
				response->SW1 = response->data[apdu->body.LE];
				response->SW2 = response->data[apdu->body.LE + 1];

				//for (i = 0; i < apdu->body.LE + 2; i++)
				//	LOG("response->data[%d] = 0x%x\n", i, response->data[i]);
			}else{
				LOG("receive data error, ret = 0x%x\n", ret);
			}

			return ret;
		} 
		else {
			return HAL_ERROR;
		}
	}

	/* wait for SW1 SW2*/
	if (HAL_SMARTCARD_Receive(&hsmartcard2, &locData, 1, SC_RECEIVE_TIMEOUT) != 0) {
	return HAL_ERROR;
	}
	else{
	//LOG("SW1 = 0x%x\n", locData);
	response->SW1 = locData;
	}

	if (HAL_SMARTCARD_Receive(&hsmartcard2, &locData, 1, SC_RECEIVE_TIMEOUT) != 0) {
	return HAL_ERROR;
	}
	else{
	//LOG("SW2 = 0x%x\n", locData);
	response->SW2 = locData;
	}

	return HAL_OK;
}


int SC_AnswerReq(void)
{
	int len = 0;

	memset(SC_ATR_Table, 0, MAX_ATR_LENGTH);
	memset(&g_card_atr, 0, sizeof(g_card_atr));
	
	krhino_task_sleep(RHINO_CONFIG_TICKS_PER_SECOND/100);
	SC_Reset(GPIO_PIN_SET);
	
	//LOG("hal_smartcard_Init OK\n");
	len = iso7816_get_atr(SC_ATR_Table);
	if (len <= 0x0 || (SC_ATR_Table[0] != ATR_BYTE0)) {
            /* get error ATR */
	     LOG("iso7816_get_atr error\n");
            return HAL_ERROR;
        }
	//LOG("iso7816_get_atr OK\n");
	iso7816_decode_atr(SC_ATR_Table);
	 
	 return HAL_OK;
}

void SC_Stop(void)
{
  SC_Reset(GPIO_PIN_RESET);
}

static int string_to_apdu(SC_ADPU_Commands *apdu,
                          unsigned char *string,
                          int length)
{
    int i = 0;

    apdu->header.CLA = string[0];
    apdu->header.INS = string[1];
    apdu->header.P1 = string[2];
    apdu->header.P2 = string[3];
	  apdu->header.P3 = string[4];

    if (length > 5) {
        apdu->body.LC = string[4];
        for (i = 0; i < apdu->body.LC; i++) {
            apdu->body.Data[i] = string[5+i];
        }
    }
    else {
        apdu->body.LE = string[4];
    }

    return 0;
}

static int response_to_string(unsigned char *string,
                              SC_ADPU_Response *response,
                              int data_length)
{
    int i = 0;
    for (i = 0; i < data_length; i++) {
        string[i] = response->data[i];
    }
    string[i++] = response->SW1;
    string[i++] = response->SW2;

    return i;
}

int DeviceOpen(void **handle)
{
    uint8_t ret = HAL_OK;
	
    *handle = "device open";

    if (SC_AnswerReq() != HAL_OK) {
        /* get error ATR */
	 LOG("SC_Start error\n");
        return HAL_ERROR;
    }
    //exchange baud
    krhino_task_sleep(RHINO_CONFIG_TICKS_PER_SECOND/10);
    return HAL_OK;
}

int DeviceTransmit(void *handle, unsigned char *input, int input_len,
                    unsigned char *output, int *output_len)
{
    SC_ADPU_Commands apdu;
    SC_ADPU_Response response;
    int ret = 0;

    memset(&apdu, 0, sizeof(SC_ADPU_Commands));
    string_to_apdu(&apdu, input, input_len);

    ret = t0_send_command_recv_status(&apdu, &response);
    if (ret != HAL_OK) {
	 LOG("T0 cmd failed\n");
        return HAL_ERROR;
    }

    *output_len = response_to_string(output, &response, apdu.body.LE);
    if (response.SW1 == 0 && response.SW2 == 0) {
        return HAL_ERROR;
    }

    return HAL_OK;
}

int DeviceClose(void *handle)
{
    SC_Stop();

    return HAL_OK;
}

static int  id2_open(void)
{
	char test[2][20];
	int ret = 0;
	ret = DeviceOpen(test);
	/*
	if(ret == HAL_OK)
		LOG("DeviceOpen ok\n");
	else
		LOG("DeviceOpen error\n");
	*/
	return ret;
}

static int id2_com(void)
{
	uint8_t retval = 0;
	uint8_t ins[] = {0x00, 0x36, 0x00, 0x00,  0x03, 0x41, 0x00, 0x41};
	uint8_t ins2[] = {0x00, 0xC0, 0x00, 0x00, 0x1D};
	uint8_t ins_out[31] = {0};
	int32_t resp_len = 0;
	//LOG("id2_com\n");
	retval = DeviceTransmit(NULL, ins, sizeof(ins), ins_out, &resp_len);
	if(retval){
		LOG("ins cmd error\n");
		return HAL_ERROR;
	}
	retval = DeviceTransmit(NULL, ins2, sizeof(ins2), ins_out, &resp_len);
	if(retval){
		LOG("ins2 cmd error\n");
		return HAL_ERROR;
	}
	//LOG("resp_len = %d\n", resp_len);
	//LOG("ins_out[29] = 0x%x\n", ins_out[29]);
	//LOG("ins_out[30] = 0x%x\n", ins_out[30]);
	if(resp_len != (0x1D + 2) || ins_out[resp_len -2] != 0x90 || ins_out[resp_len -1] != 0x00){
		LOG("cmd respone length or SW error\n");
		return HAL_ERROR;
	}
	//LOG("id2_com exit\n");
	return HAL_OK;
}

static void id2_close(void)
{
	DeviceClose(NULL);
}

int id2_test_se(void)
{
	int ret = 0;
    	LOG("se %s:%d %s\r\n", __func__, __LINE__, aos_task_name());

	ret = id2_open();
	if(ret)
		goto end;
	ret = id2_com();
	if(ret)
		goto end;
end:
	id2_close();
	/*
	if(ret)
		LOG("id2 se test error\n");
	else
		LOG("id2 se test  ok\n");
	*/
	return ret;
}

