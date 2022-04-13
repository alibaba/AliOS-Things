/*
 * Copyright (C) 2019 MediaTek Inc.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 */

#ifndef _MTK_HIFIXDSP_COMMON_
#define _MTK_HIFIXDSP_COMMON_

/*
 * ADSP reserve memory ID definition
 */
enum ADSP_RESERVE_MEM_ID {
	ADSP_A_AUDIO_MEM_ID,
	ADSP_NUMS_SYSRAM_ID,
};

/*
 * ADSP hardware semaphore ID definition
 *
 * Here, add SEMA-ID definition MUST be
 * sync with the SEMA-ID definition in FreeRTOS DSP project.
 */
enum ADSP_HW_SEMAPHORE_ID {
	ADSP_HW_SEMA_EXAMPLE = 0,
	/* add semaphore id here */
	ADSP_HW_SEMA_MAX = 8
};


/* Callback type define */
typedef void (*callback_fn)(void *arg);

/*
 * Public function API for Audio system
 */
extern int hifixdsp_run_status(void);
extern int async_load_hifixdsp_bin_and_run(callback_fn callback, void *param);
extern int hifixdsp_stop_run(void);
extern int hw_semaphore_get(enum ADSP_HW_SEMAPHORE_ID sema_id,
			unsigned int timeout);
extern int hw_semaphore_release(enum ADSP_HW_SEMAPHORE_ID sema_id);
extern unsigned long adsp_get_reserve_sysram_phys(enum ADSP_RESERVE_MEM_ID id);
extern unsigned long adsp_get_reserve_sysram_virt(enum ADSP_RESERVE_MEM_ID id);
extern unsigned long adsp_get_reserve_sysram_size(enum ADSP_RESERVE_MEM_ID id);
extern unsigned long adsp_get_shared_dtcm_phys_for_ipc(void);
extern unsigned long adsp_get_shared_dtcm_virt_for_ipc(void);
extern unsigned long adsp_get_shared_dtcm_size_for_ipc(void);
extern void *adsp_get_shared_sysram_phys2virt(unsigned long addr);
extern unsigned long adsp_get_shared_sysram_virt2phys(void *addr);
extern unsigned long adsp_hal_phys_addr_cpu2dsp(unsigned long addr);
extern unsigned long adsp_hal_phys_addr_dsp2cpu(unsigned long addr);

#endif /*_MTK_HIFIXDSP_COMMON_*/
