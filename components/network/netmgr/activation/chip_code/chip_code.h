#ifndef _CHIP_CODE_H_
#define _CHIP_CODE_H_

#ifndef UINT32
#define UINT32 unsigned int
#endif

#ifndef UINT8
#define UINT8 unsigned char
#endif

#ifndef UINT16
#define UINT16 unsigned short
#endif

#ifndef NULL
#define NULL (void *)0
#endif

enum CHIP_VENDOR {
    UNKNOW_CHIP_VENDOR = 0, //全0为无效识别码保留
    BEKEN_CHIP_VENDOR,
    C_SKY_CHIP_VENDOR,
    CYPRESS_CHIP_VENDOR,
    DAHUA_CHIP_VENDOR,
    ESP_CHIP_VENDOR,
    GD_CHIP_VENDOR,
    LINUX_VENDOR,
    NXP_VENDOR,
    MICO_CHIP_VENDOR,
    NORDIC_CHIP_VENDOR,
    RENASAS_CHIP_VENDOR,
    RDA_CHIP_VENDOR,
    REALTEK_CHIP_VENDOR,
    STM_CHIP_VENDOR,
    XRADIO_CHIP_VENDOR,
    MTK_CHIP_VENDOR,
    ASR_CHIP_VENDOR,
    AIR_CHIP_VENDOR,
    NEOWAY_CHIP_VENDOR,
    FIBOCOM_CHIP_VENDOR,
    QUECTEL_CHIP_VENDOR,
    CHIP_VENDOR_LIMIT = 0xFFFF
};

enum CHIP_ID {
    UNKNOW_CHIP_ID = 0, //全0为无效识别码保留
    bk7231   ,
    bk7231u  ,
    csky     ,
    cy8c4147 ,
    cy8c6347 ,
    dahua    ,
    es8p508x ,
    esp32    ,
    esp8266  ,
    gd32f4xx ,
    linuxhost,//linux会被编译器认为是一个特殊符号，不能使用
    lpc54102 ,
    mkl27z644,
    moc108   ,
    mx1290   ,
    nrf52xxx ,
    r5f100lea,
    r5f565ne ,
    r7f0c004 ,
    rda5981x ,
    rda8955  ,
    rtl8710bn,
    stm32f4xx,
    stm32f4xx_cube  ,
    stm32f7xx,
    stm32l0xx,
    stm32l475,
    stm32l4xx,
    stm32l4xx_cube,
    xr871,
    mtk6261m,
    mx1101,
    asr3601,
    air202f,
    N10,
    l718,
    ec100,
    ec200,
    mtk6261d,
    CHIP_ID_LIMIT = 0xFFFF
};

typedef struct __chip_code {
    UINT16  vendor;
    UINT16  id;
    const char  *mcu_family_name;
} chip_code_st;

extern chip_code_st g_chip_codes[];
extern chip_code_st *get_chip_code( char *name);

#endif
