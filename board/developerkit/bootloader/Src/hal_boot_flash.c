#include "stm32l4xx_hal.h"

#define FLASH_NB_DOUBLE_WORDS_IN_ROW  32

FLASH_ProcessTypeDef pFlash = {
            .Lock = HAL_UNLOCKED, \
            .ErrorCode = HAL_FLASH_ERROR_NONE, \
            .ProcedureOnGoing = FLASH_PROC_NONE, \
            .Address = 0U, \
            .Bank = FLASH_BANK_1, \
            .Page = 0U, \
            .NbPagesToErase = 0U, \
            .CacheToReactivate = FLASH_CACHE_DISABLED
};

HAL_StatusTypeDef FLASH_WaitForLastOperation(uint32_t Timeout);
HAL_StatusTypeDef HAL_FLASH_Unlock(void)
{
    HAL_StatusTypeDef status = HAL_OK;
    if(READ_BIT(FLASH->CR, FLASH_CR_LOCK) != 0U) {
        /* Authorize the FLASH Registers access */
        WRITE_REG(FLASH->KEYR, FLASH_KEY1);
        WRITE_REG(FLASH->KEYR, FLASH_KEY2);
        /* Verify Flash is unlocked */
        if(READ_BIT(FLASH->CR, FLASH_CR_LOCK) != 0U) {
            status = HAL_ERROR;
        }
    }
    return status;
}

HAL_StatusTypeDef HAL_FLASH_OB_Lock(void)
{
  /* Set the OPTLOCK Bit to lock the FLASH Option Byte Registers access */
  SET_BIT(FLASH->CR, FLASH_CR_OPTLOCK);

  return HAL_OK;
}

HAL_StatusTypeDef HAL_FLASH_Lock(void)
{
    SET_BIT(FLASH->CR, FLASH_CR_LOCK);
    return HAL_OK;
}
static uint32_t FLASH_OB_GetUser(void)
{
    uint32_t user_config = READ_REG(FLASH->OPTR);
    CLEAR_BIT(user_config, FLASH_OPTR_RDP);
    return user_config;
}
static void FLASH_OB_GetPCROP(uint32_t * PCROPConfig, uint32_t * PCROPStartAddr, uint32_t * PCROPEndAddr)
{
    uint32_t reg_value;
    uint32_t bank1_addr;
    uint32_t bank2_addr;

    if (READ_BIT(SYSCFG->MEMRMP, SYSCFG_MEMRMP_FB_MODE) == 0U) {
        bank1_addr = FLASH_BASE;
        bank2_addr = FLASH_BASE + FLASH_BANK_SIZE;
    }
    else {
        bank1_addr = FLASH_BASE + FLASH_BANK_SIZE;
        bank2_addr = FLASH_BASE;
    }
    if(((*PCROPConfig) & FLASH_BANK_BOTH) == FLASH_BANK_1) {
        reg_value       = (READ_REG(FLASH->PCROP1SR) & FLASH_PCROP1SR_PCROP1_STRT);
        *PCROPStartAddr = (reg_value << 3) + bank1_addr;
        reg_value     = (READ_REG(FLASH->PCROP1ER) & FLASH_PCROP1ER_PCROP1_END);
        *PCROPEndAddr = (reg_value << 3) + bank1_addr;
    }
    else if(((*PCROPConfig) & FLASH_BANK_BOTH) == FLASH_BANK_2) {
        reg_value       = (READ_REG(FLASH->PCROP2SR) & FLASH_PCROP2SR_PCROP2_STRT);
        *PCROPStartAddr = (reg_value << 3) + bank2_addr;
        reg_value     = (READ_REG(FLASH->PCROP2ER) & FLASH_PCROP2ER_PCROP2_END);
        *PCROPEndAddr = (reg_value << 3) + bank2_addr;
    }
    else {
        /* Nothing to do */
    }

    *PCROPConfig |= (READ_REG(FLASH->PCROP1ER) & FLASH_PCROP1ER_PCROP_RDP);
}

static uint32_t FLASH_OB_GetRDP(void)
{
    uint32_t rdp_level = READ_BIT(FLASH->OPTR, FLASH_OPTR_RDP);

    if ((rdp_level != OB_RDP_LEVEL_0) && (rdp_level != OB_RDP_LEVEL_2)) {
        return (OB_RDP_LEVEL_1);
    }
    else {
        return (READ_BIT(FLASH->OPTR, FLASH_OPTR_RDP));
    }
}

static void FLASH_OB_GetWRP(uint32_t WRPArea, uint32_t * WRPStartOffset, uint32_t * WRDPEndOffset)
{
    /* Get the configuration of the write protected area */
    if(WRPArea == OB_WRPAREA_BANK1_AREAA) {
        *WRPStartOffset = READ_BIT(FLASH->WRP1AR, FLASH_WRP1AR_WRP1A_STRT);
        *WRDPEndOffset = (READ_BIT(FLASH->WRP1AR, FLASH_WRP1AR_WRP1A_END) >> 16);
    }
    else if(WRPArea == OB_WRPAREA_BANK1_AREAB) {
        *WRPStartOffset = READ_BIT(FLASH->WRP1BR, FLASH_WRP1BR_WRP1B_STRT);
        *WRDPEndOffset = (READ_BIT(FLASH->WRP1BR, FLASH_WRP1BR_WRP1B_END) >> 16);
    }
    else if(WRPArea == OB_WRPAREA_BANK2_AREAA) {
        *WRPStartOffset = READ_BIT(FLASH->WRP2AR, FLASH_WRP2AR_WRP2A_STRT);
        *WRDPEndOffset = (READ_BIT(FLASH->WRP2AR, FLASH_WRP2AR_WRP2A_END) >> 16);
    }
    else if(WRPArea == OB_WRPAREA_BANK2_AREAB) {
        *WRPStartOffset = READ_BIT(FLASH->WRP2BR, FLASH_WRP2BR_WRP2B_STRT);
        *WRDPEndOffset = (READ_BIT(FLASH->WRP2BR, FLASH_WRP2BR_WRP2B_END) >> 16);
    }
    else {
        /* Nothing to do */
    }
}

void HAL_FLASHEx_OBGetConfig(FLASH_OBProgramInitTypeDef *pOBInit)
{
    pOBInit->OptionType = (OPTIONBYTE_RDP | OPTIONBYTE_USER);
    if((pOBInit->WRPArea == OB_WRPAREA_BANK1_AREAA) || (pOBInit->WRPArea == OB_WRPAREA_BANK1_AREAB) ||
        (pOBInit->WRPArea == OB_WRPAREA_BANK2_AREAA) || (pOBInit->WRPArea == OB_WRPAREA_BANK2_AREAB))
    {
        pOBInit->OptionType |= OPTIONBYTE_WRP;
        FLASH_OB_GetWRP(pOBInit->WRPArea, &(pOBInit->WRPStartOffset), &(pOBInit->WRPEndOffset));
    }
    pOBInit->RDPLevel = FLASH_OB_GetRDP();
    pOBInit->USERConfig = FLASH_OB_GetUser();

    if((pOBInit->PCROPConfig == FLASH_BANK_1) || (pOBInit->PCROPConfig == FLASH_BANK_2)) {
        pOBInit->OptionType |= OPTIONBYTE_PCROP;
        /* Get the Proprietary code readout protection */
        FLASH_OB_GetPCROP(&(pOBInit->PCROPConfig), &(pOBInit->PCROPStartAddr), &(pOBInit->PCROPEndAddr));
    }
}

static HAL_StatusTypeDef FLASH_OB_WRPConfig(uint32_t WRPArea, uint32_t WRPStartOffset, uint32_t WRDPEndOffset)
{
    HAL_StatusTypeDef status;
    /* Check the parameters */
    assert_param(IS_OB_WRPAREA(WRPArea));
    assert_param(IS_FLASH_PAGE(WRPStartOffset));
    assert_param(IS_FLASH_PAGE(WRDPEndOffset));
    /* Wait for last operation to be completed */
    status = FLASH_WaitForLastOperation((uint32_t)FLASH_TIMEOUT_VALUE);
    if(status == HAL_OK) {
        /* Configure the write protected area */
        if(WRPArea == OB_WRPAREA_BANK1_AREAA) {
            MODIFY_REG(FLASH->WRP1AR, (FLASH_WRP1AR_WRP1A_STRT | FLASH_WRP1AR_WRP1A_END),
                (WRPStartOffset | (WRDPEndOffset << 16)));
        }
        else if(WRPArea == OB_WRPAREA_BANK1_AREAB) {
            MODIFY_REG(FLASH->WRP1BR, (FLASH_WRP1BR_WRP1B_STRT | FLASH_WRP1BR_WRP1B_END),
            (WRPStartOffset | (WRDPEndOffset << 16)));
        }
        else if(WRPArea == OB_WRPAREA_BANK2_AREAA) {
            MODIFY_REG(FLASH->WRP2AR, (FLASH_WRP2AR_WRP2A_STRT | FLASH_WRP2AR_WRP2A_END),
                (WRPStartOffset | (WRDPEndOffset << 16)));
        }
        else if(WRPArea == OB_WRPAREA_BANK2_AREAB) {
            MODIFY_REG(FLASH->WRP2BR, (FLASH_WRP2BR_WRP2B_STRT | FLASH_WRP2BR_WRP2B_END),
                (WRPStartOffset | (WRDPEndOffset << 16)));
        }
        else {
            /* Nothing to do */
        }
        /* Set OPTSTRT Bit */
        SET_BIT(FLASH->CR, FLASH_CR_OPTSTRT);
        /* Wait for last operation to be completed */
        status = FLASH_WaitForLastOperation((uint32_t)FLASH_TIMEOUT_VALUE);
        /* If the option byte program operation is completed, disable the OPTSTRT Bit */
        CLEAR_BIT(FLASH->CR, FLASH_CR_OPTSTRT);
    }
    return status;
}

static HAL_StatusTypeDef FLASH_OB_RDPConfig(uint32_t RDPLevel)
{
  HAL_StatusTypeDef status;

  /* Check the parameters */
  assert_param(IS_OB_RDP_LEVEL(RDPLevel));

  /* Wait for last operation to be completed */
  status = FLASH_WaitForLastOperation((uint32_t)FLASH_TIMEOUT_VALUE);

  if(status == HAL_OK)
  {
    /* Configure the RDP level in the option bytes register */
    MODIFY_REG(FLASH->OPTR, FLASH_OPTR_RDP, RDPLevel);

    /* Set OPTSTRT Bit */
    SET_BIT(FLASH->CR, FLASH_CR_OPTSTRT);

    /* Wait for last operation to be completed */
    status = FLASH_WaitForLastOperation((uint32_t)FLASH_TIMEOUT_VALUE);

    /* If the option byte program operation is completed, disable the OPTSTRT Bit */
    CLEAR_BIT(FLASH->CR, FLASH_CR_OPTSTRT);
  }

  return status;
}
static HAL_StatusTypeDef FLASH_OB_UserConfig(uint32_t UserType, uint32_t UserConfig)
{
  uint32_t optr_reg_val = 0;
  uint32_t optr_reg_mask = 0;
  HAL_StatusTypeDef status;

  /* Check the parameters */
  assert_param(IS_OB_USER_TYPE(UserType));

  /* Wait for last operation to be completed */
  status = FLASH_WaitForLastOperation((uint32_t)FLASH_TIMEOUT_VALUE);

  if(status == HAL_OK)
  {
    if((UserType & OB_USER_BOR_LEV) != 0U)
    {
      /* BOR level option byte should be modified */
      assert_param(IS_OB_USER_BOR_LEVEL(UserConfig & FLASH_OPTR_BOR_LEV));

      /* Set value and mask for BOR level option byte */
      optr_reg_val |= (UserConfig & FLASH_OPTR_BOR_LEV);
      optr_reg_mask |= FLASH_OPTR_BOR_LEV;
    }

    if((UserType & OB_USER_nRST_STOP) != 0U)
    {
      /* nRST_STOP option byte should be modified */
      assert_param(IS_OB_USER_STOP(UserConfig & FLASH_OPTR_nRST_STOP));

      /* Set value and mask for nRST_STOP option byte */
      optr_reg_val |= (UserConfig & FLASH_OPTR_nRST_STOP);
      optr_reg_mask |= FLASH_OPTR_nRST_STOP;
    }

    if((UserType & OB_USER_nRST_STDBY) != 0U)
    {
      /* nRST_STDBY option byte should be modified */
      assert_param(IS_OB_USER_STANDBY(UserConfig & FLASH_OPTR_nRST_STDBY));

      /* Set value and mask for nRST_STDBY option byte */
      optr_reg_val |= (UserConfig & FLASH_OPTR_nRST_STDBY);
      optr_reg_mask |= FLASH_OPTR_nRST_STDBY;
    }

    if((UserType & OB_USER_nRST_SHDW) != 0U)
    {
      /* nRST_SHDW option byte should be modified */
      assert_param(IS_OB_USER_SHUTDOWN(UserConfig & FLASH_OPTR_nRST_SHDW));

      /* Set value and mask for nRST_SHDW option byte */
      optr_reg_val |= (UserConfig & FLASH_OPTR_nRST_SHDW);
      optr_reg_mask |= FLASH_OPTR_nRST_SHDW;
    }

    if((UserType & OB_USER_IWDG_SW) != 0U)
    {
      /* IWDG_SW option byte should be modified */
      assert_param(IS_OB_USER_IWDG(UserConfig & FLASH_OPTR_IWDG_SW));

      /* Set value and mask for IWDG_SW option byte */
      optr_reg_val |= (UserConfig & FLASH_OPTR_IWDG_SW);
      optr_reg_mask |= FLASH_OPTR_IWDG_SW;
    }

    if((UserType & OB_USER_IWDG_STOP) != 0U)
    {
      /* IWDG_STOP option byte should be modified */
      assert_param(IS_OB_USER_IWDG_STOP(UserConfig & FLASH_OPTR_IWDG_STOP));

      /* Set value and mask for IWDG_STOP option byte */
      optr_reg_val |= (UserConfig & FLASH_OPTR_IWDG_STOP);
      optr_reg_mask |= FLASH_OPTR_IWDG_STOP;
    }

    if((UserType & OB_USER_IWDG_STDBY) != 0U)
    {
      /* IWDG_STDBY option byte should be modified */
      assert_param(IS_OB_USER_IWDG_STDBY(UserConfig & FLASH_OPTR_IWDG_STDBY));

      /* Set value and mask for IWDG_STDBY option byte */
      optr_reg_val |= (UserConfig & FLASH_OPTR_IWDG_STDBY);
      optr_reg_mask |= FLASH_OPTR_IWDG_STDBY;
    }

    if((UserType & OB_USER_WWDG_SW) != 0U)
    {
      /* WWDG_SW option byte should be modified */
      assert_param(IS_OB_USER_WWDG(UserConfig & FLASH_OPTR_WWDG_SW));

      /* Set value and mask for WWDG_SW option byte */
      optr_reg_val |= (UserConfig & FLASH_OPTR_WWDG_SW);
      optr_reg_mask |= FLASH_OPTR_WWDG_SW;
    }

    if((UserType & OB_USER_BFB2) != 0U)
    {
      /* BFB2 option byte should be modified */
      assert_param(IS_OB_USER_BFB2(UserConfig & FLASH_OPTR_BFB2));

      /* Set value and mask for BFB2 option byte */
      optr_reg_val |= (UserConfig & FLASH_OPTR_BFB2);
      optr_reg_mask |= FLASH_OPTR_BFB2;
    }

    if((UserType & OB_USER_DUALBANK) != 0U)
    {
      /* DUALBANK option byte should be modified */
      assert_param(IS_OB_USER_DUALBANK(UserConfig & FLASH_OPTR_DUALBANK));

      /* Set value and mask for DUALBANK option byte */
      optr_reg_val |= (UserConfig & FLASH_OPTR_DUALBANK);
      optr_reg_mask |= FLASH_OPTR_DUALBANK;
    }
    if((UserType & OB_USER_nBOOT1) != 0U)
    {
      /* nBOOT1 option byte should be modified */
      assert_param(IS_OB_USER_BOOT1(UserConfig & FLASH_OPTR_nBOOT1));

      /* Set value and mask for nBOOT1 option byte */
      optr_reg_val |= (UserConfig & FLASH_OPTR_nBOOT1);
      optr_reg_mask |= FLASH_OPTR_nBOOT1;
    }

    if((UserType & OB_USER_SRAM2_PE) != 0U)
    {
      /* SRAM2_PE option byte should be modified */
      assert_param(IS_OB_USER_SRAM2_PARITY(UserConfig & FLASH_OPTR_SRAM2_PE));

      /* Set value and mask for SRAM2_PE option byte */
      optr_reg_val |= (UserConfig & FLASH_OPTR_SRAM2_PE);
      optr_reg_mask |= FLASH_OPTR_SRAM2_PE;
    }

    if((UserType & OB_USER_SRAM2_RST) != 0U)
    {
      /* SRAM2_RST option byte should be modified */
      assert_param(IS_OB_USER_SRAM2_RST(UserConfig & FLASH_OPTR_SRAM2_RST));

      /* Set value and mask for SRAM2_RST option byte */
      optr_reg_val |= (UserConfig & FLASH_OPTR_SRAM2_RST);
      optr_reg_mask |= FLASH_OPTR_SRAM2_RST;
    }
    if((UserType & OB_USER_nSWBOOT0) != 0U)
    {
      /* nSWBOOT0 option byte should be modified */
      assert_param(IS_OB_USER_SWBOOT0(UserConfig & FLASH_OPTR_nSWBOOT0));

      /* Set value and mask for nSWBOOT0 option byte */
      optr_reg_val |= (UserConfig & FLASH_OPTR_nSWBOOT0);
      optr_reg_mask |= FLASH_OPTR_nSWBOOT0;
    }

    if((UserType & OB_USER_nBOOT0) != 0U)
    {
      /* nBOOT0 option byte should be modified */
      assert_param(IS_OB_USER_BOOT0(UserConfig & FLASH_OPTR_nBOOT0));

      /* Set value and mask for nBOOT0 option byte */
      optr_reg_val |= (UserConfig & FLASH_OPTR_nBOOT0);
      optr_reg_mask |= FLASH_OPTR_nBOOT0;
    }
    /* Configure the option bytes register */
    MODIFY_REG(FLASH->OPTR, optr_reg_mask, optr_reg_val);

    /* Set OPTSTRT Bit */
    SET_BIT(FLASH->CR, FLASH_CR_OPTSTRT);

    /* Wait for last operation to be completed */
    status = FLASH_WaitForLastOperation((uint32_t)FLASH_TIMEOUT_VALUE);

    /* If the option byte program operation is completed, disable the OPTSTRT Bit */
    CLEAR_BIT(FLASH->CR, FLASH_CR_OPTSTRT);
  }

  return status;
}

HAL_StatusTypeDef FLASH_OB_PCROPConfig(uint32_t PCROPConfig, uint32_t PCROPStartAddr, uint32_t PCROPEndAddr)
{
  HAL_StatusTypeDef status;
  uint32_t reg_value;
  uint32_t bank1_addr;
  uint32_t bank2_addr;
  /* Check the parameters */
  assert_param(IS_FLASH_BANK_EXCLUSIVE(PCROPConfig & FLASH_BANK_BOTH));
  assert_param(IS_OB_PCROP_RDP(PCROPConfig & FLASH_PCROP1ER_PCROP_RDP));
  assert_param(IS_FLASH_MAIN_MEM_ADDRESS(PCROPStartAddr));
  assert_param(IS_FLASH_MAIN_MEM_ADDRESS(PCROPEndAddr));

  /* Wait for last operation to be completed */
  status = FLASH_WaitForLastOperation((uint32_t)FLASH_TIMEOUT_VALUE);

  if(status == HAL_OK)
  {
    if (READ_BIT(SYSCFG->MEMRMP, SYSCFG_MEMRMP_FB_MODE) == 0U)
    {
      bank1_addr = FLASH_BASE;
      bank2_addr = FLASH_BASE + FLASH_BANK_SIZE;
    }
    else
    {
      bank1_addr = FLASH_BASE + FLASH_BANK_SIZE;
      bank2_addr = FLASH_BASE;
    }
    {
      /* Configure the Proprietary code readout protection */
      if((PCROPConfig & FLASH_BANK_BOTH) == FLASH_BANK_1)
      {
        reg_value = ((PCROPStartAddr - bank1_addr) >> 3);
        MODIFY_REG(FLASH->PCROP1SR, FLASH_PCROP1SR_PCROP1_STRT, reg_value);

        reg_value = ((PCROPEndAddr - bank1_addr) >> 3);
        MODIFY_REG(FLASH->PCROP1ER, FLASH_PCROP1ER_PCROP1_END, reg_value);
      }
      else if((PCROPConfig & FLASH_BANK_BOTH) == FLASH_BANK_2)
      {
        reg_value = ((PCROPStartAddr - bank2_addr) >> 3);
        MODIFY_REG(FLASH->PCROP2SR, FLASH_PCROP2SR_PCROP2_STRT, reg_value);

        reg_value = ((PCROPEndAddr - bank2_addr) >> 3);
        MODIFY_REG(FLASH->PCROP2ER, FLASH_PCROP2ER_PCROP2_END, reg_value);
      }
      else
      {
        /* Nothing to do */
      }
    }

    MODIFY_REG(FLASH->PCROP1ER, FLASH_PCROP1ER_PCROP_RDP, (PCROPConfig & FLASH_PCROP1ER_PCROP_RDP));

    /* Set OPTSTRT Bit */
    SET_BIT(FLASH->CR, FLASH_CR_OPTSTRT);

    /* Wait for last operation to be completed */
    status = FLASH_WaitForLastOperation((uint32_t)FLASH_TIMEOUT_VALUE);

    /* If the option byte program operation is completed, disable the OPTSTRT Bit */
    CLEAR_BIT(FLASH->CR, FLASH_CR_OPTSTRT);
  }

  return status;
}

HAL_StatusTypeDef HAL_FLASHEx_OBProgram(FLASH_OBProgramInitTypeDef *pOBInit)
{
    HAL_StatusTypeDef status = HAL_OK; 
    __HAL_LOCK(&pFlash);
    assert_param(IS_OPTIONBYTE(pOBInit->OptionType));
    pFlash.ErrorCode = HAL_FLASH_ERROR_NONE;
    /* Write protection configuration */
    if((pOBInit->OptionType & OPTIONBYTE_WRP) != 0U) {
        /* Configure of Write protection on the selected area */
        if(FLASH_OB_WRPConfig(pOBInit->WRPArea, pOBInit->WRPStartOffset, pOBInit->WRPEndOffset) != HAL_OK) {
            status = HAL_ERROR;
        }
    }
    if((pOBInit->OptionType & OPTIONBYTE_RDP) != 0U) {
        /* Configure the Read protection level */
        if(FLASH_OB_RDPConfig(pOBInit->RDPLevel) != HAL_OK) {
            status = HAL_ERROR;
        }
    }
    if((pOBInit->OptionType & OPTIONBYTE_USER) != 0U) {
        if(FLASH_OB_UserConfig(pOBInit->USERType, pOBInit->USERConfig) != HAL_OK) {
            status = HAL_ERROR;
        }
    }

    /* PCROP Configuration */
    if((pOBInit->OptionType & OPTIONBYTE_PCROP) != 0U) {
        if (pOBInit->PCROPStartAddr != pOBInit->PCROPEndAddr) {
            /* Configure the Proprietary code readout protection */
            if(FLASH_OB_PCROPConfig(pOBInit->PCROPConfig, pOBInit->PCROPStartAddr, pOBInit->PCROPEndAddr) != HAL_OK) {
                status = HAL_ERROR;
            }
        }
    }
    __HAL_UNLOCK(&pFlash);
    return status;
}

HAL_StatusTypeDef HAL_FLASH_OB_Unlock(void)
{
    if(READ_BIT(FLASH->CR, FLASH_CR_OPTLOCK) != 0U) {
        WRITE_REG(FLASH->OPTKEYR, FLASH_OPTKEY1);
        WRITE_REG(FLASH->OPTKEYR, FLASH_OPTKEY2);
    }
    else {
        return HAL_ERROR;
    }
    return HAL_OK;
}

HAL_StatusTypeDef HAL_FLASH_OB_Launch(void)
{
    SET_BIT(FLASH->CR, FLASH_CR_OBL_LAUNCH);
    return(FLASH_WaitForLastOperation((uint32_t)FLASH_TIMEOUT_VALUE));
}

void FLASH_MassErase(uint32_t Banks)
{
    assert_param(IS_FLASH_BANK(Banks));
    /* Set the Mass Erase Bit for the bank 1 if requested */
    if((Banks & FLASH_BANK_1) != 0U) {
        SET_BIT(FLASH->CR, FLASH_CR_MER1);
    }
    if((Banks & FLASH_BANK_2) != 0U) {
        SET_BIT(FLASH->CR, FLASH_CR_MER2);
    }
    SET_BIT(FLASH->CR, FLASH_CR_STRT);
}

void FLASH_PageErase(uint32_t Page, uint32_t Banks)
{
    /* Check the parameters */
    assert_param(IS_FLASH_PAGE(Page));
    assert_param(IS_FLASH_BANK_EXCLUSIVE(Banks));
    if((Banks & FLASH_BANK_1) != 0U) {
        CLEAR_BIT(FLASH->CR, FLASH_CR_BKER);
    }
    else {
        SET_BIT(FLASH->CR, FLASH_CR_BKER);
    }
    MODIFY_REG(FLASH->CR, FLASH_CR_PNB, ((Page & 0xFFU) << FLASH_CR_PNB_Pos));
    SET_BIT(FLASH->CR, FLASH_CR_PER);
    SET_BIT(FLASH->CR, FLASH_CR_STRT);
}

HAL_StatusTypeDef FLASH_WaitForLastOperation(uint32_t Timeout)
{
    uint32_t tickstart = HAL_GetTick();
    uint32_t error;
    while(__HAL_FLASH_GET_FLAG(FLASH_FLAG_BSY)) {
        if(Timeout != HAL_MAX_DELAY) {
            if((HAL_GetTick() - tickstart) >= Timeout) {
                return HAL_TIMEOUT;
            }
        }
    }
    error = (FLASH->SR & FLASH_FLAG_SR_ERRORS);
    error |= (FLASH->ECCR & FLASH_FLAG_ECCD);
    if(error != 0u) {
        /*Save the error code*/
        pFlash.ErrorCode |= error;
        /* Clear error programming flags */
        __HAL_FLASH_CLEAR_FLAG(error);
        return HAL_ERROR;
    }

    /* Check FLASH End of Operation flag  */
    if (__HAL_FLASH_GET_FLAG(FLASH_FLAG_EOP)) {
        /* Clear FLASH End of Operation pending bit */
        __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_EOP);
    }

    /* If there is an error flag set */
    return HAL_OK;
}

void FLASH_FlushCaches(void)
{
    FLASH_CacheTypeDef cache = pFlash.CacheToReactivate;

    /* Flush instruction cache  */
    if((cache == FLASH_CACHE_ICACHE_ENABLED) ||
        (cache == FLASH_CACHE_ICACHE_DCACHE_ENABLED)) {
        /* Reset instruction cache */
        __HAL_FLASH_INSTRUCTION_CACHE_RESET();
        /* Enable instruction cache */
        __HAL_FLASH_INSTRUCTION_CACHE_ENABLE();
    }

    /* Flush data cache */
    if((cache == FLASH_CACHE_DCACHE_ENABLED) || 
        (cache == FLASH_CACHE_ICACHE_DCACHE_ENABLED)) {
        /* Reset data cache */
        __HAL_FLASH_DATA_CACHE_RESET();
        /* Enable data cache */
        __HAL_FLASH_DATA_CACHE_ENABLE();
    }

    /* Reset internal variable */
    pFlash.CacheToReactivate = FLASH_CACHE_DISABLED;
}

HAL_StatusTypeDef HAL_FLASHEx_Erase(FLASH_EraseInitTypeDef *pEraseInit, uint32_t *PageError)
{
    HAL_StatusTypeDef status;
    uint32_t page_index;
    /* Process Locked */
    __HAL_LOCK(&pFlash);
    /* Check the parameters */
    assert_param(IS_FLASH_TYPEERASE(pEraseInit->TypeErase));
    /* Wait for last operation to be completed */
    status = FLASH_WaitForLastOperation((uint32_t)FLASH_TIMEOUT_VALUE);
    if (status == HAL_OK) {
        pFlash.ErrorCode = HAL_FLASH_ERROR_NONE;

        /* Deactivate the cache if they are activated to avoid data misbehavior */
        if(READ_BIT(FLASH->ACR, FLASH_ACR_ICEN) != 0U) {
            /* Disable instruction cache  */
            __HAL_FLASH_INSTRUCTION_CACHE_DISABLE();
            if(READ_BIT(FLASH->ACR, FLASH_ACR_DCEN) != 0U) {
                /* Disable data cache  */
                __HAL_FLASH_DATA_CACHE_DISABLE();
                pFlash.CacheToReactivate = FLASH_CACHE_ICACHE_DCACHE_ENABLED;
            }
            else {
                pFlash.CacheToReactivate = FLASH_CACHE_ICACHE_ENABLED;
            }
        }
        else if(READ_BIT(FLASH->ACR, FLASH_ACR_DCEN) != 0U) {
            /* Disable data cache  */
            __HAL_FLASH_DATA_CACHE_DISABLE();
            pFlash.CacheToReactivate = FLASH_CACHE_DCACHE_ENABLED;
        }
        else {
            pFlash.CacheToReactivate = FLASH_CACHE_DISABLED;
        }

        if (pEraseInit->TypeErase == FLASH_TYPEERASE_MASSERASE) {
            /* Mass erase to be done */
            FLASH_MassErase(pEraseInit->Banks);
            /* Wait for last operation to be completed */
            status = FLASH_WaitForLastOperation((uint32_t)FLASH_TIMEOUT_VALUE);
            CLEAR_BIT(FLASH->CR, (FLASH_CR_MER1 | FLASH_CR_MER2));
        }
        else {
            /*Initialization of PageError variable*/
            *PageError = 0xFFFFFFFFU;
            for(page_index = pEraseInit->Page; page_index < (pEraseInit->Page + pEraseInit->NbPages); page_index++) {
                FLASH_PageErase(page_index, pEraseInit->Banks);
                /* Wait for last operation to be completed */
                status = FLASH_WaitForLastOperation((uint32_t)FLASH_TIMEOUT_VALUE);

                /* If the erase operation is completed, disable the PER Bit */
                CLEAR_BIT(FLASH->CR, (FLASH_CR_PER | FLASH_CR_PNB));

                if (status != HAL_OK) {
                    /* In case of error, stop erase procedure and return the faulty address */
                    *PageError = page_index;
                    break;
                }
            }
        }
        /* Flush the caches to be sure of the data consistency */
        FLASH_FlushCaches();
    }

    /* Process Unlocked */
    __HAL_UNLOCK(&pFlash);

    return status;
}

void FLASH_Program_DoubleWord(uint32_t Address, uint64_t Data)
{
    /* Check the parameters */
    assert_param(IS_FLASH_PROGRAM_ADDRESS(Address));

    /* Set PG bit */
    SET_BIT(FLASH->CR, FLASH_CR_PG);

    /* Program the double word */
    *(__IO uint32_t*)Address = (uint32_t)Data;
    *(__IO uint32_t*)(Address+4U) = (uint32_t)(Data >> 32);
}

void FLASH_Program_Fast(uint32_t Address, uint32_t DataAddress)
{
    uint8_t row_index = (2*FLASH_NB_DOUBLE_WORDS_IN_ROW);
    __IO uint32_t *dest_addr = (__IO uint32_t*)Address;
    __IO uint32_t *src_addr = (__IO uint32_t*)DataAddress;

    /* Check the parameters */
    assert_param(IS_FLASH_MAIN_MEM_ADDRESS(Address));

    /* Set FSTPG bit */
    SET_BIT(FLASH->CR, FLASH_CR_FSTPG);

    /* Disable interrupts to avoid any interruption during the loop */
    __disable_irq();

    /* Program the double word of the row */
    do
    {
        *dest_addr = *src_addr;
        dest_addr++;
        src_addr++;
        row_index--;
    } while (row_index != 0U);

    /* Re-enable the interrupts */
    __enable_irq();
}

HAL_StatusTypeDef HAL_FLASH_Program(uint32_t TypeProgram, uint32_t Address, uint64_t Data)
{
    HAL_StatusTypeDef status;
    uint32_t prog_bit = 0;

    /* Process Locked */
    __HAL_LOCK(&pFlash);

    /* Check the parameters */
    assert_param(IS_FLASH_TYPEPROGRAM(TypeProgram));

    /* Wait for last operation to be completed */
    status = FLASH_WaitForLastOperation((uint32_t)FLASH_TIMEOUT_VALUE);

    if(status == HAL_OK) {
        pFlash.ErrorCode = HAL_FLASH_ERROR_NONE;

        /* Deactivate the data cache if they are activated to avoid data misbehavior */
        if(READ_BIT(FLASH->ACR, FLASH_ACR_DCEN) != 0U) {
            /* Disable data cache  */
            __HAL_FLASH_DATA_CACHE_DISABLE();
            pFlash.CacheToReactivate = FLASH_CACHE_DCACHE_ENABLED;
        }
        else {
            pFlash.CacheToReactivate = FLASH_CACHE_DISABLED;
        }

        if(TypeProgram == FLASH_TYPEPROGRAM_DOUBLEWORD) {
            /* Program double-word (64-bit) at a specified address */
            FLASH_Program_DoubleWord(Address, Data);
            prog_bit = FLASH_CR_PG;
        }
        else if((TypeProgram == FLASH_TYPEPROGRAM_FAST) || (TypeProgram == FLASH_TYPEPROGRAM_FAST_AND_LAST)) {
            /* Fast program a 32 row double-word (64-bit) at a specified address */
            FLASH_Program_Fast(Address, (uint32_t)Data);

            /* If it is the last row, the bit will be cleared at the end of the operation */
            if(TypeProgram == FLASH_TYPEPROGRAM_FAST_AND_LAST) {
                prog_bit = FLASH_CR_FSTPG;
            }
        }
        else {
            /* Nothing to do */
        }

        /* Wait for last operation to be completed */
        status = FLASH_WaitForLastOperation((uint32_t)FLASH_TIMEOUT_VALUE);

        /* If the program operation is completed, disable the PG or FSTPG Bit */
        if (prog_bit != 0U) {
            CLEAR_BIT(FLASH->CR, prog_bit);
        }

        /* Flush the caches to be sure of the data consistency */
        FLASH_FlushCaches();
    }

    /* Process Unlocked */
    __HAL_UNLOCK(&pFlash);

    return status;
}


#define ROUND_DOWN(a,b) (((a) / (b)) * (b))
#define MIN(a,b)        (((a) < (b)) ? (a) : (b))

#define FLASH_ALIGN_MASK ~(sizeof(uint64_t) - 1)
#define FLASH_ALIGN sizeof(uint64_t)

#define ROUND_MASK(a)        ((a) - 1)
#define ROUND_UP(a)          (((a) + ROUND_MASK(FLASH_ALIGN)) & ~ROUND_MASK(FLASH_ALIGN))

uint32_t boot_flash_get_bank(uint32_t addr)
{
    uint32_t bank = 0;
    if (READ_BIT(SYSCFG->MEMRMP, SYSCFG_MEMRMP_FB_MODE) == 0) {
        bank = (addr < (FLASH_BASE + FLASH_BANK_SIZE)) ? FLASH_BANK_1 : FLASH_BANK_2;
    }
    else {
        bank = (addr < (FLASH_BASE + FLASH_BANK_SIZE)) ? FLASH_BANK_2 : FLASH_BANK_1;
    }

    return bank;
}

uint32_t boot_flash_get_pageinbank(uint32_t addr)
{
    uint32_t page = 0xffffffff;

    if ( ((FLASH_BASE + FLASH_SIZE) > addr) && (addr >= FLASH_BASE) ) {
        if ( addr < (FLASH_BASE + FLASH_BANK_SIZE) ) {
            page = (addr - FLASH_BASE) / FLASH_PAGE_SIZE;
        }
        else {
            page = (addr - FLASH_BASE - FLASH_BANK_SIZE) / FLASH_PAGE_SIZE;
        }
    }
    return page;
}


uint32_t boot_flash_flat_addr(uint32_t addr)
{
    uint32_t flat_addr = 0;

    if (addr < FLASH_BASE || addr >= FLASH_BASE + FLASH_SIZE) {
        return addr;
    }
    if (READ_BIT(SYSCFG->MEMRMP, SYSCFG_MEMRMP_FB_MODE) == 0) {
        flat_addr = addr;
    }
    else {
        flat_addr = addr < (FLASH_BASE + FLASH_BANK_SIZE) ? addr + FLASH_BANK_SIZE : addr - FLASH_BANK_SIZE;
    }
    return flat_addr;
}

static int hal_flash_write_lowlevel(unsigned int start_addr, const void *buf, unsigned int buf_size)
{
    int ret = 0;
    uint64_t* src_data_p;
    unsigned int byte_count = 0;
    uint8_t status = HAL_OK;

    src_data_p = (uint64_t*)buf;
    while(byte_count < buf_size) {
        __disable_irq();
        status = HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, start_addr + byte_count, *src_data_p);
        __enable_irq();
        if(status !=HAL_OK) {
            ret = -1;
            break;
        }
        byte_count += FLASH_ALIGN;
        src_data_p++;
    }
    return ret;
}

int boot_flash_read_at(uint32_t address, uint8_t *pData, uint32_t len_bytes)
{
    uint32_t i;
    int ret = -1;
    uint8_t *src = (uint8_t *)(address);
    uint8_t *dst = (uint8_t *)(pData);

    for (i = 0; i < len_bytes; i++) {
        *(dst++) = *(src++);
    }
    ret = 0;
    return ret;
}


int hal_boot_flash_write(unsigned int addr, const void *buf,  unsigned int buf_size)
{
    int ret = 0;
    unsigned int start_addr;
    unsigned int len_l, len_r, len;
    unsigned char buffer[8] = {0x00};
    if (HAL_FLASH_Unlock() != 0) {
        return -1;
    }
    __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_ALL_ERRORS);

    start_addr = boot_flash_flat_addr(addr);
    len_l = ROUND_UP(start_addr) - start_addr;  /* left unalign size*/
    len_r = (start_addr + buf_size) - ROUND_DOWN((start_addr + buf_size), 8); // right unalign size
    len = buf_size - len_l - len_r; /*align size*/
    if (len_l != 0) {
        ret = boot_flash_read_at(ROUND_DOWN(start_addr, 8), buffer, FLASH_ALIGN);
        memcpy(buffer + FLASH_ALIGN - len_l, buf, len_l);
        ret = hal_flash_write_lowlevel(ROUND_DOWN(start_addr, 8), (uint32_t *)buffer, FLASH_ALIGN);
    }

    if(len != 0) {
        ret = hal_flash_write_lowlevel(start_addr + len_l, (uint32_t *)((uint32_t)buf + len_l), len);
    }

    if (len_r != 0) {
        ret = boot_flash_read_at(ROUND_DOWN(start_addr + buf_size, 8), buffer, FLASH_ALIGN);
        memcpy(buffer, (unsigned char*)buf + len_l + len, len_r);
        ret = hal_flash_write_lowlevel(ROUND_DOWN(start_addr + buf_size, 8), (uint32_t *)buffer, FLASH_ALIGN);
    }

    if (HAL_FLASH_Lock() != 0) {
        ret = -1;
    }
    return ret;
}

int  hal_boot_flash_read(unsigned int  addr, void *buf, unsigned int buf_size)
{
    uint32_t start_addr, len;
    uint64_t *pdata;
    if (buf == NULL) {
        return -1;
    }
    start_addr = boot_flash_flat_addr(addr);
    boot_flash_read_at(start_addr, buf, buf_size);
    return 0;
}

int hal_boot_flash_erase(unsigned int address, unsigned int len_bytes)
{
    int ret = -1;
    uint32_t start_addr;
    FLASH_EraseInitTypeDef EraseInit;
    uint32_t PageError = 0;
    HAL_FLASH_Unlock();
    __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_ALL_ERRORS);

    start_addr = ROUND_DOWN(address, FLASH_PAGE_SIZE);
    start_addr = boot_flash_flat_addr(address);
    EraseInit.TypeErase = FLASH_TYPEERASE_PAGES;
    EraseInit.Banks = boot_flash_get_bank(start_addr);
    if (EraseInit.Banks != boot_flash_get_bank(start_addr + len_bytes - 1)) {
        printf("banks error\r\n");
    } else {
        EraseInit.Page = boot_flash_get_pageinbank(start_addr);
        EraseInit.NbPages = boot_flash_get_pageinbank(start_addr + len_bytes - 1) - EraseInit.Page + 1;
        ret = HAL_FLASHEx_Erase(&EraseInit, &PageError);
        if (ret == HAL_OK) {
            ret = 0;
        }
        else {
            printf("erase error\r\n");
        }
    }
    HAL_FLASH_Lock();
    return ret;
}


