#ifndef  __XM_I2C_H_
#define  __XM_I2C_H_
#define I2C_WAIT_TIME_OUT       0x3000
#define I2C_ADDRESS_BASE	(0x10000000)

#define I2C_CTRL		 	(I2C_ADDRESS_BASE + 0X00)
#define I2C_TRANS_ORDER    	(I2C_ADDRESS_BASE + 0X04)
#define I2C_BUS_WIDTH      	(I2C_ADDRESS_BASE + 0X08)
#define I2C_ADDRESS        	(I2C_ADDRESS_BASE + 0X0C)
#define I2C_TXFIFO         	(I2C_ADDRESS_BASE + 0X10)
#define I2C_RXFIFO         	(I2C_ADDRESS_BASE + 0X14)
#define I2C_TX_NUM         	(I2C_ADDRESS_BASE + 0X18)
#define I2C_RX_NUM         	(I2C_ADDRESS_BASE + 0X1C)
#define I2C_THIGH          	(I2C_ADDRESS_BASE + 0X20)
#define I2C_TLOW           	(I2C_ADDRESS_BASE + 0X24)
#define I2C_TSU_STA        	(I2C_ADDRESS_BASE + 0X28)
#define I2C_THD_STA        	(I2C_ADDRESS_BASE + 0X2C)
#define I2C_TSU_STOP       	(I2C_ADDRESS_BASE + 0X30)
#define I2C_TDAT_CHG       	(I2C_ADDRESS_BASE + 0X34)
#define I2C_TDAT_SMP       	(I2C_ADDRESS_BASE + 0X38)
#define I2C_WAIT_TIME      	(I2C_ADDRESS_BASE + 0X3C)
#define I2C_INT_MASK       	(I2C_ADDRESS_BASE + 0X40)
#define I2C_INT_CLR        	(I2C_ADDRESS_BASE + 0X44)
#define I2C_SOURCE_INT     	(I2C_ADDRESS_BASE + 0X48)
#define I2C_INT_REG        	(I2C_ADDRESS_BASE + 0X4C)
#define I2C_TXFIFO_NUM     	(I2C_ADDRESS_BASE + 0X50)
#define I2C_RXFIFO_NUM     	(I2C_ADDRESS_BASE + 0X54)


#define GPIO1_MULT_USE_EN  (0x10020004)

int i2c_read(unsigned char i2cnum, unsigned char devaddr, unsigned int regaddr, unsigned int reglen, unsigned int datalen);
int i2c_write(unsigned char i2cnum, unsigned char devaddr, unsigned int regaddr, unsigned int reglen, unsigned int data, unsigned int datalen);
int xm_i2c_init(void);


#endif
