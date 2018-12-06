/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */


/* ARM exception IDs                                      */
#define  OS_CPU_ARM_EXCEPT_RESET                      0x00u
#define  OS_CPU_ARM_EXCEPT_UNDEF_INSTR                0x01u
#define  OS_CPU_ARM_EXCEPT_SWI                        0x02u
#define  OS_CPU_ARM_EXCEPT_PREFETCH_ABORT             0x03u
#define  OS_CPU_ARM_EXCEPT_DATA_ABORT                 0x04u
#define  OS_CPU_ARM_EXCEPT_ADDR_ABORT                 0x05u
#define  OS_CPU_ARM_EXCEPT_IRQ                        0x06u
#define  OS_CPU_ARM_EXCEPT_FIQ                        0x07u
#define  OS_CPU_ARM_EXCEPT_NBR                        0x08u

void aos_cpu_except_handler(int except_id) {

    switch (except_id) {
    case OS_CPU_ARM_EXCEPT_FIQ:

        IRQ_HDLR();
        break;

    case OS_CPU_ARM_EXCEPT_IRQ:
        IRQ_HDLR();

        break;

    case OS_CPU_ARM_EXCEPT_RESET:
        /* Insert code to handle a Reset exception               */
        while(1);

    case OS_CPU_ARM_EXCEPT_UNDEF_INSTR:
        /* Insert code to handle a Undefine Instruction exception */
        while(1);

    case OS_CPU_ARM_EXCEPT_SWI:
        /* Insert code to handle a Software exception             */
        while(1);

    case OS_CPU_ARM_EXCEPT_PREFETCH_ABORT:
        /* Insert code to handle a Prefetch Abort exception       */
        while(1);

    case OS_CPU_ARM_EXCEPT_DATA_ABORT:
        /* Insert code to handle a Data Abort exception           */
        while(1);

    case OS_CPU_ARM_EXCEPT_ADDR_ABORT:
        /* Insert code to handle a Address Abort exception        */
        while(1);
    default:

        while (1) {

            //printf("err aos_cpu_except_handler  out:%d\r\n",except_id);
        }

    }
}

