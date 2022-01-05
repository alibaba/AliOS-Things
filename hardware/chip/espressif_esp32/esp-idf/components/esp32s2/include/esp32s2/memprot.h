// Copyright 2020 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.


/* INTERNAL API
 * generic interface to MMU memory protection features
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    MEMPROT_IRAM0 =     0x00000000,
    MEMPROT_DRAM0 =     0x00000001,
    MEMPROT_UNKNOWN
} mem_type_prot_t;


/**
 * @brief Returns splitting address for required memory region
 *
 * @param mem_type Memory protection area type (see mem_type_prot_t enum)
 *
 * @return Splitting address for the memory region required.
 * The address is given by region-specific global symbol exported from linker script,
 * it is not read out from related configuration register.
 */
uint32_t *IRAM_ATTR esp_memprot_get_split_addr(mem_type_prot_t mem_type);

/**
 * @brief Initializes illegal memory access control (MMU) for required memory section.
 *
 * All memory access interrupts share ETS_MEMACCESS_ERR_INUM input channel, it is caller's
 * responsibility to properly detect actual intr. source as well as possible prioritization in case
 * of multiple source reported during one intr.handling routine run
 *
 *  @param mem_type Memory protection area type (see mem_type_prot_t enum)
 */
void esp_memprot_intr_init(mem_type_prot_t mem_type);

/**
 * @brief Enable/disable the memory protection interrupt
 *
 * @param mem_type Memory protection area type (see mem_type_prot_t enum)
 * @param enable enable/disable
 */
void esp_memprot_intr_ena(mem_type_prot_t mem_type, bool enable);

/**
 * @brief Detects whether any of the memory protection interrupts is active
 *
 * @return true/false
 */
bool esp_memprot_is_assoc_intr_any(void);

/**
 * @brief Detects whether specific memory protection interrupt is active
 *
 * @param mem_type Memory protection area type (see mem_type_prot_t enum)
 *
 * @return true/false
 */
bool esp_memprot_is_assoc_intr(mem_type_prot_t mem_type);

/**
 * @brief Sets a request for clearing interrupt-on flag for specified memory region (register write)
 *
 * @note When called without actual interrupt-on flag set, subsequent occurrence of related interrupt is ignored.
 * Should be used only after the real interrupt appears, typically as the last step in interrupt handler's routine.
 *
 * @param mem_type Memory protection area type (see mem_type_prot_t enum)
 */
void esp_memprot_clear_intr(mem_type_prot_t mem_type);

/**
 * @brief Detects which memory protection interrupt is active, check order: IRAM0, DRAM0
 *
 * @return Memory protection area type (see mem_type_prot_t enum)
 */
mem_type_prot_t IRAM_ATTR esp_memprot_get_intr_memtype(void);

/**
 * @brief Gets interrupt status register contents for specified memory region
 *
 * @param mem_type Memory protection area type (see mem_type_prot_t enum)
 *
 * @return Contents of status register
 */
uint32_t esp_memprot_get_fault_reg(mem_type_prot_t mem_type);

/**
 * @brief Get details of given interrupt status
 *
 * @param mem_type Memory protection area type (see mem_type_prot_t enum)
 * @param faulting_address Faulting address causing the interrupt [out]
 * @param op_type Operation being processed at the faulting address [out]
 *               IRAM0: 0 - read, 1 - write
 *               DRAM0: 0 - read, 1 - write
 * @param op_subtype Additional info for op_type [out]
 *               IRAM0: 0 - instruction segment access, 1 - data segment access
 *               DRAM0: 0 - non-atomic operation, 1 - atomic operation
 */
void IRAM_ATTR esp_memprot_get_fault_status(mem_type_prot_t mem_type, uint32_t **faulting_address, uint32_t *op_type, uint32_t *op_subtype);

/**
 * @brief Gets string representation of required memory region identifier
 *
 * @param mem_type Memory protection area type (see mem_type_prot_t enum)
 *
 * @return mem_type as string
 */
const char *IRAM_ATTR esp_memprot_type_to_str(mem_type_prot_t mem_type);

/**
 * @brief Detects whether any of the interrupt locks is active (requires digital system reset to unlock)
 *
 * @return true/false
 */
bool esp_memprot_is_locked_any(void);

/**
 * @brief Sets lock for specified memory region.
 *
 * Locks can be unlocked only by digital system reset
 *
 * @param mem_type Memory protection area type (see mem_type_prot_t enum)
 */
void esp_memprot_set_lock(mem_type_prot_t mem_type);

/**
 * @brief Gets lock status for required memory region
 *
 * @param mem_type Memory protection area type (see mem_type_prot_t enum)
 *
 * @return true/false (locked/unlocked)
 */
bool esp_memprot_get_lock(mem_type_prot_t mem_type);

/**
 * @brief Gets interrupt permission control register contents for required memory region
 *
 * @param mem_type Memory protection area type (see mem_type_prot_t enum)
 *
 * @return Permission control register contents
 */
uint32_t esp_memprot_get_ena_reg(mem_type_prot_t mem_type);

/**
 * @brief Gets interrupt permission settings for unified management block
 *
 * Gets interrupt permission settings register contents for required memory region, returns settings for unified management blocks
 *
 * @param mem_type Memory protection area type (see mem_type_prot_t enum)
 *
 * @return Permission settings register contents
 */
uint32_t esp_memprot_get_perm_uni_reg(mem_type_prot_t mem_type);

/**
 * @brief Gets interrupt permission settings for split management block
 *
 * Gets interrupt permission settings register contents for required memory region, returns settings for split management blocks
 *
 * @param mem_type Memory protection area type (see mem_type_prot_t enum)
 *
 * @return Permission settings register contents
 */
uint32_t esp_memprot_get_perm_split_reg(mem_type_prot_t mem_type);

/**
 * @brief Detects whether any of the memory protection interrupts is enabled
 *
 * @return true/false
 */
bool esp_memprot_is_intr_ena_any(void);

/**
 * @brief Gets interrupt-enabled flag for given memory region
 *
 * @param mem_type Memory protection area type (see mem_type_prot_t enum)
 *
 * @return Interrupt-enabled value
 */
uint32_t esp_memprot_get_intr_ena_bit(mem_type_prot_t mem_type);

/**
 * @brief Gets interrupt-active flag for given memory region
 *
 * @param mem_type Memory protection area type (see mem_type_prot_t enum)
 *
 * @return Interrupt-active value
 */
uint32_t esp_memprot_get_intr_on_bit(mem_type_prot_t mem_type);

/**
 * @brief Gets interrupt-clear request flag for given memory region
 *
 * @param mem_type Memory protection area type (see mem_type_prot_t enum)
 *
 * @return Interrupt-clear request value
 */
uint32_t esp_memprot_get_intr_clr_bit(mem_type_prot_t mem_type);

/**
 * @brief Gets read permission value for specified block and memory region
 *
 * Returns read permission bit value for required unified-management block (0-3) in given memory region.
 * Applicable to all memory types.
 *
 * @param mem_type Memory protection area type (see mem_type_prot_t enum)
 * @param block Memory block identifier (0-3)
 *
 * @return Read permission value for required block
 */
uint32_t esp_memprot_get_uni_block_read_bit(mem_type_prot_t mem_type, uint32_t block);

/**
 * @brief Gets write permission value for specified block and memory region
 *
 * Returns write permission bit value for required unified-management block (0-3) in given memory region.
 * Applicable to all memory types.
 *
 * @param mem_type Memory protection area type (see mem_type_prot_t enum)
 * @param block Memory block identifier (0-3)
 *
 * @return Write permission value for required block
 */
uint32_t esp_memprot_get_uni_block_write_bit(mem_type_prot_t mem_type, uint32_t block);

/**
 * @brief Gets execute permission value for specified block and memory region
 *
 * Returns execute permission bit value for required unified-management block (0-3) in given memory region.
 * Applicable only to IRAM memory types
 *
 * @param mem_type Memory protection area type (see mem_type_prot_t enum)
 * @param block Memory block identifier (0-3)
 *
 * @return Execute permission value for required block
 */
uint32_t esp_memprot_get_uni_block_exec_bit(mem_type_prot_t mem_type, uint32_t block);

/**
 * @brief Sets permissions for specified block in DRAM region
 *
 * Sets Read and Write permission for specified unified-management block (0-3) in given memory region.
 * Applicable only to DRAM memory types
 *
 * @param mem_type Memory protection area type (see mem_type_prot_t enum)
 * @param block Memory block identifier (0-3)
 * @param write_perm Write permission flag
 * @param read_perm Read permission flag
 */
void esp_memprot_set_uni_block_perm_dram(mem_type_prot_t mem_type, uint32_t block, bool write_perm, bool read_perm);

/**
 * @brief Sets permissions for high and low memory segment in DRAM region
 *
 * Sets Read and Write permission for both low and high memory segments given by splitting address.
 * The splitting address must be equal to or higher then beginning of block 5
 * Applicable only to DRAM memory types
 *
 * @param mem_type Memory protection area type (see mem_type_prot_t enum)
 * @param split_addr Address to split the memory region to lower and higher segment
 * @param lw Low segment Write permission flag
 * @param lr Low segment Read permission flag
 * @param hw High segment Write permission flag
 * @param hr High segment Read permission flag
 */
void esp_memprot_set_prot_dram(mem_type_prot_t mem_type, uint32_t *split_addr, bool lw, bool lr, bool hw, bool hr);

/**
 * @brief Sets permissions for specified block in IRAM region
 *
 * Sets Read, Write and Execute permission for specified unified-management block (0-3) in given memory region.
 * Applicable only to IRAM memory types
 *
 * @param mem_type Memory protection area type (see mem_type_prot_t enum)
 * @param block Memory block identifier (0-3)
 * @param write_perm Write permission flag
 * @param exec_perm Execute permission flag
 */
void esp_memprot_set_uni_block_perm_iram(mem_type_prot_t mem_type, uint32_t block, bool write_perm, bool read_perm, bool exec_perm);

/**
 * @brief Sets permissions for high and low memory segment in IRAM region
 *
 * Sets Read, Write and Execute permission for both low and high memory segments given by splitting address.
 * The splitting address must be equal to or higher then beginning of block 5
 * Applicable only to IRAM memory types
 *
 * @param mem_type Memory protection area type (see mem_type_prot_t enum)
 * @param split_addr Address to split the memory region to lower and higher segment
 * @param lw Low segment Write permission flag
 * @param lr Low segment Read permission flag
 * @param lx Low segment Execute permission flag
 * @param hw High segment Write permission flag
 * @param hr High segment Read permission flag
 * @param hx High segment Execute permission flag
 */
void esp_memprot_set_prot_iram(mem_type_prot_t mem_type, uint32_t *split_addr, bool lw, bool lr, bool lx, bool hw, bool hr, bool hx);

/**
 * @brief Activates memory protection for all supported memory region types
 *
 * @note The feature is disabled when JTAG interface is connected
 *
 * @param invoke_panic_handler map mem.prot interrupt to ETS_MEMACCESS_ERR_INUM and thus invokes panic handler when fired ('true' not suitable for testing)
 * @param lock_feature sets LOCK bit, see esp_memprot_set_lock() ('true' not suitable for testing)
 */
void esp_memprot_set_prot(bool invoke_panic_handler, bool lock_feature);

/**
 * @brief Get permission settings bits for IRAM split mgmt based on current split address
 *
 * @param mem_type Memory protection area type (see mem_type_prot_t enum)
 * @param lw Low segment Write permission flag
 * @param lr Low segment Read permission flag
 * @param lx Low segment Execute permission flag
 * @param hw High segment Write permission flag
 * @param hr High segment Read permission flag
 * @param hx High segment Execute permission flag
 */
void esp_memprot_get_perm_split_bits_iram(mem_type_prot_t mem_type, bool *lw, bool *lr, bool *lx, bool *hw, bool *hr, bool *hx);

/**
 * @brief Get permission settings bits for DRAM split mgmt based on current split address
 *
 * @param mem_type Memory protection area type (see mem_type_prot_t enum)
 * @param lw Low segment Write permission flag
 * @param lr Low segment Read permission flag
 * @param hw High segment Write permission flag
 * @param hr High segment Read permission flag
 */
void esp_memprot_get_perm_split_bits_dram(mem_type_prot_t mem_type, bool *lw, bool *lr, bool *hw, bool *hr);

#ifdef __cplusplus
}
#endif
