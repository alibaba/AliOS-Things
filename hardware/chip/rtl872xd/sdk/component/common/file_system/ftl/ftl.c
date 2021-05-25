/////////////////////////////////////////////////
//
// FTL , flash_translation_layer
// Realtek CN3-BT, Raven Su
//
/////////////////////////////////////////////////

#include "platform_stdlib.h"
#include "device_lock.h"
#include "freertos_service.h"
#include "osdep_service.h"

#include "ftl_int.h"
#include "ftl.h"

#include "flash_api.h"

//////////////////////////////////////////////////

#define FTL_PRINT_LEVEL	FTL_LEVEL_ERROR

#define FTL_PRINTF(LEVEL, pFormat, ...)     do {\
   if (LEVEL <= FTL_PRINT_LEVEL)\
        printf("["#LEVEL"]:"pFormat, ##__VA_ARGS__);\
}while(0)

//////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////
// [config]

//#define DBG_EN 1
//#define USE_LOCAL_DBG_DIRECT 1
//#define MONITOR_STATUS_INFO 1

#define FTL_MAGIC_PATTERN_VER01 0x635E
#define FTL_MAGIC_PATTERN_VER02 0x777F

//#define EXTRA_DEBUG 1
//#define FEATURE_WRITE_RECYCLE 1
//#define SAVE_TO_STORAGE_RECONFIRM_EN 1

#define FTL_USE_MAPPING_TABLE			1
#define FTL_ONLY_GC_IN_IDLE				0
#define FTL_APP_LOGICAL_ADDR_BASE		0


#if defined(WIN32) && (WIN32 == 1)
#define RAVEN_DEBUG 1
#endif

/////////////////////////////////////////////////////////////////
#define LOGIC_ADDR_MAP_BIT_NUM 12

struct Page_T *g_pPage = 0;
uint16_t       g_free_cell_index;
uint8_t        g_cur_pageID;
uint8_t        g_doingGarbageCollection = 0;
uint8_t        g_PAGE_num = 0;
uint8_t        g_free_page_count;

#if defined(FEATURE_WRITE_RECYCLE) && (FEATURE_WRITE_RECYCLE == 1)
uint8_t  g_read_pageID;
uint16_t g_read_data_index;
#endif

#if defined(MONITOR_STATUS_INFO) && (MONITOR_STATUS_INFO == 1)
#define MONITOR_STATUS_INFO_TABLE_SIZE (10)
uint32_t g_EraseCnt[ MONITOR_STATUS_INFO_TABLE_SIZE ]; // test 10
uint32_t g_GarbageCnt;
uint32_t g_RecycleCnt;
uint32_t g_WriteCnt;
#endif

#if 1//CONFIG_EFLASH_BOARD_EXIST

/////////////////////////////////////////////////////////////////

#define FMC_PAGE_SIZE            0x1000
#define PAGE_element             (FMC_PAGE_SIZE/4)
#define PAGE_element_data        ( (FMC_PAGE_SIZE/8)-1)    // 511
#define MAX_logical_address_size (((PAGE_element_data*(g_PAGE_num-1))-1)<<2)

#define MAPPING_TABLE_SIZE   (MAX_logical_address_size / 4 * LOGIC_ADDR_MAP_BIT_NUM / 8)

#define BIT_VALID           BIT31

#define WRITABLE_32BIT           0xffffffff

#define INFO_beg_index (0)
#define INFO_end_index (1)
#define INFO_size      (2)

#define FTL_ASSERT(x)   //PLATFORM_ASSERT(x)

// 2K bytes / per page
struct Page_T
{
    uint32_t Data[PAGE_element];
};


QueueHandle_t ftl_sem = NULL;
uint8_t *ftl_mapping_table = NULL;
bool do_gc_in_idle = FALSE;
uint8_t idle_gc_page_thres = 1;
uint16_t idle_gc_cell_thres = PAGE_element / 2;

extern uint32_t ftl_write(uint16_t logical_addr, uint32_t w_data);
extern bool ftl_page_erase(struct Page_T *p);
void ftl_mapping_table_init(void);
uint16_t read_mapping_table(uint16_t logical_addr);

uint32_t ftl_page_read(struct Page_T *p, uint32_t index)
{
    uint32_t rdata = 0;
	flash_t flash;

    if (index < PAGE_element)
    {
        if (flash_read_word(&flash, (uint32_t)&p->Data[index], &rdata))
        {
            return rdata;
        }
    }
    else
    {
        FTL_ASSERT(0);
    }


    return rdata;
}

void ftl_flash_write(uint32_t start_addr, uint32_t data)
{
	flash_t flash;

	device_mutex_lock(RT_DEV_LOCK_FLASH);
	flash_write_word(&flash, start_addr, data);
	device_mutex_unlock(RT_DEV_LOCK_FLASH);
}

bool ftl_flash_erase(uint32_t type, uint32_t addr)
{
	flash_t flash;

	if(EraseSector == type) {
		device_mutex_lock(RT_DEV_LOCK_FLASH);
	    flash_erase_sector(&flash, addr);
		device_mutex_unlock(RT_DEV_LOCK_FLASH);

		return TRUE;
	} else 
		return FALSE;

}

uint32_t ftl_page_write(struct Page_T *p, uint32_t index, uint32_t data)
{
	flash_t flash;

    if (index < PAGE_element)
    {
        ftl_flash_write((uint32_t)&p->Data[index], data);
        uint32_t rdata = 0;
        flash_read_word(&flash, (uint32_t)&p->Data[index], &rdata);
        if (data != rdata)
        {
            FTL_PRINTF(FTL_LEVEL_ERROR, "[ftl](ftl_page_write) P: %x, idx: %d, D: 0x%08x, read back: %x \n",
                               p, index, data, rdata);
            return FTL_WRITE_ERROR_READ_BACK;

        }
        return FTL_WRITE_SUCCESS;
#if defined(MONITOR_STATUS_INFO) && (MONITOR_STATUS_INFO == 1)
        ++g_WriteCnt;
#endif
    }
    else
    {
        return FTL_WRITE_ERROR_OUT_OF_SPACE;
    }

}

uint8_t ftl_get_page_seq(struct Page_T *p)
{
    uint32_t tmp = ftl_page_read(p, INFO_beg_index);

    tmp >>= 16;

    return tmp;
}


uint8_t ftl_key_get_length(uint32_t key)
{
    if (flash_get_bit(key, BIT_VALID))
    {
        // valid
        uint8_t length = (key >> 16);
        return length;
    }
    else
    {
        // invalid
        return 0x0;
    }
}

uint32_t ftl_key_init(uint16_t logical_addr, uint8_t length)
{
    FTL_ASSERT(length == 1); // todo

    uint32_t result;

    result = length;
    result <<= 16;

    result |= logical_addr;
    result |= 0xff000000;

    return result;
}

// logical_addr is 4 bytes alignment addr
uint32_t ftl_check_logical_addr(uint16_t logical_addr)
{
    if (logical_addr & 0x3)
    {
        FTL_ASSERT(0);
        return FTL_ERROR_INVALID_LOGICAL_ADDR;
    }

    if (logical_addr >= MAX_logical_address_size)
    {
        FTL_PRINTF(FTL_LEVEL_ERROR, "ftl_check_logical_addr logical_addr exceed MAX_logical_address_size (0x%x, 0x%x)",
                           logical_addr,
                           MAX_logical_address_size);
        return FTL_ERROR_OUT_OF_SPACE;
    }

    return FTL_SUCCESS;
}


uint32_t ftl_page_is_valid(struct Page_T *p) // return 0 is valid
{
    uint32_t info = ftl_page_read(p, INFO_beg_index);

    if (flash_get_bit(info, BIT_VALID))
    {
        info &= 0xffff;

        if (info == FTL_MAGIC_PATTERN_VER01)
        {
            return FTL_SUCCESS;
        }
    }

    return FTL_ERROR_PAGE_FORMAT;
}

/*get invalid page count */
uint8_t ftl_get_free_page_count(void)
{
    uint8_t CurPageID = g_cur_pageID;

    FTL_ASSERT(CurPageID < g_PAGE_num);

    uint8_t FreeCount = 0;
    uint8_t NextPageID = CurPageID;

    uint8_t TryCnt = g_PAGE_num;

    while (TryCnt--)
    {
        ++NextPageID;
        NextPageID %= g_PAGE_num;

        if (ftl_page_is_valid(g_pPage + NextPageID) != 0)
        {
            ++FreeCount;
        }
        else
        {
            break;
        }
    }

    return FreeCount;
}


uint32_t ftl_get_page_end_position(struct Page_T *p, uint16_t *pEndPos)
{

    uint32_t tmp = ftl_page_read(p, INFO_end_index);

    if (flash_get_bit(tmp, BIT_VALID))
    {
        tmp &= 0xffff;

        *pEndPos = tmp;

        return FTL_SUCCESS;
    }
    else
    {
        return FTL_ERROR_PAGE_END_FORMAT;
    }

}


uint8_t ftl_get_prev_page(uint8_t CurPageID, uint8_t *pPrePageID) // 0 is ok
{
    uint8_t result = 1;

    uint8_t PrePageID;
    if (CurPageID == 0)
    {
        PrePageID = g_PAGE_num - 1;
    }
    else
    {
        PrePageID = CurPageID - 1;
    }

    if (0 == ftl_page_is_valid(g_pPage + PrePageID))
    {
        uint8_t cur_Sequence = ftl_get_page_seq(g_pPage + CurPageID);
        uint8_t pre_Sequence = ftl_get_page_seq(g_pPage + PrePageID);

        if (cur_Sequence)
        {
            if (pre_Sequence + 1 == cur_Sequence)
            {

                *pPrePageID = PrePageID;
                result = 0;
            }
        }
        else
        {
            // cur_Sequence is 0
            if (pre_Sequence == 0xff)  // wrap
            {
                *pPrePageID = PrePageID;
                result = 0;
            }
        }
    }
    else
    {
        result = 2 ;
    }

    return result;
}


uint8_t ftl_page_can_addr_drop(uint16_t logical_addr, uint8_t EndPageID)
{
    uint8_t found = 0;

    if (FTL_USE_MAPPING_TABLE == 1)
    {
        uint16_t phy_addr = read_mapping_table(logical_addr);
        if ((phy_addr != 0) && (phy_addr / PAGE_element != EndPageID))
        {
            found = 1;
        }
        return found;
    }

    uint8_t  pageID = g_cur_pageID;
    uint16_t key_index = g_free_cell_index - 1;
L_retry:

    if (EndPageID != pageID)
    {
        for (; key_index >= 3; key_index -= 2)
        {
            uint32_t key = ftl_page_read(g_pPage + pageID, key_index);

            uint32_t length = ftl_key_get_length(key);

            if (length == 1)
            {
                uint16_t addr = key & 0xffff;
                if (addr == logical_addr)
                {
                    found = 1;
                    //r_data = ftl_page_read( g_pPage+pageID,key_index-1 );
                    break;
                }
            }
            else
            {
                FTL_PRINTF(FTL_LEVEL_ERROR, "[ftl] length != 1! func: %s, line: %d", __FUNCTION__, __LINE__);
            }
        }

        uint8_t prePageID;
        if (found == 0 && 0 == ftl_get_prev_page(pageID, &prePageID))
        {
            uint16_t EndPos;

            if (0 == ftl_get_page_end_position(g_pPage + prePageID, &EndPos))
            {
                pageID = prePageID;
                key_index = EndPos;

                goto L_retry;
            }
            else
            {
                // todo, error recovery
                //TODO;

                pageID = prePageID;
                key_index = PAGE_element - 1;

                goto L_retry;
            }
        }
    }

    return found;
}

uint8_t ftl_page_get_oldest(void)
{
    uint8_t UsedPageCnt = g_PAGE_num - ftl_get_free_page_count();

    FTL_ASSERT(UsedPageCnt >= 1);

    --UsedPageCnt;

    uint8_t OldestPage;

    if (g_cur_pageID >= UsedPageCnt)
    {
        OldestPage = g_cur_pageID - UsedPageCnt;
    }
    else
    {
        OldestPage = g_cur_pageID + g_PAGE_num - UsedPageCnt;
    }

    // make sure valid
    FTL_ASSERT(0 == ftl_page_is_valid(g_pPage + OldestPage));

#if 0 // EXTRA_DEBUG
    if (OldestPage == 0)
    {
        // make sure next page is invalid
        ASSERT(ftl_page_is_valid(g_pPage + PAGE_num - 1));
    }
    else
    {
        // make sure next page is invalid
        ASSERT(ftl_page_is_valid(g_pPage + OldestPage - 1));
    }
#endif

    FTL_ASSERT(OldestPage != g_cur_pageID);

    //FLASH_PRINT_TRACE1("OldestPage: %d\r\n", OldestPage);

    return OldestPage;
}



uint16_t   ftl_page_garbage_collect_Imp(void)
{
    uint16_t RecycleNum = 0;


    //ftl_ioctl( FTL_IOCTL_DEBUG, 0, 0);

    int8_t retry_count = g_PAGE_num - ftl_get_free_page_count() - 1;
    FTL_ASSERT(g_PAGE_num > retry_count && retry_count >= 0);


    // todo, pre-seek without any format

    uint8_t Recycle_page;
    uint16_t key_index;
    uint16_t error;

L_retry:

#if defined(MONITOR_STATUS_INFO) && (MONITOR_STATUS_INFO == 1)
    ++g_GarbageCnt;
#endif


    Recycle_page = ftl_page_get_oldest();

    error = ftl_get_page_end_position(g_pPage + Recycle_page, &key_index);
    if (error)
    {
        // error
        key_index = PAGE_element - 1;
        //TODO;
    }

    // drop or copy it
    for (; key_index >= 3; key_index -= 2)
    {
        uint32_t key = ftl_page_read(g_pPage + Recycle_page, key_index);

        uint32_t length = ftl_key_get_length(key);
        if (length == 1)
        {
            uint16_t addr = key & 0xffff;

            if (ftl_page_can_addr_drop(addr, Recycle_page))
            {
                // drop / recycle
                //DPRINTF("drop [%d] addr: 0x%08x \n", candidate_key_index, candidate_addr);

                ++RecycleNum;
            }
            else
            {
                // copy it
                //DPRINTF("copy [%d] addr: 0x%08x \n", candidate_key_index, candidate_addr);

                uint32_t rdata = ftl_page_read(g_pPage + Recycle_page, key_index - 1);

                // write to another place
                ftl_write(addr, rdata);
            }
        }
        else
        {
            FTL_PRINTF(FTL_LEVEL_ERROR, "ftl_page_garbage_collect_Imp:length != 1!recycle page:%x, retry_count:%x, index:%x, read value:%x",
                               Recycle_page, retry_count, key_index, key);

            ++RecycleNum;
        }
    }

    // make the page to invalid page
    //ftl_page_write(g_pPage + Recycle_page, INFO_beg_index, 0x0);
    if (!ftl_page_erase(g_pPage + Recycle_page))
    {
        g_free_page_count = ftl_get_free_page_count();
        return RecycleNum;
    }


    FTL_PRINTF(FTL_LEVEL_INFO, "[ftl] ftl_page_garbage_collect_Imp: Recycle_page:%d, RecycleNum:%d, retry_count:%d",
                      Recycle_page, RecycleNum, retry_count);

    if (RecycleNum == 0)
    {
        if (retry_count)
        {
            --retry_count;

            //DPRINTF("retry\n");
            goto L_retry;
        }
    }

    //ftl_ioctl( FTL_IOCTL_DEBUG, 1, 0);
    //ftl_ioctl( FTL_IOCTL_DEBUG, 0, 0);

    //DPRINTF("----ftl_page_garbage_collect_Imp, RecycleNum:%d \n", RecycleNum);

    g_free_page_count = ftl_get_free_page_count();
    return RecycleNum;
}

uint8_t ftl_page_garbage_collect(uint32_t page_thresh, uint32_t cell_thresh)
{
    uint8_t result = 0;

    if (NULL != ftl_sem)
    {
		xSemaphoreTakeRecursive(ftl_sem, portMAX_DELAY);
    }

    if (g_doingGarbageCollection == 0)
    {
        g_doingGarbageCollection = 1;

        if (g_free_page_count <= page_thresh)
        {
            if (g_free_cell_index <= cell_thresh)
            {
                FTL_PRINTF(FTL_LEVEL_INFO, "[ftl] doGarbageCollection: page thres %d, cell thres %d", page_thresh,
                                  cell_thresh);
                ftl_page_garbage_collect_Imp();
                result = 1;
            }
        }

        g_doingGarbageCollection = 0;
    }

    if (NULL != ftl_sem)
    {
		xSemaphoreGiveRecursive(ftl_sem);
    }

    return result;
}

void ftl_garbage_collect_in_idle(void)
{
    if (g_pPage == NULL)
    {
        return ;
    }
    if (do_gc_in_idle)
    {
        if (g_free_page_count <= idle_gc_page_thres)
        {
            if (g_free_cell_index <= idle_gc_cell_thres)
            {
                ftl_page_garbage_collect(idle_gc_page_thres, idle_gc_cell_thres);
            }
        }
    }
}

void ftl_set_page_end_position(struct Page_T *p, uint16_t Endpos)
{
    uint32_t data = Endpos;
    data |= 0xffff0000;

    //ftl_page_write(p, INFO_end_index,  data);

    flash_set_bit(&data, BIT_VALID);
    FTL_ASSERT(1 == flash_get_bit(data, BIT_VALID));

    ftl_page_write(p, INFO_end_index,  data);
}

bool ftl_page_erase(struct Page_T *p)
{
    uint32_t info = ftl_page_read(p, INFO_beg_index);
    uint32_t data = 0xFFFF0000;
    data |= FTL_MAGIC_PATTERN_VER02;
    if ((info & 0xffff) == FTL_MAGIC_PATTERN_VER02)
    {
        FTL_PRINTF(FTL_LEVEL_INFO, "  ftl_page_erase with already erased page: %x \n", p);
        return TRUE;
    }

    if (ftl_flash_erase(EraseSector, (uint32_t)p))
    {
        if (FTL_WRITE_SUCCESS == ftl_page_write(p, INFO_beg_index, data))
        {
            return TRUE;
        }
    }
    return FALSE;
}

bool ftl_page_format(struct Page_T *p, uint8_t sequence)
{
    FTL_PRINTF(FTL_LEVEL_INFO, "  ftl_page_format: %x seq: %d\n", p, sequence);

    if (!ftl_page_erase(p))
    {
        return FALSE;
    }

#if defined(__ARMCC_VERSION) && (__ARMCC_VERSION >=  5060061)
    uint32_t data = sequence;
#else
    // fix keil compiler 5.5  optimization -o2 issue
    // need to add volatile in case of ingoring below ftl_set_flag()
    volatile uint32_t data = sequence;
#endif

    data <<= 16;
    data |= FTL_MAGIC_PATTERN_VER01;
    data |= 0xff000000;

//    ftl_page_write(p, INFO_beg_index, data);

    flash_set_bit((uint32_t *)&data, BIT_VALID);

    ftl_page_write(p, INFO_beg_index, data);

    g_free_page_count--;

#if defined(MONITOR_STATUS_INFO) && (MONITOR_STATUS_INFO == 1)
    ++g_EraseCnt[ ftl_get_index_from_page(p) ];
#endif

    return TRUE;
}


void ftl_recover_from_power_lost(void)
{
    //DBG_PRINT_INFO_2("ftl_recover_from_power_lost");

    if (ftl_get_free_page_count())
    {
        return;
    }

    //FLASH_PRINT_TRACE0("ftl_recover_from_power_lost");

    int16_t RecycleNum = 0;

    uint8_t Recycle_page;
    uint16_t key_index;
    uint16_t error;
    uint16_t later_to_write_item_num;

//L_retry:
    RecycleNum = 0;
    later_to_write_item_num = 0;
    Recycle_page = ftl_page_get_oldest();

    //DPRINTF("Recycle_page:%d\n", Recycle_page);

    error = ftl_get_page_end_position(g_pPage + Recycle_page, &key_index);
    if (error)
    {
        // error
        key_index = PAGE_element - 1;
    }

    // drop or copy it
    for (; key_index >= 3; key_index -= 2)
    {
        uint32_t key = ftl_page_read(g_pPage + Recycle_page, key_index);

        uint32_t length = ftl_key_get_length(key);

        if (length == 1)
        {
            uint16_t addr = key & 0xffff;

            if (ftl_page_can_addr_drop(addr, Recycle_page))
            {
                // drop / recycle
                //DBG_PRINT_INFO_2("key_index:%d, addr: 0x%08x - drop\n", key_index, addr);

                ++RecycleNum;
            }
            else
            {
                // copy it
                //DBG_PRINT_INFO_2("key_index:%d, addr: 0x%08x - copy\n", key_index, addr);

                uint32_t rdata = ftl_page_read(g_pPage + Recycle_page, key_index - 1);

                // write to another place
                if ((g_free_cell_index + length) < PAGE_element)
                {
                    --RecycleNum;

                    //FTL_ASSERT(length == 1);



                    ftl_write(addr, rdata);
                }
                else
                {
                    ++later_to_write_item_num;
                    // need more safe?? use another eflash page ?? or ram ??
                }
            }

        }
        else
        {
            //DBG_PRINT_INFO_1("length is not 1 (%d)", length);
            FTL_PRINTF(FTL_LEVEL_ERROR, "[ftl] length != 1! func: %s, line: %d", __FUNCTION__, __LINE__);
            ++RecycleNum;
        }
    }

    uint32_t handle_error = 0;
    if (later_to_write_item_num)
    {
        handle_error = 1;
    }

    // make the page to invalid page
    ftl_page_write(g_pPage + Recycle_page, INFO_beg_index, 0x0);

    if (handle_error)
    {
        ftl_ioctl(FTL_IOCTL_CLEAR_ALL, 0, 0);
        FTL_PRINTF(FTL_LEVEL_WARN, "recovery FTL fail... clean all");
    }

#ifdef DBG_EN
    ftl_ioctl(FTL_IOCTL_DEBUG, 0, 0);
#endif


}

uint16_t read_mapping_table(uint16_t logical_addr)
{
    uint32_t bit_index = logical_addr / 4 * LOGIC_ADDR_MAP_BIT_NUM;
    uint32_t byte_index = bit_index / 8;
    uint32_t byte_offset = bit_index % 8;

    uint16_t phy_addr = ftl_mapping_table[byte_index] + (ftl_mapping_table[byte_index + 1] << 8);
    phy_addr = (phy_addr & (0xfff << byte_offset)) >> byte_offset;
    phy_addr *= 2;

    return phy_addr;
}

void write_mapping_table(uint16_t logical_addr, uint8_t pageID, uint16_t cell_index)
{
    uint32_t bit_index = (logical_addr / 4) *
                         LOGIC_ADDR_MAP_BIT_NUM;//use 12 bit to represent one logical address
    uint32_t byte_index = bit_index / 8;
    uint32_t byte_offset = bit_index % 8;
    uint32_t phy_addr_offset = (pageID * PAGE_element + cell_index) / 2;//8 bytes aligned

    if (4 == byte_offset)
    {
        uint8_t phy_addr_offset_l = phy_addr_offset & 0x0f;
        phy_addr_offset_l = (phy_addr_offset_l << 4) & 0xf0;
        ftl_mapping_table[byte_index] &= 0x0f;
        ftl_mapping_table[byte_index] |= phy_addr_offset_l;

        uint8_t phy_addr_offset_h = (phy_addr_offset >> 4) & 0xff;
        ftl_mapping_table[byte_index + 1] = phy_addr_offset_h;
    }
    else if (0 == byte_offset)
    {
        uint8_t phy_addr_offset_l = phy_addr_offset & 0xff;
        ftl_mapping_table[byte_index] = phy_addr_offset_l;

        uint8_t phy_addr_offset_h = (phy_addr_offset >> 8) & 0x0f;
        ftl_mapping_table[byte_index + 1] &= 0xf0;
        ftl_mapping_table[byte_index + 1] |= phy_addr_offset_h;
    }
    else
    {
        //error
    }
}

// logical_addr is 4 bytes alignment addr
uint32_t ftl_read(uint16_t logical_addr, uint32_t *value)
{
    uint32_t ret = FTL_READ_SUCCESS;


    if (ftl_check_logical_addr(logical_addr))
    {
        //PLATFORM_ASSERT(0);
        return FTL_READ_ERROR_INVALID_LOGICAL_ADDR;
    }
    else
    {
        if (FTL_USE_MAPPING_TABLE == 0)
        {
            uint8_t  found = 0;
            uint8_t pageID = g_cur_pageID;
            int32_t key_index = g_free_cell_index - 1;

L_retry:

            // todo, length is 1
            //PRINTF("pageID,key_index: %d, %d \r\n", pageID, key_index);

            for (; key_index >= 3; key_index -= 2)
            {
                uint32_t key = ftl_page_read(g_pPage + pageID, key_index);

                uint32_t length = ftl_key_get_length(key);

                if (length == 0)
                {
                    continue;
                }

                //PLATFORM_ASSERT( length == 1);

                if (length == 1)
                {
                    uint16_t addr = key & 0xffff;
                    if (addr == logical_addr)
                    {
                        found = 1;
                        *value = ftl_page_read(g_pPage + pageID, key_index - 1);
                        ret = 0;

#if defined(FEATURE_WRITE_RECYCLE) && (FEATURE_WRITE_RECYCLE == 1)
                        g_read_pageID = pageID;
                        g_read_data_index = key_index - 1;
#endif

                        break;
                    }
                }
                else
                {
                    FTL_PRINTF(FTL_LEVEL_ERROR, "[ftl] length != 1!  line: %d", __LINE__);
                }

            }

            uint8_t prePageID;
            if (found == 0 && 0 == ftl_get_prev_page(pageID, &prePageID))
            {
                uint16_t EndPos;

                if (0 == ftl_get_page_end_position(g_pPage + prePageID, &EndPos))
                {
                    key_index = EndPos;
                    pageID = prePageID;

                    goto L_retry;
                }
                else
                {
                    //TODO;
                    // todo, error recovery

                    key_index = PAGE_element - 1;
                    pageID = prePageID;

                    goto L_retry;
                }
            }

            if (!found)
            {
                ret = FTL_READ_ERROR_READ_NOT_FOUND;
                //ftl_ioctl( FTL_IOCTL_DEBUG,0,0);
            }
        }
        else
        {
            uint16_t phy_addr = read_mapping_table(logical_addr);//index of data element

            if (phy_addr == 0)//0 means invalid logical address
            {
                //FTL_PRINTF(FTL_LEVEL_ERROR, "[ftl] invalid logical address! line: %d", __LINE__);
                return FTL_READ_ERROR_READ_NOT_FOUND;
            }

            uint8_t pageID = phy_addr / PAGE_element;
            uint16_t key_index = phy_addr % PAGE_element;

            uint32_t key = ftl_page_read(g_pPage + pageID, key_index + 1);

            uint32_t length = ftl_key_get_length(key);

            //PLATFORM_ASSERT( length == 1);

            if (length == 1)
            {
                uint16_t addr = key & 0xffff;
                if (addr == logical_addr)
                {
                    *value = ftl_page_read(g_pPage + pageID, key_index);
                    ret = 0;
                }
                else
                {
                    ret = FTL_READ_ERROR_PARSE_ERROR;
                    FTL_PRINTF(FTL_LEVEL_ERROR, "[ftl] logical address parse error! func: %s, line: %d", __FUNCTION__, __LINE__);
                }
            }
            else
            {
                ret = FTL_READ_ERROR_PARSE_ERROR;
                FTL_PRINTF(FTL_LEVEL_ERROR, "[ftl] length != 1! func: %s, line: %d", __FUNCTION__, __LINE__);
            }
        }
    }

    FTL_PRINTF(FTL_LEVEL_WARN, "[ftl] r 0x%08x: 0x%08x (%d)\r\n", logical_addr, *value, ret);

    return  ret;
}

// return 0 success
// return !0 fail
uint32_t ftl_save_to_storage(void *pdata_tmp, uint16_t offset, uint16_t size)
{
    uint8_t *pdata8 = (uint8_t *)pdata_tmp;

    if (g_pPage == NULL)
    {
        return FTL_WRITE_ERROR_NOT_INIT;
    }

    if ((offset & 0x3) || (size <= 0) || (size & 0x3))
    {
        return FTL_WRITE_ERROR_INVALID_PARAMETER;
    }

#if defined(SAVE_TO_STORAGE_RECONFIRM_EN) && (SAVE_TO_STORAGE_RECONFIRM_EN == 1)
    uint16_t bak_offset = offset;
    uint16_t bak_size = size;
#endif


    uint32_t ret = 0;

    while (size > 0)
    {
        uint32_t data32 = (uint32_t)(pdata8[0] |
                                     (pdata8[1] << 8) |
                                     (pdata8[2] << 16) |
                                     (pdata8[3] << 24));

        ret = ftl_write(offset, data32);
        FTL_ASSERT(result == 0);

        if (ret)
        {
            break;
        }

        offset += 4;
        size -= 4;
        pdata8 += 4;
    }


#if defined(SAVE_TO_STORAGE_RECONFIRM_EN) && (SAVE_TO_STORAGE_RECONFIRM_EN == 1)


    if (ret == 0)
    {
        pdata8 = (uint8_t *)pdata_tmp;
        offset = bak_offset;
        size = bak_size;

        while (size > 0)
        {
            uint32_t data32 = (uint32_t)(pdata8[0] |
                                         (pdata8[1] << 8) |
                                         (pdata8[2] << 16) |
                                         (pdata8[3] << 24));
            uint32_t read_data32, error;
            error = ftl_read(offset, &read_data32);
            FTL_ASSERT(error == 0);
            FTL_ASSERT(read_data32 == data32);
            if (error)
            {
                ret = FTL_WRITE_ERROR_READ_BACK;
                break;
            }

            if (read_data32 != data32)
            {
                ret = FTL_WRITE_ERROR_VERIFY;
                break;
            }

            offset += 4;
            size -= 4;
            pdata8 += 4;
        }


    }

#endif

    return ret;
}

// return 0 success
// return !0 fail
uint32_t ftl_load_from_storage(void *pdata_tmp, uint16_t offset, uint16_t size)
{
    if (g_pPage == NULL)
    {
        return FTL_READ_ERROR_NOT_INIT;
    }

    if ((offset & 0x3) || (size <= 0) || (size & 0x3))
    {
        return FTL_READ_ERROR_INVALID_PARAMETER;
    }

    uint8_t *pdata8 = (uint8_t *)pdata_tmp;
    uint32_t ret = 0, data32;

    while (size > 0)
    {
        ret = ftl_read(offset, &data32);
        if (ret != 0)
        {
            break;
        }

        pdata8[0] = (data32 & 0xFF);
        pdata8[1] = (data32 & 0xFF00) >> 8;
        pdata8[2] = (data32 & 0xFF0000) >> 16;
        pdata8[3] = (data32 & 0xFF000000) >> 24;

        offset += 4;
        size -= 4;
        pdata8 += 4;
    }

    return ret;
}

uint32_t ftl_write(uint16_t logical_addr, uint32_t w_data)
{
    uint32_t ret = FTL_WRITE_SUCCESS;

    uint8_t sem_flag = FALSE;

    if (0 != __get_IPSR())
    {
        FTL_PRINTF(FTL_LEVEL_WARN, "[ftl] FTL_write should not be called in interrupt handler!\n");
        return FTL_WRITE_ERROR_IN_INTR;
    }

    if (NULL != ftl_sem)
    {
		if(xSemaphoreTakeRecursive(ftl_sem, portMAX_DELAY) == TRUE)
        {
            sem_flag = TRUE;
        }
    }

    if (ftl_check_logical_addr(logical_addr))
    {
        FTL_ASSERT(0);
        ret = FTL_WRITE_ERROR_INVALID_ADDR;
    }
    else
    {

        // todo, try to find old cell, check data is the same or not
        // or use the same cell if all bits match non "0->1" patterns

        uint8_t length = 1; // 1*4 bytes , todo

L_retry:

        if ((g_free_cell_index + length) < PAGE_element)
        {
            FTL_ASSERT(WRITABLE_32BIT == ftl_page_read(g_pPage + g_cur_pageID, g_free_cell_index));
            FTL_ASSERT(WRITABLE_32BIT == ftl_page_read(g_pPage + g_cur_pageID,
                                                       g_free_cell_index + length));

            uint32_t key = ftl_key_init(logical_addr, length);

            //ftl_page_write(g_pPage + g_cur_pageID, g_free_cell_index + 1, key);
            ftl_page_write(g_pPage + g_cur_pageID, g_free_cell_index, w_data);

            flash_set_bit(&key, BIT_VALID);

            ftl_page_write(g_pPage + g_cur_pageID, g_free_cell_index + 1, key);

            if (FTL_USE_MAPPING_TABLE == 1) //mapping table otp
            {
                write_mapping_table(logical_addr, g_cur_pageID, g_free_cell_index);
            }

            g_free_cell_index += (length + 1);

            ret = FTL_WRITE_SUCCESS;
        }
        else
        {
            // try to find out free cell
			
            uint16_t tmp;
            if (ftl_get_page_end_position(g_pPage + g_cur_pageID, &tmp))
            {
                // invalid end pos
                // so set end pos
                ftl_set_page_end_position(g_pPage + g_cur_pageID, g_free_cell_index - 1);
            }

            // find invalid(free) page
            uint8_t new_cur_pageID = g_cur_pageID + 1;
            new_cur_pageID %= g_PAGE_num;

            if (ftl_page_is_valid(g_pPage + new_cur_pageID))
            {
                //DPRINTF("ftl_write: before format\n");
                //ftl_ioctl( FTL_IOCTL_DEBUG, 0, 0);

                // invalid page and format it
                uint8_t new_sequence = ftl_get_page_seq(g_pPage + g_cur_pageID) + 1;
                if (!ftl_page_format(g_pPage + new_cur_pageID, new_sequence))
                {
                    return FTL_WRITE_ERROR_ERASE_FAIL;
                }

                // updata current page info
                g_cur_pageID = new_cur_pageID;
                g_free_cell_index = INFO_size;

                if (!g_doingGarbageCollection)
                {
                    if (FTL_ONLY_GC_IN_IDLE == 1)
                    {
                        ret = FTL_WRITE_ERROR_NEED_GC;
                    }
                    else
                    {
                        ftl_page_garbage_collect(0, PAGE_element / 2);
                    }
                }

                goto L_retry;
            }
            else
            {
                // out of space
                FTL_ASSERT(0);
                ret = FTL_WRITE_ERROR_OUT_OF_SPACE;
            }
        }
    }

    if (sem_flag)
    {
        xSemaphoreGiveRecursive(ftl_sem);
    }

    FTL_PRINTF(FTL_LEVEL_WARN, "[ftl] w 0x%08x: 0x%08x (%d)\r\n", logical_addr, w_data, ret);

    return ret;
}



uint32_t ftl_ioctl(uint32_t cmd, uint32_t p1, uint32_t p2)
{
    if (g_pPage == NULL)
    {
        return FTL_READ_ERROR_NOT_INIT;
    }

    uint32_t result = __LINE__;

    switch (cmd)
    {
    case FTL_IOCTL_DEBUG:
        {
            uint8_t free_page = ftl_get_free_page_count();
            FTL_PRINTF(FTL_LEVEL_INFO, "[ftl] PAGEnum: %d, PAGEelement: %d, PageFree: %d CurPage: %d, FreeCell_idx: %d\n",
                              g_PAGE_num, PAGE_element, free_page, g_cur_pageID, g_free_cell_index);
            FTL_PRINTF(FTL_LEVEL_INFO, "[FTL] PAGE HEAD(%x, %x, %x, %x), PAGE TAIL(%x, %x, %x, %x)",
                              ftl_page_read(g_pPage, INFO_beg_index), ftl_page_read(g_pPage + 1, INFO_beg_index),
                              ftl_page_read(g_pPage + 2, INFO_beg_index), ftl_page_read(g_pPage + 3, INFO_beg_index),
                              ftl_page_read(g_pPage, INFO_end_index), ftl_page_read(g_pPage + 1, INFO_end_index),
                              ftl_page_read(g_pPage + 2, INFO_end_index), ftl_page_read(g_pPage + 3, INFO_end_index));

#if defined(MONITOR_STATUS_INFO) && (MONITOR_STATUS_INFO == 1)
            {
                uint32_t i;
                for (i = 0; i < MONITOR_STATUS_INFO_TABLE_SIZE; ++i)
                {
                    LOG_PRINT_INFO_2("      EraseCnt[%02d]: %d \n", i, g_EraseCnt[i]);
                }
            }
            LOG_PRINT_INFO_1("      doGarbageCollectionCnt: %d\n", g_GarbageCnt);
            LOG_PRINT_INFO_1("      RecycleCnt: %d\n", g_RecycleCnt);
            LOG_PRINT_INFO_1("      WriteCnt: %d\n", g_WriteCnt);

#endif
            result = 0;
        }
        break;
    case FTL_IOCTL_CLEAR_ALL:
        {
            uint32_t i;
            for (i = 0; i < g_PAGE_num; ++i)
            {
                ftl_page_write(g_pPage + i, INFO_beg_index, 0x0);
            }

            //clear ftl_mapping_table
            memset(ftl_mapping_table, 0, MAPPING_TABLE_SIZE);

            // updata current page info
            g_cur_pageID = 0;
            g_free_cell_index = INFO_size;

            //fix after FTL_IOCTL_CLEAR_ALL may not do gc bug
            g_free_page_count = ftl_get_free_page_count();

            result = ftl_page_format(g_pPage + g_cur_pageID, 0) ? 0 : 1;

#if defined(MONITOR_STATUS_INFO) && (MONITOR_STATUS_INFO == 1)
            {
                uint32_t i;
                for (i = 0; i < MONITOR_STATUS_INFO_TABLE_SIZE; ++i)
                {
                    g_EraseCnt[i] = 0;
                }
                g_GarbageCnt = 0;
                g_RecycleCnt = 0;
                g_WriteCnt = 0;
            }
#endif

        }
        break;
    case FTL_IOCTL_ERASE_INVALID_PAGE:
        {
            uint8_t free_page = ftl_get_free_page_count();
            uint8_t idx = 1, i;
            FTL_PRINTF(FTL_LEVEL_INFO, "FTL_IOCTL_ERASE_INVALID_PAGE free_page:%x", free_page);
            if (free_page == 0)
            {
                return __LINE__;
            }
            for (; idx <= free_page; idx++)
            {
                i = (g_cur_pageID + idx) % g_PAGE_num;
                if (ftl_page_is_valid(g_pPage + i) != 0)
                {
                    result = ftl_page_erase(g_pPage + i) ?  0 : 1;
                }
            }
        }
        break;
    case FTL_IOCTL_ENABLE_GC_IN_IDLE:
        {
            do_gc_in_idle = TRUE;
            idle_gc_page_thres = p1;
            idle_gc_cell_thres = p2;
            result = 0;
        }
        break;
    case FTL_IOCTL_DISABLE_GC_IN_IDLE:
        {
            do_gc_in_idle = FALSE;
            result = 0;
        }
        break;
    case FTL_IOCTL_DO_GC_IN_APP:
        {
            ftl_page_garbage_collect(p1, p2);
            result = 0;
        }
        break;
    default:
        break;
    }

    return result;
}

#if 0
uint32_t ftl_load(void *pdata, uint16_t offset, uint16_t size)
{
    return ftl_load_from_storage(pdata, offset + FTL_APP_LOGICAL_ADDR_BASE * 1024,
                                 size);
}

uint32_t ftl_save(void *pdata, uint16_t offset, uint16_t size)
{
    return ftl_save_to_storage(pdata, offset + FTL_APP_LOGICAL_ADDR_BASE * 1024, size);
}
#endif

uint32_t ftl_init(uint32_t u32PageStartAddr, uint8_t pagenum)
{
    if (pagenum < 3)
    {
        pagenum = 3;
    }
    g_PAGE_num = pagenum;

    if (ftl_sem == NULL)
    {
		ftl_sem = xSemaphoreCreateRecursiveMutex();
    }

    g_pPage = (struct Page_T *)(u32PageStartAddr);

    // find latest valid page by sequence num

    uint8_t cur_valid = 0;
    uint8_t cur_sequence = 0;
    uint8_t cur_pageID = 0;

    uint8_t need_to_update = 0;

    uint32_t i;
    for (i = 0; i < g_PAGE_num; ++i)
    {
        if (0 == ftl_page_is_valid(g_pPage + i))
        {
            if (cur_valid)
            {
                uint8_t tmp_sequence = ftl_get_page_seq(g_pPage + i);
                if (tmp_sequence > cur_sequence) // newer
                {
                    if (tmp_sequence == (cur_sequence + 1))
                    {
                        need_to_update = 1;
                    }
                    else
                    {
                        //DPRINTF("tmp_sequence,cur_sequence: %d,%d\n", tmp_sequence, cur_sequence );
                    }

                }
                else if (tmp_sequence == 0 && cur_sequence == 0xff) // wrap
                {
                    need_to_update = 1;
                }
                else
                {
                    // old
                }
            }
            else
            {
                need_to_update = 1;
            }
        }

        if (need_to_update)
        {
            need_to_update = 0;

            cur_valid = 1;
            cur_pageID = i;
            cur_sequence = ftl_get_page_seq(g_pPage + i);
        }
    }

    if (!cur_valid)
    {
        // not any valid, first time to init

        cur_pageID = 0;
        cur_sequence = 0;

        if (!ftl_page_format(g_pPage + cur_pageID, cur_sequence))
        {
            g_pPage = NULL;
            FTL_PRINTF(FTL_LEVEL_ERROR, "ftl init fail");
            return FTL_INIT_ERROR_ERASE_FAIL;
        }
    }

    // find free CELL from bottom
    uint16_t free_cell_index = INFO_size;
    for (i = PAGE_element - 1 ; i >= INFO_size ; --i)
    {
        if (WRITABLE_32BIT != ftl_page_read(g_pPage + cur_pageID, i))
        {
            free_cell_index = i + 1;
            break;
        }
    }


#if defined(WIN32) && (WIN32 == 1)
#if defined(EXTRA_DEBUG) && (EXTRA_DEBUG == 1)
    uint8_t   bak_g_cur_pageID      = g_cur_pageID;
    uint16_t  bak_g_free_cell_index = g_free_cell_index;
#endif
#endif

    g_cur_pageID = cur_pageID;
    g_free_cell_index = free_cell_index;

#if defined(WIN32) && (WIN32 == 1)
#if defined(EXTRA_DEBUG) && (EXTRA_DEBUG == 1)

    //if(1==rand()%100)
    //{
    //    // raven.test fail
    //    bak_g_cur_pageID = 7;
    //}

    if (g_cur_pageID != bak_g_cur_pageID)
    {
        DPRINTF("\n\n[debug]\n");

        ftl_ioctl(FTL_IOCTL_DEBUG, 1, 0);

        DPRINTF("old_g_cur_pageID: %d, ", bak_g_cur_pageID);
        DPRINTF("old_g_free_cell_index: %d\n", bak_g_free_cell_index);
    }

    FTL_ASSERT(g_cur_pageID == bak_g_cur_pageID);
    if (bak_g_free_cell_index == 0)
    {
        bak_g_free_cell_index = 2;
    }
    FTL_ASSERT(g_free_cell_index == bak_g_free_cell_index);

#endif
#endif

    ftl_ioctl(FTL_IOCTL_DEBUG, 0, 0);

    if (FTL_USE_MAPPING_TABLE == 1) //mapping table
    {
        ftl_mapping_table_init();
    }

    ftl_recover_from_power_lost();
    g_free_page_count = ftl_get_free_page_count();

    return 0;
}

void ftl_mapping_table_init(void)
{
    if (NULL == ftl_mapping_table)
    {
        //ftl_mapping_table = os_mem_zalloc((RAM_TYPE)ftl_config.ftl_mapping_table_ram_type,
        //                                  MAPPING_TABLE_SIZE);//table is initialised as 0

		ftl_mapping_table = rtw_zmalloc(MAPPING_TABLE_SIZE);
    }

    uint8_t pageID = g_cur_pageID;
    int32_t key_index = g_free_cell_index - 1;

L_retry:

    // todo, length is 1
    //PRINTF("pageID,key_index: %d, %d \r\n", pageID, key_index);

    for (; key_index >= 3; key_index -= 2)
    {
        uint32_t key = ftl_page_read(g_pPage + pageID, key_index);

        uint32_t length = ftl_key_get_length(key);

        if (length == 0)
        {
            continue;
        }

        //PLATFORM_ASSERT( length == 1);

        if (length == 1)
        {
            uint16_t addr = key & 0xffff;
            if (!read_mapping_table(addr))
            {
                write_mapping_table(addr, pageID, key_index - 1);
            }
        }
        else
        {
            FTL_PRINTF(FTL_LEVEL_ERROR, "[ftl] length != 1! func: %s, line: %d", __FUNCTION__, __LINE__);
        }

    }

    uint8_t prePageID;
    if (0 == ftl_get_prev_page(pageID, &prePageID))
    {
        uint16_t EndPos;

        if (0 == ftl_get_page_end_position(g_pPage + prePageID, &EndPos))
        {
            key_index = EndPos;
            pageID = prePageID;

            goto L_retry;
        }
        else
        {
            //TODO;
            // todo, error recovery

            key_index = PAGE_element - 1;
            pageID = prePageID;

            goto L_retry;
        }
    }
}

#if 0//WIN32

uint32_t FMC_Erase_Page(uint32_t u32PageAddr)
{
    struct Page_T *p = (struct Page_T *) u32PageAddr;

    uint32_t i;

    for (i = 0; i < (PAGE_element); ++i)
    {
        p->Data[i] = WRITABLE_32BIT;
    }
    return 0;
}

uint32_t FMC_Read(uint32_t u32Addr)
{
    uint32_t *pData = (uint32_t *) u32Addr;

    return *pData;
}

uint32_t FMC_Write(uint32_t u32Addr, uint32_t u32Data)
{
    uint32_t *pData = (uint32_t *) u32Addr;

    // check
    uint32_t i;
    for (i = 0; i < 32; ++i)
    {
        uint8_t bit_from = ((*pData) & (0x1 << i)) ? 1 : 0;
        uint8_t bit_to   = (u32Data & (0x1 << i)) ? 1 : 0;

        if (bit_from == 0 && bit_to == 1)
        {
            BUG;
        }
    }

    *pData = u32Data;

    ASSERT(u32Data == FMC_Read(u32Addr));

    return FMC_SUCCESS;
}

#endif // WIN32


#else // CONFIG_EFLASH_BOARD_EXIST


#if 0

// from kevin's reply
FPGA mode support Data SRAM range 0x2000_0000 - 0x2003_FFFF(64k)
real SRAM range is 0x2001_0000 - 0x2001_0000(64KB)
test SRAM range is 0x2001_0000 - 0x2003_FFFF(192KB)


// raven.todo
/* tmp for fake flash in ram section */
FAKE_FLASH_MAIN(rwx)  : ORIGIN = 0x20010000, LENGTH = 256K
                                                          FAKE_FLASH_INFO(rwx)  : ORIGIN = 0x20050000, LENGTH = 2K
                                                                  FAKE_FLASH_REDU(rwx)  : ORIGIN = 0x20050800, LENGTH = 2K
                                                                          FAKE_FLASH_TRIM(rwx)  : ORIGIN = 0x20051000, LENGTH = 2K
#endif

#define FAKE_PAGE_num (5)

#define PAGE_element_data        ( (FMC_PAGE_SIZE/8)-1)    // 255
#define MAX_logical_u32_count    ((PAGE_element_data*(FAKE_PAGE_num-1))-1)
#define MAX_logical_address_size (MAX_logical_u32_count<<2)
#define Fake_FTL_BASE (0x20010000)

                                                                                  typedef struct
{
    uint32_t m_table[MAX_logical_u32_count];
} Fake_FTL_T;

#define Fake_FTL  ((Fake_FTL_T *) Fake_FTL_BASE)   // Pointer to FMC register structure

// logical_addr is 4 bytes alignment addr
uint32_t ftl_check_logical_addr(uint16_t logical_addr)
{
    if (logical_addr & 0x3)
    {
        while (1);
    }

    if (logical_addr >= MAX_logical_address_size)
    {
        DPRINTF("logical_addr >= MAX_logical_address_size \n");

        while (1);
    }

    return 0;
}

uint32_t ftl_init(uint8_t pagenum)
{
    pagenum = FAKE_PAGE_num;

    DPRINTF("[Fake_FTL in RAM] init(%d) \n", pagenum);

    UNUSED(g_pPage);
    UNUSED(g_last_error_code);
    UNUSED(g_free_cell_index);
    UNUSED(g_cur_pageID);
    UNUSED(g_doingGarbageCollection);
    UNUSED(g_PAGE_num);

#if 0

    uint32_t i;

    for (i = 0; i < (MAX_logical_u32_count); ++i)
    {
        Fake_FTL->m_table[i] = 0xdeadbeef;
    }

    for (i = 0; i < (MAX_logical_u32_count); ++i)
    {
        if (Fake_FTL->m_table[i] != 0xdeadbeef)
        {
            DPRINTF("%d\n", i);

            BUG;
            while (1);
        }
    }

#endif

    return FMC_SUCCESS;
}

// logical_addr is 4 bytes alignment addr
uint32_t ftl_read(uint16_t logical_addr)
{
    ftl_check_logical_addr(logical_addr);

    logical_addr >>= 2;
    uint32_t r_data = Fake_FTL->m_table[logical_addr];

    ASIC_DPRINTF("[ftl] r 0x%08x: 0x%08x \r\n", logical_addr << 2, r_data);

    return r_data;
}

uint32_t ftl_write(uint16_t logical_addr, uint32_t w_data)
{
    ftl_check_logical_addr(logical_addr);

    logical_addr >>= 2;
    Fake_FTL->m_table[logical_addr] = w_data;

    ASIC_DPRINTF("[ftl] w 0x%08x: 0x%08x \r\n", logical_addr << 2, w_data);

    return FMC_SUCCESS;
}

uint32_t ftl_get_error()
{
    return FMC_SUCCESS;
}

uint32_t ftl_ioctl(uint32_t cmd, uint32_t p1, uint32_t p2)
{
    return FMC_SUCCESS;
}



#endif // !CONFIG_EFLASH_BOARD_EXIST


