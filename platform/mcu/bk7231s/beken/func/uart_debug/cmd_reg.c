#include "include.h"
#include "uart_debug_pub.h"
#include "cmd_reg.h"
#include "mem_pub.h"
#include "str_pub.h"
#include "arm_arch.h"

int do_reg(cmd_tbl_t *cmdtp, int flag, int argc, char *const argv[])
{
    char cmd0 = 0;
    char cmd1 = 0;
    char cmd2 = 0;
    UINT8 fail = 0;
    UINT32 i;
    UINT32 arg_id = 1;
    UINT32 arg_cnt = argc;
    UINT32 regr[REG_RD_MAX_CNT] = {0};
    UINT32 regr_cnt = 0;
    UINT32 regw = 0;
    UINT32 regw_value = 0;

    /*step0, parameter conversion*/
    arg_cnt -= 1; /* CmdWord*/

    while(arg_cnt > 0)
    {
        cmd0 = argv[arg_id][0];
        cmd1 = argv[arg_id][1];

        switch(cmd0)
        {
        case '-':
        {
            arg_cnt -= 1; /* ''-''  */

            switch(cmd1)
            {
            case 'r':
                if(arg_cnt < 1)
                {
                    fail = 1;
                    break;
                }

                while(1)
                {
                    if((regr_cnt >= REG_RD_MAX_CNT)
                            || (0 == arg_cnt))
                    {
                        break;
                    }

                    cmd2 = argv[arg_id + 1][0];
                    if('-' == cmd2)
                    {
                        break;
                    }

                    regr[regr_cnt ++] = os_strtoul(argv[arg_id + 1], NULL, 16);
                    arg_id += 1;
                    arg_cnt -= 1;
                }

                break;

            case 'w':
                if(arg_cnt < 2)
                {
                    fail = 1;
                    break;
                }

                regw = os_strtoul(argv[arg_id + 1], NULL, 16);
                regw_value = os_strtoul(argv[arg_id + 2], NULL, 0);

                arg_cnt -= 2;
                arg_id += 2;
                break;

            default:
                fail = 1;
                break;
            }
        }
        break;

        default:
            fail = 1;
            break;
        }

        if(fail)
        {
            goto fail_exit;
        }
    }

    /*step1, parameter check*/
    if(regw & REG_MASK)
    {
        goto fail_exit;
    }

    for(i = 0; i < regr_cnt; i ++)
    {
        if(regr[i] & REG_MASK)
        {
            goto fail_exit;
        }
    }

    /*step2, handle*/
    if(regw)
    {
        REG_WRITE(regw, regw_value);
        CREG_PRT("           write register: 0x%x, value: 0x%x\r\n", regw, regw_value);
    }

    for(i = 0; i < regr_cnt; i ++)
    {
        CREG_PRT("           read register: 0x%x, value: 0x%x\r\n", regr[i], REG_READ(regr[i]));
    }

    return 0;

fail_exit:
    return 1;
}

// eof

