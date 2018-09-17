#include <finsh.h>
#include <rtthread.h>
#include <stdio.h>
#include <stdlib.h>
#include "board.h"
#include "xm_i2c.h"

struct rt_mutex g_i2c_lock;

#define I2C_SPIN_INIT     rt_mutex_init(&g_i2c_lock, "i2c_lock", RT_IPC_FLAG_FIFO)   
#define I2C_SPIN_LOCK     rt_mutex_take(&g_i2c_lock,RT_WAITING_FOREVER)
#define I2C_SPIN_UNLOCK   rt_mutex_release(&g_i2c_lock)

#define printk rt_kprintf
extern void udelay(unsigned long usec);

int i2c_write(unsigned char i2cnum, unsigned char devaddr, unsigned int regaddr, unsigned int reglen, unsigned int data, unsigned int datalen)
{
	unsigned int i = 0;
	unsigned int time = 0;
	unsigned int tmp = 0;

	I2C_SPIN_LOCK;

	writel(devaddr, i2cnum * 0x10000 + I2C_ADDRESS );
	writel(reglen + datalen, i2cnum * 0x10000 + I2C_TX_NUM);

	tmp = readl(i2cnum * 0x10000 + I2C_CTRL);
	tmp |= (1 << 6);      //  高电平删除txfifo中的数据
	writel(tmp, i2cnum * 0x10000 + I2C_CTRL);
	udelay(100);
	tmp &= ~(1 << 6);     //  清完txfifo中的数据后再将对应位恢复为低电平。
	writel(tmp, i2cnum * 0x10000 + I2C_CTRL);
	udelay(100);
	for(i = 0; i < reglen; i++)
	{   
		tmp = (regaddr >> ((reglen - i - 1) * 8) & 0xFF); 
		writel(tmp, i2cnum * 0x10000 + I2C_TXFIFO);
	}
	for(i = 0; i < datalen; i++)
	{   
		tmp = (data >> ((datalen - i - 1) * 8) & 0xFF); 
		writel(tmp, i2cnum * 0x10000 + I2C_TXFIFO);
	}
	writel(0, i2cnum * 0x10000 + I2C_TXFIFO);

	tmp = (1 << 0) | (1 << 1) | (1 << 3);  //写
	writel(tmp, i2cnum * 0x10000 + I2C_CTRL);
	/* 等待直至数据传输完毕 */    
	time = 0;
	while((readl(i2cnum * 0x10000 + I2C_SOURCE_INT) & (1 << 4)) == 0)
	{
		if(time++ > I2C_WAIT_TIME_OUT)
		{
			printk( "i2c_write timeout %#x\n", readl(i2cnum * 0x10000 + I2C_SOURCE_INT));
			writel(0x01, i2cnum * 0x10000 + I2C_CTRL);
			writel(0xFFFF, i2cnum * 0x10000 + I2C_INT_CLR);
			I2C_SPIN_UNLOCK;
			return -1;
		}
		udelay(10);
	}
	
	writel((1 << 4), i2cnum * 0x10000 + I2C_INT_CLR);

	I2C_SPIN_UNLOCK;
	return 0;
}

int i2c_read(unsigned char i2cnum, unsigned char devaddr, unsigned int regaddr, unsigned int reglen, unsigned int datalen)
{
	unsigned int i = 0;
	unsigned int time = 0;
	unsigned int tmp = 0;
	unsigned int data = 0;

	I2C_SPIN_LOCK;

	writel(devaddr, i2cnum * 0x10000 + I2C_ADDRESS);
	writel(reglen, i2cnum * 0x10000 + I2C_TX_NUM);
	
	tmp = readl(i2cnum * 0x10000 + I2C_CTRL);
	tmp |= (1 << 6);      //  高电平删除txfifo中的数据
	writel(tmp, i2cnum * 0x10000 + I2C_CTRL);
	udelay(100);
	tmp &= ~(1 << 6);     //  清完txfifo中的数据后再将对应位恢复为低电平。
	writel(tmp, i2cnum * 0x10000 + I2C_CTRL);
	udelay(100);
	for(i = 0; i < reglen; i++)
	{   
		tmp = (regaddr >> ((reglen - i - 1) * 8) & 0xFF); 
		writel(tmp, i2cnum * 0x10000 + I2C_TXFIFO);
	}
	writel(0, i2cnum * 0x10000 + I2C_TXFIFO);

	tmp = (1 << 0) | (1 << 1) | (1 << 3);  //写
	writel(tmp, i2cnum * 0x10000 + I2C_CTRL);
	/* 等待直至数据传输完毕 */    
	time = 0;
	while((readl(i2cnum * 0x10000 + I2C_SOURCE_INT) & (1 << 4)) == 0)
	{
		if(time++ > I2C_WAIT_TIME_OUT)
		{
			printk( "i2c_write timeout %#x\n", readl(i2cnum * 0x10000 + I2C_SOURCE_INT));
			writel(0x01, i2cnum * 0x10000 + I2C_CTRL);
			writel(0xFFFF, i2cnum * 0x10000 + I2C_INT_CLR);
			I2C_SPIN_UNLOCK;
			return -1;
		}
		udelay(10);
	}
	
	writel((1 << 4), i2cnum * 0x10000 + I2C_INT_CLR);

	writel(datalen, i2cnum * 0x10000 + I2C_RX_NUM);

	tmp = readl(i2cnum * 0x10000 + I2C_CTRL);
	tmp |= (1 << 7);      //  高电平删除rxfifo中的数据
	writel(tmp, i2cnum * 0x10000 + I2C_CTRL);
	udelay(100);
	tmp &= ~(1 << 7);     //  清完rxfifo中的数据后再将对应位恢复为低电平。
	writel(tmp, i2cnum * 0x10000 + I2C_CTRL);
	udelay(100);

	printk("054=%d\n",readl(0x10000054));

	tmp = (1 << 0) | (1 << 1) | (1 << 11);  //读
	writel(tmp, i2cnum * 0x10000 + I2C_CTRL);
	/* 等待读完 */    
	time = 0;
	while((readl(i2cnum * 0x10000 + I2C_SOURCE_INT) & (1 << 5)) == 0)
	{
		if(time++ > I2C_WAIT_TIME_OUT)
		{
			printk( "i2c_read timeout %#x\n", readl(i2cnum * 0x10000 + I2C_SOURCE_INT));
			writel(0x01, i2cnum * 0x10000 + I2C_CTRL);
			writel(0xFFFF, i2cnum * 0x10000 + I2C_INT_CLR);
			I2C_SPIN_UNLOCK;
			return -1;
		}
		udelay(10);
	}

	for(i = 0; i < datalen; i++)
	{   
		data <<= 8;
		printk("54=%d\n",readl(0x10000054));

		data |= readl(i2cnum * 0x10000 + I2C_RXFIFO);
	}
	writel((1 << 5), i2cnum * 0x10000 + I2C_INT_CLR);
	
	I2C_SPIN_UNLOCK;
	return data;
}

#if 0
static long i2c_ioctl(unsigned int cmd, unsigned long arg)
{
	I2C_DATA_S *argp = (I2C_DATA_S *)arg;
	int ret = 0;

	unsigned char i2cnum = -1;
	unsigned char devaddr;
	unsigned int regaddr; 
	unsigned int reglen;
	unsigned int datalen;
	unsigned int wdata;
	i2cnum = 1; //170116
	switch (cmd)
	{   
		case CMD_I2C_WRITE:
		{   
			devaddr = argp->dev_addr;
			regaddr = argp->reg_addr;
		    reglen = argp->addr_byte_num;
		    wdata = argp->data;   
		    datalen = argp->data_byte_num;
			
			rt_kprintf("i2c_wirte: dev_addr:0x%2x; reg_addr:0x%2x; reg_value:0x%2x; reg_width: %d; data_width: %d.\n", 
        	devaddr, regaddr, wdata, reglen, datalen);
		    ret = i2c_write(i2cnum, devaddr, regaddr, reglen, wdata, datalen);
			if(ret == 0)
			{   
				//printk("line :%d      write ok!\n",__LINE__);
			}   
			break;
		}  
		case CMD_I2C_READ:
		{   
			devaddr = argp->dev_addr;
			regaddr= argp->reg_addr;
			reglen= argp->addr_byte_num;
			datalen= argp->data_byte_num;   
			
			ret = i2c_read(i2cnum, devaddr, regaddr, reglen, datalen);
			argp->data = ret ;
			rt_kprintf("i2c_read: dev_addr:0x%2x; reg_addr:0x%2x; reg_value:0x%2x; reg_width: %d; data_width: %d.\n", 
						devaddr, regaddr, ret, reglen, datalen);

			break;
		}   
		default:
		{   
			printk("xm_i2c command error!\n");
			return -1;
		}   
	}   

	return 0;
}
#endif

static int i2c_init(void)
{
	unsigned int tmp = 0;
	unsigned int i2cnum = 0;
	tmp = readl(GPIO1_MULT_USE_EN);
	//I2C0 gpio
	tmp &= ~((1 << 0) | (1 << 2));
	tmp |= (1 << 8) | (1 << 10);

	//I2C1 gpio
	tmp &= ~((1 << 4) | (1 << 5) | (1 << 12) | (1 << 13));
	tmp |= (1 << 20) | (1 << 21);

	writel(tmp, GPIO1_MULT_USE_EN);

	for(i2cnum = 0; i2cnum < 2; i2cnum++)
	{
		writel(0xa8, i2cnum * 0x10000 + I2C_THIGH);
		writel(0xa8, i2cnum * 0x10000 + I2C_TLOW);
		writel(0xa8, i2cnum * 0x10000 + I2C_TSU_STA);
		writel(0xa8, i2cnum * 0x10000 + I2C_THD_STA);
		writel(0xa8, i2cnum * 0x10000 + I2C_TSU_STOP);
		writel(0x50, i2cnum * 0x10000 + I2C_TDAT_CHG);
		writel(0xfc, i2cnum * 0x10000 + I2C_TDAT_SMP);
	}

	return 0;
}

int xm_i2c_init(void)
{
	i2c_init();
	I2C_SPIN_INIT;
	rt_kprintf("xm_i2c init is ok!\n");
	return 0;
}



int i2cread(int argc, char *argv[])
{
	unsigned char devaddr;
	unsigned int regaddr;
	unsigned int reglen = 1;
	unsigned int rdata;
	unsigned int datalen = 1;
	unsigned int devaddr_ul;
	unsigned int regaddr_end;

	if (argc < 3)
    {
    	printf("usage: %s <device_addr> <reg_addr> <end_reg_addr> <reg_width> <data_width>. sample: \n", argv[0]);
        printf("------------------------------------------------------------------------------------\n");
        printf("\t\t%s 0x56 0x0 0x10 2 2. \n", argv[0]);
        printf("\t\t%s 0x56 0x0 0x10. default reg_width and data_width is 1. \n", argv[0]);

        return -1;
    }

	devaddr_ul = strtoul(argv[1], NULL, 16);
	devaddr = devaddr_ul & 0xff;
	regaddr = strtoul(argv[2], NULL, 16);
	if(argc > 3)
	{
		regaddr_end = strtoul(argv[3], NULL, 16);

		if (regaddr_end < regaddr)
        {
            printf("end addr(0x%2x) should bigger than start addr(0x%2x)\n",
                regaddr_end, regaddr);
            return 0;
        }

		if(argc > 4)
			reglen = strtoul(argv[4], NULL, 16);

		if(argc > 5)
			datalen = strtoul(argv[5], NULL, 16);
	}
	else
    {
        regaddr_end = regaddr;
    }

	printf("dev_addr:0x%2x; reg_addr:0x%2x; reg_addr_end:0x%2x; reg_width: %d; data_width: %d. \n", devaddr,
            regaddr, regaddr_end, reglen, datalen);
    {
        unsigned int curaddr;

        for (curaddr = regaddr; curaddr <= regaddr_end; curaddr ++)
        {
            rdata = i2c_read(0, devaddr, curaddr, reglen, datalen);
            printf("0x%x 0x%x\n", curaddr, rdata);
        }
    }
	return 0;
}

int i2cwrite(int argc, char *argv[])
{
	unsigned char devaddr;
	unsigned int regaddr;
	unsigned int reglen = 1;
	unsigned int wdata;
	unsigned int datalen = 1;
	unsigned int devaddr_ul;

	if(argc < 4)
    {
    	printf("usage: %s <device_addr> <reg_addr> <value> <reg_width> <data_width>. sample:\n", argv[0]);
        printf("----------------------------------------------------------------------------\n");
        printf("\t\t%s 0x56 0x0 0x28 2 2. \n", argv[0]);
        printf("\t\t%s 0x56 0x0 0x28. default reg_width and data_width is 1. \n", argv[0]);
        return -1;
    }
	
	devaddr_ul = strtoul(argv[1], NULL, 16);
	devaddr = devaddr_ul & 0xff;
	regaddr = strtoul(argv[2], NULL, 16);
	wdata = strtoul(argv[3], NULL, 16);
	if(argc > 4)
	{
		reglen = strtoul(argv[4], NULL, 16);

		if(argc > 5)
			datalen = strtoul(argv[5], NULL, 16);
	}

	    printf("dev_addr:0x%2x; reg_addr:0x%2x; reg_value:0x%2x; reg_width: %d; data_width: %d.\n", devaddr,
            regaddr, wdata, reglen, datalen);
 
	i2c_write(0, devaddr, regaddr, reglen, wdata, datalen);
	return 0;
}

INIT_DEVICE_EXPORT(xm_i2c_init);
MSH_CMD_EXPORT(i2cread, i2c read test);
MSH_CMD_EXPORT(i2cwrite, i2c write test);

