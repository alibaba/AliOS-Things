/*
* Copyright (c) 2016, Freescale Semiconductor, Inc.
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without modification,
* are permitted provided that the following conditions are met:
*
* o Redistributions of source code must retain the above copyright notice, this list
*   of conditions and the following disclaimer.
*
* o Redistributions in binary form must reproduce the above copyright notice, this
*   list of conditions and the following disclaimer in the documentation and/or
*   other materials provided with the distribution.
*
* o Neither the name of Freescale Semiconductor, Inc. nor the names of its
*   contributors may be used to endorse or promote products derived from this
*   software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
* ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
* DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
* ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
* (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
* LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
* ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
* (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
* SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "fsl_phy.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*! @brief Defines the timeout macro. */
#define PHY_TIMEOUT_COUNT 0xFFFFU

/*******************************************************************************
 * Prototypes
 ******************************************************************************/

/*!
 * @brief Get the ENET instance from peripheral base address.
 *
 * @param base ENET peripheral base address.
 * @return ENET instance.
 */
extern uint32_t ENET_GetInstance(ENET_Type *base);

/*******************************************************************************
 * Variables
 ******************************************************************************/

#if !(defined(FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL) && FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL)
/*! @brief Pointers to enet clocks for each instance. */
extern clock_ip_name_t s_enetClock[FSL_FEATURE_SOC_LPC_ENET_COUNT];
#endif /* FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL */

/*******************************************************************************
 * Code
 ******************************************************************************/

status_t PHY_Init(ENET_Type *base, uint32_t phyAddr)
{
    uint16_t reg;
    uint32_t delay = PHY_TIMEOUT_COUNT;
    uint32_t instance = ENET_GetInstance(base);

#if !(defined(FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL) && FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL)
    /* Set SMI first. */
    CLOCK_EnableClock(s_enetClock[instance]);
#endif /* FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL */

    ENET_SetSMI(base);

    /* Reset PHY and wait until completion. */
    PHY_Write(base, phyAddr, PHY_BASICCONTROL_REG, PHY_BCTL_RESET_MASK);
    do
    {
        reg = PHY_Read(base, phyAddr, PHY_BASICCONTROL_REG);
    } while (delay-- && reg & PHY_BCTL_RESET_MASK);

    if (!delay)
    {
        return kStatus_Fail;
    }

    /* Set the ability. */
    PHY_Write(base, phyAddr, PHY_AUTONEG_ADVERTISE_REG, (PHY_ALL_CAPABLE_MASK | 0x1U));

    /* Start Auto negotiation and wait until auto negotiation completion */
    PHY_Write(base, phyAddr, PHY_BASICCONTROL_REG, (PHY_BCTL_AUTONEG_MASK | PHY_BCTL_RESTART_AUTONEG_MASK));
    delay = PHY_TIMEOUT_COUNT;
    do
    {
        reg = PHY_Read(base, phyAddr, PHY_SEPCIAL_CONTROL_REG);
        delay --;
    } while (delay && ((reg & PHY_SPECIALCTL_AUTONEGDONE_MASK) == 0));

    if (!delay)
    {
        return kStatus_Fail;
    }

    return kStatus_Success;
}

void PHY_Write(ENET_Type *base, uint32_t phyAddr, uint32_t phyReg, uint32_t data)
{
    ENET_StartSMIWrite(base, phyAddr, phyReg, data);
    while (ENET_IsSMIBusy(base))
        ;
}

uint16_t PHY_Read(ENET_Type *base, uint32_t phyAddr, uint32_t phyReg)
{
    ENET_StartSMIRead(base, phyAddr, phyReg);
    while (ENET_IsSMIBusy(base))
        ;
    return ENET_ReadSMIData(base);
}

bool PHY_GetLinkStatus(ENET_Type *base, uint32_t phyAddr)
{
    uint16_t reg;

    /* Read the basic status register. */
    reg = PHY_Read(base, phyAddr, PHY_BASICSTATUS_REG);
    if (reg & PHY_BSTATUS_LINKSTATUS_MASK)
    {
        /* link up. */
        return true;
    }
    else
    {
        return false;
    }
}

void PHY_GetLinkSpeedDuplex(ENET_Type *base, uint32_t phyAddr, phy_speed_t *speed, phy_duplex_t *duplex)
{
    assert(duplex);
    assert(speed);

    uint32_t reg;

    /* Read the control two register. */
    reg = PHY_Read(base, phyAddr, PHY_SEPCIAL_CONTROL_REG);

    if (reg & PHY_SPECIALCTL_DUPLEX_MASK)
    {
        /* Full duplex. */
        *duplex = kPHY_FullDuplex;
    }
    else
    {
        /* Half duplex. */
        *duplex = kPHY_HalfDuplex;
    }

    if (reg & PHY_SPECIALCTL_100SPEED_MASK)
    {
        /* 100M speed. */
        *speed = kPHY_Speed100M;
    }
    else
    { /* 10M speed. */
        *speed = kPHY_Speed10M;
    }
}
