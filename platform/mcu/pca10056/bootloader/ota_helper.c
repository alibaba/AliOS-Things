#include <string.h>
#include "ota_helper.h"
#include "nrf52840.h"
#include "nrf52840_bitfields.h"
//#include "nrf_dfu_types.h"
#include <stdio.h>

#define printf

static inline void wait_for_flash_ready(void)
{
    while (NRF_NVMC->READY == NVMC_READY_READY_Busy) {;}
}


void nrf_nvmc_page_erase(uint32_t address)
{
    // Enable erase.
    NRF_NVMC->CONFIG = NVMC_CONFIG_WEN_Een;
    __ISB();
    __DSB();

    // Erase the page
    NRF_NVMC->ERASEPAGE = address;
    wait_for_flash_ready();

    NRF_NVMC->CONFIG = NVMC_CONFIG_WEN_Ren;
    __ISB();
    __DSB();
}


void nrf_nvmc_write_byte(uint32_t address, uint8_t value)
{
    uint32_t byte_shift = address & (uint32_t)0x03;
    uint32_t address32 = address & ~byte_shift; // Address to the word this byte is in.
    uint32_t value32 = (*(uint32_t*)address32 & ~((uint32_t)0xFF << (byte_shift << (uint32_t)3)));
    value32 = value32 + ((uint32_t)value << (byte_shift << 3));

    // Enable write.
    NRF_NVMC->CONFIG = NVMC_CONFIG_WEN_Wen;
    __ISB();
    __DSB();

    *(uint32_t*)address32 = value32;
    wait_for_flash_ready();

    NRF_NVMC->CONFIG = NVMC_CONFIG_WEN_Ren;
    __ISB();
    __DSB();
}

void nrf_nvmc_write_word(uint32_t address, uint32_t value)
{
    // Enable write.
    NRF_NVMC->CONFIG = NVMC_CONFIG_WEN_Wen;
    __ISB();
    __DSB();

    *(uint32_t*)address = value;
    wait_for_flash_ready();

    NRF_NVMC->CONFIG = NVMC_CONFIG_WEN_Ren;
    __ISB();
    __DSB();
}


void nrf_nvmc_write_bytes(uint32_t address, const uint8_t * src, uint32_t num_bytes)
{
    for (uint32_t i = 0; i < num_bytes; i++)
    {
       nrf_nvmc_write_byte(address + i, src[i]);
    }
}


void nrf_nvmc_write_words(uint32_t address, const uint32_t * src, uint32_t num_words)
{
    // Enable write.
    NRF_NVMC->CONFIG = NVMC_CONFIG_WEN_Wen;
    __ISB();
    __DSB();

    for (uint32_t i = 0; i < num_words; i++)
    {
        ((uint32_t*)address)[i] = src[i];
        wait_for_flash_ready();
    }

    NRF_NVMC->CONFIG = NVMC_CONFIG_WEN_Ren;
    __ISB();
    __DSB();
}

static int flash_read(uint32_t src, void * p_dest, uint32_t len)
{
    memcpy(p_dest, (uint32_t*)src, len);

    return 0;
}

static int flash_write(uint32_t               dest,
                       void           const * p_src,
                       uint32_t               len,
                       void                 * p_param)
{
    nrf_nvmc_write_words(dest, (uint32_t*)p_src, (len / PROGRAM_UNIT));

    return 0;
}


static int flash_erase(uint32_t               page_addr,
                       uint32_t               len,
                       void                 * p_param)
{
    uint32_t progress = 0;

    while (progress != len)
    {
        nrf_nvmc_page_erase(page_addr + (progress * ERASE_UNIT));
        progress++;
    }

    return 0;
}

static nrf_dfu_settings_t s_dfu_settings;
static nrf_dfu_settings_t *s_dfu_settings_in_flash = (nrf_dfu_settings_t *)SETTINGS_OFFSET;

static uint32_t align_to_page(uint32_t val, uint32_t page_size)
{
    return ((val + page_size - 1 ) &~ (page_size - 1));
}

static int flash_compare(uint8_t *dest, uint8_t *src, uint32_t len)
{
    while (len-- != 0) {
            if (*dest != *src) return (*dest - *src);
              dest++; src++;
        }
        
        return 0;
}

static uint32_t crc32_compute(uint8_t const * p_data, uint32_t size, uint32_t const * p_crc)
{
    uint32_t crc;

    crc = (p_crc == NULL) ? 0xFFFFFFFF : ~(*p_crc);
    for (uint32_t i = 0; i < size; i++)
    {
        crc = crc ^ p_data[i];
        for (uint32_t j = 8; j > 0; j--)
        {
            crc = (crc >> 1) ^ (0xEDB88320U & ((crc & 1) ? 0xFFFFFFFF : 0));
        }
    }
    return ~crc;
}

static uint32_t nrf_dfu_settings_crc_get(void)
{
    // The crc is calculated from the s_dfu_settings struct, except the crc itself and the init command
    return crc32_compute((uint8_t*)&s_dfu_settings + 4, DFU_SETTINGS_INIT_COMMAND_OFFSET  - 4, NULL);
}

int nrf_dfu_settings_write()
{
    int err_code;

    // Not setting the callback function because ERASE is required before STORE
    // Only report completion on successful STORE.
    err_code = flash_erase((uint32_t)s_dfu_settings_in_flash, SETTINGS_SIZE / PAGE_SIZE, NULL);

    if (err_code != 0)
    {
        return -1;
    }

    s_dfu_settings.crc = nrf_dfu_settings_crc_get();

    static nrf_dfu_settings_t temp_dfu_settings;
    memcpy(&temp_dfu_settings, &s_dfu_settings, sizeof(nrf_dfu_settings_t));

    err_code = flash_write((uint32_t)s_dfu_settings_in_flash,
                           &temp_dfu_settings,
                           sizeof(nrf_dfu_settings_t),
                           NULL);

    if (err_code != 0)
    {
        return -1;
    }

    return 0;
}

static void nrf_dfu_invalidate_bank(nrf_dfu_bank_t * p_bank)
{
    // Set the bank-code to invalid, and reset size/CRC
    memset(p_bank, 0, sizeof(nrf_dfu_bank_t));

    // Reset write pointer after completed operation
    s_dfu_settings.write_offset = 0;

    // Reset SD size
    s_dfu_settings.sd_size = 0;

    // Promote dual bank layout
    s_dfu_settings.bank_layout = NRF_DFU_BANK_LAYOUT_DUAL;

    // Signify that bank 0 is empty
    s_dfu_settings.bank_current = NRF_DFU_CURRENT_BANK_0;
}

/** @brief Function to continue application update.
 *
 * @details This function will be called after reset if there is a valid application in Bank1
 *          required to be copied down to Bank 0.
 *
 * @param[in]       src_addr            Source address of the application to copy from Bank1 to Bank0.
 *
 * @retval  NRF_SUCCESS                 Continuation was successful.
 * @retval  NRF_ERROR_NULL              Invalid data during compare.
 * @retval  FS_ERR_UNALIGNED_ADDR       A call to fstorage was not aligned to a page boundary or the address was not word aliged.
 * @retval  FS_ERR_INVALID_ADDR         The destination of a call to fstorage does not point to
 *                                      the start of a flash page or the operation would
 *                                      go beyond the flash memory boundary.
 * @retval  FS_ERR_NOT_INITIALIZED      The fstorage module is not initialized.
 * @retval  FS_ERR_INVALID_CFG          The initialization of the fstorage module is invalid.
 * @retval  FS_ERR_NULL_ARG             A call to fstorage had an invalid NULL argument.
 * @retval  FS_ERR_INVALID_ARG          A call to fstorage had invalid arguments.
 * @retval  FS_ERR_QUEUE_FULL           If the internal operation queue of the fstorage module is full.
 * @retval  FS_ERR_FAILURE_SINCE_LAST   If an error occurred in another transaction and fstorage cannot continue before
 *                                      the event has been dealt with.
 */
static uint32_t nrf_dfu_app_continue(uint32_t src_addr)
{
    // This function is only in use when new app is present in Bank 1
    uint32_t const image_size  = s_dfu_settings.bank_1.image_size;
    uint32_t const split_size  = PAGE_SIZE; // Must be page aligned

    uint32_t ret_val     = 0;
    uint32_t target_addr = MAIN_APPLICATION_START_ADDR + s_dfu_settings.write_offset;
    uint32_t length_left = (image_size - s_dfu_settings.write_offset);
    uint32_t cur_len;
    uint32_t crc;
    uint32_t i = 0;

    src_addr += s_dfu_settings.write_offset;

    printf("In %s line %d, copying bank 1 to bank 0 ...\r\n", __func__, __LINE__);
    printf("target_addr: 0x%x, length_left: 0x%x\r\n", target_addr, length_left);

    // Copy the application down safely
    do
    {
        cur_len = (length_left > split_size) ? split_size : length_left;

        // Erase the target page
        ret_val = flash_erase(target_addr, split_size / PAGE_SIZE, NULL);
        if (ret_val != 0)
        {
            return ret_val;
        }

        // Flash one page
        ret_val = flash_write(target_addr, (void const *)src_addr, cur_len, NULL);
        if (ret_val != 0)
        {
            return ret_val;
        }

        ret_val = flash_compare((uint8_t*)target_addr, (uint8_t*)src_addr, cur_len);
        if (ret_val != 0)
        {
            return ret_val;
        }

        s_dfu_settings.write_offset += cur_len;
        ret_val = nrf_dfu_settings_write();
        if (ret_val != 0) {
            printf("In %s, nrf_dfu_settings_write failed.\r\n", __func__);
            return -1;
        }

        target_addr += cur_len;
        src_addr += cur_len;

        length_left -= cur_len;
                
        printf(".");
    }
    while (length_left > 0);
    printf("\r\nCongratulations! Copy finished!\r\n");

    printf("In %s line %d, calculating crc ...\r\n", __func__, __LINE__);
        
    // Check the CRC of the copied data. Enable if so.
    crc = crc32_compute((uint8_t*)MAIN_APPLICATION_START_ADDR, image_size, NULL);

    printf("In %s line %d, the crc calculated is 0x%x, the expected crc is 0x%x.\r\n", \
           __func__, __LINE__, crc, s_dfu_settings.bank_1.image_crc);
        
    if (crc == s_dfu_settings.bank_1.image_crc)
    {
        printf("Good, calculated crc and read crc is equal!\r\n");
        s_dfu_settings.bank_0.bank_code = NRF_DFU_BANK_VALID_APP;
        s_dfu_settings.bank_0.image_crc = crc;
        s_dfu_settings.bank_0.image_size = image_size;
    }
    else
    {
        printf("Bad, calculated crc and read crc is NOT equal!\r\n");
        // crc calu failed;
    }

    printf("In %s line %d, invaliding bank 1 ...\r\n", __func__, __LINE__);
        
    nrf_dfu_invalidate_bank(&s_dfu_settings.bank_1);
        
    printf("In %s line %d, writing settings ...\r\n", __func__, __LINE__);
           s_dfu_settings.init_command[0] = UPDATE_FINISHED;
    ret_val = nrf_dfu_settings_write();

    printf("In %s line %d, end, last ret_val is %d.\r\n", __func__, __LINE__, ret_val);

    return ret_val;
}

static uint32_t nrf_dfu_continue_bank(nrf_dfu_bank_t * p_bank, uint32_t src_addr)
{
    uint32_t ret_val = 0;

    switch (p_bank->bank_code)
    {
       case NRF_DFU_BANK_VALID_APP:
            if(s_dfu_settings.bank_current == NRF_DFU_CURRENT_BANK_1)
            {
                // Only continue copying if valid app in Bank 1
                printf("In nrf_dfu_continue_bank, will copy bank 1 into bank 0 here.\r\n");
                ret_val = nrf_dfu_app_continue(src_addr);
            }
                printf("Valid app in bank_0, no copy required.\r\n");
            break;
       case NRF_DFU_BANK_VALID_SD:
       case NRF_DFU_BANK_VALID_BL:
             case NRF_DFU_BANK_INVALID:
       default:
            printf("In %s %d, not handled bank_code: %d.\r\n", __func__, __LINE__, p_bank->bank_code);
            break;
    }

    return ret_val;
}

static uint32_t nrf_dfu_continue()
{
    uint32_t            ret_val;
    nrf_dfu_bank_t    * p_bank;
    uint32_t            src_addr = MAIN_APPLICATION_START_ADDR;

    if (s_dfu_settings.bank_layout == NRF_DFU_BANK_LAYOUT_SINGLE )
    {
        printf("In nrf_dfu_continue, SINGLE layout, bank_0 used.\r\n");
        p_bank = &(s_dfu_settings.bank_0);
    }
    else if(s_dfu_settings.bank_current == NRF_DFU_CURRENT_BANK_0)
    {
        printf("In nrf_dfu_continue, bank_0 (current bank) used.\r\n");
        p_bank = &(s_dfu_settings.bank_0);
    }
    else
    {
        printf("In nrf_dfu_continue, bank_1 used.\r\n");
        p_bank = &(s_dfu_settings.bank_1);
        src_addr += align_to_page(s_dfu_settings.bank_0.image_size, PAGE_SIZE) + 0x1000;
    }

    printf("In nrf_dfu_continue, src_addr is 0x%x.\r\n", src_addr);

    ret_val = nrf_dfu_continue_bank(p_bank, src_addr);
    return ret_val;
}

static void nrf_dfu_init()
{
    //memcpy((void *)&s_dfu_settings, s_dfu_settings_in_flash, sizeof(nrf_dfu_settings_t));
    flash_read((uint32_t)s_dfu_settings_in_flash, (void *)&s_dfu_settings, sizeof(nrf_dfu_settings_t));
}

void ota_app(void)
{
    nrf_dfu_init();
    printf("After nrf_dfu_init\r\n");
    nrf_dfu_continue();
    printf("ota_app finished.\r\n");
}
