
#include <platform_stdlib.h>
#include "rl6548.h"
#include "audio_internel.h"

//#define ENABLE_PA

static u8 sp_rx_buf[SP_DMA_PAGE_SIZE * SP_DMA_PAGE_NUM]__attribute__((aligned(32)));
static u8 sp_tx_buf[SP_DMA_PAGE_SIZE * SP_DMA_PAGE_NUM]__attribute__((aligned(32)));
static u8 sp_zero_buf[SP_ZERO_BUF_SIZE]__attribute__((aligned(32)));
static u8 sp_full_buf[SP_FULL_BUF_SIZE]__attribute__((aligned(32)));

static u8 rx_data_cache[SP_DMA_PAGE_SIZE];
static uint32_t rx_data_cache_len = 0;


static SP_InitTypeDef SP_InitStruct;
static SP_GDMA_STRUCT SPGdmaStruct;
static SP_OBJ sp_obj;
static SP_RX_INFO sp_rx_info;
static SP_TX_INFO sp_tx_info;
static int RX_GDMA_flag = 0; //0, start, 1, stop, 2, close
static int TX_GDMA_flag = 0;
static SP_OBJ sp_obj;
static SP_InitTypeDef SP_InitStruct;


static u8 *sp_get_free_tx_page(void)
{
    pTX_BLOCK ptx_block = &(sp_tx_info.tx_block[sp_tx_info.tx_usr_cnt]);

    if (ptx_block->tx_gdma_own)
        return NULL;
    else{
        return (u8*)ptx_block->tx_addr;
    }
}

static void sp_write_tx_page(u8 *src, u32 length)
{
    pTX_BLOCK ptx_block = &(sp_tx_info.tx_block[sp_tx_info.tx_usr_cnt]);

    memcpy((void*)ptx_block->tx_addr, src, length);
    ptx_block-> tx_gdma_own = 1;
    ptx_block -> tx_length = length;
    sp_tx_info.tx_usr_cnt++;
    if (sp_tx_info.tx_usr_cnt == SP_DMA_PAGE_NUM){
        sp_tx_info.tx_usr_cnt = 0;
    }
}

static void sp_release_tx_page(void)
{
    pTX_BLOCK ptx_block = &(sp_tx_info.tx_block[sp_tx_info.tx_gdma_cnt]);

    if (sp_tx_info.tx_empty_flag){
    }
    else{
        ptx_block->tx_gdma_own = 0;
        sp_tx_info.tx_gdma_cnt++;
        if (sp_tx_info.tx_gdma_cnt == SP_DMA_PAGE_NUM){
            sp_tx_info.tx_gdma_cnt = 0;
        }
    }
}

static u8 *sp_get_ready_tx_page(void)
{
    pTX_BLOCK ptx_block = &(sp_tx_info.tx_block[sp_tx_info.tx_gdma_cnt]);

    if (ptx_block->tx_gdma_own){
        sp_tx_info.tx_empty_flag = 0;
        return (u8*)ptx_block->tx_addr;
    }
    else{
        sp_tx_info.tx_empty_flag = 1;
        return (u8*)sp_tx_info.tx_zero_block.tx_addr;	//for audio buffer empty case
    }
}

static u32 sp_get_ready_tx_length(void)
{
    pTX_BLOCK ptx_block = &(sp_tx_info.tx_block[sp_tx_info.tx_gdma_cnt]);

    if (sp_tx_info.tx_empty_flag){
        return sp_tx_info.tx_zero_block.tx_length;
    }
    else{
        return ptx_block->tx_length;
    }
}

static void sp_tx_complete(void *Data)
{
    SP_GDMA_STRUCT *gs = (SP_GDMA_STRUCT *) Data;
    PGDMA_InitTypeDef GDMA_InitStruct;
    u32 tx_addr;
    u32 tx_length;

    GDMA_InitStruct = &(gs->SpTxGdmaInitStruct);

    /* Clear Pending ISR */
    GDMA_ClearINT(GDMA_InitStruct->GDMA_Index, GDMA_InitStruct->GDMA_ChNum);
    sp_release_tx_page();
    tx_addr = (u32)sp_get_ready_tx_page();
    tx_length = sp_get_ready_tx_length();
    if(TX_GDMA_flag == 0) {
        AUDIO_SP_TXGDMA_Restart(GDMA_InitStruct->GDMA_Index, GDMA_InitStruct->GDMA_ChNum, tx_addr, tx_length);
    } else {
        GDMA_ChnlFree(SPGdmaStruct.SpTxGdmaInitStruct.GDMA_Index, SPGdmaStruct.SpTxGdmaInitStruct.GDMA_ChNum);
        AUDIO_SP_TdmaCmd(AUDIO_SPORT_DEV, DISABLE);
        // AUDIO_SP_TxStart(AUDIO_SPORT_DEV, DISABLE);

        if(TX_GDMA_flag == 2){
            RCC_PeriphClockCmd(APBPeriph_AUDIOC, APBPeriph_AUDIOC_CLOCK, DISABLE);
            RCC_PeriphClockCmd(APBPeriph_SPORT, APBPeriph_SPORT_CLOCK, DISABLE);
            CODEC_DeInit(APP_LINE_OUT);
            PLL_PCM_Set(DISABLE);
        }
    }
}

static u8 *sp_get_ready_rx_page(void)
{
    pRX_BLOCK prx_block = &(sp_rx_info.rx_block[sp_rx_info.rx_usr_cnt]);

    if (prx_block->rx_gdma_own)
        return NULL;
    else{
        return prx_block->rx_addr;
    }
}

static void sp_read_rx_page(u8 *dst, u32 length)
{
    pRX_BLOCK prx_block = &(sp_rx_info.rx_block[sp_rx_info.rx_usr_cnt]);

    memcpy(dst, prx_block->rx_addr, length);
    prx_block->rx_gdma_own = 1;
    sp_rx_info.rx_usr_cnt++;
    if (sp_rx_info.rx_usr_cnt == SP_DMA_PAGE_NUM){
        sp_rx_info.rx_usr_cnt = 0;
    }
}


static void sp_release_rx_page(void)
{
    pRX_BLOCK prx_block = &(sp_rx_info.rx_block[sp_rx_info.rx_gdma_cnt]);

    if (sp_rx_info.rx_full_flag){
    }
    else{
        prx_block->rx_gdma_own = 0;
        sp_rx_info.rx_gdma_cnt++;
        if (sp_rx_info.rx_gdma_cnt == SP_DMA_PAGE_NUM){
            sp_rx_info.rx_gdma_cnt = 0;
        }
    }
}


static u8 *sp_get_free_rx_page(void)
{
    pRX_BLOCK prx_block = &(sp_rx_info.rx_block[sp_rx_info.rx_gdma_cnt]);

    if (prx_block->rx_gdma_own){
        sp_rx_info.rx_full_flag = 0;
        return prx_block->rx_addr;
    }
    else{
        sp_rx_info.rx_full_flag = 1;
        return sp_rx_info.rx_full_block.rx_addr;	//for audio buffer full case
    }
}

static u32 sp_get_free_rx_length(void)
{
    pRX_BLOCK prx_block = &(sp_rx_info.rx_block[sp_rx_info.rx_gdma_cnt]);

    if (sp_rx_info.rx_full_flag){
        return sp_rx_info.rx_full_block.rx_length;
    }
    else{
        return prx_block->rx_length;
    }
}

static void sp_init_tx_variables(void)
{
    int i;

    for(i=0; i<SP_ZERO_BUF_SIZE; i++){
        sp_zero_buf[i] = 0;
    }
    sp_tx_info.tx_zero_block.tx_addr = (u32)sp_zero_buf;
    sp_tx_info.tx_zero_block.tx_length = (u32)SP_ZERO_BUF_SIZE;

    sp_tx_info.tx_gdma_cnt = 0;
    sp_tx_info.tx_usr_cnt = 0;
    sp_tx_info.tx_empty_flag = 0;

    for(i=0; i<SP_DMA_PAGE_NUM; i++){
        sp_tx_info.tx_block[i].tx_gdma_own = 0;
        sp_tx_info.tx_block[i].tx_addr = (u32)sp_tx_buf+i*SP_DMA_PAGE_SIZE;
        sp_tx_info.tx_block[i].tx_length = SP_DMA_PAGE_SIZE;
    }
}

static void sp_init_rx_variables(void)
{
    int i;
    sp_rx_info.rx_full_block.rx_addr = (u32)sp_full_buf;
    sp_rx_info.rx_full_block.rx_length = (u32)SP_FULL_BUF_SIZE;

    sp_rx_info.rx_gdma_cnt = 0;
    sp_rx_info.rx_usr_cnt = 0;
    sp_rx_info.rx_full_flag = 0;

    for(i=0; i<SP_DMA_PAGE_NUM; i++){
        sp_rx_info.rx_block[i].rx_gdma_own = 1;
        sp_rx_info.rx_block[i].rx_addr = sp_rx_buf+i*SP_DMA_PAGE_SIZE;
        sp_rx_info.rx_block[i].rx_length = SP_DMA_PAGE_SIZE;
    }
}

static void sp_rx_complete(void *data)
{
    SP_GDMA_STRUCT *gs = (SP_GDMA_STRUCT *) data;
    PGDMA_InitTypeDef GDMA_InitStruct;
    u32 rx_addr;
    u32 rx_length;
    char *pbuf;

    GDMA_InitStruct = &(gs->SpRxGdmaInitStruct);
    DCache_Invalidate(GDMA_InitStruct->GDMA_DstAddr, GDMA_InitStruct->GDMA_BlockSize<<2);
    /* Clear Pending ISR */
    GDMA_ClearINT(GDMA_InitStruct->GDMA_Index, GDMA_InitStruct->GDMA_ChNum);
    sp_release_rx_page();
    rx_addr = (u32)sp_get_free_rx_page();
    rx_length = sp_get_free_rx_length();

    if(RX_GDMA_flag == 0) {
        AUDIO_SP_RXGDMA_Restart(GDMA_InitStruct->GDMA_Index, GDMA_InitStruct->GDMA_ChNum, rx_addr, rx_length);
    } else {
        GDMA_ChnlFree(SPGdmaStruct.SpRxGdmaInitStruct.GDMA_Index, SPGdmaStruct.SpRxGdmaInitStruct.GDMA_ChNum);
        AUDIO_SP_RdmaCmd(AUDIO_SPORT_DEV, DISABLE);
        // AUDIO_SP_RxStart(AUDIO_SPORT_DEV, DISABLE);

        if(RX_GDMA_flag == 2) {
            RCC_PeriphClockCmd(APBPeriph_AUDIOC, APBPeriph_AUDIOC_CLOCK, DISABLE);
            RCC_PeriphClockCmd(APBPeriph_SPORT, APBPeriph_SPORT_CLOCK, DISABLE);
            CODEC_DeInit(APP_AMIC_IN);
            PLL_PCM_Set(DISABLE);
        }
    }
}

static void sp_init_hal(pSP_OBJ psp_obj)
{
    u32 div;
    PLL_I2S_Set(ENABLE);		//enable 98.304MHz PLL. needed if fs=8k/16k/32k/48k/96k
    //PLL_PCM_Set(ENABLE);		//enable 45.1584MHz PLL. needed if fs=44.1k/8.2k

    RCC_PeriphClockCmd(APBPeriph_AUDIOC, APBPeriph_AUDIOC_CLOCK, ENABLE);
    RCC_PeriphClockCmd(APBPeriph_SPORT, APBPeriph_SPORT_CLOCK, ENABLE);

    //set clock divider to gen clock sample_rate*256 from 98.304M.
    switch(psp_obj->sample_rate){
        case SR_48K:
            div = 8;
            break;
        case SR_96K:
            div = 4;
            break;
        case SR_32K:
            div = 12;
            break;
        case SR_16K:
            div = 24;
            break;
        case SR_8K:
            div = 48;
            break;
        default:
            DBG_8195A("sample rate not supported!!\n");
            break;
    }
    PLL_Div(div);

    PAD_CMD(_PA_0, DISABLE);
    PAD_CMD(_PA_1, DISABLE);
    PAD_CMD(_PA_2, DISABLE);
    PAD_CMD(_PA_3, DISABLE);
    PAD_CMD(_PA_4, DISABLE);

    PAD_CMD(_PB_28, DISABLE);
    PAD_CMD(_PB_29, DISABLE);
    PAD_CMD(_PB_30, DISABLE);
    PAD_CMD(_PB_31, DISABLE);

    /*codec init*/
    CODEC_Init(psp_obj->sample_rate, psp_obj->word_len, psp_obj->mono_stereo, psp_obj->direction);


#ifdef ENABLE_PA
    CODEC_SetAmicBst(0x3, 0x1);  //right loopback
    CODEC_SetVolume(0x82, 0x82);

    u32 reg_value = 0;
    reg_value = AUDIO_SI_ReadReg(0x2);
    //reg_value &= ~(15<<BIT_MICBST_GSELL);
    reg_value |= (1<<14);
    AUDIO_SI_WriteReg(0x2, reg_value);
#else
    CODEC_SetAmicBst(0x3, 0x3);
    CODEC_SetVolume(0x92, 0x92);
#endif
}

// static
void rl6548_capture_start()
{
    u32 rx_addr;
    u32 rx_length;
    RX_GDMA_flag = 0;
    memset(rx_data_cache, 0x00, SP_DMA_PAGE_SIZE);
    rx_data_cache_len = 0;

    sp_init_rx_variables();
    // AUDIO_SP_Init(AUDIO_SPORT_DEV, &SP_InitStruct);
    AUDIO_SP_RdmaCmd(AUDIO_SPORT_DEV, ENABLE);
    AUDIO_SP_RxStart(AUDIO_SPORT_DEV, ENABLE);

    rx_addr = (u32)sp_get_free_rx_page();
    rx_length = sp_get_free_rx_length();
    AUDIO_SP_RXGDMA_Init(0, &SPGdmaStruct.SpRxGdmaInitStruct, &SPGdmaStruct, (IRQ_FUN)sp_rx_complete, rx_addr, rx_length);
}

// static 
void rl6548_capture_stop()
{
    RX_GDMA_flag = 1;
}

// static 
void rl6548_playback_start()
{
    u32 tx_addr;
    u32 tx_length;

    TX_GDMA_flag = 0;

    sp_init_tx_variables();
    // AUDIO_SP_Init(AUDIO_SPORT_DEV, &SP_InitStruct);
    AUDIO_SP_TdmaCmd(AUDIO_SPORT_DEV, ENABLE);
    AUDIO_SP_TxStart(AUDIO_SPORT_DEV, ENABLE);

    tx_addr = (u32)sp_get_ready_tx_page();
    tx_length = sp_get_ready_tx_length();
    AUDIO_SP_TXGDMA_Init(0, &SPGdmaStruct.SpTxGdmaInitStruct, &SPGdmaStruct, (IRQ_FUN)sp_tx_complete, (u8 *)tx_addr, tx_length);
}

// static 
void rl6548_playback_stop()
{
    TX_GDMA_flag = 1;
}


void *rl6548_audio_init(int sampleRate, int channels, int wordLen)
{
    printf("enter rl6548_audio_init\n");
    pSP_OBJ psp_obj = &sp_obj;

    if(channels == 1)
        psp_obj->mono_stereo= CH_MONO;
    else if(channels == 2)
        psp_obj->mono_stereo= CH_STEREO;

    psp_obj->word_len = wordLen;

    switch(sampleRate) {
        case 48000:
            psp_obj->sample_rate = SR_48K;
            break;
        case 96000:
            psp_obj->sample_rate = SR_96K;
            break;
        case 32000:
            psp_obj->sample_rate = SR_32K;
            break;
        case 16000:
            psp_obj->sample_rate = SR_16K;
            break;
        case 8000:
            psp_obj->sample_rate = SR_8K;
            break;
        case 44100:
            psp_obj->sample_rate = SR_44P1K;
            break;
        case 88200:
            psp_obj->sample_rate = SR_88P2K;
            break;
        default:
            LOGE("not support sr %d\n",sampleRate);
    }

    psp_obj->direction = APP_AMIC_IN | APP_LINE_OUT; //default

    sp_init_hal(psp_obj);
    sp_init_rx_variables();
    sp_init_tx_variables();

    /*configure Sport according to the parameters*/
    AUDIO_SP_StructInit(&SP_InitStruct);
    SP_InitStruct.SP_MonoStereo= psp_obj->mono_stereo;
    SP_InitStruct.SP_WordLen = psp_obj->word_len;

    AUDIO_SP_Init(AUDIO_SPORT_DEV, &SP_InitStruct);
    return psp_obj;
}


void rl6548_audio_deinit()
{
    printf("enter rl6548_audio_deinit\n");
    RCC_PeriphClockCmd(APBPeriph_AUDIOC, APBPeriph_AUDIOC_CLOCK, DISABLE);
    RCC_PeriphClockCmd(APBPeriph_SPORT, APBPeriph_SPORT_CLOCK, DISABLE);
    CODEC_DeInit(APP_AMIC_IN | APP_LINE_OUT);
    PLL_PCM_Set(DISABLE);
}


int rl6548_audio_write(void *h, uint8_t *buf, uint32_t size)
{
    uint32_t offset = 0;
    uint32_t tx_len;

    while(size) {
        if (sp_get_free_tx_page()){
            tx_len = (size >= SP_DMA_PAGE_SIZE) ? SP_DMA_PAGE_SIZE : size;
            sp_write_tx_page((buf+offset), tx_len);
            offset += tx_len;
            size -= tx_len;
        } else {
            rtw_msleep_os(1);
        }
    }
}


int rl6548_data_read(void *handle, void *buf, unsigned int size)
{
    int sz = size;
    char *dst = (char*)buf;

    if(sz >= rx_data_cache_len && rx_data_cache_len > 0) {
        memcpy(dst, rx_data_cache, rx_data_cache_len);
        sz -= rx_data_cache_len;
        dst += rx_data_cache_len;
        rx_data_cache_len = 0; 
    } else if(sz < rx_data_cache_len) {
        memcpy(dst, rx_data_cache, sz);
        rx_data_cache_len -= sz;
        memmove(rx_data_cache, rx_data_cache + sz, rx_data_cache_len);
        sz = 0;
    }

    while(sz > 0) {
        if (sp_get_ready_rx_page()) {
            sp_read_rx_page((u8 *)rx_data_cache, SP_DMA_PAGE_SIZE);
            rx_data_cache_len = SP_DMA_PAGE_SIZE;
            if(sz >= rx_data_cache_len) {
                memcpy(dst, rx_data_cache, rx_data_cache_len);
                sz -= rx_data_cache_len;
                dst += rx_data_cache_len;
                rx_data_cache_len = 0; 
            } else {
                memcpy(dst, rx_data_cache, sz);
                rx_data_cache_len -= sz;
                memmove(rx_data_cache, rx_data_cache + sz, rx_data_cache_len);
                sz = 0;
            }
        } else {
            rtw_msleep_os(1);
        }
    }
    return size - sz;
}


void rl6548_volume_set(int index)
{		
    int vol = (int)(VOLUME_MAX / 100 * index) ;
    SET_VOLUME(vol, vol);
    printf("Volume Set, index %d, value %d\r\n", index, vol);
}


int rl6548_volume_get()
{
    int vol = 0;
    int volume_index = 0;
    u16 volume_16 = 0;

    GET_VOLUME(&volume_16);
    printf("Volume Register Data %x", volume_16);

    vol = volume_16 & 0x00FF;
    volume_index = round(vol * 100 / VOLUME_MAX);
    return volume_index;
}


void rl6548_set_mute(int mute)
{
    if(mute)
        rl6548_volume_set(0);
}


int rl6548_get_mute()
{
    return rl6548_volume_get() == 0;
}
