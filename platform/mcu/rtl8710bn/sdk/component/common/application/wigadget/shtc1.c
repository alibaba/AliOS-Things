#include "FreeRTOS.h"
#include "task.h"

#include "device.h"
#include "PinNames.h"
#include "basic_types.h"
#include "diag.h" 
#include "osdep_service.h"
#include "i2c_api.h"
#include "pinmap.h"
#include "shtc1.h"

#ifdef CONFIG_PLATFORM_8195A
#define MBED_I2C_MTR_SDA    PB_3
#define MBED_I2C_MTR_SCL    PB_2
#endif

#ifdef CONFIG_PLATFORM_8711B
#define MBED_I2C_MTR_SDA    PA_19
#define MBED_I2C_MTR_SCL    PA_22
#endif

#define MBED_I2C_SLAVE_ADDR0    0x70
#define POLYNOMIAL 0x131 // P(x) = x^8 + x^5 + x^4 + 1 = 100110001

#define MBED_I2C_BUS_CLK        100000  //hz
#define I2C_DATA_MAX_LENGTH     16

static uint8_t i2cdata_write[I2C_DATA_MAX_LENGTH];
static uint8_t i2cdata_read[I2C_DATA_MAX_LENGTH];
static int i2cdata_read_pos;

static i2c_t   i2cmaster;

// Sensor Commands
#define READ_ID              0xEFC8 // command: read ID register
#define SOFT_RESET           0x805D // soft resetSample Code for SHTC1
#define MEAS_T_RH_POLLING    0x7866 // meas. read T first, clock stretching disabled
#define MEAS_T_RH_CLOCKSTR   0x7CA2 // meas. read T first, clock stretching enabled
#define MEAS_RH_T_POLLING    0x58E0 // meas. read RH first, clock stretching disabled
#define MEAS_RH_T_CLOCKSTR   0x5C24 // meas. read RH first, clock stretching enabled


static int SHTC1_GetID(uint16_t *id);
static void SHTC1_WriteCommand(uint16_t cmd);
static int SHTC1_Read2BytesAndCrc(uint16_t *data);
static int SHTC1_CheckCrc(uint8_t data[], uint8_t nbrOfBytes, uint8_t checksum);
static float SHTC1_CalcTemperature(uint16_t rawValue);
static float SHTC1_CalcHumidity(uint16_t rawValue);


int SHTC_Init(uint16_t *pID)
{
	int error = NO_ERROR;

	DiagPrintf("SHTC1_Init \r\n");

	i2c_init((i2c_t*)&i2cmaster, MBED_I2C_MTR_SDA ,MBED_I2C_MTR_SCL);
	i2c_frequency((i2c_t*)&i2cmaster,MBED_I2C_BUS_CLK);
 
	if (pID == NULL ) 
		return NULL_ERROR;
	error = SHTC1_GetID(pID);
	
	return error;
}

static int SHTC1_GetID(uint16_t *id)
{
	int error = NO_ERROR;
	uint8_t bytes[2];
	uint8_t checksum;

	SHTC1_WriteCommand(READ_ID);  

	i2c_read((i2c_t*)&i2cmaster, MBED_I2C_SLAVE_ADDR0, (char*)&i2cdata_read[0], 3, 1);
	i2cdata_read_pos = 0;
	error = SHTC1_Read2BytesAndCrc(id);

	return error;
}

static int SHTC1_Read2BytesAndCrc(uint16_t *data)
{
	int error;
	int readed;
	uint8_t bytes[2];
	uint8_t checksum;
  
	bytes[0] = i2cdata_read[i2cdata_read_pos++];
	bytes[1] = i2cdata_read[i2cdata_read_pos++];
	checksum = i2cdata_read[i2cdata_read_pos++];

	error = SHTC1_CheckCrc(bytes, 2, checksum);
	*data = (bytes[0] << 8) | bytes[1];  
	  
	  return error;
}

static int SHTC1_CheckCrc(uint8_t data[], uint8_t nbrOfBytes, uint8_t checksum)
{
	uint8_t bit; // bit mask
	uint8_t crc = 0xFF; // calculated checksum
	uint8_t byteCtr; // byte counter

	for(byteCtr = 0; byteCtr < nbrOfBytes; byteCtr++){
		crc ^= (data[byteCtr]);
		for(bit = 8; bit > 0; --bit){
			if(crc & 0x80)
				crc = (crc << 1) ^ POLYNOMIAL;
			else 
				crc = (crc << 1);
		}
	}

	if(crc != checksum) 
		return CHECKSUM_ERROR;
	else 
		return NO_ERROR;
}

static void SHTC1_WriteCommand(uint16_t cmd)
{
	int writebytes; 

	i2cdata_write[0] = (uint8_t)(cmd >>8);
	i2cdata_write[1] = (uint8_t)(cmd&0xFF);
  	i2c_write((i2c_t*)&i2cmaster, MBED_I2C_SLAVE_ADDR0, &i2cdata_write[0], 2, 1);
}  

static float SHTC1_CalcTemperature(uint16_t rawValue)
{
	return 175.0 * (float)rawValue / 65536.0 - 45.0;
}

static float SHTC1_CalcHumidity(uint16_t rawValue)
{
	return 100.0 * (float)rawValue / 65536.0;
} 

int SHTC_GetTempAndHumi(float *temp, float *humi)
{
	int error;
	uint16_t rawValueTemp;
	uint16_t rawValueHumi;
  
	SHTC1_WriteCommand(MEAS_T_RH_CLOCKSTR);  

	i2c_read((i2c_t*)&i2cmaster, MBED_I2C_SLAVE_ADDR0, (char*)&i2cdata_read[0], 6, 1);
	i2cdata_read_pos = 0;
	error = NO_ERROR;
	error |= SHTC1_Read2BytesAndCrc(&rawValueTemp);
	error |= SHTC1_Read2BytesAndCrc(&rawValueHumi);
 
	if ( error == NO_ERROR ) {
		*temp = SHTC1_CalcTemperature(rawValueTemp);
		*humi = SHTC1_CalcHumidity(rawValueHumi);
	}
  
	return error;
}

static void example_shtc1_thread(void *param)
{
	int error;
	uint16_t shtc1_id;	
	float temperature = 1.123f;
	float humidity = 2.456f;

	DBG_8195A("sleep 10 sec. to wait for UART console\n");
	rtw_msleep_os(10000);
	DBG_8195A("start i2c example - SHTC1\n");
	
	error = SHTC_Init(&shtc1_id);
	if ( error == NO_ERROR ) 
		DiagPrintf("SHTC1 init ok, id=0x%x\r\n", shtc1_id);
	else {
		DiagPrintf("SHTC1 init FAILED! \r\n");
		for(;;);
	}
	
	while(1){
		error = SHTC_GetTempAndHumi(&temperature, &humidity);		
		rtl_printf("temp=%f, humidity=%f, error=%d\n", temperature, humidity, error);	
		rtw_msleep_os(1000);
	}
}

void example_shtc1(void)
{
	if(xTaskCreate(example_shtc1_thread, ((const char*)"example_shtc1_thread"), 512, NULL, tskIDLE_PRIORITY + 1, NULL) != pdPASS)
		printf("\n\r%s xTaskCreate(init_thread) failed", __FUNCTION__);
}


