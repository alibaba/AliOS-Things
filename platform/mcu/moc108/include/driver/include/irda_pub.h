#ifndef _IRDA_PUB_H_
#define _IRDA_PUB_H_

#define IRDA_FAILURE                (1)
#define IRDA_SUCCESS                (0)

#define IRDA_DEV_NAME                "irda"

#define IRDA_CMD_MAGIC              (0xe290000)
enum
{
    IRDA_CMD_ACTIVE = IRDA_CMD_MAGIC + 1,
    IRDA_CMD_SET_POLARITY,
    IRDA_CMD_SET_CLK,
    IRDA_CMD_GET_KEY
};

/*******************************************************************************
* Function Declarations
*******************************************************************************/
void irda_init(void);
void irda_exit(void);
void irda_isr(void);
#endif //_IRDA_PUB_H_
