#include <k_api.h>
#include "cmsis.h"
#ifdef __CMSIS_AOS__
#undef __CMSIS_AOS__
#endif
#include "cmsis_os.h"
#include "hal_timer.h"
#include "hal_trace.h"
#include "hal_gpio.h"
//#include "aos/hal/flash.h"
#include "ota_port.h"

#define SYSTICK_USE_FASTTIMER

#if (RHINO_CONFIG_HW_COUNT > 0)
void soc_hw_timer_init(void)
{
}

hr_timer_t soc_hr_hw_cnt_get(void)
{
    return 0;
}

lr_timer_t soc_lr_hw_cnt_get(void)
{
    return (lr_timer_t)hal_sys_timer_get();
}
#endif /* RHINO_CONFIG_HW_COUNT */

#if (RHINO_CONFIG_INTRPT_STACK_OVF_CHECK > 0)
void soc_intrpt_stack_ovf_check(void)
{
}
#endif

#if (RHINO_CONFIG_DYNTICKLESS > 0)
void soc_tick_interrupt_set(tick_t next_ticks,tick_t elapsed_ticks)
{
}

tick_t soc_elapsed_ticks_get(void)
{
    return 0;
}
#endif

#if (RHINO_CONFIG_KOBJ_DYN_ALLOC > 0)
#define RHINO_HEAP_BLK_SIZE	(0x680000)
__SRAM_EXT_BSS uint32_t heap_blk0[RHINO_HEAP_BLK_SIZE/sizeof(uint32_t)] = {0};
//__SRAM_EXT_BSS uint32_t heap_blk1[RHINO_HEAP_BLK_SIZE/sizeof(uint32_t)] = {0};
k_mm_region_t g_mm_region[] = {
                               {(uint8_t *)heap_blk0, RHINO_HEAP_BLK_SIZE}
                               //{(uint8_t *)heap_blk1, RHINO_HEAP_BLK_SIZE}
                            };
int g_region_num = sizeof(g_mm_region) / sizeof(k_mm_region_t);

void soc_sys_mem_init(void)
{

}
#endif

void soc_err_proc(kstat_t err)
{
}

krhino_err_proc_t g_err_proc = soc_err_proc;

#define OS_CLOCK         OS_CLOCK_NOMINAL

#define SYS_TICK_LOAD ((uint32_t)((((float)OS_CLOCK*(float)RHINO_CONFIG_TICKS_PER_SECOND))/(float)1E6+0.5f))

__STATIC_INLINE uint32_t SysTick_Config_Alt(uint32_t ticks)
{
  if ((ticks - 1UL) > SysTick_LOAD_RELOAD_Msk)
  {
    return (1UL);                                                   /* Reload value impossible */
  }

  SysTick->LOAD  = (uint32_t)(ticks - 1UL);                         /* set reload register */
  NVIC_SetPriority (SysTick_IRQn, (1UL << __NVIC_PRIO_BITS) - 1UL); /* set Priority for Systick Interrupt */
  SysTick->VAL   = 0UL;                                             /* Load the SysTick Counter Value */
  SysTick->CTRL  = SysTick_CTRL_TICKINT_Msk   |
                   SysTick_CTRL_ENABLE_Msk;                         /* Enable SysTick IRQ and SysTick Timer */
  return (0UL);                                                     /* Function successful */
}

void SysTick_Handler(void)
{	
    krhino_intrpt_enter();
    krhino_tick_proc();
    krhino_intrpt_exit();
}

void soc_systick_init(void)
{
#ifdef SYSTICK_USE_FASTTIMER
    hal_systick_timer_open(RHINO_CONFIG_TICKS_PER_SECOND, SysTick_Handler);
#else
    SysTick_Config_Alt(SYS_TICK_LOAD);
#endif
}

void soc_systick_start(void)
{
#ifdef SYSTICK_USE_FASTTIMER
    hal_systick_timer_start();
#else
    SysTick->CTRL  |= SysTick_CTRL_ENABLE_Msk;
#endif
}

void soc_systick_stop(void)
{
#ifdef SYSTICK_USE_FASTTIMER
    hal_systick_timer_stop();
#else
    SysTick->CTRL  &= ~SysTick_CTRL_ENABLE_Msk;
#endif
}

//This needs to be implemented on cortexM
#include <task_group.h>

int arm_install_hw_watchpoint(int i, u32 addr)
{
    printk("%s:not implemented\r\n", __func__);
    return 0;
}

void arm_clear_hw_watchpoint(int i)
{
    printk("%s:not implemented\r\n", __func__);
}

kstat_t sys_krhino_rpc_regist_stub(void *arg)
{
    printk("%s:not implemented\r\n", __func__);
    return RHINO_SUCCESS;
}

kstat_t sys_krhino_rpc_unregist_stub(void *arg)
{
    printk("%s:not implemented\r\n", __func__);
    return RHINO_SUCCESS;
}

kstat_t sys_krhino_rpc_req_stub(void *arg)
{
    printk("%s:not implemented\r\n", __func__);
    return RHINO_SUCCESS;
}

kstat_t sys_krhino_rpc_rsp_stub(void *arg)
{
    printk("%s:not implemented\r\n", __func__);
    return RHINO_SUCCESS;
}

int k_shmm_unmap_area(void *start, size_t len) {
    printk("%s:not implemented\r\n", __func__);
    return -1;
}

#if 0
enum ota_link bes_get_current_link(void);
void * board_get_boot_startaddr()
{
    hal_logic_partition_t info;
    hal_partition_t in_partition;
    enum ota_link link_start = bes_get_current_link();
    if(link_start < 0){
        link_start = OTA_LINK_A;  //default
    }

    if(link_start == OTA_LINK_A){
        in_partition = HAL_PARTITION_APPLICATION;
    }else{	
        in_partition = HAL_PARTITION_OTA_TEMP;
    }	

    if (hal_flash_info_get(in_partition, &info) != 0) {
        printf("board_get_boot_startaddr fail, No boot partition register for this board!\n");
        return NULL;
    }

    return info.partition_start_addr;
}
#endif

int board_get_boot_partition()
{
    hal_logic_partition_t info;
    hal_partition_t in_partition;
    enum ota_link link_start = bes_get_current_link();
    if(link_start < 0){
        link_start = OTA_LINK_A;  //default
    }

    if(link_start == OTA_LINK_A){
        in_partition = HAL_PARTITION_APPLICATION;
    }else{	
        in_partition = HAL_PARTITION_OTA_TEMP;
    }	

    return in_partition;
}