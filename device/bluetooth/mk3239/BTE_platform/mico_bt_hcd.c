#include "mico.h"
#include "platform.h"
#include "platform_config.h"

#ifdef MICO_USE_BT_PARTITION

static uint32_t image_size = 0x0;
void get_one_command(char *out, int offset)
{
    //uint32_t size = 0;
    uint16_t len = 0;

    //resource_read(&bt_patchram_image, offset + 2, 2, &size, &len);
    //len &= 0x0ff;
    //resource_read(&bt_patchram_image, offset, len + 3, &size, out);
    //uint32_t buffer_size;

    volatile uint32_t read_address = offset + 2;
    mico_logic_partition_t *driver_partition = MicoFlashGetInfo( MICO_PARTITION_BT_FIRMWARE );

    if ( image_size == 0) {
        image_size = driver_partition->partition_length;
    }

    MicoFlashRead( MICO_PARTITION_BT_FIRMWARE, &read_address, (uint8_t *)&len, 1);
    len = len & 0x00ff;
    read_address = offset;
    MicoFlashRead( MICO_PARTITION_BT_FIRMWARE, &read_address, (uint8_t *)out, len + 3);

    read_address = offset + 10;
}

uint32_t get_hcd_content_length()
{
#define READ_LEN 2048
    if (image_size == 0) {
        mico_logic_partition_t *driver_partition = MicoFlashGetInfo( MICO_PARTITION_BT_FIRMWARE );
        uint32_t offset = driver_partition->partition_length;
        uint32_t *p;
        uint32_t *buf = (uint32_t *)malloc(READ_LEN);

        uint32_t image_size2 = driver_partition->partition_length;
        do {
            offset -= READ_LEN; // Next block
            MicoFlashRead( MICO_PARTITION_BT_FIRMWARE, &offset, (uint8_t *)buf, READ_LEN);
            offset -= READ_LEN; // MicoFlashRead will increase FlashAddress READ_LEN, move back.
            p = buf + (READ_LEN - 4) / sizeof(uint32_t);
            while (p >= buf) {
                if (*p != 0xFFFFFFFF) {
                    goto EXIT;
                }
                p--;
                image_size2 -= 4;
            }
        } while (offset > 0);

EXIT:
        free(buf);
        image_size = image_size2; //35584;//image_size2;
    }
    return image_size;
}

#else

extern const uint8_t brcm_patchram_buf[];
extern const int brcm_patch_ram_length;

/*******************************************************************************
**
** Function         get_one_command
**
** Description      Called by an application to get a one complete HCI command
**
** Parameters       out - (output) buffer where to put the command.
**                  offset - position of the HCD file
** Returns          none
**
*******************************************************************************/
void get_one_command(char *out, int offset)
{
    int len = brcm_patchram_buf[2 + offset] & 0x00ff;
    memcpy(out, brcm_patchram_buf + offset, 3 + len);
}

/*******************************************************************************
**
** Function         get_hcd_content_length
**
** Description      get the total length of the HCD content
**
** Parameters       none
**
** Returns          none
**
*******************************************************************************/
uint32_t get_hcd_content_length()
{
    return brcm_patch_ram_length;
}
#endif
