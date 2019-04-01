/**
 *
 * @page ATCMD AT Command Manual V2.0
 *
 * @section ATCMD_general General AT Command
 * 
 * @subsection ATCMD_general_1 List all AT Command
 *
 * | command     | AT+H |
 * |---------------|-------------------|
 * | param         | none         |
 * | return value  |  AT+H=OK               |
 * | example       | AT+H |
 *
 * @subsection ATCMD_general_2 System Reboot
 *
 * | command     | AT+REBOOT|
 * |---------------|-------------------|
 * | param         | none        |
 * | return value  | AT+REBOOT=OK               |
 * | example       | AT+REBOOT |
 *
 * @subsection ATCMD_general_3 Get Firmware Version
 * | command     | AT+VERSION=?|
 * |---------------|-------------------|
 * | param         | none       |
 * | return value  | +VERSION:<number>              |
 * | example       | AT+VERSION=? |
 *
 * @subsection ATCMD_general_4 Get Manufacture Information
 * | command     | AT+MF_INFO=?|
 * |---------------|-------------------|
 * | param         | none         |
 * | return value  | +MF_INFO:<manufacture>               |
 * | example       | AT+MF_INFO=? |
 *
 *
 * @subsection ATCMD_general_5 Write MAC1 address to Flash
 * | command     | AT+RF_WRITE_MAC1=<mac1_address> |
 * |---------------|-------------------|
 * | param         | <mac1_address>: xx:xx:xx:xx:xx:xx         |
 * | return value  | AT+RF_WRITE_MAC1=OK               |
 * | example       | AT+RF_WRITE_MAC1=00:23:45:67:89:15 |
 *
 * @subsection ATCMD_general_6 Read MAC1 address from flash
 * | command     | AT+RF_READ_MAC1=? |
 * |---------------|-------------------|
 * | param         | none         |
 * | return value  | AT+RF_READ_MAC1=00:23:45:67:89:15               |
 * | example       | AT+RF_READ_MAC1=? |
 *
 * @subsection ATCMD_general_7 Write MAC2 address to Flash
 * | command     | AT+RF_WRITE_MAC2=<mac2_address> |
 * |---------------|-------------------|
 * | param         | <mac2_address>: xx:xx:xx:xx:xx:xx          |
 * | return value  | AT+RF_WRITE_MAC2=OK               |
 * | example       | AT+RF_WRITE_MAC2=00:23:45:67:89:15 |
 *
 * @subsection ATCMD_general_8 Read MAC2 address from flash
 * | command     | AT+RF_READ_MAC2=?  |
 * |---------------|-------------------|
 * | param         | none         |
 * | return value  | AT+RF_READ_MAC2=00:23:45:67:89:15              |
 * | example       | AT+RF_READ_MAC2=> |
 *
 * @subsection ATCMD_general_9 Enable Efuse Write function
 * | command     | AT+EFUSE_ENABLE_WRITE  |
 * |---------------|-------------------|
 * | param         | none         |
 * | return value  | AT+EFUSE_ENABLE_WRITE=OK               |
 * | example       | AT+EFUSE_ENABLE_WRITE  |
 *
 * @subsection ATCMD_general_10 Write MAC address to Efuse
 * | command     | AT+EFUSE_WRITE_MAC=<mac_address>    |
 * |---------------|-------------------|
 * | param         | <mac_address>: xx:xx:xx:xx:xx:xx:xx         |
 * | return value  | AT+EFUSE_WRITE_MAC=OK               |
 * | example       | AT+EFUSE_WRITE_MAC=00:11:22:33:44:55  |
 *
 * @subsection ATCMD_general_11 Read MAC address from Efuse
 * | command     | AT+EFUSE_READ_MAC=?   |
 * |---------------|-------------------|
 * | param         | none         |
 * | return value  | AT+EFUSE_READ_MAC=00:11:22:33:44:55               |
 * | example       | AT+EFUSE_READ_MAC=? |
 *
 * @subsection ATCMD_general_12 Dump Efuse Context
 * | command     | AT+EFUSE_DUMP   |
 * |---------------|-------------------|
 * | param         | none         |
 * | return value  | AT+EFUSE_DUMP=OK               |
 * | example       | AT+EFUSE_DUMP |
 *
 * @subsection ATCMD_general_13 UART FW upgrade
 * | command       | AT+UART_FW_UPGRADE     |
 * |---------------|------------------------|
 * | param         | none                   |
 * | return value  | AT+UART_FW_UPGRADE=OK  |
 * | example       | AT+UART_FW_UPGRADE     |
 *
 */

#ifndef ATCMD_GENERAL_DEF_H__
#define ATCMD_GENERAL_DEF_H__

#define ATCMD_LIST                      "AT+H"
#define ATCMD_REBOOT                "AT+REBOOT"
#define ATCMD_VERSION               "AT+VERSION"
#define ATCMD_MINFO                 "AT+MF_INFO"
#define ATCMD_POWERSAVE             "AT+POWERSAVE"

#define ATCMD_REMOVE_CONF           "AT+REMOVE_CONF"

#define ATCMD_CFGPARSER             ("AT+CFGPARSER")
#define ATCMD_CFG_TESTER            ("AT+CFG_TESTER")
#define ATCMD_CFG_MAC_TESTER        ("AT+CFG_MAC_TESTER")

#define ATCMD_RADIO_WRITE_MAC1		"AT+RF_WRITE_MAC1"
#define ATCMD_RADIO_READ_MAC1			"AT+RF_READ_MAC1"
#define ATCMD_RADIO_WRITE_MAC2		"AT+RF_WRITE_MAC2"
#define ATCMD_RADIO_READ_MAC2			"AT+RF_READ_MAC2"

//efuse api
#define ATCMD_EFUSE_ENABLE_WRITE		"AT+EFUSE_ENABLE_WRITE"
#define ATCMD_EFUSE_WRITE_MAC		"AT+EFUSE_WRITE_MAC"
#define ATCMD_EFUSE_READ_MAC	        	"AT+EFUSE_READ_MAC"
#define ATCMD_EFUSE_DUMP	        	"AT+EFUSE_DUMP"

#define ATCMD_BENCH_FLASH_READ      "bflashr"
#define ATCMD_BENCH_FLASH_WRITE     "bflashw"
#define ATCMD_FLASH_WRITE           "flashw"
#define ATCMD_FLASH_READ            "flashr"
#define ATCMD_FLASH_ERASE           "flashe"
#define ATCMD_FLASH_ERASE32K        "flash32ke"
//### following are not AT CMD,they are for debugging ###
#define ATCMD_HELP                  "help"
#define ATCMD_TEST_PERIPHERAL       "testp"
#define ATCMD_SYSINFO				"sysinfo"
#define ATCMD_FSINFO				"fsinfo"
#define ATCMD_FSRESET				"fsreset"
#define ATCMD_MEMINFO				"meminfo"
#define ATCMD_MEMDUMP				"md"
#define ATCMD_WRITE_REG             "regw"
#define ATCMD_READ_REG              "regr"

// System Command
#define ATCMD_UART_FW_UPGRADE       ("AT+UART_FW_UPGRADE")
#endif
