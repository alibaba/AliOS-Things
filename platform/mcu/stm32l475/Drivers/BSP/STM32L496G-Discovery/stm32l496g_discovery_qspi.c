/**
  ******************************************************************************
  * @file    stm32l496g_discovery_qspi.c
  * @author  MCD Application Team
  * @brief   This file includes a standard driver for the MX25R6435F QSPI
  *          memory mounted on STM32L496G-Discovery board.
  @verbatim
  ==============================================================================
                     ##### How to use this driver #####
  ==============================================================================
  [..]
   (#) This driver is used to drive the MX25R6435F QSPI external
       memory mounted on STM32L496G-DISCO evaluation board.

   (#) This driver need a specific component driver (MX25R6435F) to be included with.

   (#) Initialization steps:
       (++) Initialize the QPSI external memory using the BSP_QSPI_Init() function. This
            function includes the MSP layer hardware resources initialization and the
            QSPI interface with the external memory. The BSP_QSPI_DeInit() can be used
            to deactivate the QSPI interface.

   (#) QSPI memory operations
       (++) QSPI memory can be accessed with read/write operations once it is
            initialized.
            Read/write operation can be performed with AHB access using the functions
            BSP_QSPI_Read()/BSP_QSPI_Write().
       (++) The function to the QSPI memory in memory-mapped mode is possible after
            the call of the function BSP_QSPI_EnableMemoryMappedMode().
       (++) The function BSP_QSPI_GetInfo() returns the configuration of the QSPI memory.
            (see the QSPI memory data sheet)
       (++) Perform erase block operation using the function BSP_QSPI_Erase_Block() and by
            specifying the block address. You can perform an erase operation of the whole
            chip by calling the function BSP_QSPI_Erase_Chip().
       (++) The function BSP_QSPI_GetStatus() returns the current status of the QSPI memory.
            (see the QSPI memory data sheet)
       (++) Perform erase sector operation using the function BSP_QSPI_Erase_Sector()
            which is not blocking. So the function BSP_QSPI_GetStatus() should be used
            to check if the memory is busy, and the functions BSP_QSPI_SuspendErase()/
            BSP_QSPI_ResumeErase() can be used to perform other operations during the
            sector erase.
       (++) Deep power down of the QSPI memory is managed with the call of the functions
            BSP_QSPI_EnterDeepPowerDown()/BSP_QSPI_LeaveDeepPowerDown()
  @endverbatim
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT(c) 2017 STMicroelectronics</center></h2>
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "stm32l496g_discovery_qspi.h"

/** @addtogroup BSP
  * @{
  */

/** @addtogroup STM32L496G_DISCOVERY
  * @{
  */

/** @defgroup STM32L496G_DISCOVERY_QSPI STM32L496G-DISCOVERY QSPI
  * @{
  */

/* Private constants --------------------------------------------------------*/
/** @defgroup STM32L496G_DISCOVERY_QSPI_Private_Constants Private Constants
  * @{
  */
#define QSPI_QUAD_DISABLE       0x0
#define QSPI_QUAD_ENABLE        0x1

#define QSPI_HIGH_PERF_DISABLE  0x0
#define QSPI_HIGH_PERF_ENABLE   0x1
/**
  * @}
  */
/* Private variables ---------------------------------------------------------*/

/** @defgroup STM32L496G_DISCOVERY_QSPI_Private_Variables Private Variables
  * @{
  */
QSPI_HandleTypeDef QSPIHandle;

/**
  * @}
  */


/* Private functions ---------------------------------------------------------*/

/** @defgroup STM32L496G_DISCOVERY_QSPI_Private_Functions Private Functions
  * @{
  */
static void    QSPI_MspInit(void);
static void    QSPI_MspDeInit(void);
static uint8_t QSPI_ResetMemory(QSPI_HandleTypeDef *hqspi);
static uint8_t QSPI_WriteEnable(QSPI_HandleTypeDef *hqspi);
static uint8_t QSPI_AutoPollingMemReady(QSPI_HandleTypeDef *hqspi, uint32_t Timeout);
static uint8_t QSPI_QuadMode(QSPI_HandleTypeDef *hqspi, uint8_t Operation);
static uint8_t QSPI_HighPerfMode(QSPI_HandleTypeDef *hqspi, uint8_t Operation);

/**
  * @}
  */

/* Exported functions ---------------------------------------------------------*/

/** @addtogroup STM32L496G_DISCOVERY_QSPI_Exported_Functions
  * @{
  */

/**
  * @brief  Initializes the QSPI interface.
  * @retval QSPI memory status
  */
uint8_t BSP_QSPI_Init(void)
{
  QSPIHandle.Instance = QUADSPI;

  /* Call the DeInit function to reset the driver */
  if (HAL_QSPI_DeInit(&QSPIHandle) != HAL_OK)
  {
    return QSPI_ERROR;
  }

  /* System level initialization */
  QSPI_MspInit();

  /* QSPI initialization */
  QSPIHandle.Init.ClockPrescaler     = 2; /* QSPI clock = 80MHz / (ClockPrescaler+1) = 26.67MHz */
  QSPIHandle.Init.FifoThreshold      = 4;
  QSPIHandle.Init.SampleShifting     = QSPI_SAMPLE_SHIFTING_NONE;
  QSPIHandle.Init.FlashSize          = POSITION_VAL(MX25R6435F_FLASH_SIZE) - 1;
  QSPIHandle.Init.ChipSelectHighTime = QSPI_CS_HIGH_TIME_1_CYCLE;
  QSPIHandle.Init.ClockMode          = QSPI_CLOCK_MODE_0;

  if (HAL_QSPI_Init(&QSPIHandle) != HAL_OK)
  {
    return QSPI_ERROR;
  }

  /* QSPI memory reset */
  if (QSPI_ResetMemory(&QSPIHandle) != QSPI_OK)
  {
    return QSPI_NOT_SUPPORTED;
  }

  /* QSPI quad enable */
  if (QSPI_QuadMode(&QSPIHandle, QSPI_QUAD_ENABLE) != QSPI_OK)
  {
    return QSPI_ERROR;
  }

  /* High performance mode enable */
  if (QSPI_HighPerfMode(&QSPIHandle, QSPI_HIGH_PERF_ENABLE) != QSPI_OK)
  {
    return QSPI_ERROR;
  }

  /* Re-configure the clock for the high performance mode */
  QSPIHandle.Init.ClockPrescaler = 1; /* QSPI clock = 80MHz / (ClockPrescaler+1) = 40MHz */

  if (HAL_QSPI_Init(&QSPIHandle) != HAL_OK)
  {
    return QSPI_ERROR;
  }

  return QSPI_OK;
}

/**
  * @brief  De-Initializes the QSPI interface.
  * @retval QSPI memory status
  */
uint8_t BSP_QSPI_DeInit(void)
{
  QSPIHandle.Instance = QUADSPI;

  /* Call the DeInit function to reset the driver */
  if (HAL_QSPI_DeInit(&QSPIHandle) != HAL_OK)
  {
    return QSPI_ERROR;
  }

  /* System level De-initialization */
  QSPI_MspDeInit();

  return QSPI_OK;
}

/**
  * @brief  Reads an amount of data from the QSPI memory.
  * @param  pData    : Pointer to data to be read
  * @param  ReadAddr : Read start address
  * @param  Size     : Size of data to read
  * @retval QSPI memory status
  */
uint8_t BSP_QSPI_Read(uint8_t *pData, uint32_t ReadAddr, uint32_t Size)
{
  QSPI_CommandTypeDef sCommand;

  /* Initialize the read command */
  sCommand.InstructionMode    = QSPI_INSTRUCTION_1_LINE;
  sCommand.Instruction        = QUAD_INOUT_READ_CMD;
  sCommand.AddressMode        = QSPI_ADDRESS_4_LINES;
  sCommand.AddressSize        = QSPI_ADDRESS_24_BITS;
  sCommand.Address            = ReadAddr;
  sCommand.AlternateByteMode  = QSPI_ALTERNATE_BYTES_4_LINES;
  sCommand.AlternateBytesSize = QSPI_ALTERNATE_BYTES_8_BITS;
  sCommand.AlternateBytes     = MX25R6435F_ALT_BYTES_NO_PE_MODE;
  sCommand.DataMode           = QSPI_DATA_4_LINES;
  sCommand.DummyCycles        = MX25R6435F_DUMMY_CYCLES_READ_QUAD;
  sCommand.NbData             = Size;
  sCommand.DdrMode            = QSPI_DDR_MODE_DISABLE;
  sCommand.DdrHoldHalfCycle   = QSPI_DDR_HHC_ANALOG_DELAY;
  sCommand.SIOOMode           = QSPI_SIOO_INST_EVERY_CMD;

  /* Configure the command */
  if (HAL_QSPI_Command(&QSPIHandle, &sCommand, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    return QSPI_ERROR;
  }

  /* Reception of the data */
  if (HAL_QSPI_Receive(&QSPIHandle, pData, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    return QSPI_ERROR;
  }

  return QSPI_OK;
}

/**
  * @brief  Writes an amount of data to the QSPI memory.
  * @param  pData     : Pointer to data to be written
  * @param  WriteAddr : Write start address
  * @param  Size      : Size of data to write
  * @retval QSPI memory status
  */
uint8_t BSP_QSPI_Write(uint8_t *pData, uint32_t WriteAddr, uint32_t Size)
{
  QSPI_CommandTypeDef sCommand;
  uint32_t end_addr, current_size, current_addr;

  /* Calculation of the size between the write address and the end of the page */
  current_size = MX25R6435F_PAGE_SIZE - (WriteAddr % MX25R6435F_PAGE_SIZE);

  /* Check if the size of the data is less than the remaining place in the page */
  if (current_size > Size)
  {
    current_size = Size;
  }

  /* Initialize the adress variables */
  current_addr = WriteAddr;
  end_addr = WriteAddr + Size;

  /* Initialize the program command */
  sCommand.InstructionMode   = QSPI_INSTRUCTION_1_LINE;
  sCommand.Instruction       = QUAD_PAGE_PROG_CMD;
  sCommand.AddressMode       = QSPI_ADDRESS_4_LINES;
  sCommand.AddressSize       = QSPI_ADDRESS_24_BITS;
  sCommand.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
  sCommand.DataMode          = QSPI_DATA_4_LINES;
  sCommand.DummyCycles       = 0;
  sCommand.DdrMode           = QSPI_DDR_MODE_DISABLE;
  sCommand.DdrHoldHalfCycle  = QSPI_DDR_HHC_ANALOG_DELAY;
  sCommand.SIOOMode          = QSPI_SIOO_INST_EVERY_CMD;

  /* Perform the write page by page */
  do
  {
    sCommand.Address = current_addr;
    sCommand.NbData  = current_size;

    /* Enable write operations */
    if (QSPI_WriteEnable(&QSPIHandle) != QSPI_OK)
    {
      return QSPI_ERROR;
    }

    /* Configure the command */
    if (HAL_QSPI_Command(&QSPIHandle, &sCommand, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
    {
      return QSPI_ERROR;
    }

    /* Transmission of the data */
    if (HAL_QSPI_Transmit(&QSPIHandle, pData, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
    {
      return QSPI_ERROR;
    }

    /* Configure automatic polling mode to wait for end of program */
    if (QSPI_AutoPollingMemReady(&QSPIHandle, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != QSPI_OK)
    {
      return QSPI_ERROR;
    }

    /* Update the address and size variables for next page programming */
    current_addr += current_size;
    pData += current_size;
    current_size = ((current_addr + MX25R6435F_PAGE_SIZE) > end_addr) ? (end_addr - current_addr) : MX25R6435F_PAGE_SIZE;
  }
  while (current_addr < end_addr);

  return QSPI_OK;
}

/**
  * @brief  Erases the specified block of the QSPI memory.
  * @param  BlockAddress : Block address to erase
  * @retval QSPI memory status
  */
uint8_t BSP_QSPI_Erase_Block(uint32_t BlockAddress)
{
  QSPI_CommandTypeDef sCommand;

  /* Initialize the erase command */
  sCommand.InstructionMode   = QSPI_INSTRUCTION_1_LINE;
  sCommand.Instruction       = BLOCK_ERASE_CMD;
  sCommand.AddressMode       = QSPI_ADDRESS_1_LINE;
  sCommand.AddressSize       = QSPI_ADDRESS_24_BITS;
  sCommand.Address           = BlockAddress;
  sCommand.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
  sCommand.DataMode          = QSPI_DATA_NONE;
  sCommand.DummyCycles       = 0;
  sCommand.DdrMode           = QSPI_DDR_MODE_DISABLE;
  sCommand.DdrHoldHalfCycle  = QSPI_DDR_HHC_ANALOG_DELAY;
  sCommand.SIOOMode          = QSPI_SIOO_INST_EVERY_CMD;

  /* Enable write operations */
  if (QSPI_WriteEnable(&QSPIHandle) != QSPI_OK)
  {
    return QSPI_ERROR;
  }

  /* Send the command */
  if (HAL_QSPI_Command(&QSPIHandle, &sCommand, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    return QSPI_ERROR;
  }

  /* Configure automatic polling mode to wait for end of erase */
  if (QSPI_AutoPollingMemReady(&QSPIHandle, MX25R6435F_BLOCK_ERASE_MAX_TIME) != QSPI_OK)
  {
    return QSPI_ERROR;
  }

  return QSPI_OK;
}

/**
  * @brief  Erases the specified sector of the QSPI memory.
  * @param  Sector : Sector address to erase (0 to 255)
  * @retval QSPI memory status
  * @note This function is non blocking meaning that sector erase
  *       operation is started but not completed when the function
  *       returns. Application has to call BSP_QSPI_GetStatus()
  *       to know when the device is available again (i.e. erase operation
  *       completed).
  */
uint8_t BSP_QSPI_Erase_Sector(uint32_t Sector)
{
  QSPI_CommandTypeDef sCommand;

  if (Sector >= (uint32_t)(MX25R6435F_FLASH_SIZE / MX25R6435F_SECTOR_SIZE))
  {
    return QSPI_ERROR;
  }

  /* Initialize the erase command */
  sCommand.InstructionMode   = QSPI_INSTRUCTION_1_LINE;
  sCommand.Instruction       = SECTOR_ERASE_CMD;
  sCommand.AddressMode       = QSPI_ADDRESS_1_LINE;
  sCommand.AddressSize       = QSPI_ADDRESS_24_BITS;
  sCommand.Address           = (Sector * MX25R6435F_SECTOR_SIZE);
  sCommand.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
  sCommand.DataMode          = QSPI_DATA_NONE;
  sCommand.DummyCycles       = 0;
  sCommand.DdrMode           = QSPI_DDR_MODE_DISABLE;
  sCommand.DdrHoldHalfCycle  = QSPI_DDR_HHC_ANALOG_DELAY;
  sCommand.SIOOMode          = QSPI_SIOO_INST_EVERY_CMD;

  /* Enable write operations */
  if (QSPI_WriteEnable(&QSPIHandle) != QSPI_OK)
  {
    return QSPI_ERROR;
  }

  /* Send the command */
  if (HAL_QSPI_Command(&QSPIHandle, &sCommand, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    return QSPI_ERROR;
  }

  return QSPI_OK;
}

/**
  * @brief  Erases the entire QSPI memory.
  * @retval QSPI memory status
  */
uint8_t BSP_QSPI_Erase_Chip(void)
{
  QSPI_CommandTypeDef sCommand;

  /* Initialize the erase command */
  sCommand.InstructionMode   = QSPI_INSTRUCTION_1_LINE;
  sCommand.Instruction       = CHIP_ERASE_CMD;
  sCommand.AddressMode       = QSPI_ADDRESS_NONE;
  sCommand.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
  sCommand.DataMode          = QSPI_DATA_NONE;
  sCommand.DummyCycles       = 0;
  sCommand.DdrMode           = QSPI_DDR_MODE_DISABLE;
  sCommand.DdrHoldHalfCycle  = QSPI_DDR_HHC_ANALOG_DELAY;
  sCommand.SIOOMode          = QSPI_SIOO_INST_EVERY_CMD;

  /* Enable write operations */
  if (QSPI_WriteEnable(&QSPIHandle) != QSPI_OK)
  {
    return QSPI_ERROR;
  }

  /* Send the command */
  if (HAL_QSPI_Command(&QSPIHandle, &sCommand, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    return QSPI_ERROR;
  }

  /* Configure automatic polling mode to wait for end of erase */
  if (QSPI_AutoPollingMemReady(&QSPIHandle, MX25R6435F_CHIP_ERASE_MAX_TIME) != QSPI_OK)
  {
    return QSPI_ERROR;
  }

  return QSPI_OK;
}

/**
  * @brief  Reads current status of the QSPI memory.
  * @retval QSPI memory status
  */
uint8_t BSP_QSPI_GetStatus(void)
{
  QSPI_CommandTypeDef sCommand;
  uint8_t reg;

  /* Initialize the read security register command */
  sCommand.InstructionMode   = QSPI_INSTRUCTION_1_LINE;
  sCommand.Instruction       = READ_SEC_REG_CMD;
  sCommand.AddressMode       = QSPI_ADDRESS_NONE;
  sCommand.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
  sCommand.DataMode          = QSPI_DATA_1_LINE;
  sCommand.DummyCycles       = 0;
  sCommand.NbData            = 1;
  sCommand.DdrMode           = QSPI_DDR_MODE_DISABLE;
  sCommand.DdrHoldHalfCycle  = QSPI_DDR_HHC_ANALOG_DELAY;
  sCommand.SIOOMode          = QSPI_SIOO_INST_EVERY_CMD;

  /* Configure the command */
  if (HAL_QSPI_Command(&QSPIHandle, &sCommand, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    return QSPI_ERROR;
  }

  /* Reception of the data */
  if (HAL_QSPI_Receive(&QSPIHandle, &reg, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    return QSPI_ERROR;
  }

  /* Check the value of the register */
  if ((reg & (MX25R6435F_SECR_P_FAIL | MX25R6435F_SECR_E_FAIL)) != 0)
  {
    return QSPI_ERROR;
  }
  else if ((reg & (MX25R6435F_SECR_PSB | MX25R6435F_SECR_ESB)) != 0)
  {
    return QSPI_SUSPENDED;
  }

  /* Initialize the read status register command */
  sCommand.Instruction       = READ_STATUS_REG_CMD;

  /* Configure the command */
  if (HAL_QSPI_Command(&QSPIHandle, &sCommand, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    return QSPI_ERROR;
  }

  /* Reception of the data */
  if (HAL_QSPI_Receive(&QSPIHandle, &reg, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    return QSPI_ERROR;
  }

  /* Check the value of the register */
  if ((reg & MX25R6435F_SR_WIP) != 0)
  {
    return QSPI_BUSY;
  }
  else
  {
    return QSPI_OK;
  }
}

/**
  * @brief  Return the configuration of the QSPI memory.
  * @param  pInfo : pointer on the configuration structure
  * @retval QSPI memory status
  */
uint8_t BSP_QSPI_GetInfo(QSPI_Info *pInfo)
{
  /* Configure the structure with the memory configuration */
  pInfo->FlashSize          = MX25R6435F_FLASH_SIZE;
  pInfo->EraseSectorSize    = MX25R6435F_SECTOR_SIZE;
  pInfo->EraseSectorsNumber = (MX25R6435F_FLASH_SIZE / MX25R6435F_SECTOR_SIZE);
  pInfo->ProgPageSize       = MX25R6435F_PAGE_SIZE;
  pInfo->ProgPagesNumber    = (MX25R6435F_FLASH_SIZE / MX25R6435F_PAGE_SIZE);

  return QSPI_OK;
}

/**
  * @brief  Configure the QSPI in memory-mapped mode
  * @retval QSPI memory status
  */
uint8_t BSP_QSPI_EnableMemoryMappedMode(void)
{
  QSPI_CommandTypeDef      sCommand;
  QSPI_MemoryMappedTypeDef sMemMappedCfg;

  /* Configure the command for the read instruction */
  sCommand.InstructionMode    = QSPI_INSTRUCTION_1_LINE;
  sCommand.Instruction        = QUAD_INOUT_READ_CMD;
  sCommand.AddressMode        = QSPI_ADDRESS_4_LINES;
  sCommand.AddressSize        = QSPI_ADDRESS_24_BITS;
  sCommand.AlternateByteMode  = QSPI_ALTERNATE_BYTES_4_LINES;
  sCommand.AlternateBytesSize = QSPI_ALTERNATE_BYTES_8_BITS;
  sCommand.AlternateBytes     = MX25R6435F_ALT_BYTES_NO_PE_MODE;
  sCommand.DataMode           = QSPI_DATA_4_LINES;
  sCommand.DummyCycles        = MX25R6435F_DUMMY_CYCLES_READ_QUAD;
  sCommand.DdrMode            = QSPI_DDR_MODE_DISABLE;
  sCommand.DdrHoldHalfCycle   = QSPI_DDR_HHC_ANALOG_DELAY;
  sCommand.SIOOMode           = QSPI_SIOO_INST_EVERY_CMD;

  /* Configure the memory mapped mode */
  sMemMappedCfg.TimeOutActivation = QSPI_TIMEOUT_COUNTER_DISABLE;

  if (HAL_QSPI_MemoryMapped(&QSPIHandle, &sCommand, &sMemMappedCfg) != HAL_OK)
  {
    return QSPI_ERROR;
  }

  return QSPI_OK;
}

/**
  * @brief  This function suspends an ongoing erase command.
  * @retval QSPI memory status
  */
uint8_t BSP_QSPI_SuspendErase(void)
{
  QSPI_CommandTypeDef sCommand;

  /* Check whether the device is busy (erase operation is
  in progress).
  */
  if (BSP_QSPI_GetStatus() == QSPI_BUSY)
  {
    /* Initialize the erase command */
    sCommand.InstructionMode   = QSPI_INSTRUCTION_1_LINE;
    sCommand.Instruction       = PROG_ERASE_SUSPEND_CMD;
    sCommand.AddressMode       = QSPI_ADDRESS_NONE;
    sCommand.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
    sCommand.DataMode          = QSPI_DATA_NONE;
    sCommand.DummyCycles       = 0;
    sCommand.DdrMode           = QSPI_DDR_MODE_DISABLE;
    sCommand.DdrHoldHalfCycle  = QSPI_DDR_HHC_ANALOG_DELAY;
    sCommand.SIOOMode          = QSPI_SIOO_INST_EVERY_CMD;

    /* Send the command */
    if (HAL_QSPI_Command(&QSPIHandle, &sCommand, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
    {
      return QSPI_ERROR;
    }

    if (BSP_QSPI_GetStatus() == QSPI_SUSPENDED)
    {
      return QSPI_OK;
    }

    return QSPI_ERROR;
  }

  return QSPI_OK;
}

/**
  * @brief  This function resumes a paused erase command.
  * @retval QSPI memory status
  */
uint8_t BSP_QSPI_ResumeErase(void)
{
  QSPI_CommandTypeDef sCommand;

  /* Check whether the device is in suspended state */
  if (BSP_QSPI_GetStatus() == QSPI_SUSPENDED)
  {
    /* Initialize the erase command */
    sCommand.InstructionMode   = QSPI_INSTRUCTION_1_LINE;
    sCommand.Instruction       = PROG_ERASE_RESUME_CMD;
    sCommand.AddressMode       = QSPI_ADDRESS_NONE;
    sCommand.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
    sCommand.DataMode          = QSPI_DATA_NONE;
    sCommand.DummyCycles       = 0;
    sCommand.DdrMode           = QSPI_DDR_MODE_DISABLE;
    sCommand.DdrHoldHalfCycle  = QSPI_DDR_HHC_ANALOG_DELAY;
    sCommand.SIOOMode          = QSPI_SIOO_INST_EVERY_CMD;

    /* Send the command */
    if (HAL_QSPI_Command(&QSPIHandle, &sCommand, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
    {
      return QSPI_ERROR;
    }

    /*
    When this command is executed, the status register write in progress bit is set to 1, and
    the flag status register program erase controller bit is set to 0. This command is ignored
    if the device is not in a suspended state.
    */

    if (BSP_QSPI_GetStatus() == QSPI_BUSY)
    {
      return QSPI_OK;
    }

    return QSPI_ERROR;
  }

  return QSPI_OK;
}

/**
  * @brief  This function enter the QSPI memory in deep power down mode.
  * @retval QSPI memory status
  */
uint8_t BSP_QSPI_EnterDeepPowerDown(void)
{
  QSPI_CommandTypeDef sCommand;

  /* Initialize the deep power down command */
  sCommand.InstructionMode   = QSPI_INSTRUCTION_1_LINE;
  sCommand.Instruction       = DEEP_POWER_DOWN_CMD;
  sCommand.AddressMode       = QSPI_ADDRESS_NONE;
  sCommand.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
  sCommand.DataMode          = QSPI_DATA_NONE;
  sCommand.DummyCycles       = 0;
  sCommand.DdrMode           = QSPI_DDR_MODE_DISABLE;
  sCommand.DdrHoldHalfCycle  = QSPI_DDR_HHC_ANALOG_DELAY;
  sCommand.SIOOMode          = QSPI_SIOO_INST_EVERY_CMD;

  /* Send the command */
  if (HAL_QSPI_Command(&QSPIHandle, &sCommand, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    return QSPI_ERROR;
  }

  /* ---          Memory takes 10us max to enter deep power down          --- */
  /* --- At least 30us should be respected before leaving deep power down --- */

  return QSPI_OK;
}

/**
  * @brief  This function leave the QSPI memory from deep power down mode.
  * @retval QSPI memory status
  */
uint8_t BSP_QSPI_LeaveDeepPowerDown(void)
{
  QSPI_CommandTypeDef sCommand;

  /* Initialize the erase command */
  sCommand.InstructionMode   = QSPI_INSTRUCTION_1_LINE;
  sCommand.Instruction       = NO_OPERATION_CMD;
  sCommand.AddressMode       = QSPI_ADDRESS_NONE;
  sCommand.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
  sCommand.DataMode          = QSPI_DATA_NONE;
  sCommand.DummyCycles       = 0;
  sCommand.DdrMode           = QSPI_DDR_MODE_DISABLE;
  sCommand.DdrHoldHalfCycle  = QSPI_DDR_HHC_ANALOG_DELAY;
  sCommand.SIOOMode          = QSPI_SIOO_INST_EVERY_CMD;

  /* Send the command */
  if (HAL_QSPI_Command(&QSPIHandle, &sCommand, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    return QSPI_ERROR;
  }

  /* --- A NOP command is sent to the memory, as the nCS should be low for at least 20 ns --- */
  /* ---                  Memory takes 35us min to leave deep power down                  --- */

  return QSPI_OK;
}
/**
  * @}
  */

/** @addtogroup STM32L496G_DISCOVERY_QSPI_Private_Functions
  * @{
  */

/**
  * @brief  Initializes the QSPI MSP.
  * @retval None
  */
static void QSPI_MspInit(void)
{
  GPIO_InitTypeDef GPIO_InitStruct;

  /* Enable the QuadSPI memory interface clock */
  __HAL_RCC_QSPI_CLK_ENABLE();

  /* Reset the QuadSPI memory interface */
  __HAL_RCC_QSPI_FORCE_RESET();
  __HAL_RCC_QSPI_RELEASE_RESET();

  /* Enable GPIO clocks */
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /* QSPI CS GPIO pin configuration  */
  GPIO_InitStruct.Pin       = GPIO_PIN_11;
  GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull      = GPIO_PULLUP;
  GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF10_QUADSPI;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /* QSPI CLK, D0, D1, D2 and D3 GPIO pins configuration  */
  GPIO_InitStruct.Pin       = (GPIO_PIN_3 | GPIO_PIN_6 | GPIO_PIN_7);
  GPIO_InitStruct.Pull      = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  GPIO_InitStruct.Pin       = (GPIO_PIN_0 | GPIO_PIN_1);
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
}

/**
  * @brief  De-Initializes the QSPI MSP.
  * @retval None
  */
static void QSPI_MspDeInit(void)
{
  GPIO_InitTypeDef GPIO_InitStruct;

  /* QSPI CLK, CS, D0-D3 GPIO pins de-configuration  */

  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  HAL_GPIO_DeInit(GPIOA, (GPIO_PIN_6 | GPIO_PIN_7));
  HAL_GPIO_DeInit(GPIOB, (GPIO_PIN_0 | GPIO_PIN_1));

  /* Set GPIOB pin 11 in pull up mode (optimum default setting) */
  GPIO_InitStruct.Mode      = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pin       = GPIO_PIN_11;
  GPIO_InitStruct.Pull      = GPIO_NOPULL;
  GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /* Set GPIOA pin 3 in no pull, low state (optimum default setting) */
  GPIO_InitStruct.Mode      = GPIO_MODE_OUTPUT_PP  ;
  GPIO_InitStruct.Pull      = GPIO_NOPULL;
  GPIO_InitStruct.Pin       = GPIO_PIN_3;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_3, GPIO_PIN_RESET);

  /* Reset the QuadSPI memory interface */
  __HAL_RCC_QSPI_FORCE_RESET();
  __HAL_RCC_QSPI_RELEASE_RESET();

  /* Disable the QuadSPI memory interface clock */
  __HAL_RCC_QSPI_CLK_DISABLE();
}

/**
  * @brief  This function reset the QSPI memory.
  * @param  hqspi : QSPI handle
  * @retval None
  */
static uint8_t QSPI_ResetMemory(QSPI_HandleTypeDef *hqspi)
{
  QSPI_CommandTypeDef sCommand;

  /* Initialize the reset enable command */
  sCommand.InstructionMode   = QSPI_INSTRUCTION_1_LINE;
  sCommand.Instruction       = RESET_ENABLE_CMD;
  sCommand.AddressMode       = QSPI_ADDRESS_NONE;
  sCommand.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
  sCommand.DataMode          = QSPI_DATA_NONE;
  sCommand.DummyCycles       = 0;
  sCommand.DdrMode           = QSPI_DDR_MODE_DISABLE;
  sCommand.DdrHoldHalfCycle  = QSPI_DDR_HHC_ANALOG_DELAY;
  sCommand.SIOOMode          = QSPI_SIOO_INST_EVERY_CMD;

  /* Send the command */
  if (HAL_QSPI_Command(&QSPIHandle, &sCommand, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    return QSPI_ERROR;
  }

  /* Send the reset memory command */
  sCommand.Instruction = RESET_MEMORY_CMD;
  if (HAL_QSPI_Command(&QSPIHandle, &sCommand, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    return QSPI_ERROR;
  }

  /* Configure automatic polling mode to wait the memory is ready */
  if (QSPI_AutoPollingMemReady(&QSPIHandle, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != QSPI_OK)
  {
    return QSPI_ERROR;
  }

  return QSPI_OK;
}

/**
  * @brief  This function send a Write Enable and wait it is effective.
  * @param  hqspi : QSPI handle
  * @retval None
  */
static uint8_t QSPI_WriteEnable(QSPI_HandleTypeDef *hqspi)
{
  QSPI_CommandTypeDef     sCommand;
  QSPI_AutoPollingTypeDef sConfig;

  /* Enable write operations */
  sCommand.InstructionMode   = QSPI_INSTRUCTION_1_LINE;
  sCommand.Instruction       = WRITE_ENABLE_CMD;
  sCommand.AddressMode       = QSPI_ADDRESS_NONE;
  sCommand.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
  sCommand.DataMode          = QSPI_DATA_NONE;
  sCommand.DummyCycles       = 0;
  sCommand.DdrMode           = QSPI_DDR_MODE_DISABLE;
  sCommand.DdrHoldHalfCycle  = QSPI_DDR_HHC_ANALOG_DELAY;
  sCommand.SIOOMode          = QSPI_SIOO_INST_EVERY_CMD;

  if (HAL_QSPI_Command(&QSPIHandle, &sCommand, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    return QSPI_ERROR;
  }

  /* Configure automatic polling mode to wait for write enabling */
  sConfig.Match           = MX25R6435F_SR_WEL;
  sConfig.Mask            = MX25R6435F_SR_WEL;
  sConfig.MatchMode       = QSPI_MATCH_MODE_AND;
  sConfig.StatusBytesSize = 1;
  sConfig.Interval        = 0x10;
  sConfig.AutomaticStop   = QSPI_AUTOMATIC_STOP_ENABLE;

  sCommand.Instruction    = READ_STATUS_REG_CMD;
  sCommand.DataMode       = QSPI_DATA_1_LINE;

  if (HAL_QSPI_AutoPolling(&QSPIHandle, &sCommand, &sConfig, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    return QSPI_ERROR;
  }

  return QSPI_OK;
}

/**
  * @brief  This function read the SR of the memory and wait the EOP.
  * @param  hqspi   : QSPI handle
  * @param  Timeout : Timeout for auto-polling
  * @retval None
  */
static uint8_t QSPI_AutoPollingMemReady(QSPI_HandleTypeDef *hqspi, uint32_t Timeout)
{
  QSPI_CommandTypeDef     sCommand;
  QSPI_AutoPollingTypeDef sConfig;

  /* Configure automatic polling mode to wait for memory ready */
  sCommand.InstructionMode   = QSPI_INSTRUCTION_1_LINE;
  sCommand.Instruction       = READ_STATUS_REG_CMD;
  sCommand.AddressMode       = QSPI_ADDRESS_NONE;
  sCommand.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
  sCommand.DataMode          = QSPI_DATA_1_LINE;
  sCommand.DummyCycles       = 0;
  sCommand.DdrMode           = QSPI_DDR_MODE_DISABLE;
  sCommand.DdrHoldHalfCycle  = QSPI_DDR_HHC_ANALOG_DELAY;
  sCommand.SIOOMode          = QSPI_SIOO_INST_EVERY_CMD;

  sConfig.Match           = 0;
  sConfig.Mask            = MX25R6435F_SR_WIP;
  sConfig.MatchMode       = QSPI_MATCH_MODE_AND;
  sConfig.StatusBytesSize = 1;
  sConfig.Interval        = 0x10;
  sConfig.AutomaticStop   = QSPI_AUTOMATIC_STOP_ENABLE;

  if (HAL_QSPI_AutoPolling(&QSPIHandle, &sCommand, &sConfig, Timeout) != HAL_OK)
  {
    return QSPI_ERROR;
  }

  return QSPI_OK;
}

/**
  * @brief  This function enables/disables the Quad mode of the memory.
  * @param  hqspi     : QSPI handle
  * @param  Operation : QSPI_QUAD_ENABLE or QSPI_QUAD_DISABLE mode
  * @retval None
  */
static uint8_t QSPI_QuadMode(QSPI_HandleTypeDef *hqspi, uint8_t Operation)
{
  QSPI_CommandTypeDef sCommand;
  uint8_t reg;

  /* Read status register */
  sCommand.InstructionMode   = QSPI_INSTRUCTION_1_LINE;
  sCommand.Instruction       = READ_STATUS_REG_CMD;
  sCommand.AddressMode       = QSPI_ADDRESS_NONE;
  sCommand.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
  sCommand.DataMode          = QSPI_DATA_1_LINE;
  sCommand.DummyCycles       = 0;
  sCommand.NbData            = 1;
  sCommand.DdrMode           = QSPI_DDR_MODE_DISABLE;
  sCommand.DdrHoldHalfCycle  = QSPI_DDR_HHC_ANALOG_DELAY;
  sCommand.SIOOMode          = QSPI_SIOO_INST_EVERY_CMD;

  if (HAL_QSPI_Command(&QSPIHandle, &sCommand, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    return QSPI_ERROR;
  }

  if (HAL_QSPI_Receive(&QSPIHandle, &reg, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    return QSPI_ERROR;
  }

  /* Enable write operations */
  if (QSPI_WriteEnable(&QSPIHandle) != QSPI_OK)
  {
    return QSPI_ERROR;
  }

  /* Activate/deactivate the Quad mode */
  if (Operation == QSPI_QUAD_ENABLE)
  {
    SET_BIT(reg, MX25R6435F_SR_QE);
  }
  else
  {
    CLEAR_BIT(reg, MX25R6435F_SR_QE);
  }

  sCommand.Instruction = WRITE_STATUS_CFG_REG_CMD;

  if (HAL_QSPI_Command(&QSPIHandle, &sCommand, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    return QSPI_ERROR;
  }

  if (HAL_QSPI_Transmit(&QSPIHandle, &reg, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    return QSPI_ERROR;
  }

  /* Wait that memory is ready */
  if (QSPI_AutoPollingMemReady(&QSPIHandle, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != QSPI_OK)
  {
    return QSPI_ERROR;
  }

  /* Check the configuration has been correctly done */
  sCommand.Instruction = READ_STATUS_REG_CMD;

  if (HAL_QSPI_Command(&QSPIHandle, &sCommand, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    return QSPI_ERROR;
  }

  if (HAL_QSPI_Receive(&QSPIHandle, &reg, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    return QSPI_ERROR;
  }

  if ((((reg & MX25R6435F_SR_QE) == 0) && (Operation == QSPI_QUAD_ENABLE)) ||
      (((reg & MX25R6435F_SR_QE) != 0) && (Operation == QSPI_QUAD_DISABLE)))
  {
    return QSPI_ERROR;
  }

  return QSPI_OK;
}

/**
  * @brief  This function enables/disables the high performance mode of the memory.
  * @param  hqspi     : QSPI handle
  * @param  Operation : QSPI_HIGH_PERF_ENABLE or QSPI_HIGH_PERF_DISABLE high performance mode
  * @retval None
  */
static uint8_t QSPI_HighPerfMode(QSPI_HandleTypeDef *hqspi, uint8_t Operation)
{
  QSPI_CommandTypeDef sCommand;
  uint8_t reg[3];

  /* Read status register */
  sCommand.InstructionMode   = QSPI_INSTRUCTION_1_LINE;
  sCommand.Instruction       = READ_STATUS_REG_CMD;
  sCommand.AddressMode       = QSPI_ADDRESS_NONE;
  sCommand.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
  sCommand.DataMode          = QSPI_DATA_1_LINE;
  sCommand.DummyCycles       = 0;
  sCommand.NbData            = 1;
  sCommand.DdrMode           = QSPI_DDR_MODE_DISABLE;
  sCommand.DdrHoldHalfCycle  = QSPI_DDR_HHC_ANALOG_DELAY;
  sCommand.SIOOMode          = QSPI_SIOO_INST_EVERY_CMD;

  if (HAL_QSPI_Command(&QSPIHandle, &sCommand, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    return QSPI_ERROR;
  }

  if (HAL_QSPI_Receive(&QSPIHandle, &(reg[0]), HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    return QSPI_ERROR;
  }

  /* Read configuration registers */
  sCommand.Instruction = READ_CFG_REG_CMD;
  sCommand.NbData      = 2;

  if (HAL_QSPI_Command(&QSPIHandle, &sCommand, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    return QSPI_ERROR;
  }

  if (HAL_QSPI_Receive(&QSPIHandle, &(reg[1]), HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    return QSPI_ERROR;
  }

  /* Enable write operations */
  if (QSPI_WriteEnable(&QSPIHandle) != QSPI_OK)
  {
    return QSPI_ERROR;
  }

  /* Activate/deactivate the Quad mode */
  if (Operation == QSPI_HIGH_PERF_ENABLE)
  {
    SET_BIT(reg[2], MX25R6435F_CR2_LH_SWITCH);
  }
  else
  {
    CLEAR_BIT(reg[2], MX25R6435F_CR2_LH_SWITCH);
  }

  sCommand.Instruction = WRITE_STATUS_CFG_REG_CMD;
  sCommand.NbData      = 3;

  if (HAL_QSPI_Command(&QSPIHandle, &sCommand, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    return QSPI_ERROR;
  }

  if (HAL_QSPI_Transmit(&QSPIHandle, &(reg[0]), HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    return QSPI_ERROR;
  }

  /* Wait that memory is ready */
  if (QSPI_AutoPollingMemReady(&QSPIHandle, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != QSPI_OK)
  {
    return QSPI_ERROR;
  }

  /* Check the configuration has been correctly done */
  sCommand.Instruction = READ_CFG_REG_CMD;
  sCommand.NbData      = 2;

  if (HAL_QSPI_Command(&QSPIHandle, &sCommand, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    return QSPI_ERROR;
  }

  if (HAL_QSPI_Receive(&QSPIHandle, &(reg[0]), HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    return QSPI_ERROR;
  }

  if ((((reg[1] & MX25R6435F_CR2_LH_SWITCH) == 0) && (Operation == QSPI_HIGH_PERF_ENABLE)) ||
      (((reg[1] & MX25R6435F_CR2_LH_SWITCH) != 0) && (Operation == QSPI_HIGH_PERF_DISABLE)))
  {
    return QSPI_ERROR;
  }

  return QSPI_OK;
}

/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

