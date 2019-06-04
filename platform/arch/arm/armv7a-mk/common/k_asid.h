#ifndef K_ASID_H
#define K_ASKD_H

#define ARM_MAX_ASIDS 16

/**
 * @brief allocate free asid from system
 *
 * @return On success, return the allocated asid,
 *         else return -1 to indicate no free asid is availible
 */
int32_t k_asid_alloc(void);

/**
 * @brief free asid to system
 *
 * @param[in] asid  The asid to be freed
 *
 * @return On success, return 0, else return -1 to indicate
 *         that the asid is invalid
 */
int32_t k_asid_dealloc(int32_t asid);

/**
 * @brief Rserve system asid so that it will not be allocated in future
 *
 * @param asid  The asid to be reserved.
 *
 * @return On success, return 0, else return -1 to indicate
 *         that the asid is invalid.
 */
int32_t k_asid_reserve(int32_t asid);

#endif /* K_ASID_H */

