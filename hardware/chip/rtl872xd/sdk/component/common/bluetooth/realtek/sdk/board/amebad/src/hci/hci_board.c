/**
 * Copyright (c) 2017, Realsil Semiconductor Corporation. All rights reserved.
 *
 */

#include <stdio.h>
#include <string.h>
#include "os_sched.h"
#include "os_pool.h"
#include "os_sync.h"
#include "os_mem.h"

#include "hci_tp.h"
#include "hci_uart.h"
#include "bt_types.h"
#include "trace_app.h"

#include "bt_board.h"
#include "hci_board.h"
#include "hci_process.h"
#include "build_info.h"
//#include "bt_intf.h"
#include "wifi_conf.h" //for wifi_disable_powersave and wifi_resume_powersave

#define hci_board_32reg_set(addr, val) HAL_WRITE32(addr, 0, val)
#define hci_board_32reg_read(addr) HAL_READ32(addr, 0)

#define BT_EFUSE_TABLE_LEN             0x20


#define BT_CONFIG_SIGNATURE             0x8723ab55
#define BT_CONFIG_HEADER_LEN            6

typedef struct {
    u32 IQK_xx;
    u32 IQK_yy;
    u32 IDAC;
    u32 QDAC;
}BT_Cali_TypeDef;
BT_Cali_TypeDef g_iqk_data;

const BAUDRATE_MAP baudrates[] =
{
    {0x0000701d, 115200},
    {0x0252C00A, 230400},
    {0x03F75004, 921600},
    {0x05F75004, 921600},
    {0x00005004, 1000000},
    {0x04928002, 1500000},
    {0x00005002, 2000000},
    {0x0000B001, 2500000},
    {0x04928001, 3000000},
    {0x052A6001, 3500000},
    {0x00005001, 4000000},
};
unsigned int baudrates_length = sizeof(baudrates) / sizeof(BAUDRATE_MAP);

static uint32_t hci_tp_baudrate;
uint8_t  hci_tp_lgc_efuse[BT_EFUSE_TABLE_LEN];
uint8_t  hci_tp_phy_efuse[19];


static uint32_t cal_bit_shift(uint32_t Mask)
{
    uint32_t i;
    for(i=0; i<31;i++)
    {
        if(((Mask>>i) & 0x1)==1)
            break;
    }
    return (i);
}

static void set_reg_value(uint32_t reg_address, uint32_t Mask , uint32_t val)
{
    uint32_t shift = 0;
    uint32_t data = 0;
    data = hci_board_32reg_read(reg_address);
    shift = cal_bit_shift(Mask);
    data = ((data & (~Mask)) | (val << shift));
    hci_board_32reg_set(reg_address, data);
    data = hci_board_32reg_read(reg_address);
}

bool hci_rtk_parse_config(uint8_t *config_buf, uint16_t config_len, uint8_t *efuse_buf)
{

    uint32_t signature;
    uint16_t payload_len;
    uint16_t entry_offset;
    uint16_t entry_len;
    uint8_t *p_entry;
    uint8_t *p;
    uint8_t *p_len;
    uint8_t i;
    uint16_t tx_flatk;

    //enter the  config_len

    p = config_buf;
    p_len = config_buf + 4;

extern uint32_t bt_flatk_8721d(uint16_t txgain_flatk);

    LE_STREAM_TO_UINT32(signature, p);
    LE_STREAM_TO_UINT16(payload_len, p);

    if (signature != BT_CONFIG_SIGNATURE)
    {
        HCI_PRINT_ERROR1("hci_rtk_parse_config: invalid signature 0x%08x", signature);

        return false;
    }

    if (payload_len != config_len - BT_CONFIG_HEADER_LEN)
    {
        HCI_PRINT_WARN2("hci_rtk_parse_config: invalid len, stated %u, calculated %u",
                         payload_len, config_len - BT_CONFIG_HEADER_LEN);
        LE_UINT16_TO_STREAM(p_len, config_len - BT_CONFIG_HEADER_LEN);  //just avoid the length is not coreect
        /* FIX the len */
       // return false;
    }
#define BT_EFUSE_BLOCK1_OFFSET   0x06
#define BT_EFUSE_BLOCK2_OFFSET   0x0c
#define BT_EFUSE_BLOCK3_OFFSET   0x12

    p_entry = config_buf + BT_CONFIG_HEADER_LEN;

    while (p_entry < config_buf + config_len)
    {
        p = p_entry;
        LE_STREAM_TO_UINT16(entry_offset, p);
        LE_STREAM_TO_UINT8(entry_len, p);
        p_entry = p + entry_len;

        switch (entry_offset)
        {
        case 0x000c:
            if ((rltk_wlan_is_mp())||(!CHECK_SW(EFUSE_SW_UPPERSTACK_SWITCH)))
            {
                //default use the 115200
                hci_board_debug("set hci baudrate 115200\n");
                memcpy(p,&(baudrates[0].bt_baudrate),4);
            }
            LE_STREAM_TO_UINT32(hci_tp_baudrate, p);

#if 0
            if (entry_len >= 12)
            {
                p_hci_rtk->hw_flow_cntrl |= 0x80;   /* bit7 set hw flow control */
                p += 8;

                if (*p & 0x04)  /* offset 0x18, bit2 */
                {
                    p_hci_rtk->hw_flow_cntrl |= 1;  /* bit0 enable hw flow control */
                }
            }
            HCI_PRINT_TRACE2("hci_rtk_parse_config: baudrate 0x%08x, hw flow control 0x%02x",
                             p_hci_rtk->baudrate,p_hci_rtk->hw_flow_cntrl);
            //hci_board_debug("hci_rtk_parse_config: baudrate 0x%08x\n", p_hci_rtk->baudrate);
#endif
            break;
        case 0x0018:
            if ((rltk_wlan_is_mp())||(!CHECK_SW(EFUSE_SW_UPPERSTACK_SWITCH)))
            {
                //hci_board_debug("hci uart flow ctrl: 0x%02x\n", p[0]);
                p[0] =p[0] & (~ BIT2) ;
                hci_board_debug("close hci uart flow ctrl: 0x%02x\n", p[0]);
                //close the flow control
            }
            break;
        case 0x0030:
            if (entry_len == 6)
            {
                if ((efuse_buf[0] != 0xff) && (efuse_buf[1] != 0xff))
                {
                    //memcpy(p,&efuse_buf[0],6);
                    /*
                    hci_board_debug("\r\nBT ADDRESS:\r\n");
                    for(int i = 0 ;i <6;i ++)
                    {
						p[i] = efuse_buf[5-i];
                        hci_board_debug("%02x:",efuse_buf[i]);
                    }
                    hci_board_debug("\r\n");*/
                    for(int i = 0 ;i <6;i ++)
                    { 
                        p[i] = efuse_buf[5-i]; 
                    }

                    hci_board_debug("\nBT ADDRESS: %02x:%02x:%02x:%02x:%02x:%02x\r\n",
                        efuse_buf[0], efuse_buf[1], efuse_buf[2],
                        efuse_buf[3], efuse_buf[4], efuse_buf[5]);
				}
                else
                {
                    hci_board_debug("hci_rtk_parse_config: BT ADDRESS  %02x %02x %02x %02x %02x %02x, use the defaut config\n",
                              p[0], p[1], p[2], p[3], p[4], p[5]);
                }
            }
            break;
        case 0x194:
            if(efuse_buf[LEFUSE(0x196)]== 0xff)
            {
                if(!(hci_tp_phy_efuse[2]& BIT0))
                {
                    //0
                    tx_flatk=hci_tp_phy_efuse[0xa] | hci_tp_phy_efuse[0xb]<<8;
                    bt_flatk_8721d(tx_flatk);
                    hci_board_debug("\r\n WRITE  physical FLATK=tx_flatk=%x \r\n", tx_flatk);
                }

                break;
            }
            else
            {

                p[0]= efuse_buf[LEFUSE(0x196)];
                if(efuse_buf[LEFUSE(0x196)] & BIT1)
                {
                    p[1]= efuse_buf[LEFUSE(0x197)];
                }

                if(efuse_buf[LEFUSE(0x196)] & BIT2)
                {
                    p[2]= efuse_buf[LEFUSE(0x198)];
                    p[3]= efuse_buf[LEFUSE(0x199)];
                    
                    tx_flatk=efuse_buf[LEFUSE(0x198)] | efuse_buf[LEFUSE(0x199)]<<8;
                    bt_flatk_8721d(tx_flatk);
                    hci_board_debug("\r\n WRITE logic FLATK=tx_flatk=%x \r\n", tx_flatk);

                }
                else
                {
                    if(!(hci_tp_phy_efuse[2]& BIT0))
                    {
                        //0
                        tx_flatk=hci_tp_phy_efuse[0xa] | hci_tp_phy_efuse[0xb]<<8;

                        bt_flatk_8721d(tx_flatk);
                        hci_board_debug("\r\n WRITE  physical FLATK=tx_flatk=%x \r\n", tx_flatk);
                    }

                }

                if(efuse_buf[LEFUSE(0x196)] & BIT5)
                {
                    p[4]= efuse_buf[LEFUSE(0x19a)];
                    p[5]= efuse_buf[LEFUSE(0x19b)];
                }
            }

            if(!CHECK_SW(EFUSE_SW_DRIVER_DEBUG_LOG))
            {
                for(i = 0;i < entry_len;i ++)
                {
                    hci_board_debug("\r\n logic efuseMap[%x] = %x\r\n",0x196+i, p[i]);
                }
            }
            break;
        case 0x19f:
            for(i = 0;i <entry_len;i ++)
            {
                if(efuse_buf[LEFUSE(0x19c+i)] != 0xff)
                {
                    p[i]= efuse_buf[LEFUSE(0x19c+i)];
                    if(!CHECK_SW(EFUSE_SW_DRIVER_DEBUG_LOG))
                    {
                        hci_board_debug("\r\n logic efuseMap[%x] = %x\r\n",0x19c+i, p[i]);
                    }
                }
            }
            break;
        case 0x1A4:
            for(i = 0;i < entry_len;i ++)
            {
                if(efuse_buf[LEFUSE(0x1a2+i)] != 0xff)
                {
                    p[i]= efuse_buf[LEFUSE(0x1A2+i)];
                    if(!CHECK_SW(EFUSE_SW_DRIVER_DEBUG_LOG))
                    {
                        hci_board_debug("\r\n logic efuseMap[%x] = %x\r\n",0x1A2+i, p[i]);
                    }
                }
            }
            break;
        default:
            HCI_PRINT_TRACE2("hci_rtk_parse_config: entry offset 0x%04x, len 0x%02x",
                             entry_offset, entry_len);
            break;
        }
    }
    return true;
}


int hci_flash_stream_read(uint8_t *address, uint32_t len, uint8_t * data)
{
    memcpy(data, (const void *)address, len);
    return 1;
}

uint8_t *hci_find_patch_address(void)
{
    if(!CHECK_SW(EFUSE_SW_DRIVER_DEBUG_LOG))
    {
    hci_board_debug("\nWe use DEBUG VAL:0x%x\n", HAL_READ32(0x08000000, 0x3028));
    }

    if(CHECK_SW(EFUSE_SW_USE_FLASH_PATCH))
    {
        //1
        return (uint8_t *)rltk_bt_get_patch_code();
    }
    else if (ota_get_cur_index() == OTA_INDEX_1)
    {
        hci_board_debug("\nWe use BT ROM OTA2 PATCH ADDRESS:0x%x\n", MERGE_PATCH_ADDRESS_OTA2);
        HCI_PRINT_INFO1("\nWe use BT ROM OTA2 PATCH ADDRESS:0x%x\n", MERGE_PATCH_ADDRESS_OTA2);
        return (uint8_t *)MERGE_PATCH_ADDRESS_OTA2;
    }
    else
    {
        hci_board_debug("\nWe use BT ROM OTA1 PATCH ADDRESS:0x%x\n", MERGE_PATCH_ADDRESS_OTA1);
        HCI_PRINT_INFO1("\nWe use BT ROM OTA1 PATCH ADDRESS:0x%x\n", MERGE_PATCH_ADDRESS_OTA1);
        return (uint8_t *)MERGE_PATCH_ADDRESS_OTA1;
    }
}


/** @brief configuration */
unsigned char rtlbt_init_config[] =
{
    0x55, 0xab, 0x23, 0x87,

	0x19, 0x00,
    //0x10, 0x00,
    0x30, 0x00, 0x06, 0x11, 0x28, 0x36, 0x12, 0x51, 0x89, /* BT MAC address */
    //  0x07, 0x00,
    0x0c, 0x00, 0x04, 0x04, 0x50, 0xF7, 0x03, /* Baudrate 921600 */
	0x18, 0x00, 0x01,0x5c, /* flow control */
//efuse value
    0x94, 0x01, 0x06, 0x08, 0x00, 0x00, 0x00,0x27, 0x07,

    0x9f, 0x01, 0x05, 0x23, 0x23 , 0x23, 0x23,0x59,

    0xA4, 0x01, 0x04, 0xFE, 0xFE , 0xFE, 0xFE
};

extern unsigned char rtlbt_config[];
extern unsigned int  rtlbt_config_len;

uint8_t *hci_rtk_combine_config(void)
{
#define HCI_CONFIG_HEAD 6
    uint16_t config_length = rtlbt_config_len + sizeof(rtlbt_init_config) - HCI_CONFIG_HEAD;

    uint8_t *full_config_buf = os_mem_zalloc(RAM_TYPE_DATA_ON, config_length);
    uint8_t *p_len = full_config_buf+4;

    memcpy(full_config_buf,rtlbt_init_config, sizeof(rtlbt_init_config));
    memcpy(full_config_buf+sizeof(rtlbt_init_config),rtlbt_config+HCI_CONFIG_HEAD, rtlbt_config_len-HCI_CONFIG_HEAD);

    HCI_PRINT_WARN1("hci_rtk_combine_config: invalid len, calculated %u",   config_length);

    LE_UINT16_TO_STREAM(p_len, config_length);  //just avoid the length is not coreect
    if(!CHECK_SW(EFUSE_SW_DRIVER_DEBUG_LOG))
    {
        hci_board_debug("hci_rtk_combine_config: all config length is %x\r\n",config_length);
        for(uint8_t i=0;i< config_length;i++)
        {
            hci_board_debug("\n:%02x", full_config_buf[i]);
        }
    }
    return full_config_buf;
}

uint16_t fix_config_len(void)
{
    return (rtlbt_config_len + sizeof(rtlbt_init_config) - HCI_CONFIG_HEAD);
}


bool hci_rtk_find_patch(uint8_t bt_hci_chip_id)
{
    extern unsigned int  rtlbt_fw_len;
    extern unsigned char rtlbt_config[];
    extern unsigned int  rtlbt_config_len;
    const uint8_t no_patch_sing[4]= {0xFF, 0xFF, 0xFF, 0xFF};
    uint8_t tmp_patch_head[8];


    uint8_t            *fw_buf;
    uint8_t            *config_buf;
    uint16_t            fw_len;
    uint32_t            fw_offset;
    uint16_t            config_len;
    uint32_t            lmp_subversion;;
    uint16_t            mp_num_of_patch=0;
    uint16_t            fw_chip_id = 0;
    uint8_t             i;

    uint8_t *p_merge_addr = hci_find_patch_address();

    hci_flash_stream_read(p_merge_addr ,8, tmp_patch_head);

    hci_flash_stream_read(p_merge_addr+8 ,4, (uint8_t *)&lmp_subversion);
    //LE_ARRAY_TO_UINT32(lmp_subversion, (p_merge_addr+8));

//if single patch or merged patch
    
    if(!memcmp(tmp_patch_head, "Realtech", sizeof("Realtech")-1))
    {
        hci_flash_stream_read(p_merge_addr+0x0c ,2, (uint8_t *)&mp_num_of_patch);
        //LE_ARRAY_TO_UINT16(mp_num_of_patch, p_merge_addr+0x0c);

        if(mp_num_of_patch == 1)
        {
            hci_flash_stream_read(p_merge_addr+0x0e +2*mp_num_of_patch ,2, (uint8_t *)&fw_len);
            //LE_ARRAY_TO_UINT16(fw_len, p_merge_addr+0x0e +2*mp_num_of_patch );
            hci_flash_stream_read(p_merge_addr+0x0e +4*mp_num_of_patch ,4, (uint8_t *)&fw_offset);
            //LE_ARRAY_TO_UINT32(fw_offset, p_merge_addr+0x0e +4*mp_num_of_patch);
            if (rltk_wlan_is_mp())
            {
                hci_board_debug("\n fw_chip_id patch =%x,mp_num_of_patch=%x \n", fw_chip_id,mp_num_of_patch);
                hci_board_debug("\n  lmp_subversion=%x , fw_len =%x, fw_offset = %x \n", lmp_subversion, fw_len, fw_offset);
            }
        }
        else
        {
            for(i = 0 ;i<mp_num_of_patch; i++)
            {
                hci_flash_stream_read(p_merge_addr+0x0e + 2*i ,2, (uint8_t *)&fw_chip_id);
               // LE_ARRAY_TO_UINT16(fw_chip_id, p_merge_addr+0x0e + 2*i);

                if(fw_chip_id == bt_hci_chip_id)
                {
                    hci_flash_stream_read(p_merge_addr+0x0e +2*mp_num_of_patch + 2*i ,2, (uint8_t *)&fw_len);
                    //LE_ARRAY_TO_UINT16(fw_len, p_merge_addr+0x0e +2*mp_num_of_patch + 2*i);
                    hci_flash_stream_read(p_merge_addr+0x0e +4*mp_num_of_patch + 4*i ,4, (uint8_t *)&fw_offset);
                    //LE_ARRAY_TO_UINT32(fw_offset, p_merge_addr+0x0e +4*mp_num_of_patch + 4*i);
                    if(!CHECK_SW(EFUSE_SW_DRIVER_DEBUG_LOG))
                    {
                        //0
                        hci_board_debug("\n fw_chip_id patch =%x,mp_num_of_patch=%x \n", fw_chip_id,mp_num_of_patch);
                        hci_board_debug("\n  lmp_subversion=%x , fw_len =%x, fw_offset = %x \n", lmp_subversion, fw_len, fw_offset);
                    }
                    break;
                }
            }

            if(i >= mp_num_of_patch)
            {
                hci_board_debug("\n ERROR:no match patch\n");
                return false;
            }
        }

        fw_buf = os_mem_zalloc(RAM_TYPE_DATA_ON, fw_len);
        if(fw_buf == NULL)
        {
            hci_board_debug("\n fw_buf ,malloc %d byte fail, \n", fw_len);
            return false;
        }
        else
        {
            memcpy(fw_buf,p_merge_addr+fw_offset, fw_len);
            LE_UINT32_TO_ARRAY(fw_buf+fw_len-4,lmp_subversion);
        }
    }
    else if(!memcmp(tmp_patch_head, no_patch_sing, sizeof(no_patch_sing)))
    {
        hci_board_debug("\nWrong patch head %x %x %x %x\n",p_merge_addr[0], p_merge_addr[1], p_merge_addr[2], p_merge_addr[3]);
        return false;
    }
    else
    {
        hci_board_debug("\nwe use single patch\n");
        if(p_merge_addr != (uint8_t *)rltk_bt_get_patch_code())
        {
            hci_board_debug("\nnot support single patch on rom\n");
            return false;
        }

        fw_len = rtlbt_fw_len ;
        fw_buf = os_mem_zalloc(RAM_TYPE_DATA_ON, fw_len);
        memcpy(fw_buf,(const void *)rltk_bt_get_patch_code(), fw_len);
    }

    config_buf = rtlbt_init_config;
    config_len = sizeof(rtlbt_init_config);
    //config_len = 0;

    if (config_len != 0)
    {
        if (hci_rtk_parse_config(config_buf, config_len, hci_tp_lgc_efuse) == false)
        {
            return false;
        }
        else
        {
            config_buf = hci_rtk_combine_config();
            config_len = fix_config_len();
        }
    }


    if(!CHECK_SW(EFUSE_SW_DRIVER_DEBUG_LOG))
    {
        hci_board_debug("\nWe use lmp_subversion=%x fw_buf=%x, fw_len = %x, config_buf = %x,config_len= %x, baudrate 0x%x\n", lmp_subversion,fw_buf, fw_len, config_buf, config_len, hci_tp_baudrate);
    }

    hci_set_patch(fw_buf, fw_len, config_buf, config_len, hci_tp_baudrate);

    return true;
}

void bt_read_efuse(void)
{
    u32 Idx = 0;
    u32 ret = 0;
    uint8_t *p_buf;

    //read logic efuse
    p_buf = os_mem_zalloc(RAM_TYPE_DATA_ON, 1024);
    ret = EFUSE_LMAP_READ(p_buf);
    if (ret == _FAIL)
    {
        hci_board_debug("EFUSE_LMAP_READ fail \n");
    }
    memcpy(hci_tp_lgc_efuse, p_buf+0x190,BT_EFUSE_TABLE_LEN);

    if(!CHECK_SW(EFUSE_SW_DRIVER_DEBUG_LOG))
    {
        //0
        hci_board_debug("\r\n==logic_efuse:==\r\n ");
        for (Idx = 0; Idx < BT_EFUSE_TABLE_LEN; Idx++)
        {
            hci_board_debug("\n%x:", hci_tp_lgc_efuse[Idx]);
        }

    }

    //read physical efuse
    for (Idx = 0; Idx < 16; Idx++)
    {
        EFUSE_PMAP_READ8(0, 0x120 + Idx, hci_tp_phy_efuse + Idx, L25EOUTVOLTAGE);
        if ((Idx == 7) && (hci_tp_phy_efuse[Idx] == 0))
        {
            hci_tp_phy_efuse[Idx] = 0x13;
        }
    }
    EFUSE_PMAP_READ8(0, 0x1FD, hci_tp_phy_efuse + 16, L25EOUTVOLTAGE);
    EFUSE_PMAP_READ8(0, 0x1FE, hci_tp_phy_efuse + 17, L25EOUTVOLTAGE);
    EFUSE_PMAP_READ8(0, 0x1FF, hci_tp_phy_efuse + 18, L25EOUTVOLTAGE);

    if(!CHECK_SW(EFUSE_SW_DRIVER_DEBUG_LOG))
    {
        //0
        hci_board_debug("\r\n==bt phy_efuse 0x120~0x12F:==\r\n ");
        for (Idx = 0; Idx < 16; Idx++)
        {
            hci_board_debug("\n%x:", hci_tp_phy_efuse[Idx]);
        }
        hci_board_debug("\r\n==bt phy_efuse 0x1FD~0x1FF:==\r\n ");
        for (Idx = 16; Idx < 19; Idx++)
        {
            hci_board_debug("\n%x:", hci_tp_phy_efuse[Idx]);
        }
    }
    os_mem_free(p_buf);
}


void bt_power_on(void)
{
    set_reg_value(0x40000000,BIT0 | BIT1, 3);
}

void bt_power_off(void)
{
    set_reg_value(0x40000000,BIT0 | BIT1, 0);
    rltk_coex_bt_enable(0);
    if(!rltk_wlan_is_mp()){
        //wifi_resume_powersave();
    }
}

void bt_change_gnt_bt_only(void)
{
    set_reg_value(0x40080764, BIT9 | BIT10,3 );
}

void bt_change_gnt_wifi_only(void)
{
    set_reg_value(0x40080764, BIT9 | BIT10,1);
}

void bt_reset(void)
{
    if(!rltk_wlan_is_mp()) {
        wifi_disable_powersave();
    }

    if(!CHECK_SW(EFUSE_SW_BT_FW_LOG))
    {
        //0
        //open bt log pa16
		hci_board_debug("BT FW LOG OPEN\n");
        //close please change efuse:EFUSE wmap 1a1 1 fe
        set_reg_value(0x48000440,BIT0 |BIT1 | BIT2 | BIT3 | BIT4, 17);
    }

    hci_board_debug("BT Reset...\n");
    //bt power
    bt_power_on();

    os_delay(5);

    //isolation
    set_reg_value(0x40000000,BIT16, 0);
    os_delay(5);

    //BT function enable
    set_reg_value(0x40000204,BIT24, 0);
    os_delay(5);

    set_reg_value(0x40000204,BIT24, 1);
    os_delay(50);

    //BT clock enable
    set_reg_value(0x40000214, BIT24, 1);
    os_delay(5);
}

bool hci_board_init()
{
    bool ret=false;
	
    if(!(wifi_is_up(RTW_STA_INTERFACE) || wifi_is_up(RTW_AP_INTERFACE))) {
        hci_board_debug("\nWIFI is off !Please restart BT after WIFI on!\n");
        return false;
    }
	
    HCI_PRINT_INFO1("hci_tp_open, this cut is AmebaD %X CUT",SYSCFG_CUTVersion()+10);

    if (rltk_wlan_is_mp())
    {
        hci_board_debug("\r\n==========this is BT MP DRIVER===========,\r\n this cut is AmebaD %X CUT\r\n",SYSCFG_CUTVersion()+10);
    }
    else if(!CHECK_SW(EFUSE_SW_DRIVER_DEBUG_LOG))
    {
        hci_board_debug("\r\n***********this is BT NORMAL DRIVER*******,\r\n this cut is AmebaD %X CUT\r\n",SYSCFG_CUTVersion()+10);
    }
    


    hci_board_debug("\r\nBT BUILD Date: %s \r\n",UTS_VERSION);
    bt_read_efuse();

    return true;
}

void hci_normal_start(void)
{
    if(!rltk_wlan_is_mp())
    {
	rltk_coex_bt_enable(1);
    }
}

void bt_write_phy_efuse_value(void)
{
    hci_tp_phy_efuse[0] = 0;
    hci_tp_phy_efuse[1] = 0x9e;
    hci_tp_phy_efuse[2] = 0xfe;
    hci_tp_phy_efuse[3] = g_iqk_data.IQK_xx & 0xff;
    hci_tp_phy_efuse[4] = (g_iqk_data.IQK_xx >> 8) & 0xff;
    hci_tp_phy_efuse[5] = g_iqk_data.IQK_yy & 0xff;
    hci_tp_phy_efuse[6] = (g_iqk_data.IQK_yy >> 8) & 0xff;
    //hci_tp_phy_efuse[7] = 0;//(g_iqk_data.IQK_yy >> 8) & 0xff;
    hci_tp_phy_efuse[8] = 0x19;//(g_iqk_data.IQK_yy >> 8) & 0xff;
    
    hci_tp_phy_efuse[12] = (g_iqk_data.QDAC) & 0xff;
    hci_tp_phy_efuse[13] = (g_iqk_data.IDAC) & 0xff;

    for(int i=0;i<0x10; i++)
    {
        hci_board_debug("\r\n write physical efuse 0x%x =0x%02x",0x120+i, hci_tp_phy_efuse[i]);
        EFUSE_PMAP_WRITE8(0, 0x120 + i, hci_tp_phy_efuse[i], L25EOUTVOLTAGE);
    }
}

#if 0
void hci_uart_out(void)
{
    hci_board_debug("HCI UART OUT OK: PA2 TX, PA4 RX\r\n");
    HAL_WRITE32(0x48000000, 0x5f0, 0x00000202);
    //PA2 TX
    HAL_WRITE32(0x48000000, 0x408, 0x00005C11);
    //PA4 RX
    HAL_WRITE32(0x48000000, 0x410,
            0x00005C11);

    bt_change_gnt_bt_only();
}
#endif

bool hci_board_complete(void)
{
   // uint8_t orignal_hci_tp_phy_efuse[0x10];
  /// hci_uart_out();
   //return false;


    if(rltk_wlan_is_mp())
    {
#ifdef FT_MODE
        uint8_t TestItem;
        static bool write_efuse_ok=false;
        hci_board_debug("\r\n=================original ========================\r\n");
        for (int Idx = 0; Idx < 16; Idx++)
        {
            EFUSE_PMAP_READ8(0, 0x120 + Idx, orignal_hci_tp_phy_efuse + Idx, L25EOUTVOLTAGE);
            hci_board_debug("\r\n original physical efuse 0x%x =0x%02x",0x120+Idx, orignal_hci_tp_phy_efuse[Idx]);
            if(orignal_hci_tp_phy_efuse[Idx] != 0xff)
            {
                write_efuse_ok = true;
            }
        }

        hci_board_debug("\r\n=================will write physical efuse ========================\r\n");
        for(int i=0;i<0x10; i++)
        {
            hci_board_debug("\r\n 0x%x =0x%02x",0x120+i, hci_tp_phy_efuse[i]);
            //EFUSE_PMAP_WRITE8(0, 0x120 + i, hci_tp_phy_efuse[i], L25EOUTVOLTAGE);
        }

        /*TODO:GNT_BT TO WIFI */
        bt_change_gnt_wifi_only();
        hci_board_debug("EFUSE_SW_MP_MODE: UPPERSTACK NOT UP \r\nGNT_BT %x...\n", HAL_READ32(0x40080000, 0x0764));
      	while ( 1 )
        {
            if ( GPIO_ReadDataBit( _PA_5 ) )
            {
                os_delay(2);
                if ( GPIO_ReadDataBit( _PA_5 ) )                                 /* still input high */
                {
                    GPIO_WriteBit( _PA_13, 0 );
                    TestItem	= 0;
                    TestItem	= GPIO_ReadDataBit( _PB_7 );             /* bit0 */
                    TestItem	|= (GPIO_ReadDataBit( _PA_17 ) << 1);   /* bit1 */
                    TestItem	|= (GPIO_ReadDataBit( _PA_19 ) << 2);    /* bit2 */
                    TestItem	|= (GPIO_ReadDataBit( _PA_20 ) << 3);    /* bit3 */
                    TestItem	|= (GPIO_ReadDataBit( _PA_6 ) << 4);     /* bit4 */
                    hci_board_debug( "TestItem is %d!!!\n", TestItem );
                    switch ( TestItem )
                    {
                        case 13:
                            if(write_efuse_ok == false)
                            {
                                bt_write_phy_efuse_value();
                                write_efuse_ok=true;
                            }
                            else
                            {
                                hci_board_debug("ERROR:phy_efuse has been write!!!,please check the  phyfuse value\n");
                                /*TODO, tell the GPIO*/
                            }
                            break;

                        default:
                            break;
                    }
                    hci_board_debug( "trx done\n" );
                    GPIO_WriteBit( _PA_13, 1);
                }
            }
            os_delay(15);
        }
#endif
        return false;
    }

    if(!CHECK_SW(EFUSE_SW_UPPERSTACK_SWITCH))
    {
        //0
        hci_board_debug("Not Start upperStack, normal test\n");
        return false;
    }

    hci_board_debug("Start upperStack\n");

    return true;
}

//IQK LOK ABOUT

bool bt_iqk_efuse_valid(BT_Cali_TypeDef  *bt_iqk_data)
{

    if((hci_tp_phy_efuse[1] & BIT0))
    {
        //1
        hci_board_debug("bt_iqk_efuse_valid: no data\r\n");
        return false;
    }
    else
    {
        //0
        bt_iqk_data->IQK_xx = hci_tp_phy_efuse[3] | hci_tp_phy_efuse[4] <<8;
        bt_iqk_data->IQK_yy = hci_tp_phy_efuse[5] | hci_tp_phy_efuse[6] <<8;
        bt_iqk_data->QDAC = hci_tp_phy_efuse[0x0c];
        bt_iqk_data->IDAC = hci_tp_phy_efuse[0x0d];
        hci_board_debug("physical efuse: has data hci_tp_phy_efuse[1]= %x \r\n", hci_tp_phy_efuse[1]);
        return true;
    }
}

bool bt_iqk_logic_efuse_valid(BT_Cali_TypeDef  *bt_iqk_data)
{
    if(((hci_tp_lgc_efuse[0x16] == 0xff) && (hci_tp_lgc_efuse[0x17] == 0xff))||
            ((hci_tp_lgc_efuse[0x18] == 0xff)&&(hci_tp_lgc_efuse[0x19] == 0xff))||
            (hci_tp_lgc_efuse[0x1a] == 0xff) ||
            (hci_tp_lgc_efuse[0x1b] == 0xff) )
    {
        hci_board_debug("bt_iqk_logic_efuse_valid: no data\r\n");
        return false;
    }
    else
    {
        bt_iqk_data->IQK_xx = hci_tp_lgc_efuse[0x16] | hci_tp_lgc_efuse[0x17] <<8;
        bt_iqk_data->IQK_yy = hci_tp_lgc_efuse[0x18] | hci_tp_lgc_efuse[0x19] <<8;
        bt_iqk_data->QDAC = hci_tp_lgc_efuse[0x1a];
        bt_iqk_data->IDAC = hci_tp_lgc_efuse[0x1b];
        hci_board_debug("logic efuse: has data\r\n");
        return true;
    }
}

void bt_dump_iqk(BT_Cali_TypeDef *iqk_data)
{
    hci_board_debug("bt_iqk_dump:    DUMP,\r\n");
    hci_board_debug("the IQK_xx  data is 0x%x,\r\n", iqk_data->IQK_xx);
    hci_board_debug("the IQK_yy  data is 0x%x,\r\n", iqk_data->IQK_yy);
    hci_board_debug("the QDAC  data is 0x%x,\r\n", iqk_data->QDAC);
    hci_board_debug("the IDAC  data is 0x%x,\r\n", iqk_data->IDAC);
}

extern uint32_t bt_iqk_8721d(BT_Cali_TypeDef *cal_data,BOOLEAN store);
extern uint32_t bt_lok_write(uint32_t lok_xx, uint32_t lok_yy);
extern uint32_t bt_dck_write(uint32_t q_dck, uint32_t i_dck);

bool bt_check_iqk(void)
{
    BT_Cali_TypeDef     bt_iqk_data;

    if(!(hci_tp_lgc_efuse[LEFUSE(0x1A1)] & BIT0))
    //if(!CHECK_SW(EFUSE_SW_USE_LOGIC_EFUSE))
    {
        //0
        hci_board_debug("\r\n%s: USE FIX LOGIC EFUSE\r\n",__FUNCTION__);
        if (bt_iqk_logic_efuse_valid(&bt_iqk_data))
        {
            bt_dump_iqk(&bt_iqk_data);
            bt_lok_write(bt_iqk_data.IDAC, bt_iqk_data.QDAC);
            hci_tp_phy_efuse[0] = 0;
            hci_tp_phy_efuse[1] =hci_tp_phy_efuse[1] & (~BIT0);
            //hci_tp_phy_efuse[1] = 0xfe;
            //hci_tp_phy_efuse[2] = 0xff;
            hci_tp_phy_efuse[3] = bt_iqk_data.IQK_xx & 0xff;
            hci_tp_phy_efuse[4] = (bt_iqk_data.IQK_xx >> 8) & 0xff;
            hci_tp_phy_efuse[5] = bt_iqk_data.IQK_yy & 0xff;
            hci_tp_phy_efuse[6] = (bt_iqk_data.IQK_yy >> 8) & 0xff;
            return true;
        }
		hci_board_debug("\r\n%s: LOGIC EFUSE HAS NO DATA\r\n",__FUNCTION__);
        return false;
    }

    if (bt_iqk_efuse_valid(&bt_iqk_data))
    {
        if(hci_tp_phy_efuse[0]!=0)
        {
            bt_dck_write(hci_tp_phy_efuse[0x0e], hci_tp_phy_efuse[0x0f]);
        }
        else
        {
            hci_board_debug("\r\nhci_tp_phy_efuse[0]=0,\r\n");
        }
        bt_dump_iqk(&bt_iqk_data);
        bt_lok_write(bt_iqk_data.IDAC, bt_iqk_data.QDAC);
        return true;
    }

    if (bt_iqk_logic_efuse_valid(&bt_iqk_data))
    {
        bt_dump_iqk(&bt_iqk_data);
        bt_lok_write(bt_iqk_data.IDAC, bt_iqk_data.QDAC);
        hci_tp_phy_efuse[0] = 0;
        hci_tp_phy_efuse[1] =hci_tp_phy_efuse[1] & (~BIT0);
        //hci_tp_phy_efuse[1] = 0xfe;
        //hci_tp_phy_efuse[2] = 0xff;
        hci_tp_phy_efuse[3] = bt_iqk_data.IQK_xx & 0xff;
        hci_tp_phy_efuse[4] = (bt_iqk_data.IQK_xx >> 8) & 0xff;
        hci_tp_phy_efuse[5] = bt_iqk_data.IQK_yy & 0xff;
        hci_tp_phy_efuse[6] = (bt_iqk_data.IQK_yy >> 8) & 0xff;
        return true;
    }
    else
    {
        hci_board_debug("bt_check_iqk:  NO IQK LOK DATA need start LOK,\r\n");
        //bt_change_gnt_wifi_only();
        //bt_adda_dck_8721d();
        reset_iqk_type();
        return false;
    }
    //return true;
}

bool hci_start_iqk(void)
{
    u32 ret = 0;
   //bt_change_gnt_wifi_only();

    if(rltk_wlan_is_mp())  //JUST FOR DEBUG
    {
        hci_board_debug("BT \\GNT_BT %x LOG...\n", HAL_READ32(0x40080000, 0x0764));
       // os_sched_suspend();
        //uint32_t flag = os_lock();
        ret = bt_iqk_8721d(&g_iqk_data, 0);
        //os_sched_resume();
       // os_unlock(flag);
        bt_dump_iqk(&g_iqk_data);

        hci_board_debug("\r\n Please write logic efuse 0x1A6 =0x%02x", g_iqk_data.IQK_xx & 0xff);
        hci_board_debug("\r\n Please write logic efuse 0x1A7 =0x%02x", (g_iqk_data.IQK_xx >> 8) & 0xff);
        hci_board_debug("\r\n Please write logic efuse 0x1A8 =0x%02x", g_iqk_data.IQK_yy & 0xff);
        hci_board_debug("\r\n Please write logic efuse 0x1A9 =0x%02x", (g_iqk_data.IQK_yy >> 8) & 0xff);
        hci_board_debug("\r\n Please write logic efuse 0x1AA =0x%02x", g_iqk_data.QDAC);
        hci_board_debug("\r\n Please write logic efuse 0x1AB =0x%02x\r\n", g_iqk_data.IDAC);

    }
    else
    {
        ret = bt_iqk_8721d(&g_iqk_data, 0);
        bt_dump_iqk(&g_iqk_data);
    }


    if(_FAIL == ret)
    {
        hci_board_debug("bt_check_iqk:Warning: IQK Fail, please connect driver !!!!!!!!!\r\n");
        return false;
    }

    bt_lok_write(g_iqk_data.IDAC, g_iqk_data.QDAC);

	hci_tp_phy_efuse[0] = 0;
    hci_tp_phy_efuse[1] =hci_tp_phy_efuse[1] & (~BIT0);
    //hci_tp_phy_efuse[1] = 0xfe;
    //hci_tp_phy_efuse[2] = 0xff;
    hci_tp_phy_efuse[3] = g_iqk_data.IQK_xx & 0xff;
    hci_tp_phy_efuse[4] = (g_iqk_data.IQK_xx >> 8) & 0xff;
    hci_tp_phy_efuse[5] = g_iqk_data.IQK_yy & 0xff;
    hci_tp_phy_efuse[6] = (g_iqk_data.IQK_yy >> 8) & 0xff;

    //bt_write_lgc_efuse_value();

    return true;
}

void bt_write_lgc_efuse_value(void)
{
    hci_tp_lgc_efuse[0x16] = g_iqk_data.IQK_xx & 0xff;
    hci_tp_lgc_efuse[0x17] = (g_iqk_data.IQK_xx >> 8) & 0xff;
    hci_tp_lgc_efuse[0x18] = g_iqk_data.IQK_yy & 0xff;
    hci_tp_lgc_efuse[0x19] = (g_iqk_data.IQK_yy >> 8) & 0xff;
    hci_tp_lgc_efuse[0x1a] = g_iqk_data.QDAC;
    hci_tp_lgc_efuse[0x1b] = g_iqk_data.IDAC;

    hci_board_debug("\r\n write logic efuse 0x1A6 =0x%02x", hci_tp_lgc_efuse[0x16]);
    hci_board_debug("\r\n write logic efuse 0x1A7 =0x%02x", hci_tp_lgc_efuse[0x17]);
    hci_board_debug("\r\n write logic efuse 0x1A8 =0x%02x", hci_tp_lgc_efuse[0x18]);
    hci_board_debug("\r\n write logic efuse 0x1A9 =0x%02x", hci_tp_lgc_efuse[0x19]);
    hci_board_debug("\r\n write logic efuse 0x1Aa =0x%02x", hci_tp_lgc_efuse[0x1a]);
    hci_board_debug("\r\n write logic efuse 0x1Ab =0x%02x", hci_tp_lgc_efuse[0x1b]);
    //EFUSE_LMAP_WRITE(0x1A4,8,(uint8_t *)&hci_tp_lgc_efuse[0x14]);
}


void bt_show_efuse_value(void)
{
    int i = 0;
    for(i=0;i<0x10;i++)
    {
        hci_board_debug("\r\n write physical efuse 0x%x =0x%02x",0x120+i, hci_tp_phy_efuse[i]);
    }
}



