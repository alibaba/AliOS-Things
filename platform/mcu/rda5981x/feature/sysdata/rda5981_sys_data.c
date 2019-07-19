#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "rda5981_sys_data.h"
#include "rda_def.h"
#include "aos/hal/flash.h"
#include "rda_flash.h"

static r_u32 sys_data_location = 0;

/*
 * return 0:ok, else:error.
 */
rda5981_sys_data_t rda5981_sys_data;
r_u8 read_flag = 0;
r_s32 rda5981_write_sys_data(r_u8 *data, r_u16 len, r_u32 flag)
{
    //WLAND_DBG(INFO, "Enter, flag:0x%x\r\n", flag);

    if (!read_flag) {
        if(0 == rda5981_read_flash(sys_data_location,
            (char *)(&rda5981_sys_data), sizeof(rda5981_sys_data)))
            read_flag = 1;
        else
            return -1;
    }

    if ((rda5981_sys_data.flag & RDA5981_SYS_DATA_FLAG_UNINITIALIZED) ||
        strcmp((const char *)(rda5981_sys_data.rda5981_flag), RDA5981_FLAG_FLAG)) {
        memset(&rda5981_sys_data, 0xff, sizeof(rda5981_sys_data));
        rda5981_sys_data.flag = flag;
        strcpy((char *)(rda5981_sys_data.rda5981_flag), RDA5981_FLAG_FLAG);
    }

    rda5981_sys_data.flag |= flag;

    switch (flag) {
    case RDA5981_SYS_DATA_FLAG_STA:
        memcpy(&rda5981_sys_data.sta_data, data,
            min(sizeof(rda5981_sys_data.sta_data), len));
        break;
    case RDA5981_SYS_DATA_FLAG_MAC:
        memcpy(rda5981_sys_data.mac_addr, data,
            min(sizeof(rda5981_sys_data.mac_addr), len));
        break;
    case RDA5981_SYS_DATA_FLAG_PMK:
        memcpy(rda5981_sys_data.pmk, data,
            min(sizeof(rda5981_sys_data.pmk), len));
        break;
    case RDA5981_SYS_DATA_FLAG_IP:
        memcpy(rda5981_sys_data.ip, data,
            min(RDA5981_SYS_DATA_IP_LENGTH, len));
        break;
    case RDA5981_SYS_DATA_FLAG_PARTER_DATA_LEN:
        memcpy(&rda5981_sys_data.parter_data_len, data,
            min(sizeof(rda5981_sys_data.parter_data_len), len));
        break;
    case RDA5981_SYS_DATA_FLAG_TX_POWER:
        memcpy(&rda5981_sys_data.tx_power, data,
            min(sizeof(rda5981_sys_data.tx_power), len));
        break;
    case RDA5981_SYS_DATA_FLAG_XTAL_CAL:
        memcpy(&rda5981_sys_data.xtal_cal, data,
            min(sizeof(rda5981_sys_data.xtal_cal), len));
        break;
    case RDA5981_SYS_DATA_FLAG_TX_POWER_RF:
        memcpy(&rda5981_sys_data.tx_power_rf, data,
            min(sizeof(rda5981_sys_data.tx_power_rf), len));
        break;
    case RDA5981_SYS_DATA_FLAG_TX_POWER_PHY_GN:
        memcpy(&rda5981_sys_data.tx_power_phy_gn, data,
            min(sizeof(rda5981_sys_data.tx_power_phy_gn), len));
        break;
    case RDA5981_SYS_DATA_FLAG_TX_POWER_PHY_B:
        memcpy(&rda5981_sys_data.tx_power_phy_b, data,
            min(sizeof(rda5981_sys_data.tx_power_phy_b), len));
        break;
    case RDA5981_SYS_DATA_FLAG_AP:
        memcpy(&rda5981_sys_data.ap_data, data,
            min(sizeof(rda5981_sys_data.ap_data), len));
        break;
    case RDA5981_SYS_DATA_FLAG_APNET:
        memcpy(&rda5981_sys_data.ap_net_data, data,
            min(sizeof(rda5981_sys_data.ap_net_data), len));
        break;
    case RDA5981_SYS_DATA_FLAG_DHCP:
        memcpy(&rda5981_sys_data.dhcp, data,
            min(sizeof(rda5981_sys_data.dhcp), len));
        break;
    case RDA5981_SYS_DATA_FLAG_UART:
        memcpy(&rda5981_sys_data.uart, data,
            min(sizeof(rda5981_sys_data.uart), len));
        break;
    case RDA5981_SYS_DATA_FLAG_RF:
        memcpy(&rda5981_sys_data.rf, data,
            min(sizeof(rda5981_sys_data.rf), len));
        break;
    case RDA5981_SYS_DATA_FLAG_RF_CHANNELS:
        memcpy(&rda5981_sys_data.rf_channels, data,
            min(sizeof(rda5981_sys_data.rf_channels), len));
        break;
    case RDA5981_SYS_DATA_FLAG_PHY:
        memcpy(&rda5981_sys_data.phy, data,
            min(sizeof(rda5981_sys_data.phy), len));
        break;
    case RDA5981_SYS_DATA_FLAG_PHY_CHANNELS:
        memcpy(&rda5981_sys_data.phy_channels, data,
            min(sizeof(rda5981_sys_data.phy_channels), len));
        break;
    case RDA5981_SYS_DATA_FLAG_TX_POWER_OFFSET:
        memcpy(&rda5981_sys_data.tp_offset, data,
            min(sizeof(rda5981_sys_data.tp_offset), len));
        break;
    default:
        return -1;
    }
    rda5981_erase_flash(sys_data_location, sizeof(rda5981_sys_data));
    return rda5981_write_flash(sys_data_location,
        (char *)(&rda5981_sys_data), sizeof(rda5981_sys_data));
}

/*
 * return 0:ok, else:error.
 */
r_s32 rda5981_erase_sys_data(r_u32 flag)
{
    //RDA5981_SYS_data rda5981_sys_data;

    //WLAND_DBG(INFO, "Enter, flag:0x%x\r\n", flag);

    if (!read_flag) {
        read_flag = 1;
        rda5981_read_flash(sys_data_location,
            (char *)(&rda5981_sys_data), sizeof(rda5981_sys_data));
    }

    if (rda5981_sys_data.flag & RDA5981_SYS_DATA_FLAG_UNINITIALIZED)//flash init is 0xffffffff
        return -1;

    if ((rda5981_sys_data.flag & flag) == 0)
        return 0;

    rda5981_sys_data.flag &= (~flag);

    if (rda5981_sys_data.flag == 0)
        rda5981_sys_data.flag = RDA5981_SYS_DATA_FLAG_UNINITIALIZED;

    if (flag & RDA5981_SYS_DATA_FLAG_STA) {
        memset(&rda5981_sys_data.sta_data, 0xff,
            sizeof(rda5981_sys_data.sta_data));
    }
    if (flag & RDA5981_SYS_DATA_FLAG_MAC) {
        memset(rda5981_sys_data.mac_addr, 0xff,
            sizeof(rda5981_sys_data.mac_addr));
    }
    if (flag & RDA5981_SYS_DATA_FLAG_PMK) {
        memset(rda5981_sys_data.pmk, 0xff,
            sizeof(rda5981_sys_data.pmk));
    }
    if (flag & RDA5981_SYS_DATA_FLAG_IP) {
        memset(rda5981_sys_data.ip, 0xff,
            RDA5981_SYS_DATA_IP_LENGTH);
    }
    if (flag & RDA5981_SYS_DATA_FLAG_PARTER_DATA_LEN) {
        memset(&rda5981_sys_data.parter_data_len, 0xff,
            sizeof(rda5981_sys_data.parter_data_len));
    }
    if (flag & RDA5981_SYS_DATA_FLAG_TX_POWER) {
        memset(&rda5981_sys_data.tx_power, 0xff,
            sizeof(rda5981_sys_data.tx_power));
    }
    if (flag & RDA5981_SYS_DATA_FLAG_XTAL_CAL) {
        memset(&rda5981_sys_data.xtal_cal, 0xff,
            sizeof(rda5981_sys_data.xtal_cal));
    }
    if (flag & RDA5981_SYS_DATA_FLAG_TX_POWER_RF) {
        memset(&rda5981_sys_data.tx_power_rf, 0xff,
            sizeof(rda5981_sys_data.tx_power_rf));
    }
    if (flag & RDA5981_SYS_DATA_FLAG_TX_POWER_PHY_GN) {
        memset(&rda5981_sys_data.tx_power_phy_gn, 0xff,
            sizeof(rda5981_sys_data.tx_power_phy_gn));
    }
    if (flag & RDA5981_SYS_DATA_FLAG_TX_POWER_PHY_B) {
        memset(&rda5981_sys_data.tx_power_phy_b, 0xff,
            sizeof(rda5981_sys_data.tx_power_phy_b));
    }
    if (flag & RDA5981_SYS_DATA_FLAG_AP) {
        memset(&rda5981_sys_data.ap_data, 0xff,
            sizeof(rda5981_sys_data.ap_data));
    }
    if (flag & RDA5981_SYS_DATA_FLAG_APNET) {
        memset(&rda5981_sys_data.ap_net_data, 0xff,
            sizeof(rda5981_sys_data.ap_net_data));
    }
    if (flag & RDA5981_SYS_DATA_FLAG_DHCP) {
        memset(&rda5981_sys_data.dhcp, 0xff,
            sizeof(rda5981_sys_data.dhcp));
    }
    if (flag & RDA5981_SYS_DATA_FLAG_UART) {
        memset(&rda5981_sys_data.uart, 0xff,
            sizeof(rda5981_sys_data.uart));
    }
    if (flag & RDA5981_SYS_DATA_FLAG_RF) {
        memset(&rda5981_sys_data.rf, 0xff,
            sizeof(rda5981_sys_data.rf));
    }
    if (flag & RDA5981_SYS_DATA_FLAG_RF_CHANNELS) {
        memset(&rda5981_sys_data.rf_channels, 0xff,
            sizeof(rda5981_sys_data.rf_channels));
    }
    if (flag & RDA5981_SYS_DATA_FLAG_PHY) {
        memset(&rda5981_sys_data.phy, 0xff,
            sizeof(rda5981_sys_data.phy));
    }
    if (flag & RDA5981_SYS_DATA_FLAG_PHY_CHANNELS) {
        memset(&rda5981_sys_data.phy_channels, 0xff,
            sizeof(rda5981_sys_data.phy_channels));
    }
    if (flag & RDA5981_SYS_DATA_FLAG_TX_POWER_OFFSET) {
        memset(&rda5981_sys_data.tp_offset, 0xff,
            sizeof(rda5981_sys_data.tp_offset));
    }
    rda5981_erase_flash(sys_data_location, sizeof(rda5981_sys_data));
    return rda5981_write_flash(sys_data_location,
        (char *)(&rda5981_sys_data), sizeof(rda5981_sys_data));
}

/*
 * return 0:ok, else:error.
 */
r_s32 rda5981_read_sys_data(r_u8 *data, r_u16 len, r_u32 flag)
{
    //RDA5981_SYS_data rda5981_sys_data;
    //WLAND_DBG(INFO, "Enter, flag:0x%x\r\n", flag);

    if (!read_flag) {
        read_flag = 1;
        rda5981_read_flash(sys_data_location,
            (char *)(&rda5981_sys_data), sizeof(rda5981_sys_data));
    }

    if (rda5981_sys_data.flag & RDA5981_SYS_DATA_FLAG_UNINITIALIZED)//flash init is 0xffffffff
        return -1;

    if (strcmp((const char *)(rda5981_sys_data.rda5981_flag), RDA5981_FLAG_FLAG))
        return -2;

    if ((rda5981_sys_data.flag & flag) == 0)
        return -3;

    switch (flag) {
    case RDA5981_SYS_DATA_FLAG_STA:
        memcpy(data, &rda5981_sys_data.sta_data,
            min(sizeof(rda5981_sys_data.sta_data), len));
        break;
    case RDA5981_SYS_DATA_FLAG_MAC:
        memcpy(data, rda5981_sys_data.mac_addr,
            min(sizeof(rda5981_sys_data.mac_addr), len));
        break;
    case RDA5981_SYS_DATA_FLAG_PMK:
        memcpy(data, rda5981_sys_data.pmk,
            min(sizeof(rda5981_sys_data.pmk), len));
        break;
    case RDA5981_SYS_DATA_FLAG_IP:
        memcpy(data, rda5981_sys_data.ip,
            min(RDA5981_SYS_DATA_IP_LENGTH, len));
        break;
    case RDA5981_SYS_DATA_FLAG_PARTER_DATA_LEN:
        memcpy(data, &rda5981_sys_data.parter_data_len,
            min(sizeof(rda5981_sys_data.parter_data_len), len));
        break;
    case RDA5981_SYS_DATA_FLAG_TX_POWER:
        memcpy(data, &rda5981_sys_data.tx_power,
            min(sizeof(rda5981_sys_data.tx_power), len));
        break;
    case RDA5981_SYS_DATA_FLAG_XTAL_CAL:
        memcpy(data, &rda5981_sys_data.xtal_cal,
            min(sizeof(rda5981_sys_data.xtal_cal), len));
        break;
    case RDA5981_SYS_DATA_FLAG_TX_POWER_RF:
        memcpy(data, &rda5981_sys_data.tx_power_rf,
            min(sizeof(rda5981_sys_data.tx_power_rf), len));
        break;
    case RDA5981_SYS_DATA_FLAG_TX_POWER_PHY_GN:
        memcpy(data, &rda5981_sys_data.tx_power_phy_gn,
            min(sizeof(rda5981_sys_data.tx_power_phy_gn), len));
        break;
    case RDA5981_SYS_DATA_FLAG_TX_POWER_PHY_B:
        memcpy(data, &rda5981_sys_data.tx_power_phy_b,
            min(sizeof(rda5981_sys_data.tx_power_phy_b), len));
        break;
    case RDA5981_SYS_DATA_FLAG_AP:
        memcpy(data, &rda5981_sys_data.ap_data,
            min(sizeof(rda5981_sys_data.ap_data), len));
        break;
    case RDA5981_SYS_DATA_FLAG_APNET:
        memcpy(data, &rda5981_sys_data.ap_net_data,
            min(sizeof(rda5981_sys_data.ap_net_data), len));
        break;
    case RDA5981_SYS_DATA_FLAG_DHCP:
        memcpy(data, &rda5981_sys_data.dhcp,
            min(sizeof(rda5981_sys_data.dhcp), len));
        break;
    case RDA5981_SYS_DATA_FLAG_UART:
        memcpy(data, &rda5981_sys_data.uart,
            min(sizeof(rda5981_sys_data.uart), len));
        break;
    case RDA5981_SYS_DATA_FLAG_RF:
        memcpy(data, &rda5981_sys_data.rf,
            min(sizeof(rda5981_sys_data.rf), len));
        break;
    case RDA5981_SYS_DATA_FLAG_RF_CHANNELS:
        memcpy(data, &rda5981_sys_data.rf_channels,
            min(sizeof(rda5981_sys_data.rf_channels), len));
        break;
    case RDA5981_SYS_DATA_FLAG_PHY:
        memcpy(data, &rda5981_sys_data.phy,
            min(sizeof(rda5981_sys_data.phy), len));
        break;
    case RDA5981_SYS_DATA_FLAG_PHY_CHANNELS:
        memcpy(data, &rda5981_sys_data.phy_channels,
            min(sizeof(rda5981_sys_data.phy_channels), len));
        break;
    case RDA5981_SYS_DATA_FLAG_TX_POWER_OFFSET:
        memcpy(data, &rda5981_sys_data.tp_offset,
            min(sizeof(rda5981_sys_data.tp_offset), len));
        break;
    default:
        return -1;
    }
    return 0;
}
#if 0
/*
 * return 0:ok, else:error.
 */
r_s32 rda5981_flash_read_mac_addr(r_u8 *mac_addr)
{
    r_s32 ret;
    //WLAND_DBG(DEBUG, "Enter\r\n");
    ret = rda5981_read_sys_data(mac_addr, 6, RDA5981_SYS_DATA_FLAG_MAC);
    if (ret)
        //WLAND_DBG(ERROR,  "read mac addr from flash fail\r\n");
    else
        //WLAND_DBG(INFO, "Done(ret:%d)"MACDBG"\r\n", ret, MAC2STRDBG(mac_addr));
    return ret;
}

/*
 * return 0:ok, else:error.
 */
r_s32 rda5981_flash_write_mac_addr(r_u8 *mac_addr)
{
    //WLAND_DBG(INFO, "Enter"MACDBG"\r\n", MAC2STRDBG(mac_addr));
    return rda5981_write_sys_data(mac_addr, 6, RDA5981_SYS_DATA_FLAG_MAC);
}
#endif

#define HAL_PARTITION_SYS_DATA      HAL_PARTITION_CUSTOM_1

r_s32 rda5981_init_sys_data()
{
    //WLAND_DBG(INFO, "Enter set userdata addr: %x:%x:%x\r\n", sys_data_addr);
    hal_partition_t pno = HAL_PARTITION_SYS_DATA;

    hal_logic_partition_t info;
    hal_logic_partition_t *partition_info = &info;

    if (hal_flash_info_get(pno, partition_info) != 0) {
        return -1;
    }

    if ((partition_info->partition_start_addr)&(SECTOR_SIZE-1))
        return -1;
    if ((partition_info->partition_start_addr) <= RDA5991H_PARTITION_TABLE_END_ADDR)
        return -2;
    if ((partition_info->partition_start_addr + partition_info->partition_length) > 0x18000000 + FLASH_SIZE)
        return -3;

    sys_data_location = partition_info->partition_start_addr;

    return 0;
}

#if 0
r_s32 rda5981_reboot_to_addr(r_u32 firmware_info_addr, r_u32 reboot_addr)
{
    int ret;
    int retry = 0;
    struct firmware_info info;
    WLAND_DBG(INFO, "Enter:0x%x\n", reboot_addr);
    rda5981_read_flash(firmware_info_addr, (char *)(&info), sizeof(info));

    if (1 || info.magic == RDA5981_FIRMWARE_MAGIC) {
        info.bootaddr = reboot_addr;
        info.bootmagic = RDA5981_FIRMWARE_MAGIC;
write:
        if ((retry++) > 5) {
            WLAND_DBG(ERROR, "try %d time, fail\n", retry-1);
            return -1;
        }
        ret = rda5981_erase_flash(firmware_info_addr, sizeof(info));
        if (ret) {
            WLAND_DBG(ERROR, "erase fail\n");
            goto write;
        }

        ret = rda5981_write_flash(firmware_info_addr, (char *)(&info), sizeof(info));
        if (ret) {
            WLAND_DBG(ERROR, "write fail\n");
            goto write;
        }
        rda_wdt_softreset();
        return 0;
    } else {
        WLAND_DBG(ERROR, "firmware magic error:0x%x\n", info.magic);
        return -1;
    }
}
#endif
r_s32 rda5981_write_sys_data_regs(r_u8 *reg, r_u8 *value, r_u32 flag)
{
    r_u16 reg16 = 0;
    r_u32 reg32 = 0;
    r_u8 idx = 0;

    if (!read_flag) {
        if(0 == rda5981_read_flash(sys_data_location,
            (char *)(&rda5981_sys_data), sizeof(rda5981_sys_data)))
            read_flag = 1;
        else
            return -1;
    }

    if ((rda5981_sys_data.flag & RDA5981_SYS_DATA_FLAG_UNINITIALIZED) ||
        strcmp((const char *)(rda5981_sys_data.rda5981_flag), RDA5981_FLAG_FLAG)) {
        memset(&rda5981_sys_data, 0xff, sizeof(rda5981_sys_data));
        rda5981_sys_data.flag = flag;
        strcpy((char *)(rda5981_sys_data.rda5981_flag), RDA5981_FLAG_FLAG);
    }

    rda5981_sys_data.flag |= flag;

    switch (flag) {
    case RDA5981_SYS_DATA_FLAG_RF:
        if (rda5981_sys_data.rf.valid != 1) {
            rda5981_sys_data.rf.valid = 1;
            rda5981_sys_data.rf.flag = 0;
        }

        reg16 = *((r_u16 *)reg);
        if (rda5981_sys_data.rf.flag != 0) {
            for (idx = 0; idx < 8; idx++) {
                if ((rda5981_sys_data.rf.flag & BIT(idx)) &&
                    (rda5981_sys_data.rf.reg_val[idx][0] == reg16)) {
                    rda5981_sys_data.rf.reg_val[idx][1] = *((r_u16 *)value);
                    break;
                } else {
                    continue;
                }
            }
            if ((8 == idx) && (0xFF == rda5981_sys_data.rf.flag))
                return -2;
        }

        if ((8 == idx) || (0 == rda5981_sys_data.rf.flag)) {
            for (idx = 0; idx < 8; idx++) {
                if (!(rda5981_sys_data.rf.flag & BIT(idx))) {
                    rda5981_sys_data.rf.reg_val[idx][0] = reg16;
                    rda5981_sys_data.rf.reg_val[idx][1] = *((r_u16 *)value);
                    rda5981_sys_data.rf.flag |= BIT(idx);
                    break;
                } else {
                    continue;
                }
            }
        }

        break;
    case RDA5981_SYS_DATA_FLAG_RF_CHANNELS:
        if (rda5981_sys_data.rf_channels.valid != 1) {
            rda5981_sys_data.rf_channels.valid = 1;
            rda5981_sys_data.rf_channels.flag = 0;
        }

        reg16 = *((r_u16 *)reg);
        if (rda5981_sys_data.rf_channels.flag != 0) {
            for (idx = 0; idx < 8; idx++) {
                if ((rda5981_sys_data.rf_channels.flag & BIT(idx)) &&
                    (rda5981_sys_data.rf_channels.reg_val[idx][0] == reg16)) {
                    memcpy(&rda5981_sys_data.rf_channels.reg_val[idx][1], value, 14 * sizeof(r_u16));
                    break;
                } else {
                    continue;
                }
            }
            if ((8 == idx) && (0xFF == rda5981_sys_data.rf_channels.flag))
                return -2;
        }

        if ((8 == idx) || (0 == rda5981_sys_data.rf_channels.flag)) {
            for (idx = 0; idx < 8; idx++) {
                if (!(rda5981_sys_data.rf_channels.flag & BIT(idx))) {
                    rda5981_sys_data.rf_channels.reg_val[idx][0] = reg16;
                    memcpy(&rda5981_sys_data.rf_channels.reg_val[idx][1], value, 14 * sizeof(r_u16));
                    rda5981_sys_data.rf_channels.flag |= BIT(idx);
                    break;
                } else {
                    continue;
                }
            }
        }
        break;
    case RDA5981_SYS_DATA_FLAG_PHY:
        if (rda5981_sys_data.phy.valid != 1) {
            rda5981_sys_data.phy.valid = 1;
            rda5981_sys_data.phy.flag = 0;
        }

        reg32 = *((r_u32 *)reg);
        if (rda5981_sys_data.phy.flag != 0) {
            for (idx = 0; idx < 8; idx++) {
                if ((rda5981_sys_data.phy.flag & BIT(idx)) &&
                    (rda5981_sys_data.phy.reg_val[idx][0] == reg32)) {
                    rda5981_sys_data.phy.reg_val[idx][1] = *((r_u32 *)value);
                    break;
                } else {
                    continue;
                }
            }
            if ((8 == idx) && (0xFF == rda5981_sys_data.phy.flag))
                return -2;
        }

        if ((8 == idx) || (0 == rda5981_sys_data.phy.flag)) {
            for (idx = 0; idx < 8; idx++) {
                if (!(rda5981_sys_data.phy.flag & BIT(idx))) {
                    rda5981_sys_data.phy.reg_val[idx][0] = reg32;
                    rda5981_sys_data.phy.reg_val[idx][1] = *((r_u32 *)value);
                    rda5981_sys_data.phy.flag |= BIT(idx);
                    break;
                } else {
                    continue;
                }
            }
        }
        break;
    case RDA5981_SYS_DATA_FLAG_PHY_CHANNELS:
        if (rda5981_sys_data.phy_channels.valid != 1) {
            rda5981_sys_data.phy_channels.valid = 1;
            rda5981_sys_data.phy_channels.flag = 0;
        }

        reg32 = *((r_u32 *)reg);
        if (rda5981_sys_data.phy_channels.flag != 0) {
            for (idx = 0; idx < 8; idx++) {
                if ((rda5981_sys_data.phy_channels.flag & BIT(idx)) &&
                    (rda5981_sys_data.phy_channels.reg_val[idx][0] == reg32)) {
                    memcpy(&rda5981_sys_data.phy_channels.reg_val[idx][1], value, 14 * sizeof(r_u32));
                    break;
                } else {
                    continue;
                }
            }
            if ((8 == idx) && (0xFF == rda5981_sys_data.phy_channels.flag))
                return -2;
        }

        if ((8 == idx) || (0 == rda5981_sys_data.phy_channels.flag)) {
            for (idx = 0; idx < 8; idx++) {
                if (!(rda5981_sys_data.phy_channels.flag & BIT(idx))) {
                    rda5981_sys_data.phy_channels.reg_val[idx][0] = reg32;
                    memcpy(&rda5981_sys_data.phy_channels.reg_val[idx][1], value, 14 * sizeof(r_u32));
                    rda5981_sys_data.phy_channels.flag |= BIT(idx);
                    break;
                } else {
                    continue;
                }
            }
        }
        break;
    default:
        //WLAND_DBG(ERROR, "Unknown flag:0x%08x\n", flag);
        return -3;
    }

    rda5981_erase_flash(sys_data_location, sizeof(rda5981_sys_data));
    return rda5981_write_flash(sys_data_location,
        (char *)(&rda5981_sys_data), sizeof(rda5981_sys_data));
}

r_s32 rda5981_erase_sys_data_regs(r_u8 *reg, r_u32 flag)
{
    r_u16 reg16 = 0;
    r_u32 reg32 = 0;
    r_u8 idx = 0;

    if (!read_flag) {
        if(0 == rda5981_read_flash(sys_data_location,
            (char *)(&rda5981_sys_data), sizeof(rda5981_sys_data)))
            read_flag = 1;
        else
            return -1;
    }

    if (rda5981_sys_data.flag & RDA5981_SYS_DATA_FLAG_UNINITIALIZED)//flash init is 0xffffffff
        return -1;

    if ((rda5981_sys_data.flag & flag) == 0)
        return 0;

    switch (flag) {
    case RDA5981_SYS_DATA_FLAG_RF:
        if (rda5981_sys_data.rf.valid != 1) {
            return 0;
        }

        reg16 = *((r_u16 *)reg);
        if (rda5981_sys_data.rf.flag != 0) {
            for (idx = 0; idx < 8; idx++) {
                if ((rda5981_sys_data.rf.flag & BIT(idx)) &&
                    (rda5981_sys_data.rf.reg_val[idx][0] == reg16)) {
                    memset(&rda5981_sys_data.rf.reg_val[idx][0], 0xFF, 2 * sizeof(r_u16));
                    rda5981_sys_data.rf.flag &= ~BIT(idx);
                    break;
                } else {
                    continue;
                }
            }
        }

        if (0 == rda5981_sys_data.rf.flag) {
            rda5981_sys_data.rf.valid = 0xFFFFFFFF;
            rda5981_sys_data.rf.flag = 0xFFFFFFFF;
            rda5981_sys_data.flag &= (~flag);
        }

        break;
    case RDA5981_SYS_DATA_FLAG_RF_CHANNELS:
        if (rda5981_sys_data.rf_channels.valid != 1) {
            return 0;
        }

        reg16 = *((r_u16 *)reg);
        if (rda5981_sys_data.rf_channels.flag != 0) {
            for (idx = 0; idx < 8; idx++) {
                if ((rda5981_sys_data.rf_channels.flag & BIT(idx)) &&
                    (rda5981_sys_data.rf_channels.reg_val[idx][0] == reg16)) {
                    memset(&rda5981_sys_data.rf_channels.reg_val[idx][0], 0xFF, 15 * sizeof(r_u16));
                    rda5981_sys_data.rf_channels.flag &= ~BIT(idx);
                    break;
                } else {
                    continue;
                }
            }
        }

        if (0 == rda5981_sys_data.rf_channels.flag) {
            rda5981_sys_data.rf_channels.valid = 0xFFFFFFFF;
            rda5981_sys_data.rf_channels.flag = 0xFFFFFFFF;
            rda5981_sys_data.flag &= (~flag);
        }

        break;
    case RDA5981_SYS_DATA_FLAG_PHY:
        if (rda5981_sys_data.phy.valid != 1) {
            return 0;
        }

        reg32 = *((r_u32 *)reg);
        if (rda5981_sys_data.phy.flag != 0) {
            for (idx = 0; idx < 8; idx++) {
                if ((rda5981_sys_data.phy.flag & BIT(idx)) &&
                    (rda5981_sys_data.phy.reg_val[idx][0] == reg32)) {
                    memset(&rda5981_sys_data.phy.reg_val[idx][0], 0xFF, 2 * sizeof(r_u32));
                    rda5981_sys_data.phy.flag &= ~BIT(idx);
                    break;
                } else {
                    continue;
                }
            }
        }

        if (0 == rda5981_sys_data.phy.flag) {
            rda5981_sys_data.phy.valid = 0xFFFFFFFF;
            rda5981_sys_data.phy.flag = 0xFFFFFFFF;
            rda5981_sys_data.flag &= (~flag);
        }

        break;
    case RDA5981_SYS_DATA_FLAG_PHY_CHANNELS:
        if (rda5981_sys_data.phy_channels.valid != 1) {
            return 0;
        }

        reg32 = *((r_u32 *)reg);
        if (rda5981_sys_data.phy_channels.flag != 0) {
            for (idx = 0; idx < 8; idx++) {
                if ((rda5981_sys_data.phy_channels.flag & BIT(idx)) &&
                    (rda5981_sys_data.phy_channels.reg_val[idx][0] == reg32)) {
                    memset(&rda5981_sys_data.phy_channels.reg_val[idx][0], 0xFF, 15 * sizeof(r_u32));
                    rda5981_sys_data.phy_channels.flag &= ~BIT(idx);
                    break;
                } else {
                    continue;
                }
            }
        }

        if (0 == rda5981_sys_data.phy_channels.flag) {
            rda5981_sys_data.phy_channels.valid = 0xFFFFFFFF;
            rda5981_sys_data.phy_channels.flag = 0xFFFFFFFF;
            rda5981_sys_data.flag &= (~flag);
        }

        break;
    default:
        //WLAND_DBG(ERROR, "Unknown flag:0x%08x\n", flag);
        return -3;
    }

    rda5981_erase_flash(sys_data_location, sizeof(rda5981_sys_data));
    return rda5981_write_flash(sys_data_location,
        (char *)(&rda5981_sys_data), sizeof(rda5981_sys_data));
}


r_s32 rda5981_read_sys_data_regs(r_u8 *reg, r_u8 *value, r_u32 flag)
{
    r_u16 reg16 = 0;
    r_u32 reg32 = 0;
    r_u8 idx = 0;

    if (!read_flag) {
        if(0 == rda5981_read_flash(sys_data_location,
            (char *)(&rda5981_sys_data), sizeof(rda5981_sys_data)))
            read_flag = 1;
        else
            return -1;
    }

    if (rda5981_sys_data.flag & RDA5981_SYS_DATA_FLAG_UNINITIALIZED)//flash init is 0xffffffff
        return -1;

    if (strcmp((const char *)(rda5981_sys_data.rda5981_flag), RDA5981_FLAG_FLAG))
        return -2;

    if ((rda5981_sys_data.flag & flag) == 0)
        return -3;

    switch (flag) {
    case RDA5981_SYS_DATA_FLAG_RF:
        if (rda5981_sys_data.rf.valid != 1) {
            return -4;
        }

        reg16 = *((r_u16 *)reg);
        for (idx = 0; idx < 8; idx++) {
            if ((rda5981_sys_data.rf.flag & BIT(idx)) &&
                (rda5981_sys_data.rf.reg_val[idx][0] == reg16)) {
                memcpy(value, &rda5981_sys_data.rf.reg_val[idx][1], sizeof(r_u16));
                break;
            } else {
                continue;
            }
        }

        if (8 == idx)
            return -4;

        break;
    case RDA5981_SYS_DATA_FLAG_RF_CHANNELS:
        if (rda5981_sys_data.rf_channels.valid != 1) {
            return -4;
        }

        reg16 = *((r_u16 *)reg);
        if (rda5981_sys_data.rf_channels.flag != 0) {
            for (idx = 0; idx < 8; idx++) {
                if ((rda5981_sys_data.rf_channels.flag & BIT(idx)) &&
                    (rda5981_sys_data.rf_channels.reg_val[idx][0] == reg16)) {
                    memcpy(value, &rda5981_sys_data.rf_channels.reg_val[idx][1], 14 * sizeof(r_u16));
                    break;
                } else {
                    continue;
                }
            }
        }

        if (8 == idx)
            return -4;

        break;
    case RDA5981_SYS_DATA_FLAG_PHY:
        if (rda5981_sys_data.phy.valid != 1) {
            return -4;
        }

        reg32 = *((r_u32 *)reg);
        if (rda5981_sys_data.phy.flag != 0) {
            for (idx = 0; idx < 8; idx++) {
                if ((rda5981_sys_data.phy.flag & BIT(idx)) &&
                    (rda5981_sys_data.phy.reg_val[idx][0] == reg32)) {
                    memcpy(value, &rda5981_sys_data.phy.reg_val[idx][1], sizeof(r_u32));
                    break;
                } else {
                    continue;
                }
            }
        }

        if (8 == idx)
            return -4;

        break;
    case RDA5981_SYS_DATA_FLAG_PHY_CHANNELS:
        if (rda5981_sys_data.phy_channels.valid != 1) {
            return -4;
        }

        reg32 = *((r_u32 *)reg);
        if (rda5981_sys_data.phy_channels.flag != 0) {
            for (idx = 0; idx < 8; idx++) {
                if ((rda5981_sys_data.phy_channels.flag & BIT(idx)) &&
                    (rda5981_sys_data.phy_channels.reg_val[idx][0] == reg32)) {
                    memcpy(value, &rda5981_sys_data.phy_channels.reg_val[idx][1], 14 * sizeof(r_u32));
                    break;
                } else {
                    continue;
                }
            }
        }

        if (8 == idx)
            return -4;

        break;
    default:
        //WLAND_DBG(ERROR, "Unknown flag:0x%08x\n", flag);
        return -3;
    }

    return 0;
}
#if 0
int rda5981_flash_get_mid(u8 *mid)
{
    *mid = 0xC8;

    return 0;
}

int rda5981_flash_get_did(u8 *did)
{
    *did = 0x13;

    return 0;
}

int rda5981_flash_get_jdid(u16 *jdid)
{
    *jdid = 0x4014;

    return 0;
}

int rda5981_read_default_config(char *buf, u32 len, u32 flag)
{
    int ret = 0;
    u32 addr;
    u32 addr_offset = 0;
    char *temp_buf = NULL, *temp_buf_aligned;

    switch (flag) {
    case RDA5981_VBAT_CAL:
        addr = 0x18000088;
        break;
    case RDA5981_GPADC0_CAL:
    case RDA5981_GPADC1_CAL:
        addr = 0x1800008C;
        break;
    case RDA5981_PRODUCT_ID:
        addr = 0x18000084;
        break;
    case RDA5981_POWER_CLASS:
        addr = 0x18000085;
        break;
    default:
        WLAND_DBG(ERROR, "Unknown flag\n");
        return -1;
    }

    addr_offset = addr % 4;
    addr = (addr - addr_offset) & (flash_size -1);

    if (((u32)buf % 4) || (addr_offset != 0)) {
        temp_buf = (char *)malloc(addr_offset + len + 4);
        if (temp_buf == NULL) {
            ret = -1;
            goto out;
        }
        if ((u32)temp_buf % 4)
            temp_buf_aligned = temp_buf + (4 - (u32)temp_buf % 4);
        else
            temp_buf_aligned = temp_buf;
    } else {
        temp_buf_aligned = buf;
    }

    core_util_critical_section_enter();
    spi_flash_flush_cache();
    SPI_FLASH_READ_DATA_FOR_MBED((void *)addr, temp_buf_aligned, (len + addr_offset));
    core_util_critical_section_exit();

    if (temp_buf_aligned != buf)
        memcpy(buf, temp_buf_aligned + addr_offset, len);

out:
    if (temp_buf)
        free(temp_buf);

    return ret;
}

int rda5981_flash_read_vbat_cal(float *k, float *b)
{
    int ret = 0;
    u32 value = 0;
    u32 x1 = 0;//y1 3.0f
    u32 x2 = 0;//y2 4.2f
    float k_tmp = 0;
    float b_tmp = 0;

    ret = rda5981_read_default_config((char *)&value, 4, RDA5981_VBAT_CAL);

    if (ret < 0) {
        WLAND_DBG(ERROR, "read vbat_cal form flash fail\n");
        return -1;
    }
    if ((0 == (value & 0xFFFFFUL)) || (0xFFFFFUL == (value & 0xFFFFFUL))) {
        WLAND_DBG(ERROR, "invalid vbat_cal:0x%08x\n", value);
        return -1;
    } else {
        x1 = value & 0x3FFUL;
        x2 = (value >> 10) & 0x3FFUL;
    }

    if (x1 == x2) {
        return -1;
    }

    k_tmp = (4.2f - 3.0f) / (float)(x2 - x1);
    b_tmp = 4.2f - k_tmp * x2;

    *k = k_tmp;
    *b = b_tmp;

    return ret;
}
#endif
#if 0
int rda5981_flash_read_gpadc0_cal(float *k, float *b)
{
    int ret = 0;
    u32 value = 0;
    u32 x1 = 0;//y1 0.2f
    u32 x2 = 0;//y2 1.6f
    float k_tmp = 0;
    float b_tmp = 0;

    ret = rda5981_read_default_config((char *)&value, 4, RDA5981_GPADC0_CAL);

    if (ret < 0) {
        WLAND_DBG(ERROR, "read vbat_cal form flash fail\n");
        return -1;
    }
    if ((0 == (value & 0x3FFFFFFF)) || (0x3FFFFFFF == (value & 0x3FFFFFFF))) {
        WLAND_DBG(ERROR, "invalid vbat_cal:0x%08x\n", value);
        return -1;
    } else {
        x1 = value & 0x3FFUL;
        x2 = (value >> 20) & 0x3FFUL;
    }

    if (x1 == x2) {
        return -1;
    }

    k_tmp = (1.6f - 0.2f) / (float)(x2 - x1);
    b_tmp = 1.6f - k_tmp * x2;

    *k = k_tmp;
    *b = b_tmp;

    return ret;
}
#endif


