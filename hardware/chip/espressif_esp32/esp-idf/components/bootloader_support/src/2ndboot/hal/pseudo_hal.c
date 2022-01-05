#if 1
#include <string.h>
#include <stdint.h>
#include <limits.h>
#include <sys/param.h>

#include "esp_attr.h"
#include "esp_log.h"
#include "esp32/rom/uart.h"
#include "esp32/rom/ets_sys.h"
#include "sdkconfig.h"
#if CONFIG_IDF_TARGET_ESP32
#include "soc/dport_reg.h"
#include "esp32/rom/cache.h"
#include "esp32/rom/spi_flash.h"
#include "esp32/rom/secure_boot.h"
#elif CONFIG_IDF_TARGET_ESP32S2
#include "esp32s2/rom/cache.h"
#include "esp32s2/rom/spi_flash.h"
#include "esp32s2/rom/secure_boot.h"
#include "soc/extmem_reg.h"
#include "soc/cache_memory.h"
#elif CONFIG_IDF_TARGET_ESP32S3
#include "esp32s3/rom/cache.h"
#include "esp32s3/rom/spi_flash.h"
#include "esp32s3/rom/secure_boot.h"
#include "soc/extmem_reg.h"
#include "soc/cache_memory.h"
#elif CONFIG_IDF_TARGET_ESP32C3
#include "esp32c3/rom/cache.h"
#include "esp32c3/rom/efuse.h"
#include "esp32c3/rom/ets_sys.h"
#include "esp32c3/rom/spi_flash.h"
#include "esp32c3/rom/crc.h"
#include "esp32c3/rom/uart.h"
#include "esp32c3/rom/gpio.h"
#include "esp32c3/rom/secure_boot.h"
#include "soc/extmem_reg.h"
#include "soc/cache_memory.h"
#elif CONFIG_IDF_TARGET_ESP32H2
#include "esp32h2/rom/cache.h"
#include "esp32h2/rom/efuse.h"
#include "esp32h2/rom/ets_sys.h"
#include "esp32h2/rom/spi_flash.h"
#include "esp32h2/rom/crc.h"
#include "esp32h2/rom/uart.h"
#include "esp32h2/rom/gpio.h"
#include "esp32h2/rom/secure_boot.h"
#include "soc/extmem_reg.h"
#include "soc/cache_memory.h"
#else // CONFIG_IDF_TARGET_*
#error "Unsupported IDF_TARGET"
#endif

#include "soc/soc.h"
#include "soc/cpu.h"
#include "soc/rtc.h"
#include "soc/gpio_periph.h"
#include "soc/efuse_periph.h"
#include "soc/rtc_periph.h"
#include "soc/timer_periph.h"

#include "esp_image_format.h"
#include "esp_secure_boot.h"
#include "esp_flash_encrypt.h"
#include "esp_flash_partitions.h"
#include "bootloader_random.h"
#include "bootloader_config.h"
#include "bootloader_common.h"
#include "bootloader_utility.h"
#include "bootloader_sha.h"
#include "esp_efuse.h"
#include "nbpatch.h"
#include "libc.h"
#include "updater.h"
#include "bootloader_flash.h"
void sys_delayms(volatile int ms) 
{
    ets_delay_us(ms * 1000);	
}

void sys_reboot(void) 
{
    bootloader_reset();
}

/* watchdog APIs */
void wdg_init(unsigned int ms) 
{
}

void wdg_feed(void) 
{
}

void wdg_finish(void) 
{
}

void uart_init(void) 
{
}

void uart_send(unsigned char *pReq, int ReqSize)
{
	if( !pReq || ReqSize == 0 )
		return;
	for (int i = 0; i < ReqSize; i++) {
        uart_tx_one_char(pReq[i]);
	}
}

void uart_send_string(char *pstr) 
{
	uart_send((unsigned char *)pstr, (int)strlen(pstr));
}

unsigned char uart_recv_byte(unsigned char *c) 
{
	unsigned char nc   = 0;
    int ret;

    ret = uart_rx_one_char(&nc);	
	if (ret == 0) {
		*c = nc;
		return 1;
	}
	return 0;

}

void uart_send_byte(unsigned char buf) {
    uart_tx_one_char(buf);
}

int flash_erase_single(unsigned int offset, unsigned int len) 
{
    if (len % 0x1000 != 0) {
        len = ((len + 0x1000 - 1) / 0x1000) * 0x1000;
    }
    return bootloader_flash_erase_range(offset, len);
}

int flash_erase(unsigned int offset, unsigned int len) 
{
    return bootloader_flash_erase_range(offset, len);
}

int flash_read_data(unsigned int offset, unsigned char *buf, unsigned int len) 
{
    return bootloader_flash_read(offset, buf, len, false); 
}

int flash_write_data(unsigned int address, unsigned char *buf, unsigned int len) 
{	
    return bootloader_flash_write(address, buf, len, false);
}

hal_logic_partition_t *flash_get_info(hal_partition_t no)
{
    const esp_partition_info_t *partitions;
    const char *partition_usage;
    esp_err_t err;
    int num_partitions;
    static hal_logic_partition_t g_partition_table[HAL_PARTITION_MAX];
    static bool inited = 0;

final:    
    if (inited) {
        if (!memcmp(&g_partition_table[no],
                  &g_partition_table[HAL_PARTITION_FTL], 
                  sizeof(hal_logic_partition_t))) {
            return NULL;
        } else {
            printf("%s no:%d offset:0x%x  size:0x%x\r\n", __func__, no, 
              g_partition_table[no].partition_start_addr, g_partition_table[no].partition_length);
            return &g_partition_table[no];
        }
    }
    partitions = bootloader_mmap(ESP_PARTITION_TABLE_OFFSET, ESP_PARTITION_TABLE_MAX_LEN);
    if (!partitions) {
        return NULL;
    }

    err = esp_partition_table_verify(partitions, true, &num_partitions);
    if (err != ESP_OK) {
        return NULL;
    }

    for (int i = 0; i < num_partitions; i++) {
        const esp_partition_info_t *partition = &partitions[i];
        
        switch (partition->type) {
        case PART_TYPE_APP: /* app partition */
            switch (partition->subtype) {
            case PART_SUBTYPE_FACTORY: /* factory binary */
                break;
            case PART_SUBTYPE_TEST: /* test binary */
                break;
            case 0x10: /* test binary */
                g_partition_table[HAL_PARTITION_APPLICATION].partition_start_addr =
                    partition->pos.offset;
                g_partition_table[HAL_PARTITION_APPLICATION].partition_length =
                    partition->pos.size;
                break;
            case 0x11: /* test binary */
                g_partition_table[HAL_PARTITION_OTA_TEMP].partition_start_addr =
                    partition->pos.offset;
                g_partition_table[HAL_PARTITION_OTA_TEMP].partition_length =
                    partition->pos.size;
                break;
            default: 
                break;
            }
            break; /* PART_TYPE_APP */
        case PART_TYPE_DATA: /* data partition */
            switch (partition->subtype) {
            case PART_SUBTYPE_DATA_OTA: /* ota data */
                break;
            case PART_SUBTYPE_DATA_RF:
                break;
            case PART_SUBTYPE_DATA_WIFI:
                break;
            case PART_SUBTYPE_DATA_NVS_KEYS:
                break;
            case PART_SUBTYPE_DATA_EFUSE_EM:
                break;
            case 0x91:
                g_partition_table[HAL_PARTITION_PARAMETER_1].partition_start_addr = 
                        partition->pos.offset;
                g_partition_table[HAL_PARTITION_PARAMETER_1].partition_length = 
                        partition->pos.size;
                break;
            case 0x92:
                g_partition_table[HAL_PARTITION_PARAMETER_3].partition_start_addr = 
                        partition->pos.offset;
                g_partition_table[HAL_PARTITION_PARAMETER_3].partition_length = 
                        partition->pos.size;
                break;
            default:
                break;
            }
            break; /* PARTITION_USAGE_DATA */
        default: /* other partition type */
            break;
        }
       
    } 
    bootloader_munmap(partitions);
    inited = 1;
    goto final; 
    return NULL;
}

static const unsigned long crc_table[256] = {
    0x00000000, 0x77073096, 0xEE0E612C, 0x990951BA,
    0x076DC419, 0x706AF48F, 0xE963A535, 0x9E6495A3,
    0x0EDB8832, 0x79DCB8A4, 0xE0D5E91E, 0x97D2D988,
    0x09B64C2B, 0x7EB17CBD, 0xE7B82D07, 0x90BF1D91,
    0x1DB71064, 0x6AB020F2, 0xF3B97148, 0x84BE41DE,
    0x1ADAD47D, 0x6DDDE4EB, 0xF4D4B551, 0x83D385C7,
    0x136C9856, 0x646BA8C0, 0xFD62F97A, 0x8A65C9EC,
    0x14015C4F, 0x63066CD9, 0xFA0F3D63, 0x8D080DF5,
    0x3B6E20C8, 0x4C69105E, 0xD56041E4, 0xA2677172,
    0x3C03E4D1, 0x4B04D447, 0xD20D85FD, 0xA50AB56B,
    0x35B5A8FA, 0x42B2986C, 0xDBBBC9D6, 0xACBCF940,
    0x32D86CE3, 0x45DF5C75, 0xDCD60DCF, 0xABD13D59,
    0x26D930AC, 0x51DE003A, 0xC8D75180, 0xBFD06116,
    0x21B4F4B5, 0x56B3C423, 0xCFBA9599, 0xB8BDA50F,
    0x2802B89E, 0x5F058808, 0xC60CD9B2, 0xB10BE924,
    0x2F6F7C87, 0x58684C11, 0xC1611DAB, 0xB6662D3D,
    0x76DC4190, 0x01DB7106, 0x98D220BC, 0xEFD5102A,
    0x71B18589, 0x06B6B51F, 0x9FBFE4A5, 0xE8B8D433,
    0x7807C9A2, 0x0F00F934, 0x9609A88E, 0xE10E9818,
    0x7F6A0DBB, 0x086D3D2D, 0x91646C97, 0xE6635C01,
    0x6B6B51F4, 0x1C6C6162, 0x856530D8, 0xF262004E,
    0x6C0695ED, 0x1B01A57B, 0x8208F4C1, 0xF50FC457,
    0x65B0D9C6, 0x12B7E950, 0x8BBEB8EA, 0xFCB9887C,
    0x62DD1DDF, 0x15DA2D49, 0x8CD37CF3, 0xFBD44C65,
    0x4DB26158, 0x3AB551CE, 0xA3BC0074, 0xD4BB30E2,
    0x4ADFA541, 0x3DD895D7, 0xA4D1C46D, 0xD3D6F4FB,
    0x4369E96A, 0x346ED9FC, 0xAD678846, 0xDA60B8D0,
    0x44042D73, 0x33031DE5, 0xAA0A4C5F, 0xDD0D7CC9,
    0x5005713C, 0x270241AA, 0xBE0B1010, 0xC90C2086,
    0x5768B525, 0x206F85B3, 0xB966D409, 0xCE61E49F,
    0x5EDEF90E, 0x29D9C998, 0xB0D09822, 0xC7D7A8B4,
    0x59B33D17, 0x2EB40D81, 0xB7BD5C3B, 0xC0BA6CAD,
    0xEDB88320, 0x9ABFB3B6, 0x03B6E20C, 0x74B1D29A,
    0xEAD54739, 0x9DD277AF, 0x04DB2615, 0x73DC1683,
    0xE3630B12, 0x94643B84, 0x0D6D6A3E, 0x7A6A5AA8,
    0xE40ECF0B, 0x9309FF9D, 0x0A00AE27, 0x7D079EB1,
    0xF00F9344, 0x8708A3D2, 0x1E01F268, 0x6906C2FE,
    0xF762575D, 0x806567CB, 0x196C3671, 0x6E6B06E7,
    0xFED41B76, 0x89D32BE0, 0x10DA7A5A, 0x67DD4ACC,
    0xF9B9DF6F, 0x8EBEEFF9, 0x17B7BE43, 0x60B08ED5,
    0xD6D6A3E8, 0xA1D1937E, 0x38D8C2C4, 0x4FDFF252,
    0xD1BB67F1, 0xA6BC5767, 0x3FB506DD, 0x48B2364B,
    0xD80D2BDA, 0xAF0A1B4C, 0x36034AF6, 0x41047A60,
    0xDF60EFC3, 0xA867DF55, 0x316E8EEF, 0x4669BE79,
    0xCB61B38C, 0xBC66831A, 0x256FD2A0, 0x5268E236,
    0xCC0C7795, 0xBB0B4703, 0x220216B9, 0x5505262F,
    0xC5BA3BBE, 0xB2BD0B28, 0x2BB45A92, 0x5CB36A04,
    0xC2D7FFA7, 0xB5D0CF31, 0x2CD99E8B, 0x5BDEAE1D,
    0x9B64C2B0, 0xEC63F226, 0x756AA39C, 0x026D930A,
    0x9C0906A9, 0xEB0E363F, 0x72076785, 0x05005713,
    0x95BF4A82, 0xE2B87A14, 0x7BB12BAE, 0x0CB61B38,
    0x92D28E9B, 0xE5D5BE0D, 0x7CDCEFB7, 0x0BDBDF21,
    0x86D3D2D4, 0xF1D4E242, 0x68DDB3F8, 0x1FDA836E,
    0x81BE16CD, 0xF6B9265B, 0x6FB077E1, 0x18B74777,
    0x88085AE6, 0xFF0F6A70, 0x66063BCA, 0x11010B5C,
    0x8F659EFF, 0xF862AE69, 0x616BFFD3, 0x166CCF45,
    0xA00AE278, 0xD70DD2EE, 0x4E048354, 0x3903B3C2,
    0xA7672661, 0xD06016F7, 0x4969474D, 0x3E6E77DB,
    0xAED16A4A, 0xD9D65ADC, 0x40DF0B66, 0x37D83BF0,
    0xA9BCAE53, 0xDEBB9EC5, 0x47B2CF7F, 0x30B5FFE9,
    0xBDBDF21C, 0xCABAC28A, 0x53B39330, 0x24B4A3A6,
    0xBAD03605, 0xCDD70693, 0x54DE5729, 0x23D967BF,
    0xB3667A2E, 0xC4614AB8, 0x5D681B02, 0x2A6F2B94,
    0xB40BBE37, 0xC30C8EA1, 0x5A05DF1B, 0x2D02EF8D,
};

unsigned long crc32(unsigned long crc, const unsigned char *buf, unsigned int len)
{
    unsigned long c;

    c = crc ^ 0xffffffffL;

    while (len) {
        c = crc_table[(c ^ (*buf++)) & 0xff] ^ (c >> 8);
        len--;
    }

    return c ^ 0xffffffffL;
}

int ota_set_bootinfo_crc32value(struct ota_boot_info *info)
{
    uint32_t crc32_value = 0;
    uint8_t *flash_pointer = NULL;

    flash_pointer = (uint8_t *)(info);
    crc32_value = crc32(crc32_value, (uint8_t *)(flash_pointer + OTA_BOOT_INFO_HEAD_LEN), info->info_len);
    info->crc32 = crc32_value;

    return 0;
}

int ota_get_bootinfo(struct ota_boot_info *info, enum bootinfo_zone zone)
{
    uint32_t lock = 0;
    uint32_t start_addr = 0;
    int32_t ret = 0;
    hal_logic_partition_t* partition_info;
    volatile char *flashPointer = NULL;

    if (zone >= OTA_BOOTINFO_ZONEMAX) {
        return OTA_FAILE;
    }        
    partition_info = (hal_logic_partition_t *)flash_get_info(HAL_PARTITION_PARAMETER_3);
    if(partition_info == NULL) {
        return OTA_FAILE;
    }        
    if (zone == OTA_BOOTINFO_ZONEA) {
        start_addr = partition_info->partition_start_addr;
    } else if (zone == OTA_BOOTINFO_ZONEB) {
        start_addr = partition_info->partition_start_addr + OTA_BOOT_INFO_SIZE;
    }        
    
    flashPointer = (volatile char *)bootloader_mmap(start_addr, OTA_BOOT_INFO_SIZE);
    memcpy((uint8_t*)info, (void *)flashPointer, sizeof(struct ota_boot_info));
    bootloader_munmap(flashPointer);
    return 0;
}

int ota_set_bootinfo(struct ota_boot_info *info, enum bootinfo_zone zone)
{
    int ret = 0;
    uint32_t lock = 0;
    uint32_t start_addr = 0;
    uint8_t buffer[FLASH_SECTOR_SIZE_IN_BYTES] = {0};
    hal_logic_partition_t* partition_info = NULL;

    if (zone >= OTA_BOOTINFO_ZONEMAX) {
        return OTA_FAILE;
    }

    ret = ota_set_bootinfo_crc32value(info);
    if (ret) {
        return OTA_FAILE;
    }
    partition_info = (hal_logic_partition_t *)flash_get_info(HAL_PARTITION_PARAMETER_3);
    if(partition_info == NULL) {
        return OTA_FAILE;
    }
    if (zone == OTA_BOOTINFO_ZONEA) {
        start_addr = partition_info->partition_start_addr;
    } else if (zone == OTA_BOOTINFO_ZONEB) {
        start_addr = partition_info->partition_start_addr + OTA_BOOT_INFO_SIZE;
    }


    memcpy(buffer, (uint8_t *)info, sizeof(struct ota_boot_info));
    ret = flash_erase(start_addr, FLASH_SECTOR_SIZE_IN_BYTES);
    if (ret != 0) {
        goto end;
    }
    ret = flash_write_data(start_addr, buffer, FLASH_SECTOR_SIZE_IN_BYTES);
    if (ret != 0) {
        goto end;
    }

end:
    return ret;
}

int ota_set_bootinfo_to_zoneAB(struct ota_boot_info *info)
{
    int ret = 0;

    ret = ota_set_bootinfo(info, OTA_BOOTINFO_ZONEA);
    if (ret) {
    }

    ret = ota_set_bootinfo(info, OTA_BOOTINFO_ZONEB);
    if (ret) {
    }

    return ret;
}

void ota_set_zoneAB_bootinfo_to_default(void)
{
    struct ota_boot_info info;
    int ret = 0;


    //set boot info to a and b.
    memset(&info, 0, sizeof(struct ota_boot_info));
    info.linkA_used_flag         = OTA_LINK_USED; 
    info.linkB_used_flag         = OTA_LINK_USED; //if RTOSB not write, set OTA_LINK_NOTUSED.
    info.info_len                = OTA_BOOT_INFO_BODY_LEN;
    info.odm_type                = 0;
    info.reserved                = 0;
    info.update_link             = OTA_LINK_A; //default:OTA_LINK_A
    info.reboot_reason           = 0;
    info.crash_reboot_count      = 0;
    info.secureERR_reboot_count  = 0;
    info.reboot_count_max        = 0;
    info.fallback_disable        = 1;
    
    ret = ota_set_bootinfo_to_zoneAB(&info);
    if (ret) {
        return;
    }
}

int ota_check_bootinfo(enum bootinfo_zone zone)
{
    uint32_t crc32_value = 0;
    uint32_t flash_offset = 0;
    int32_t ret = OTA_OK;
    uint8_t *flash_pointer = NULL;
    hal_logic_partition_t *partition_info = NULL;
    const struct ota_boot_info *info;

    if (zone >= OTA_BOOTINFO_ZONEMAX) {
        return OTA_FAILE;
    }

    partition_info = (hal_logic_partition_t *)flash_get_info(HAL_PARTITION_PARAMETER_3);
    if(partition_info == NULL) {
        return OTA_FAILE;
    }

    //get boot info to choose linkA or linkB.
    if (zone == OTA_BOOTINFO_ZONEA) {
        flash_offset = partition_info->partition_start_addr;
    } else if (zone == OTA_BOOTINFO_ZONEB) {
        flash_offset = partition_info->partition_start_addr + OTA_BOOT_INFO_SIZE; //boot info zoneB start address
    }

    info = (const struct ota_boot_info *)bootloader_mmap(flash_offset, OTA_BOOT_INFO_SIZE);
    if (info->update_link >= OTA_LINK_MAX) {
        ret = OTA_FAILE;
        goto end;
    } else if ((info->update_link == OTA_LINK_A) && (info->linkA_used_flag != OTA_LINK_USED)) {
        ret = OTA_FAILE;
        goto end;

    } else if ((info->update_link == OTA_LINK_B) && (info->linkB_used_flag != OTA_LINK_USED)) {
        ret = OTA_FAILE;
        goto end;
    }

    flash_pointer = (uint8_t *)(info);
    crc32_value = crc32(crc32_value, (uint8_t *)(flash_pointer + OTA_BOOT_INFO_HEAD_LEN), info->info_len);
    if (crc32_value != info->crc32) { 
        ret = OTA_FAILE;
        goto end;
    }
end:
    bootloader_munmap(info);
    return ret;
}

enum bootinfo_zone ota_get_valid_bootinfo_zone(void)
{
    enum bootinfo_zone ret = OTA_BOOTINFO_ZONEA;

    ret = ota_check_bootinfo(OTA_BOOTINFO_ZONEA);
    if (!ret) {
        return OTA_BOOTINFO_ZONEA;
    } else {
        ret = ota_check_bootinfo(OTA_BOOTINFO_ZONEB);
        if (!ret) {
            return OTA_BOOTINFO_ZONEB;
        } else { //first boot or both boot info bad, set both to default.
            ota_set_zoneAB_bootinfo_to_default();
            return OTA_BOOTINFO_ZONEA;
        }
    }
    return OTA_BOOTINFO_ZONEA;
}

void ota_show_bootinfo_zoneAB(void)
{
    struct ota_boot_info info;
    uint32_t num = 0; 

    for (num = 0; num < OTA_BOOTINFO_ZONEMAX; num ++) {
        ota_get_bootinfo(&info, num);
        printf("show zone_%d boot info:\r\n", num);
        printf("crc32:0x%x\r\n", info.crc32);
        printf("info_len:0x%x\r\n", info.info_len);
        printf("odm_type:0x%x\r\n", info.odm_type);
        printf("reserved:0x%x\r\n", info.reserved);
        printf("linkA_used_flag:0x%x\r\n", info.linkA_used_flag);
        printf("linkB_used_flag:0x%x\r\n", info.linkB_used_flag);
        printf("update_link:0x%x\r\n", info.update_link);
        printf("crash_reboot_count:0x%x\r\n", info.crash_reboot_count);
        printf("secureERR_reboot_count:0x%x\r\n", info.secureERR_reboot_count);
        printf("reboot_count_max:0x%x\r\n", info.reboot_count_max);
        printf("reboot_reason:0x%x\rn", info.reboot_reason);
        printf("fallback_disable:0x%x\r\n", info.fallback_disable);
        printf("\r\n");
    }    
}

int ota_boot2_do_fallback(enum ota_link new_link)
{
    struct ota_boot_info info;
    int ret = 0;
    enum bootinfo_zone zone;

    if (new_link >= OTA_LINK_MAX) {
        return -1;
    }

    zone = ota_get_valid_bootinfo_zone();
    ota_get_bootinfo(&info, zone);

    if (new_link == OTA_LINK_A) {
        info.linkA_used_flag = OTA_LINK_USED;
        info.linkB_used_flag = OTA_LINK_NOTUSED;
    } else if (new_link == OTA_LINK_B) {
        info.linkB_used_flag = OTA_LINK_USED;
        info.linkA_used_flag = OTA_LINK_NOTUSED;
    }
    info.info_len                 = OTA_BOOT_INFO_BODY_LEN;
    info.update_link              = new_link;
    info.reboot_reason            = 0;
    info.crash_reboot_count       = 0;
    info.secureERR_reboot_count   = 0;

    ret = ota_set_bootinfo_to_zoneAB(&info);
    if (ret) {
        return -1;
    }

    return ret;
}

void ota_switch_ab_boot(void)
{       
    int ret = 0;
    struct ota_boot_info ctrl;
    enum bootinfo_zone zone = OTA_BOOTINFO_ZONEA;

    //get boot info to choose linkA or linkB.
    zone = ota_get_valid_bootinfo_zone();
    ret = ota_get_bootinfo(&ctrl, zone);
    if (ret) {
        return;
    }

    ota_show_bootinfo_zoneAB();

    if (ctrl.update_link == OTA_LINK_A) {
        ota_boot2_do_fallback(OTA_LINK_B);
    } else {
        ota_boot2_do_fallback(OTA_LINK_A);
    }

    ota_show_bootinfo_zoneAB();
}

int ota_get_ab_boot(void)
{
    struct ota_boot_info ctrl;
    int ret = 0;
    enum bootinfo_zone zone = OTA_BOOTINFO_ZONEA;

    //get boot info to choose linkA or linkB.
    zone = ota_get_valid_bootinfo_zone();
    ret = ota_get_bootinfo(&ctrl, zone);
    if (ret) {
        return 0;
    }

    if(ctrl.update_link == OTA_LINK_B) {
        return 1;
    }

    return 0;
}

int ota_2ndboot_ab_get(void) 
{
	return ota_get_ab_boot();
}

void ota_2ndboot_ab_switch(void)
{
    ota_switch_ab_boot();
}
#endif

