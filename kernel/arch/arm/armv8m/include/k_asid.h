/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#ifndef K_ASID_H
#define K_ASKD_H

/* 0 for kernel, others for APPs */
#define OS_ASID_KERNEL      0

#define OS_ASID_INV         ((uint32_t)-1)

/**
 * @brief allocate free asid from system
 *
 * @return On success, return the allocated asid,
 *         else return -1 to indicate no free asid is availible
 */
uint32_t k_asid_alloc(void);

/**
 * @brief free asid to system
 *
 * @param[in] asid  The asid to be freed
 *
 * @return On success, return 0, else return -1 to indicate
 *         that the asid is invalid
 */
int32_t k_asid_dealloc(uint32_t asid);

/**
 * @brief Rserve system asid so that it will not be allocated in future
 *
 * @param[in] asid  The asid to be reserved.
 *
 * @return On success, return 0, else return -1 to indicate
 *         that the asid is invalid.
 */
int32_t k_asid_reserve(uint32_t asid);

/**
 * @Check whether the asid is active
 *
 * @param [in] asid   The asid to be tested
 *
 * If the @asid is active, return 1, else return 0
 *
 * @note: If an asid is allocted, it's active
 */
int32_t k_asid_is_active(uint32_t asid);

void k_asid_set(uint32_t asid);

uint32_t k_asid_get();

#endif /* K_ASID_H */

