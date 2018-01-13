/*****************************************************************************
**
**  Name    udac.h
**
**  Description
**      Definitions for UDAC driver
**
*****************************************************************************/
#ifndef UDAC_H
#define UDAC_H


#define UDAC_GAIN_MAX     0x00FFF
typedef UINT16 tUDAC_GAIN;

/* API functions for DAC driver */


/*****************************************************************************
**
** Function         DAC_Init
**
** Description
**      Initialize the DAC subsystem
**
** Input parameters
**      Nothing
**
** Output parameters
**      Nothing
**
** Returns
**      Nothing
**
*****************************************************************************/
void UDAC_Init(void *p_cfg);


/*****************************************************************************
**
** Function         DAC_Read
**
** Description
**      Read current DAC gain
**
** Input parameters
**      Nothing
**
** Output parameters
**      Nothing
**
** Returns
**      Current gain setting
**
*****************************************************************************/
tUDAC_GAIN UDAC_Read(void);


/*****************************************************************************
**
** Function         DAC_Set
**
** Description
**      Set the DAC gain
**
** Input parameters
**      gain        Gain setting
**
** Output parameters
**      Nothing
**
** Returns
**      Nothing
**
*****************************************************************************/
void UDAC_Set(tUDAC_GAIN gain);

#endif /* #ifndef UDAC_H */
