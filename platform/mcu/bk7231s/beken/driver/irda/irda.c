#include "include.h"
#include "arm_arch.h"

#include "irda.h"
#include "irda_pub.h"

#include "drv_model_pub.h"
#include "intc_pub.h"


#define IR_MAP                                   \
{                                                \
    0x0, /* KEY_play   */                        \
    0x5, /* KEY_VOL-   */                        \
    0x6,  /* KEY_VOL+   */                       \
    0x9, /* KEY_PREV   */                        \
    0xa,  /* KEY_NEXT   */                       \
    0x4,  /* KEY_MODE   */                       \
	0x1,  /* KEY_CH-   */                        \
    0x2, /* KEY_CH+   */                         \
	0x8,  /* KEY_0   */                          \
	0xc, /* KEY_1   */                           \
    0xd,  /* KEY_2   */                           \
    0xe, /* KEY_3   */                            \
    0x10,  /* KEY_4   */                          \
    0x11,  /* KEY_5   */                          \
    0x12, /* KEY_6   */                           \
    0x14,  /* KEY_7   */                          \
    0x15,  /* KEY_8   */                          \
    0x16  /* KEY_9   */                           \
}
static UINT16 IR_Code[IR_CODE_SIZE] = IR_MAP;
static UINT32 Recv_IR_Code = 0xffffffff;


static SDD_OPERATIONS irda_op = {
            irda_ctrl
};

static void irda_gpio_config()
{
}

static void irda_active(UINT8 enable)
{
	UINT32 value;

	value = REG_READ(IRDA_CTRL);
	if(enable){
		value |= IRDA_NEC_EN;
	}else{
		value &= ~IRDA_NEC_EN;
	}
	REG_WRITE(IRDA_CTRL, value);
}

static void irda_set_polarity(UINT8 polarity)
{
	UINT32 value;

	value = REG_READ(IRDA_CTRL);
	if(polarity){
		value |= IRDA_POLARITY;
	}else{
		value &= ~IRDA_POLARITY;
	}
	REG_WRITE(IRDA_CTRL, value);
}

static void irda_set_clk(UINT16 clk)
{
	UINT32 value;

	value = REG_READ(IRDA_CTRL);
	value &= ~(CLK_DIVID_MASK << CLK_DIVID_POSI);
	value |= (clk << CLK_DIVID_POSI);
	REG_WRITE(IRDA_CTRL, value);
}

static UINT32 irda_get_key(void)
{
	UINT32 tmp,index;
	
	tmp = Recv_IR_Code;

	if((tmp&0xFFFF) != IR_UserCode){
		return 0xFF;
	}
	
	if((tmp&0xFF000000) & ((tmp&0xFF0000)<<8)){
		return 0xFF;
	}
	
	tmp = (tmp>>16)&0xFF;
	
	for(index=0; index<IR_CODE_SIZE; index++){
		if(tmp == IR_Code[index]){
			break;
		}
	}

	if(index == IR_CODE_SIZE){
		index=0xFF;
	}

//	IRDA_PRT("IR_Code is : %x, IR KEY is:%x\r\n", Recv_IR_Code,index);
	return index;
}

void irda_init(void)
{
	irda_gpio_config();
	
	intc_service_register(IRQ_IRDA, PRI_IRQ_IRDA, irda_isr); 
	
	sddev_register_dev(IRDA_DEV_NAME, &irda_op);
}

void irda_exit(void)
{
	sddev_unregister_dev(IRDA_DEV_NAME);
}

static UINT32 irda_ctrl(UINT32 cmd, void *param)
{
	UINT32 ret = IRDA_SUCCESS;

	switch(cmd){
		case IRDA_CMD_ACTIVE:
			irda_active(*(UINT8 *)param);
			break;
		case IRDA_CMD_SET_POLARITY:
			irda_set_polarity(*(UINT8 *)param);
			break;
		case IRDA_CMD_SET_CLK:
			irda_set_clk(*(UINT16 *)param);
			break;
		case IRDA_CMD_GET_KEY:
			*(UINT32 *)param = irda_get_key();
			break;
		default:
			ret = IRDA_FAILURE;
			break;
	}

	return ret;
}

void irda_isr(void)
{
	static UINT16 ValidFlag = 0;
	UINT32 irda_int;
	UINT32 end_int, right_int, repeat_int;

	irda_int = REG_READ(IRDA_INT);
	end_int = irda_int&IRDA_END_INT;
	right_int = irda_int&IRDA_RIGHT_INT;
	repeat_int = irda_int&IRDA_REPEAT_INT;
	
    if(end_int){
    	if (ValidFlag){
         	ValidFlag = 0x0;
		 	Recv_IR_Code = REG_READ(RX_FIFO_DOUT);
        }else{
			Recv_IR_Code=0xFFFFFFFF;
        }
			
		irda_int |= IRDA_END_INT;
    }
    
    if(right_int){
  	    ValidFlag = 1 ;
        irda_int |= IRDA_RIGHT_INT;
    }
    
    if(repeat_int){
        irda_int |= IRDA_REPEAT_INT;
    }

	REG_WRITE(IRDA_INT, irda_int);
}
