/*
 * Copyright (c) 2011-2012, Freescale Semiconductor, Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * o Redistributions of source code must retain the above copyright notice, this list
 *   of conditions and the following disclaimer.
 *
 * o Redistributions in binary form must reproduce the above copyright notice, this
 *   list of conditions and the following disclaimer in the documentation and/or
 *   other materials provided with the distribution.
 *
 * o Neither the name of Freescale Semiconductor, Inc. nor the names of its
 *   contributors may be used to endorse or promote products derived from this
 *   software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

//#include "imx_i2c.h"
#include "obds.h"

extern int i2c_device_check_max11801(unsigned int i2c_base_addr);
extern int max11801_ts_test(unsigned int i2c_base_addr);
static void max11801_ts_phy_init(unsigned int i2c_base_addr);
static void max11801_ts_interrupt(unsigned int i2c_base_addr);

unsigned char max11801_reg_read_single(unsigned int i2c_base_addr, unsigned char reg_addr, unsigned char *ret_val);
unsigned char max11801_reg_read_multi(unsigned int i2c_base_addr, unsigned char reg_addr, unsigned char *ret_val, int count);
void max11801_reg_write(unsigned int i2c_base_addr, unsigned char reg_addr, unsigned char reg_data);

extern int i2c_device_check_max11801_AUX(unsigned int i2c_base_addr);
static int max11801_aux_read(unsigned int i2c_base_addr);

 /* Register Address define */
 #define GENERNAL_STATUS_REG_0             (0x00 << 1)
 #define GENERNAL_CONF_REG_1               (0x01 << 1)
 #define MESURE_RES_CONF_REG_2             (0x02 << 1)
 #define MESURE_AVER_CONF_REG_3            (0x03 << 1)
 #define ADC_SAMPLE_TIME_CONF_REG_4        (0x04 << 1)
 #define PANEL_SETUPTIME_CONF_REG_5        (0x05 << 1)
 #define DELAY_CONVERSION_CONF_REG_6       (0x06 << 1)
 #define TOUCH_DETECT_PULLUP_CONF_REG_7    (0x07 << 1)
 #define AUTO_MODE_TIME_CONF_REG_8         (0x08 << 1) 
 #define APERTURE_CONF_REG_9               (0x09 << 1)  
 #define AUX_MESURE_CONF_REG_A             (0x0a << 1)
 #define OP_MODE_CONF_REG_B                (0x0b << 1)
 
 /* FIFO is found only in max11800 and max11801 */
 #define FIFO_RD_CMD                     (0x50 << 1)
 #define MAX11801_X_MSB					 (0x52 << 1)
 #define MAX11801_X_LSB					 (0x53 << 1)
 #define MAX11801_Y_MSB					 (0x54 << 1)
 #define MAX11801_Y_LSB					 (0x55 << 1)
 #define MAX11801_AUX_MSB				 (0x5A << 1)
 #define MAX11801_AUX_LSB				 (0x5B << 1)
 
 #define MAX11801_AUX_CONV				 (0x76 <<1)	
 #define MAX11801_FIFO_INT               (1 << 2)
 #define MAX11801_FIFO_OVERFLOW          (1 << 3)
 
 #define XY_BUFSIZE                      4
 #define XY_BUF_OFFSET                   4
 
 #define MAX11801_MAX_X                  0xfff
 #define MAX11801_MAX_Y                  0xfff
 
 #define MEASURE_TAG_OFFSET              2
 #define MEASURE_TAG_MASK                (3 << MEASURE_TAG_OFFSET)
 #define EVENT_TAG_OFFSET                0
 #define EVENT_TAG_MASK                  (3 << EVENT_TAG_OFFSET)
 #define MEASURE_X_TAG                   (0 << MEASURE_TAG_OFFSET)
 #define MEASURE_Y_TAG                   (1 << MEASURE_TAG_OFFSET)
 
  //These are the state of touch event state machine
 enum {
         EVENT_INIT,
         EVENT_MIDDLE,
         EVENT_RELEASE,
         EVENT_FIFO_END
 };
 
static void max11801_ts_phy_init(unsigned int i2c_base_addr)
{

	//unsigned int ret;
    //unsigned char data;

	// 50K ts pull up, disabled internal pullup, cmoos output, enabled int
	max11801_reg_write(i2c_base_addr, GENERNAL_CONF_REG_1, 0xF0);    
	// internal ADC runs at normal power, 12 bit convertion
	max11801_reg_write(i2c_base_addr, MESURE_RES_CONF_REG_2, 0x00);   
	// Average X,Y, take 16 samples, average eight media sample 
	max11801_reg_write(i2c_base_addr, MESURE_AVER_CONF_REG_3, 0xff); 
	// 2us before conversion
	max11801_reg_write(i2c_base_addr, ADC_SAMPLE_TIME_CONF_REG_4, 0x00); 
	// X,Y panel setup time set to 8 us
	max11801_reg_write(i2c_base_addr, PANEL_SETUPTIME_CONF_REG_5, 0x11);
	// 1ms delay conversion
	max11801_reg_write(i2c_base_addr, DELAY_CONVERSION_CONF_REG_6, 0x00);
	// Rough pullup time (2uS), Fine pullup time (10us)  
	max11801_reg_write(i2c_base_addr, TOUCH_DETECT_PULLUP_CONF_REG_7, 0x10);
	// Auto mode init period = 5ms , scan period = 5ms
	max11801_reg_write(i2c_base_addr, AUTO_MODE_TIME_CONF_REG_8, 0xaa);    
	// Aperture X,Y set to +- 4LSB 
	max11801_reg_write(i2c_base_addr, APERTURE_CONF_REG_9, 0x33);   
	// Aux. 10 usc initial delay, 2 us sample, single sample mode, 12 bit resolution
	max11801_reg_write(i2c_base_addr, AUX_MESURE_CONF_REG_A, 0x00);   
	/* Enable Power, enable Automode, enable Aperture, enable Average X,Y */
	max11801_reg_write(i2c_base_addr, OP_MODE_CONF_REG_B, 0x44);     

		/*
		ret = max11801_reg_read_single(i2c_base_addr, GENERNAL_CONF_REG_1, &data);
		printf("reg1 = 0x%02X\n", data);
		ret = max11801_reg_read_single(i2c_base_addr, MESURE_RES_CONF_REG_2, &data);
		printf("reg2 = 0x%02X\n", data);
		ret = max11801_reg_read_single(i2c_base_addr, MESURE_AVER_CONF_REG_3, &data);
		printf("reg3 = 0x%02X\n", data);
		ret = max11801_reg_read_single(i2c_base_addr, ADC_SAMPLE_TIME_CONF_REG_4, &data);
		printf("reg4 = 0x%02X\n", data);
		
		ret = max11801_reg_read_single(i2c_base_addr, PANEL_SETUPTIME_CONF_REG_5, &data);
		printf("reg5 = 0x%02X\n", data);
		ret = max11801_reg_read_single(i2c_base_addr, DELAY_CONVERSION_CONF_REG_6, &data);
		printf("reg6 = 0x%02X\n", data);
		ret = max11801_reg_read_single(i2c_base_addr, TOUCH_DETECT_PULLUP_CONF_REG_7, &data);
		printf("reg7 = 0x%02X\n", data);
		ret = max11801_reg_read_single(i2c_base_addr, AUTO_MODE_TIME_CONF_REG_8, &data);
		printf("reg8 = 0x%02X\n", data);
		ret = max11801_reg_read_single(i2c_base_addr, APERTURE_CONF_REG_9, &data);
		printf("reg9 = 0x%02X\n", data);
		
		ret = max11801_reg_read_single(i2c_base_addr, AUX_MESURE_CONF_REG_A, &data);
		printf("reg10 = 0x%02X\n", data);
		ret = max11801_reg_read_single(i2c_base_addr, OP_MODE_CONF_REG_B, &data);
		printf("reg11 = 0x%02X\n", data);
		*/
}

static int max11801_aux_read(unsigned int i2c_base_addr)
{
		unsigned int ret;
		unsigned char data;
        uint8_t buf[14] = { 0 };
        int x = -1;

		max11801_reg_write(i2c_base_addr, OP_MODE_CONF_REG_B, 0x00);      // Direct conversion mode                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                         
		
		ret = max11801_reg_read_single(i2c_base_addr, GENERNAL_STATUS_REG_0, &data);
		//printf("STATUS:  0x%02X\n",data);

		max11801_reg_write(i2c_base_addr, MAX11801_AUX_CONV, 0x00);     // start conversion
		
		ret = max11801_reg_read_single(i2c_base_addr, MAX11801_AUX_MSB, &data);
		if (ret !=0) goto out;
		buf[0] = data;
		ret = max11801_reg_read_single(i2c_base_addr, MAX11801_AUX_LSB, &data);
		if (ret !=0) goto out;
		buf[1] = data;
		//printf("buf[0] = 0x%02X \n",buf[0]);	
		//printf("buf[1] = 0x%02X \n",buf[1]);	

		x = (buf[0] << XY_BUF_OFFSET) +	(buf[1] >> XY_BUF_OFFSET);

		//printf("Volage = %02d, ", (3.15 * x) / 4096)  );
		
		return x;
out:
		x = -1;	
        return x;
}

static void max11801_ts_interrupt(unsigned int i2c_base_addr)
{
        unsigned int status;
		unsigned int i;
		unsigned int ret;
		unsigned char data;
        uint8_t buf[XY_BUFSIZE];
        int x = -1;
        int y = -1;

		status = max11801_reg_read_single(i2c_base_addr, GENERNAL_STATUS_REG_0,&data);
		status = data;
		//printf ("Status=%02X \n",status);
		if (status & (MAX11801_FIFO_INT | MAX11801_FIFO_OVERFLOW)) 
		{
			/*
			status = max11801_reg_read_single(i2c_base_addr, GENERNAL_STATUS_REG_0,&data);
			status = max11801_reg_read_single(i2c_base_addr, MAX11801_X_MSB,&data);
			buf[0] = data; 
			status = max11801_reg_read_single(i2c_base_addr, MAX11801_X_LSB,&data);
			buf[1] = data;
			status = max11801_reg_read_single(i2c_base_addr, MAX11801_Y_MSB,&data);
			buf[2] = data;
			status = max11801_reg_read_single(i2c_base_addr, MAX11801_Y_LSB,&data);
			buf[3] = data;			
			*/
			
			ret = max11801_reg_read_multi(i2c_base_addr, MAX11801_X_MSB, buf, 4);
			
			// write to operating mode register to clear the FIFO/int flag
			max11801_reg_write(i2c_base_addr, OP_MODE_CONF_REG_B, 0x44);     //0x36 

			//
			//We should get 4 bytes buffer that contains X,Y
			//and event tag
			//
			if (ret!=0x00)
				goto out;

			for (i = 0; i < XY_BUFSIZE; i += XY_BUFSIZE / 2) {
					if ((buf[i + 1] & MEASURE_TAG_MASK) == MEASURE_X_TAG)
							x = (buf[i] << XY_BUF_OFFSET) +
								(buf[i + 1] >> XY_BUF_OFFSET);
					else if ((buf[i + 1] & MEASURE_TAG_MASK) == MEASURE_Y_TAG)
							y = (buf[i] << XY_BUF_OFFSET) +
								(buf[i + 1] >> XY_BUF_OFFSET);
			}

			printf("LCD: X=%04d Y=%04d \n",x,y);
			
			/*
			if ((buf[1] & EVENT_TAG_MASK) != (buf[3] & EVENT_TAG_MASK))
					goto out;
			
			switch (buf[1] & EVENT_TAG_MASK) 
			{
				case EVENT_INIT:
					// fall through 
				case EVENT_MIDDLE:
						//printf ("X = %d   , y = %d\n",x,y);
						//input_report_abs(data->input_dev, ABS_X, x);
						//input_report_abs(data->input_dev, ABS_Y, y);
						//input_event(data->input_dev, EV_KEY, BTN_TOUCH, 1);
						//input_sync(data->input_dev);
						break;

				case EVENT_RELEASE:
						printf ("event release\n");
						//input_event(data->input_dev, EV_KEY, BTN_TOUCH, 0);
						//input_sync(data->input_dev);
						break;

				case EVENT_FIFO_END:
						break;
			}
			*/
		}
out:
		i = 0x00;
       // return IRQ_HANDLED;
}

int max11801_ts_test(unsigned int i2c_base_addr)
{
	unsigned char uc = NONE_CHAR;
	volatile unsigned char pin=0;
	
    i2c_init(i2c_base_addr, 100000);   //170000    100000
	max11801_ts_phy_init(i2c_base_addr);

	printf("Starting showing Touchscreen touches. Press 'x' to exit.\n");
	while (1) {
		pin = gpio_get_level(GPIO_PORT3,26);    // check TS_INT
		printf("MAX11801 TS_INT pin%01X. Press 'x' to exit.\n", pin);
		if (pin == GPIO_LOW_LEVEL)
		{
			max11801_ts_interrupt(i2c_base_addr);
		}

		hal_delay_us(1000*500);  // wait 1 msec * 500 = 500msecs 
        uc = getchar();
		if (uc == 'x' || uc == 'X') {
			printf("\n");
			break;
		}

	}

    return 0;
}

unsigned char max11801_reg_read_single(unsigned int i2c_base_addr, unsigned char reg_addr, unsigned char *ret_val)
{
    struct imx_i2c_request rq = {0};
    unsigned char buf[1];

    rq.ctl_addr = i2c_base_addr;
    rq.dev_addr = MAX11801_I2C_ID;
    rq.reg_addr = reg_addr;
    rq.reg_addr_sz = 1;
    rq.buffer_sz = 1;
    rq.buffer = buf;

    if (i2c_xfer(&rq, I2C_READ) != 0) {
//    if (i2c_xfer(i2c_base_addr, &rq, I2C_READ) != 0) {
        printf("%s() error. return\n", __FUNCTION__);
        return -1;
    }

    *ret_val = buf[0];
	
	//printf ("%s()  MAX  I2C ADD = 0x%02X   ret_val =  0x%02X\n", __FUNCTION__, rq.dev_addr,buf[0]);  
    return 0;
}

unsigned char max11801_reg_read_multi(unsigned int i2c_base_addr, unsigned char reg_addr, unsigned char *ret_val, int count)
{
    struct imx_i2c_request rq = {0};

    rq.ctl_addr = i2c_base_addr;	
    rq.dev_addr = MAX11801_I2C_ID;
    rq.reg_addr = reg_addr;
    rq.reg_addr_sz = 1;
    rq.buffer_sz = count;
    rq.buffer = ret_val;

        if (i2c_xfer(&rq, I2C_READ) != 0) {
//	if (i2c_xfer(i2c_base_addr, &rq, I2C_READ) != 0) {
		printf("%s() error. return\n", __FUNCTION__);
		return -1;
	}
	
    return 0;
}

void max11801_reg_write(unsigned int i2c_base_addr, unsigned char reg_addr, unsigned char reg_data)
{
    struct imx_i2c_request rq = {0};

    rq.ctl_addr = i2c_base_addr;
    rq.dev_addr = MAX11801_I2C_ID;
    rq.reg_addr = reg_addr;
    rq.reg_addr_sz = 1;
    rq.buffer_sz = 1;
    rq.buffer = &reg_data;

    i2c_xfer(&rq, I2C_WRITE);
//    i2c_xfer(i2c_base_addr, &rq, I2C_WRITE);
}

int i2c_device_check_max11801_AUX(unsigned int i2c_base_addr)
{
   	unsigned char uc = NONE_CHAR;
	unsigned int ret;

	i2c_init(i2c_base_addr, 100000);   //170000    100000
	max11801_ts_phy_init(i2c_base_addr);
	
	while (1) {
		ret = max11801_aux_read(i2c_base_addr);
	
		printf("AUX value =%04d,   Press 'x' to exit.\n\n",ret);	
		
		hal_delay_us(1000*500);  // wait 1 msec * 500 = 500msecs 
        uc = getchar();
		if (uc == 'x' || uc == 'X') {
			printf("\n");
			break;
		}
		
	}
	
	return TEST_PASSED;
}

int i2c_device_check_max11801(unsigned int i2c_base_addr)
{
    unsigned int i;
	unsigned int ret;
    unsigned char data;
	
	unsigned char compareData[11] = {0xF0, 0x00, 0xFF, 0x00, 0x11, 0x00, 0x10, 0xaa, 0x33, 0x00, 0X44};
	unsigned char readData [11] = { 0x00 };
	
    i2c_init(i2c_base_addr, 100000);   //170000    100000
	max11801_ts_phy_init(i2c_base_addr);

    for (i = 1; i <= 11; i++) {
        if ((ret = max11801_reg_read_single(i2c_base_addr, ((unsigned char)i << 1), &data)) != 0) {
            return ret;
        }
        //printf("DATA FROM MAX11801:  reg0x%02X = 0x%02X\n", i, data);
		readData[i-1] = data;
    }

	printf ("\nStarting check...\n");
	ret = 0x00;
	for (i=0;i<=10;i++)
	{
		printf ("Data to compare = reg#%d  data=0x%02X\n",i+1,compareData[i]);
		printf ("Data read       = reg#%d  data=0x%02X\n\n",i+1,readData[i]);
		if (compareData[i] != readData[i])
		{
		    ret = 0xFF;
			break;
		}
	}
	
	if (ret == 0x00){
	    printf(" MAX11801 I2C device check passed \n");
		return TEST_PASSED;
	}
	else	
	{
		printf(" MAX11801 I2C device check failed \n");
		return TEST_FAILED;
	}
}
