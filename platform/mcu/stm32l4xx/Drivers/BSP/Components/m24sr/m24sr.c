/**
  ******************************************************************************
  * @file    m24sr.c
  * @author  MCD Application Team
  * @version V1.1.0
  * @date    21-April-2017
  * @brief   This file provides a set of functions to interface with the M24SR
  *          device.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2017 STMicroelectronics International N.V.
  * All rights reserved.</center></h2>
  *
  * Redistribution and use in source and binary forms, with or without
  * modification, are permitted, provided that the following conditions are met:
  *
  * 1. Redistribution of source code must retain the above copyright notice,
  *    this list of conditions and the following disclaimer.
  * 2. Redistributions in binary form must reproduce the above copyright notice,
  *    this list of conditions and the following disclaimer in the documentation
  *    and/or other materials provided with the distribution.
  * 3. Neither the name of STMicroelectronics nor the names of other
  *    contributors to this software may be used to endorse or promote products
  *    derived from this software without specific written permission.
  * 4. This software, including modifications and/or derivative works of this
  *    software, must execute solely and exclusively on microcontroller or
  *    microprocessor devices manufactured by or for STMicroelectronics.
  * 5. Redistribution and use of this software other than as permitted under
  *    this license is void and will automatically terminate your rights under
  *    this license.
  *
  * THIS SOFTWARE IS PROVIDED BY STMICROELECTRONICS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS, IMPLIED OR STATUTORY WARRANTIES, INCLUDING, BUT NOT
  * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
  * PARTICULAR PURPOSE AND NON-INFRINGEMENT OF THIRD PARTY INTELLECTUAL PROPERTY
  * RIGHTS ARE DISCLAIMED TO THE FULLEST EXTENT PERMITTED BY LAW. IN NO EVENT
  * SHALL STMICROELECTRONICS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
  * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
  * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
  * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
  * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
  * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
  * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "string.h"
#include "m24sr.h"

/** @addtogroup BSP
  * @{
  */
  
/** @addtogroup Components
  * @{
  */ 

/** @addtogroup M24SR
  * @{
  *  @brief  <b>This folder contains the driver layer of M24SR family (M24SR64, M24SR16, M24SR04, M24SR02)</b> 
  */

/** @defgroup M24SR_Private_Types  M24SR Driver Private Types
  * @{
  */

/**
  * @brief  Synchronization Mechanism structure 
  */
typedef enum{
  M24SR_WAITINGTIME_UNKNOWN= 0,
  M24SR_WAITINGTIME_POLLING,
  M24SR_WAITINGTIME_TIMEOUT,
  M24SR_WAITINGTIME_GPO,
  M24SR_INTERRUPT_GPO
}M24SR_WAITINGTIME_MGMT;   

/**
  * @brief  APDU-Header command structure
  */
typedef struct
{
  uint8_t CLA;  /* Command class */
  uint8_t INS;  /* Operation code */
  uint8_t P1;   /* Selection Mode */
  uint8_t P2;   /* Selection Option */
} C_APDU_Header;

/**
  * @brief  APDU-Body command structure
  */
typedef struct 
{
  uint8_t LC;                  /* Data field length */  
  uint8_t *pData ;             /* Command parameters */ 
  uint8_t LE;                  /* Expected length of data to be returned */
} C_APDU_Body;

/**
  * @brief  APDU Command structure 
  */
typedef struct
{
  C_APDU_Header Header;
  C_APDU_Body Body;
} C_APDU;

/**
  * @brief  SC response structure
  */
typedef struct
{
  uint8_t *pData ;  /* Data returned from the card */ 
                    /* pointer on the transceiver buffer = ReaderRecBuf[CR95HF_DATA_OFFSET ]; */
  uint8_t SW1;      /* Command Processing status */
  uint8_t SW2;      /* Command Processing qualification */
} R_APDU;

/**
  * @brief  GPO state structure 
  */
typedef enum{
  HIGH_IMPEDANCE= 0,
  SESSION_OPENED,
  WIP,
  I2C_ANSWER_READY,
  INTERRUPT,
  STATE_CONTROL
}M24SR_GPO_MGMT;

/**
  * @}
  */


/** @defgroup M24SR_Private_Constants  M24SR Driver Private Constants
  * @{
  */

/**
  * @brief  M24SR_Private_Code_Status 
  */
#define UB_STATUS_OFFSET                    4
#define LB_STATUS_OFFSET                    3

#define M24SR_NBBYTE_INVALID                0xFFFE

/**
  * @brief  M24SR_Private_File_Identifier
  */
#define SYSTEM_FILE_ID          0xE101   
#define CC_FILE_ID              0xE103
#define NDEF_FILE_ID            0x0001   
  
/**
  * @brief  M24SR_Private_Command_Management
  */  
/* special M24SR command ----------------------------------------------------------------------*/   
#define M24SR_OPENSESSION      0x26
#define M24SR_KILLSESSION      0x52

/* APDU Command: class list -------------------------------------------*/
#define C_APDU_CLA_DEFAULT     0x00
#define C_APDU_CLA_ST          0xA2

/*------------------------ Data Area Management Commands ---------------------*/
#define C_APDU_SELECT_FILE     0xA4
#define C_APDU_GET_RESPONCE    0xC0
#define C_APDU_STATUS          0xF2
#define C_APDU_UPDATE_BINARY   0xD6
#define C_APDU_READ_BINARY     0xB0
#define C_APDU_WRITE_BINARY    0xD0
#define C_APDU_UPDATE_RECORD   0xDC
#define C_APDU_READ_RECORD     0xB2

/*-------------------------- Safety Management Commands ----------------------*/
#define C_APDU_VERIFY          0x20
#define C_APDU_CHANGE          0x24
#define C_APDU_DISABLE         0x26
#define C_APDU_ENABLE          0x28

/*-------------------------- Gpio Management Commands ------------------------*/
#define C_APDU_INTERRUPT       0xD6

/*  Length  ----------------------------------------------------------------------------------*/
#define M24SR_STATUS_NBBYTE                       2
#define M24SR_CRC_NBBYTE                          2
#define M24SR_STATUSRESPONSE_NBBYTE               5
#define M24SR_DESELECTREQUEST_NBBYTE              3
#define M24SR_DESELECTRESPONSE_NBBYTE             3
#define M24SR_WATINGTIMEEXTRESPONSE_NBBYTE        4
#define M24SR_PASSWORD_NBBYTE                  0x10

/*  Command structure  ------------------------------------------------------------------------*/
#define M24SR_CMDSTRUCT_SELECTAPPLICATION         0x01FF
#define M24SR_CMDSTRUCT_SELECTCCFILE              0x017F
#define M24SR_CMDSTRUCT_SELECTNDEFFILE            0x017F
#define M24SR_CMDSTRUCT_READBINARY                0x019F
#define M24SR_CMDSTRUCT_UPDATEBINARY              0x017F
#define M24SR_CMDSTRUCT_VERIFYBINARYWOPWD         0x013F
#define M24SR_CMDSTRUCT_VERIFYBINARYWITHPWD       0x017F
#define M24SR_CMDSTRUCT_CHANGEREFDATA             0x017F
#define M24SR_CMDSTRUCT_ENABLEVERIFREQ            0x011F
#define M24SR_CMDSTRUCT_DISABLEVERIFREQ           0x011F
#define M24SR_CMDSTRUCT_SENDINTERRUPT             0x013F
#define M24SR_CMDSTRUCT_GPOSTATE                  0x017F

/*  Command structure Mask -------------------------------------------------------------------*/
#define M24SR_PCB_NEEDED        0x0001    /* PCB byte present or not */
#define M24SR_CLA_NEEDED        0x0002     /* CLA byte present or not */
#define M24SR_INS_NEEDED        0x0004     /* Operation code present or not*/ 
#define M24SR_P1_NEEDED         0x0008    /* Selection Mode  present or not*/
#define M24SR_P2_NEEDED         0x0010    /* Selection Option present or not*/
#define M24SR_LC_NEEDED         0x0020    /* Data field length byte present or not */
#define M24SR_DATA_NEEDED       0x0040    /* Data present or not */
#define M24SR_LE_NEEDED         0x0080    /* Expected length present or not */
#define M24SR_CRC_NEEDED        0x0100    /* 2 CRC bytes present  or not */

#define M24SR_DID_NEEDED        0x08      /* DID byte present or not */

/**
  * @brief  M24SR_Private_Offset_and_masks
  */

/*  Offset  ----------------------------------------------------------------------------------*/
#define M24SR_OFFSET_PCB                          0
#define M24SR_OFFSET_CLASS                        1
#define M24SR_OFFSET_INS                          2
#define M24SR_OFFSET_P1                           3

/*  mask  ------------------------------------------------------------------------------------*/
#define M24SR_MASK_BLOCK                          0xC0
#define M24SR_MASK_IBLOCK                         0x00
#define M24SR_MASK_RBLOCK                         0x80
#define M24SR_MASK_SBLOCK                         0xC0

/**
  * @}
  */


/** @defgroup M24SR_Private_Variables   M24SR Private Global Variables
  * @{
  */

static C_APDU               Command;
static uint8_t              DataBuffer[0xFF];
uint8_t                     uM24SRbuffer [0xFF];
static uint8_t              uDIDbyte =0x00;

uint8_t uSynchroMode = M24SR_WAITINGTIME_POLLING;
uint8_t uGpoMode     = M24SR_GPO_POLLING;
volatile uint8_t    GPO_Low = 0;

/**
  * @}
  */

/** @defgroup M24SR_Private_Macros   M24SR Private Macros
  * @{
  */  

/** @brief Get Most Significant Byte
  * @param  val: number where MSB must be extracted
  * @retval MSB
  */ 
#define GETMSB(val)     ( (uint8_t) ((val & 0xFF00 )>>8) ) 

/** @brief Get Least Significant Byte
  * @param  val: number where LSB must be extracted
  * @retval LSB
  */ 
#define GETLSB(val)     ( (uint8_t) (val & 0x00FF )) 

/** @brief Used to toggle the block number by adding 0 or 1 to default block number value
  * @param  val: number to know if incrementation is needed
  * @retval  0 or 1 if incrementation needed
  */
#define TOGGLE(val)     ((val != 0x00)? 0x00 : 0x01)

/**
  * @}
  */  
  

/** @defgroup M24SR_Private_FunctionsPrototypes   M24SR Private FunctionsPrototypes
  * @{
  */
  
static uint16_t M24SR_UpdateCrc             ( uint8_t ch, uint16_t *lpwCrc);
static uint16_t M24SR_ComputeCrc            ( uint8_t *Data, uint8_t Length);
static uint16_t M24SR_IsCorrectCRC16Residue ( uint8_t *DataIn,uint8_t Length);
static void M24SR_BuildIBlockCommand        ( uint16_t CommandStructure, C_APDU Command, uint16_t *NbByte , uint8_t *pCommand);
static uint16_t IsSBlock                    ( uint8_t *pBuffer);
static uint16_t M24SR_FWTExtension          ( uint16_t DeviceAddr, uint8_t FWTbyte);
static void     M24SR_SetI2CSynchroMode     ( uint8_t WaitingMode );
static uint16_t M24SR_IsAnswerReady         ( uint16_t DeviceAddr);
/**
  * @}
  */

  
/** @defgroup M24SR_Private_Functions   M24SR Private Functions
  * @{
  */ 
  
  /**
  * @brief  This function updates the CRC 
  * @param  None
  * @retval None
  */
static uint16_t M24SR_UpdateCrc (uint8_t ch, uint16_t *lpwCrc)
{
  ch = (ch^(uint8_t)((*lpwCrc) & 0x00FF));
  ch = (ch^(ch<<4));
  *lpwCrc = (*lpwCrc >> 8)^((uint16_t)ch << 8)^((uint16_t)ch<<3)^((uint16_t)ch>>4);
  
  return(*lpwCrc);
}

/**
  * @brief  This function returns the CRC 16 
  * @param  Data : pointer on the data used to compute the CRC16
  * @param  Length : number of byte of the data
  * @retval CRC16 
  */
static uint16_t M24SR_ComputeCrc(uint8_t *Data, uint8_t Length)
{
  uint8_t chBlock;
  uint16_t wCrc;
  
  wCrc = 0x6363; /* ITU-V.41 */
  
  do {
    chBlock = *Data++;
    M24SR_UpdateCrc(chBlock, &wCrc);
  } while (--Length);
  
  return wCrc ;
}


/**  
* @brief    This function computes the CRC16 residue as defined by CRC ISO/IEC 13239
* @param    DataIn    :  input to data 
* @param    Length     :   Number of bits of DataIn
* @retval   Status (SW1&SW2)    :   CRC16 residue is correct  
* @retval   M24SR_ERROR_CRC    :  CRC16 residue is false
*/
static uint16_t M24SR_IsCorrectCRC16Residue (uint8_t *DataIn,uint8_t Length)
{
  uint16_t ResCRC=0;
  
  /* check the CRC16 Residue */
  if (Length !=0)
    ResCRC= M24SR_ComputeCrc (DataIn, Length);
  
  if ( ResCRC == 0x0000)
  {
    /* Good CRC, but error status from M24SR */
    return( ((DataIn[Length-UB_STATUS_OFFSET]<<8) & 0xFF00) | (DataIn[Length-LB_STATUS_OFFSET] & 0x00FF) ); 
  }
  else
  {
    ResCRC=0;
    ResCRC= M24SR_ComputeCrc (DataIn, 5);
    if ( ResCRC != 0x0000)
    {
      /* Bad CRC */
      return M24SR_ERROR_CRC;
    }
    else
    {
      /* Good CRC, but error status from M24SR */
      return( ((DataIn[1]<<8) & 0xFF00) | (DataIn[2] & 0x00FF) ); 
    }
  }  
}


/**
  * @brief     This functions creates an I block command according to the structures CommandStructure and Command. 
  * @param     Command : structue which contains the field of the different parameter
  * @param     CommandStructure : structure that contain the structure of the command (if the different field are presnet or not 
  * @param     NbByte : number of byte of the command
  * @param     pCommand : pointer of the command created
  */
static void M24SR_BuildIBlockCommand ( uint16_t CommandStructure, C_APDU Command, uint16_t *NbByte , uint8_t *pCommand)
{
  uint16_t  uCRC16; 
  static uint8_t BlockNumber = 0x01;
  
  (*NbByte) = 0;
  
  /* add the PCD byte */
  if ((CommandStructure & M24SR_PCB_NEEDED) !=0)
  {
    /* toggle the block number */
    BlockNumber = TOGGLE ( BlockNumber );
    /* Add the I block byte */
    pCommand[(*NbByte)++] = 0x02 |  BlockNumber; 
  }
  
  /* add the DID byte */
  if ((BlockNumber & M24SR_DID_NEEDED) !=0)
  {
    /* Add the I block byte */
    pCommand[(*NbByte)++] = uDIDbyte; 
  }
  
  /* add the Class byte */
  if ((CommandStructure & M24SR_CLA_NEEDED) !=0)
  {
    pCommand[(*NbByte)++] = Command.Header.CLA ;
  }
  /* add the instruction byte byte */
  if ( (CommandStructure & M24SR_INS_NEEDED) !=0)
  {
    pCommand[(*NbByte)++] = Command.Header.INS ;
  }
  /* add the Selection Mode byte */
  if ((CommandStructure & M24SR_P1_NEEDED) !=0)
  {
    pCommand[(*NbByte)++] = Command.Header.P1 ;
  }
  /* add the Selection Mode byte */
  if ((CommandStructure & M24SR_P2_NEEDED) !=0)
  {
    pCommand[(*NbByte)++] = Command.Header.P2 ;
  }
  /* add Data field lengthbyte */
  if ((CommandStructure & M24SR_LC_NEEDED) !=0)
  {
    pCommand[(*NbByte)++] = Command.Body.LC ;
  }
  /* add Data field  */
  if ((CommandStructure & M24SR_DATA_NEEDED) !=0)
  {
    memcpy(&(pCommand[(*NbByte)]) ,Command.Body.pData,Command.Body.LC ) ;
    (*NbByte) += Command.Body.LC ;
  }
  /* add Le field  */
  if ((CommandStructure & M24SR_LE_NEEDED) !=0)
  {
    pCommand[(*NbByte)++] = Command.Body.LE ;
  }
  /* add CRC field  */
  if ((CommandStructure & M24SR_CRC_NEEDED) !=0)
  {
    uCRC16 = M24SR_ComputeCrc (pCommand,(uint8_t) (*NbByte));
    /* append the CRC16 */
    pCommand [(*NbByte)++] = GETLSB  (uCRC16 ) ;
    pCommand [(*NbByte)++] = GETMSB  (uCRC16 ) ;  
  } 
}

  
/**  
* @brief    This function return M24SR_STATUS_SUCCESS if the pBuffer is an s-block
* @param    pBuffer    :  pointer of the data
* @retval   M24SR_STATUS_SUCCESS  :  the data is a S-Block
* @retval   M24SR_ERROR_DEFAULT    :  the data is not a S-Block
*/
static uint16_t IsSBlock (uint8_t *pBuffer)
{
  
  if ((pBuffer[M24SR_OFFSET_PCB] & M24SR_MASK_BLOCK) == M24SR_MASK_SBLOCK)
  {
    return M24SR_STATUS_SUCCESS;
  }
  else 
  {  
    return M24SR_ERROR_DEFAULT;
  }
  
}

/**
  * @brief  This function sends the FWT extension command (S-Block format)
  * @param  DeviceAddr: I2C address of the device
  * @param  FWTbyte : FWT value
  * @retval Status (SW1&SW2) : Status of the operation to complete.
  * @retval M24SR_ERROR_TIMEOUT : The I2C timeout occured.
  */
static uint16_t M24SR_FWTExtension (uint16_t DeviceAddr, uint8_t FWTbyte)
{
  uint8_t   pBuffer[M24SR_STATUSRESPONSE_NBBYTE];
  uint16_t  status ; 
  uint16_t  NthByte = 0,
  uCRC16;
  
  /* create the response */
  pBuffer[NthByte++] = 0xF2 ;  
  pBuffer[NthByte++] = FWTbyte ;
  /* compute the CRC */
  uCRC16 = M24SR_ComputeCrc (pBuffer,0x02);
  /* append the CRC16 */
  pBuffer [NthByte++] = GETLSB  (uCRC16 ) ;
  pBuffer [NthByte++]=   GETMSB  (uCRC16 ) ;  
  
  /* send the request */ 
  status = NFC_IO_WriteMultiple(DeviceAddr, pBuffer, NthByte);
  if (status != NFC_IO_STATUS_SUCCESS)
  {
    return M24SR_ERROR_TIMEOUT;
  } 
  status = M24SR_IsAnswerReady (DeviceAddr);
  if (status != M24SR_STATUS_SUCCESS)
  {
    return status;
  } 
  /* read the response */ 
  if ( NFC_IO_ReadMultiple (DeviceAddr , pBuffer,  M24SR_STATUSRESPONSE_NBBYTE) != NFC_IO_STATUS_SUCCESS)
  {
    return M24SR_ERROR_TIMEOUT;
  } 
  
  status = M24SR_IsCorrectCRC16Residue (pBuffer, M24SR_STATUSRESPONSE_NBBYTE); 
  return status;
}

/**
  * @}
  */


/** @defgroup M24SR_Exported_Functions   M24SR Exported Functions
  * @{
  */ 

/**
  * @brief  This function initialize the M24SR device
  * @param  DeviceAddr: I2C address of the device
  * @param  GpoMode: M24SR_GPO_POLLING / M24SR_GPO_SYNCHRO / M24SR_GPO_INTERRUPT 
  * @retval None 
  */
void M24SR_Init(uint16_t DeviceAddr, uint8_t GpoMode)
{    
  uGpoMode = GpoMode;  /* Global variable initialization */

  if(uGpoMode == M24SR_GPO_INTERRUPT)
  {  
     NFC_IO_Init(0x1);
  }
  else
  {
     NFC_IO_Init(0x0);
  } 

  /* build the command */
  Command.Header.CLA = 0x00;
  Command.Header.INS = 0x00;
  /* copy the offset */
  Command.Header.P1 = 0x00 ;
  Command.Header.P2 = 0x00 ;
  /* copy the number of byte of the data field */
  Command.Body.LC = 0x00 ;
  /* copy the number of byte to read */
  Command.Body.LE = 0x00 ;
  Command.Body.pData = DataBuffer; 
  
  if((uGpoMode == M24SR_GPO_SYNCHRO) || (uGpoMode == M24SR_GPO_INTERRUPT))
  {
    if( M24SR_KillSession(DeviceAddr) == M24SR_ACTION_COMPLETED)
    {
      M24SR_ManageI2CGPO(DeviceAddr, I2C_ANSWER_READY);
      M24SR_Deselect (DeviceAddr);
    } 
  }
}

/**
  * @brief  This function initialize the M24SR device
  * @retval None 
  */
void M24SR_GPO_Callback( void )
{    
  if( uSynchroMode == M24SR_INTERRUPT_GPO)
  {
    GPO_Low = 1;
  }
}

/**
  * @brief  This function sends the GetSession command to the M24SR device
  * @param  DeviceAddr: I2C address of the device
  * @retval M24SR_ACTION_COMPLETED : the function is succesful.
  * @retval Status (SW1&SW2) : if operation does not complete.
  */
uint16_t M24SR_GetSession (uint16_t DeviceAddr)
{
  uint8_t Buffer = M24SR_OPENSESSION;
  
  if (NFC_IO_WriteMultiple(DeviceAddr, &Buffer, 0x01 ) != NFC_IO_STATUS_SUCCESS)
  {
    return M24SR_ERROR_TIMEOUT;
  }  
  /* Insure no access will be done just after open session */  
  /* The only way here is to poll I2C to know when M24SR is ready */
  /* GPO can not be use with GetSession command */
  if (NFC_IO_IsDeviceReady(DeviceAddr, NFC_IO_TRIALS) != NFC_IO_STATUS_SUCCESS)
  {
    return M24SR_ERROR_TIMEOUT;
  } 
  
  return M24SR_ACTION_COMPLETED;
}


/**
  * @brief  This function sends the KillSession command to the M24SR device
  * @param  DeviceAddr: I2C address of the device
  * @retval M24SR_ACTION_COMPLETED : the function is succesful. 
  * @retval M24SR_ERROR_TIMEOUT : The I2C timeout occured.
  */
uint16_t M24SR_KillSession (uint16_t DeviceAddr)
{
  uint8_t pBuffer[] = {M24SR_KILLSESSION};
  
  if (NFC_IO_WriteMultiple(DeviceAddr, pBuffer, 0x01) != NFC_IO_STATUS_SUCCESS)
  {
    return M24SR_ERROR_TIMEOUT;
  }   
  /* Insure no access will be done just after open session */  
  /* The only way here is to poll I2C to know when M24SR is ready */
  /* GPO can not be use with KillSession command */ 
  if (NFC_IO_IsDeviceReady(DeviceAddr, NFC_IO_TRIALS) != NFC_IO_STATUS_SUCCESS)
  {
    return M24SR_ERROR_TIMEOUT;
  }    
  return M24SR_ACTION_COMPLETED;
}


/**
  * @brief  This function sends the Deselect command (S-Block format)
  * @param  DeviceAddr: I2C address of the device
  * @retval M24SR_ACTION_COMPLETED : the function is succesful. 
  * @retval M24SR_ERROR_TIMEOUT : The I2C timeout occured. 
  */
uint16_t M24SR_Deselect (uint16_t DeviceAddr)
{
  uint8_t   pBuffer[] = {0xC2,0xE0,0xB4} ;
  uint16_t  status ; 
  
  /* send the request */ 
  if (NFC_IO_WriteMultiple(DeviceAddr, pBuffer, M24SR_DESELECTREQUEST_NBBYTE) != NFC_IO_STATUS_SUCCESS)
  {
    return M24SR_ERROR_TIMEOUT;
  }    
  status = M24SR_IsAnswerReady (DeviceAddr);
  if (status != M24SR_STATUS_SUCCESS)
  {
    return status;
  }  
  /* flush the M24SR buffer */ 
  if (NFC_IO_ReadMultiple (DeviceAddr , pBuffer,  M24SR_DESELECTREQUEST_NBBYTE) != NFC_IO_STATUS_SUCCESS)
  {
    return M24SR_ERROR_TIMEOUT;
  }  
  
  return M24SR_ACTION_COMPLETED;
}


/**
  * @brief  This function sends the SelectApplication command
  * @param  DeviceAddr: I2C address of the device
  * @retval M24SR_ACTION_COMPLETED : the function is succesful. 
  * @retval M24SR_ERROR_TIMEOUT : The I2C timeout occured.
  */
uint16_t M24SR_SelectApplication (uint16_t DeviceAddr)
{
  uint8_t   *pBuffer = uM24SRbuffer ,
  NbByteToRead = M24SR_STATUSRESPONSE_NBBYTE;
  uint8_t    uLc = 0x07,
  pData[] = {0xD2,0x76,0x00,0x00,0x85,0x01,0x01},
  uLe = 0x00;
  uint16_t  status ; 
  uint16_t  uP1P2 =0x0400,
  NbByte;
  
  /* build the command */
  Command.Header.CLA = C_APDU_CLA_DEFAULT;
  Command.Header.INS = C_APDU_SELECT_FILE;
  /* copy the offset */
  Command.Header.P1 = GETMSB  (uP1P2 ) ;
  Command.Header.P2 = GETLSB  (uP1P2 ) ;
  /* copy the number of byte of the data field */
  Command.Body.LC = uLc ;
  /* copy the data */
  memcpy(Command.Body.pData, pData, uLc);
  /* copy the number of byte to read */
  Command.Body.LE = uLe ;
  /* build the I²C command */
  M24SR_BuildIBlockCommand ( M24SR_CMDSTRUCT_SELECTAPPLICATION,  Command, &NbByte , pBuffer);
  
  /* send the request */ 
  if (NFC_IO_WriteMultiple(DeviceAddr, pBuffer, NbByte) != NFC_IO_STATUS_SUCCESS)
  {
    return M24SR_ERROR_TIMEOUT;
  } 
  status = M24SR_IsAnswerReady (DeviceAddr);
  if (status != M24SR_STATUS_SUCCESS)
  {
    return status;
  }  
  /* read the response */ 
  if (NFC_IO_ReadMultiple (DeviceAddr , pBuffer,  NbByteToRead) != NFC_IO_STATUS_SUCCESS)
  {
    return M24SR_ERROR_TIMEOUT;
  }   
  status = M24SR_IsCorrectCRC16Residue (pBuffer,NbByteToRead); 
  return status;
}


/**
  * @brief  This function sends the SelectCCFile command
  * @param  DeviceAddr: I2C address of the device
  * @retval M24SR_ACTION_COMPLETED : the function is succesful. 
  * @retval M24SR_ERROR_TIMEOUT : The I2C timeout occured.
  * @retval Status (SW1&SW2) : if operation does not complete for another reason.
  */
uint16_t M24SR_SelectCCfile (uint16_t DeviceAddr)
{
  uint8_t   *pBuffer = uM24SRbuffer ,
  NbByteToRead = M24SR_STATUSRESPONSE_NBBYTE;
  uint8_t    uLc = 0x02;
  uint16_t  status ; 
  uint16_t  uP1P2 =0x000C,
  uNbFileId =CC_FILE_ID,
  NbByte;
  
  /* build the command */
  Command.Header.CLA = C_APDU_CLA_DEFAULT;
  Command.Header.INS = C_APDU_SELECT_FILE;
  /* copy the offset */
  Command.Header.P1 = GETMSB  (uP1P2 ) ;
  Command.Header.P2 = GETLSB  (uP1P2 ) ;
  /* copy the number of byte of the data field */
  Command.Body.LC = uLc ;
  /* copy the File Id */
  Command.Body.pData[0] = GETMSB  (uNbFileId ) ;
  Command.Body.pData[1] = GETLSB  (uNbFileId ) ;
  /* build the I²C command */
  M24SR_BuildIBlockCommand ( M24SR_CMDSTRUCT_SELECTCCFILE,  Command, &NbByte , pBuffer);
  
  /* send the request */ 
  if (NFC_IO_WriteMultiple(DeviceAddr, pBuffer, NbByte) != NFC_IO_STATUS_SUCCESS)
  {
    return M24SR_ERROR_TIMEOUT;
  } 
  status = M24SR_IsAnswerReady (DeviceAddr);
  if (status != M24SR_STATUS_SUCCESS)
  {
    return status;
  } 
  /* read the response */ 
  if (NFC_IO_ReadMultiple (DeviceAddr , pBuffer,  NbByteToRead) != NFC_IO_STATUS_SUCCESS)
  {
    return M24SR_ERROR_TIMEOUT;
  } 
  
  status = M24SR_IsCorrectCRC16Residue (pBuffer,NbByteToRead); 
  return status; 
}


/**
  * @brief  This function sends the SelectSystemFile command
  * @param  DeviceAddr: I2C address of the device
  * @retval Status (SW1&SW2) : Status of the operation to complete.
  * @retval M24SR_ERROR_TIMEOUT : The I2C timeout occured.
  */
uint16_t M24SR_SelectSystemfile (uint16_t DeviceAddr)
{
  uint8_t   *pBuffer = uM24SRbuffer ,
  NbByteToRead = M24SR_STATUSRESPONSE_NBBYTE;
  uint8_t    uLc = 0x02;
  uint16_t  status ; 
  uint16_t  uP1P2 =0x000C,
  uNbFileId =SYSTEM_FILE_ID,
  NbByte;
  
  /* build the command */
  Command.Header.CLA = C_APDU_CLA_DEFAULT;
  Command.Header.INS = C_APDU_SELECT_FILE;
  /* copy the offset */
  Command.Header.P1 = GETMSB  (uP1P2 ) ;
  Command.Header.P2 = GETLSB  (uP1P2 ) ;
  /* copy the number of byte of the data field */
  Command.Body.LC = uLc ;
  /* copy the File Id */
  Command.Body.pData[0] = GETMSB  (uNbFileId ) ;
  Command.Body.pData[1] = GETLSB  (uNbFileId ) ;
  /* build the I²C command */
  M24SR_BuildIBlockCommand ( M24SR_CMDSTRUCT_SELECTCCFILE,  Command, &NbByte , pBuffer);
  
  /* send the request */ 
  if (NFC_IO_WriteMultiple(DeviceAddr, pBuffer, NbByte) != NFC_IO_STATUS_SUCCESS)
  {
    return M24SR_ERROR_TIMEOUT;
  } 
  status = M24SR_IsAnswerReady (DeviceAddr);
  if (status != M24SR_STATUS_SUCCESS)
  {
    return status;
  } 
  /* read the response */ 
  if (NFC_IO_ReadMultiple (DeviceAddr , pBuffer,  NbByteToRead) != NFC_IO_STATUS_SUCCESS)
  {
    return M24SR_ERROR_TIMEOUT;
  } 
  
  status = M24SR_IsCorrectCRC16Residue (pBuffer,NbByteToRead); 
  return status;
}


/**
  * @brief  This function sends the SelectNDEFfile command
  * @param  DeviceAddr: I2C address of the device
  * @param  NDEFfileId: NDEF identification to select NDEF in M24SR
  * @retval Status (SW1&SW2) : Status of the operation to complete.
  * @retval M24SR_ERROR_TIMEOUT : The I2C timeout occured.
  */
uint16_t M24SR_SelectNDEFfile (uint16_t DeviceAddr, uint16_t NDEFfileId)
{
  uint8_t   *pBuffer = uM24SRbuffer ,
  NbByteToRead = M24SR_STATUSRESPONSE_NBBYTE;
  uint8_t    uLc = 0x02;
  uint16_t  status ; 
  uint16_t  uP1P2 =0x000C,
  NbByte;
  
  /* build the command */
  Command.Header.CLA = C_APDU_CLA_DEFAULT;
  Command.Header.INS = C_APDU_SELECT_FILE;
  /* copy the offset */
  Command.Header.P1 = GETMSB  (uP1P2 ) ;
  Command.Header.P2 = GETLSB  (uP1P2 ) ;
  /* copy the number of byte of the data field */
  Command.Body.LC = uLc ;
  /* copy the offset */
  Command.Body.pData[0] = GETMSB  (NDEFfileId ) ;
  Command.Body.pData[1] = GETLSB  (NDEFfileId ) ;
  /* build the I²C command */
  M24SR_BuildIBlockCommand ( M24SR_CMDSTRUCT_SELECTNDEFFILE,  Command, &NbByte , pBuffer);
  
  /* send the request */ 
  if (NFC_IO_WriteMultiple(DeviceAddr, pBuffer, NbByte) != NFC_IO_STATUS_SUCCESS)
  {
    return M24SR_ERROR_TIMEOUT;
  } 
  status = M24SR_IsAnswerReady (DeviceAddr);
  if (status != M24SR_STATUS_SUCCESS)
  {
    return status;
  } 
  /* read the response */ 
  if (NFC_IO_ReadMultiple (DeviceAddr , pBuffer,  NbByteToRead) != NFC_IO_STATUS_SUCCESS)
  {
    return M24SR_ERROR_TIMEOUT;
  } 
  
  status = M24SR_IsCorrectCRC16Residue (pBuffer,NbByteToRead); 
  return status;
}


/**
  * @brief  This function sends a read binary command
  * @param  DeviceAddr: I2C address of the device
  * @param  Offset : first byte to read
  * @param  NbByteToRead : number of byte to read
  * @param  pBufferRead : pointer of the buffer read from the M24SR device
  * @retval Status (SW1&SW2) : Status of the operation to complete.
  * @retval M24SR_ERROR_TIMEOUT : The I2C timeout occured.
  */
uint16_t M24SR_ReadBinary (uint16_t DeviceAddr, uint16_t Offset ,uint8_t NbByteToRead , uint8_t *pBufferRead)
{
  uint8_t   *pBuffer = uM24SRbuffer ;
  uint16_t  status ; 
  uint16_t  NbByte;
  
  /* build the command */
  Command.Header.CLA = C_APDU_CLA_DEFAULT;
  Command.Header.INS = C_APDU_READ_BINARY;
  /* copy the offset */
  Command.Header.P1 = GETMSB  (Offset ) ;
  Command.Header.P2 = GETLSB  (Offset ) ;
  /* copy the number of byte to read */
  Command.Body.LE = NbByteToRead ;
  
  M24SR_BuildIBlockCommand ( M24SR_CMDSTRUCT_READBINARY,  Command, &NbByte , pBuffer);
  
  status = NFC_IO_WriteMultiple(DeviceAddr, pBuffer, NbByte);
  if (status != NFC_IO_STATUS_SUCCESS)
  {
    return M24SR_ERROR_TIMEOUT;
  } 
  status = M24SR_IsAnswerReady (DeviceAddr);
  if (status != M24SR_STATUS_SUCCESS)
  {
    return status;
  } 
  status = NFC_IO_ReadMultiple (DeviceAddr , pBuffer,  NbByteToRead + M24SR_STATUSRESPONSE_NBBYTE);
  if (status != NFC_IO_STATUS_SUCCESS)
  {
    return M24SR_ERROR_TIMEOUT;
  } 
  
  status = M24SR_IsCorrectCRC16Residue (pBuffer,NbByteToRead+ M24SR_STATUSRESPONSE_NBBYTE); 
  /* retrieve the data without SW1 & SW2 as provided as return value of the function */
  memcpy(pBufferRead ,&pBuffer[1],NbByteToRead);
  return status;
}


/**
  * @brief  This function sends a ST read binary command (no error if access is not inside NDEF file)
  * @param  DeviceAddr: I2C address of the device
  * @param  Offset : first byte to read
  * @param  NbByteToRead : number of byte to read
  * @param  pBufferRead : pointer of the buffer read from the M24SR device
  * @retval Status (SW1&SW2) : Status of the operation to complete.
  * @retval M24SR_ERROR_TIMEOUT : The I2C timeout occured. 
  */
uint16_t M24SR_STReadBinary (uint16_t DeviceAddr, uint16_t Offset, uint8_t NbByteToRead, uint8_t *pBufferRead)
{
  uint8_t   *pBuffer = uM24SRbuffer ;
  uint16_t  status ; 
  uint16_t  NbByte;
  
  /* build the command */
  Command.Header.CLA = C_APDU_CLA_ST;
  Command.Header.INS = C_APDU_READ_BINARY;
  /* copy the offset */
  Command.Header.P1 = GETMSB  (Offset ) ;
  Command.Header.P2 = GETLSB  (Offset ) ;
  /* copy the number of byte to read */
  Command.Body.LE = NbByteToRead ;
  
  M24SR_BuildIBlockCommand ( M24SR_CMDSTRUCT_READBINARY,  Command, &NbByte , pBuffer);
  
  status =  NFC_IO_WriteMultiple(DeviceAddr, pBuffer, NbByte);
  if (status != NFC_IO_STATUS_SUCCESS)
  {
    return M24SR_ERROR_TIMEOUT;
  } 
  status = M24SR_IsAnswerReady (DeviceAddr);
  if (status != M24SR_STATUS_SUCCESS)
  {
    return status;
  } 
  status = NFC_IO_ReadMultiple (DeviceAddr , pBuffer,  NbByteToRead + M24SR_STATUSRESPONSE_NBBYTE);
  if (status != NFC_IO_STATUS_SUCCESS)
  {
    return M24SR_ERROR_TIMEOUT;
  } 
  
  status = M24SR_IsCorrectCRC16Residue (pBuffer,NbByteToRead+ M24SR_STATUSRESPONSE_NBBYTE); 
  /* retrieve the data without SW1 & SW2 as provided as return value of the function */
  memcpy(pBufferRead ,&pBuffer[1],NbByteToRead);
  return status;
}


/**
  * @brief  This function sends a Update binary command
  * @param  DeviceAddr: I2C address of the device
  * @param  Offset : first byte to read
  * @param  NbByteToWrite : number of byte to write
  * @param  pBufferRead : pointer of the buffer read from the M24SR device
  * @retval Status (SW1&SW2) : Status of the operation to complete.
  * @retval M24SR_ERROR_TIMEOUT : The I2C timeout occured.
  */
uint16_t M24SR_UpdateBinary (uint16_t DeviceAddr, uint16_t Offset ,uint8_t NbByteToWrite,uint8_t *pDataToWrite)
{
  uint8_t   *pBuffer = uM24SRbuffer ;
  uint16_t  status ; 
  uint16_t  NbByte;
  
  /* build the command */
  Command.Header.CLA = C_APDU_CLA_DEFAULT;
  Command.Header.INS = C_APDU_UPDATE_BINARY;
  /* copy the offset */
  Command.Header.P1 = GETMSB  (Offset ) ;
  Command.Header.P2 = GETLSB  (Offset ) ;
  /* copy the number of byte of the data field */
  Command.Body.LC = NbByteToWrite ;
  /* copy the File Id */
  memcpy(Command.Body.pData ,pDataToWrite, NbByteToWrite );
  
  M24SR_BuildIBlockCommand ( M24SR_CMDSTRUCT_UPDATEBINARY,  Command, &NbByte , pBuffer);
  
  status = NFC_IO_WriteMultiple(DeviceAddr, pBuffer, NbByte);
  if (status != NFC_IO_STATUS_SUCCESS)
  {
    return M24SR_ERROR_TIMEOUT;
  } 
  status = M24SR_IsAnswerReady (DeviceAddr);
  if (status != M24SR_STATUS_SUCCESS)
  {
    return status;
  } 
  status = NFC_IO_ReadMultiple (DeviceAddr , pBuffer,  M24SR_STATUSRESPONSE_NBBYTE);
  if (status != NFC_IO_STATUS_SUCCESS)
  {
    return M24SR_ERROR_TIMEOUT;
  } 
  /* if the response is a Watiting frame extenstion request */ 
  if (IsSBlock (pBuffer) == M24SR_STATUS_SUCCESS)
  {
    /*check the CRC */ 
    if (M24SR_IsCorrectCRC16Residue (pBuffer , M24SR_WATINGTIMEEXTRESPONSE_NBBYTE) != M24SR_ERROR_CRC)
    {
      /* send the FrameExension response*/ 
      status = M24SR_FWTExtension (DeviceAddr,  pBuffer [M24SR_OFFSET_PCB+1]);
    }
  }
  else
  {  
    status = M24SR_IsCorrectCRC16Residue (pBuffer, M24SR_STATUSRESPONSE_NBBYTE); 
  }
  
  return status;
}


/**
  * @brief  This function sends the Verify command
  * @param  DeviceAddr: I2C address of the device
  * @param  uPwdId : PasswordId ( 0x0001 : Read NDEF pwd or 0x0002 : Write NDEF pwd or 0x0003 : I2C pwd)
  * @param  NbPwdByte : Number of byte ( 0x00 or 0x10)
  * @param  pPwd : pointer on the passwaord
  * @retval Status (SW1&SW2) : Status of the operation to complete.
  * @retval M24SR_ERROR_TIMEOUT : The I2C timeout occured.
  */
uint16_t M24SR_Verify (uint16_t DeviceAddr, uint16_t uPwdId, uint8_t NbPwdByte ,uint8_t *pPwd)
{
  uint8_t   *pBuffer = uM24SRbuffer ;
  uint16_t  status = 0x0000 ; 
  uint16_t  NbByte;
  
  /*check the parameters */
  if (uPwdId > 0x0003)
  {  
    return M24SR_ERROR_PARAMETER;
  }
  if ( (NbPwdByte != 0x00) && (NbPwdByte != 0x10))
  {  
    return M24SR_ERROR_PARAMETER;
  }
  
  /* build the command */
  Command.Header.CLA = C_APDU_CLA_DEFAULT;
  Command.Header.INS = C_APDU_VERIFY;
  /* copy the Password Id */
  Command.Header.P1 = GETMSB  (uPwdId ) ;
  Command.Header.P2 = GETLSB  (uPwdId ) ;
  /* copy the number of byte of the data field */
  Command.Body.LC = NbPwdByte ;
  
  if (NbPwdByte == 0x10) 
  {
    /* copy the password */
    memcpy(Command.Body.pData, pPwd, NbPwdByte);
    /* build the I²C command */
    M24SR_BuildIBlockCommand ( M24SR_CMDSTRUCT_VERIFYBINARYWITHPWD,  Command, &NbByte , pBuffer);
  }
  else 
  {
    /* build the I²C command */
    M24SR_BuildIBlockCommand ( M24SR_CMDSTRUCT_VERIFYBINARYWOPWD,  Command, &NbByte , pBuffer);
  }
  
  /* send the request */ 
  status = NFC_IO_WriteMultiple(DeviceAddr, pBuffer, NbByte);
  if (status != NFC_IO_STATUS_SUCCESS)
  {
    return M24SR_ERROR_TIMEOUT;
  } 
  /* wait for answer ready */
  status = M24SR_IsAnswerReady (DeviceAddr);
  if (status != M24SR_STATUS_SUCCESS)
  {
    return status;
  } 
  /* read the response */ 
  status = NFC_IO_ReadMultiple (DeviceAddr , pBuffer,  M24SR_STATUSRESPONSE_NBBYTE);
  if (status != NFC_IO_STATUS_SUCCESS)
  {
    return M24SR_ERROR_TIMEOUT;
  } 
  
  status = M24SR_IsCorrectCRC16Residue (pBuffer, M24SR_STATUSRESPONSE_NBBYTE); 
  return status;
}


/**
  * @brief  This function sends the ChangeReferenceData command
  * @param  DeviceAddr: I2C address of the device
  * @param  uPwdId : PasswordId ( 0x0001 : Read NDEF pwd or 0x0002 : Write NDEF pwd or 0x0003 : I2C pwd)
  * @param  pPwd : pointer on the passwaord
  * @retval Status (SW1&SW2) : Status of the operation to complete.
  * @retval M24SR_ERROR_TIMEOUT : The I2C timeout occured.
  */
uint16_t M24SR_ChangeReferenceData (uint16_t DeviceAddr, uint16_t uPwdId, uint8_t *pPwd)
{
  uint8_t   *pBuffer = uM24SRbuffer;
  uint16_t  status ; 
  uint16_t  NbByte;
  
  /*check the parameters */
  if (uPwdId > 0x0003)
  {  
    return M24SR_ERROR_PARAMETER;
  }
  
  /* build the command */
  Command.Header.CLA = C_APDU_CLA_DEFAULT;
  Command.Header.INS = C_APDU_CHANGE;
  /* copy the Password Id */
  Command.Header.P1 = GETMSB  (uPwdId ) ;
  Command.Header.P2 = GETLSB  (uPwdId ) ;
  /* copy the number of byte of the data field */
  Command.Body.LC = M24SR_PASSWORD_NBBYTE ;
  /* copy the password */
  memcpy(Command.Body.pData, pPwd, M24SR_PASSWORD_NBBYTE);
  /* build the I²C command */
  M24SR_BuildIBlockCommand ( M24SR_CMDSTRUCT_CHANGEREFDATA,  Command, &NbByte , pBuffer);
  
  
  /* send the request */ 
  status = NFC_IO_WriteMultiple(DeviceAddr, pBuffer, NbByte);
  if (status != NFC_IO_STATUS_SUCCESS)
  {
    return M24SR_ERROR_TIMEOUT;
  } 
  status = M24SR_IsAnswerReady (DeviceAddr);
  if (status != M24SR_STATUS_SUCCESS)
  {
    return status;
  } 
  /* read the response */ 
  status = NFC_IO_ReadMultiple (DeviceAddr , pBuffer,  M24SR_STATUSRESPONSE_NBBYTE);
  if (status != NFC_IO_STATUS_SUCCESS)
  {
    return M24SR_ERROR_TIMEOUT;
  } 
  
  status = M24SR_IsCorrectCRC16Residue (pBuffer, M24SR_STATUSRESPONSE_NBBYTE); 
  return status;
}


/**
  * @brief  This function sends the EnableVerificationRequirement command
  * @param  DeviceAddr: I2C address of the device
  * @param  uReadOrWrite : enable the read or write protection ( 0x0001 : Read or 0x0002 : Write  )
  * @retval Status (SW1&SW2) : Status of the operation to complete.
  * @retval M24SR_ERROR_TIMEOUT : The I2C timeout occured.
  */
uint16_t M24SR_EnableVerificationRequirement (uint16_t DeviceAddr, uint16_t uReadOrWrite)
{
  uint8_t   *pBuffer = uM24SRbuffer;
  uint16_t  status ; 
  uint16_t  NbByte;
  
  /*check the parameters */
  if ( (uReadOrWrite != 0x0001) && (uReadOrWrite != 0x0002))
  {  
    return M24SR_ERROR_PARAMETER;
  }
  
  /* build the command */
  Command.Header.CLA = C_APDU_CLA_DEFAULT;
  Command.Header.INS = C_APDU_ENABLE;
  /* copy the Password Id */
  Command.Header.P1 = GETMSB  (uReadOrWrite ) ;
  Command.Header.P2 = GETLSB  (uReadOrWrite ) ;
  /* build the I²C command */
  M24SR_BuildIBlockCommand ( M24SR_CMDSTRUCT_ENABLEVERIFREQ,  Command, &NbByte , pBuffer);
  
  /* send the request */ 
  status =  NFC_IO_WriteMultiple(DeviceAddr, pBuffer, NbByte);
  if (status != NFC_IO_STATUS_SUCCESS)
  {
    return M24SR_ERROR_TIMEOUT;
  } 
  /* The right access to be updated in EEPROM need at least 6ms */  
  status = M24SR_IsAnswerReady (DeviceAddr);
  if (status != M24SR_STATUS_SUCCESS)
  {
    return status;
  } 
  /* read the response */ 
  status =  NFC_IO_ReadMultiple (DeviceAddr , pBuffer,  M24SR_STATUSRESPONSE_NBBYTE);
  if (status != NFC_IO_STATUS_SUCCESS)
  {
    return M24SR_ERROR_TIMEOUT;
  } 
  
  status = M24SR_IsCorrectCRC16Residue (pBuffer, M24SR_STATUSRESPONSE_NBBYTE); 
  return status;
}


/**
  * @brief  This function sends the DisableVerificationRequirement command
  * @param  DeviceAddr: I2C address of the device
  * @param  uReadOrWrite : enable the read or write protection ( 0x0001 : Read or 0x0002 : Write  )
  * @retval Status (SW1&SW2) : Status of the operation to complete.
  * @retval M24SR_ERROR_TIMEOUT : The I2C timeout occured.
  */
uint16_t M24SR_DisableVerificationRequirement (uint16_t DeviceAddr, uint16_t uReadOrWrite)
{
  uint8_t   *pBuffer = uM24SRbuffer;
  uint16_t  status ; 
  uint16_t  NbByte;
  
  /*check the parameters */
  if ( (uReadOrWrite != 0x0001) && (uReadOrWrite != 0x0002))
  {  
    return M24SR_ERROR_PARAMETER;
  }
  
  /* build the command */
  Command.Header.CLA = C_APDU_CLA_DEFAULT;
  Command.Header.INS = C_APDU_DISABLE;
  /* copy the Password Id */
  Command.Header.P1 = GETMSB  (uReadOrWrite ) ;
  Command.Header.P2 = GETLSB  (uReadOrWrite ) ;
  /* build the I²C command */
  M24SR_BuildIBlockCommand ( M24SR_CMDSTRUCT_DISABLEVERIFREQ,  Command, &NbByte , pBuffer);
  
  /* send the request */ 
  status = NFC_IO_WriteMultiple(DeviceAddr, pBuffer, NbByte);
  if (status != NFC_IO_STATUS_SUCCESS)
  {
    return M24SR_ERROR_TIMEOUT;
  } 
  /* The right access to be updated in EEPROM need at least 6ms */    
  status = M24SR_IsAnswerReady (DeviceAddr);
  if (status != M24SR_STATUS_SUCCESS)
  {
    return status;
  } 
  /* read the response */ 
  status = NFC_IO_ReadMultiple (DeviceAddr , pBuffer,  M24SR_STATUSRESPONSE_NBBYTE);
  if (status != NFC_IO_STATUS_SUCCESS)
  {
    return M24SR_ERROR_TIMEOUT;
  } 
  
  status = M24SR_IsCorrectCRC16Residue (pBuffer, M24SR_STATUSRESPONSE_NBBYTE); 
  return status;
}


/**
  * @brief  This function sends the EnablePermananentState command
  * @param  DeviceAddr: I2C address of the device
  * @param  uReadOrWrite : enable the read or write protection ( 0x0001 : Read or 0x0002 : Write  )
  * @retval Status (SW1&SW2) : Status of the operation to complete.
  * @retval M24SR_ERROR_TIMEOUT : The I2C timeout occured.
  */
uint16_t M24SR_EnablePermanentState (uint16_t DeviceAddr, uint16_t uReadOrWrite  )
{
  uint8_t   *pBuffer = uM24SRbuffer;
  uint16_t  status ; 
  uint16_t  NbByte;
  
  /*check the parameters */
  if ( (uReadOrWrite != 0x0001) && (uReadOrWrite != 0x0002))
  {  
    return M24SR_ERROR_PARAMETER;
  }
  
  /* build the command */
  Command.Header.CLA = C_APDU_CLA_ST;
  Command.Header.INS = C_APDU_ENABLE;
  /* copy the Password Id */
  Command.Header.P1 = GETMSB  (uReadOrWrite ) ;
  Command.Header.P2 = GETLSB  (uReadOrWrite ) ;
  /* build the I²C command */
  M24SR_BuildIBlockCommand ( M24SR_CMDSTRUCT_ENABLEVERIFREQ,  Command, &NbByte , pBuffer);
  
  /* send the request */ 
  status = NFC_IO_WriteMultiple(DeviceAddr, pBuffer, NbByte);
  if (status != NFC_IO_STATUS_SUCCESS)
  {
    return M24SR_ERROR_TIMEOUT;
  } 
  status = M24SR_IsAnswerReady (DeviceAddr);
  if (status != M24SR_STATUS_SUCCESS)
  {
    return status;
  } 
  /* read the response */ 
  status = NFC_IO_ReadMultiple (DeviceAddr , pBuffer,  M24SR_STATUSRESPONSE_NBBYTE);
  if (status != NFC_IO_STATUS_SUCCESS)
  {
    return M24SR_ERROR_TIMEOUT;
  } 
  
  status = M24SR_IsCorrectCRC16Residue (pBuffer, M24SR_STATUSRESPONSE_NBBYTE); 
  return status;
}


/**
  * @brief  This function sends the DisablePermanentState command
  * @param  DeviceAddr: I2C address of the device
  * @param  uReadOrWrite : enable the read or write protection ( 0x0001 : Read or 0x0002 : Write  )
  * @retval Status (SW1&SW2) : Status of the operation to complete.
  * @retval M24SR_ERROR_TIMEOUT : The I2C timeout occured.
  */
uint16_t M24SR_DisablePermanentState (uint16_t DeviceAddr, uint16_t uReadOrWrite  )
{
  uint8_t   *pBuffer = uM24SRbuffer;
  uint16_t  status ; 
  uint16_t  NbByte;
  
  /*check the parameters */
  if ( (uReadOrWrite != 0x0001) && (uReadOrWrite != 0x0002))
  {  
    return M24SR_ERROR_PARAMETER;
  }
  
  /* build the command */
  Command.Header.CLA = C_APDU_CLA_ST;
  Command.Header.INS = C_APDU_DISABLE;
  /* copy the Password Id */
  Command.Header.P1 = GETMSB  (uReadOrWrite ) ;
  Command.Header.P2 = GETLSB  (uReadOrWrite ) ;
  /* build the I²C command */
  M24SR_BuildIBlockCommand ( M24SR_CMDSTRUCT_DISABLEVERIFREQ,  Command, &NbByte , pBuffer);
  
  /* send the request */ 
  status = NFC_IO_WriteMultiple(DeviceAddr, pBuffer, NbByte);
  if (status != NFC_IO_STATUS_SUCCESS)
  {
    return M24SR_ERROR_TIMEOUT;
  } 
  status = M24SR_IsAnswerReady (DeviceAddr);
  if (status != M24SR_STATUS_SUCCESS)
  {
    return status;
  } 
  /* read the response */ 
  status = NFC_IO_ReadMultiple (DeviceAddr , pBuffer,  M24SR_STATUSRESPONSE_NBBYTE);
  if (status != NFC_IO_STATUS_SUCCESS)
  {
    return M24SR_ERROR_TIMEOUT;
  } 
  
  status = M24SR_IsCorrectCRC16Residue (pBuffer, M24SR_STATUSRESPONSE_NBBYTE); 
  return status;
}

/**
  * @brief  This function generates a interrupt on GPO pin
  * @param  DeviceAddr: I2C address of the device
  * @retval Status (SW1&SW2) : Status of the operation to complete.
  * @retval M24SR_ERROR_TIMEOUT : The I2C timeout occured.
  */
uint16_t M24SR_SendInterrupt (uint16_t DeviceAddr)
{
  uint8_t   *pBuffer = uM24SRbuffer;
  uint16_t  uP1P2 =0x001E;
  uint16_t  status ; 
  uint16_t  NbByte;
  
  status = M24SR_ManageI2CGPO(DeviceAddr, INTERRUPT);
  
  /* build the command */
  Command.Header.CLA = C_APDU_CLA_ST;
  Command.Header.INS = C_APDU_INTERRUPT;
  /* copy the Password Id */
  Command.Header.P1 = GETMSB  (uP1P2 ) ;
  Command.Header.P2 = GETLSB  (uP1P2 ) ;
  Command.Body.LC = 0x00 ;
  /* build the I²C command */
  M24SR_BuildIBlockCommand ( M24SR_CMDSTRUCT_SENDINTERRUPT,  Command, &NbByte , pBuffer);
  
  /* send the request */ 
  status = NFC_IO_WriteMultiple(DeviceAddr, pBuffer, NbByte);
  if (status != NFC_IO_STATUS_SUCCESS)
  {
    return M24SR_ERROR_TIMEOUT;
  } 
  status = M24SR_IsAnswerReady (DeviceAddr);
  if (status != M24SR_STATUS_SUCCESS)
  {
    return status;
  } 
  /* read the response */ 
  status = NFC_IO_ReadMultiple (DeviceAddr , pBuffer,  M24SR_STATUSRESPONSE_NBBYTE);
  if (status != NFC_IO_STATUS_SUCCESS)
  {
    return M24SR_ERROR_TIMEOUT;
  } 
  
  status = M24SR_IsCorrectCRC16Residue (pBuffer, M24SR_STATUSRESPONSE_NBBYTE); 
  return status;
}

/**
  * @brief  This function force GPO pin to low state or high Z
  * @param  DeviceAddr: I2C address of the device
  * @param  uSetOrReset : select if GPO must be low (reset) or HiZ
  * @retval Status (SW1&SW2) : Status of the operation to complete.
  * @retval M24SR_ERROR_TIMEOUT : The I2C timeout occured.
  */
uint16_t M24SR_StateControl (uint16_t DeviceAddr, uint8_t uSetOrReset )
{
  uint8_t   *pBuffer = uM24SRbuffer;
  uint16_t  uP1P2 =0x001F;
  uint16_t  status ; 
  uint16_t  NbByte;
  
  /*check the parameters */
  if ( (uSetOrReset != 0x01) && (uSetOrReset != 0x00))
  {  
    return M24SR_ERROR_PARAMETER;
  }
  
  status = M24SR_ManageI2CGPO(DeviceAddr, STATE_CONTROL);
  
  /* build the command */
  Command.Header.CLA = C_APDU_CLA_ST;
  Command.Header.INS = C_APDU_INTERRUPT;
  /* copy the Password Id */
  Command.Header.P1 = GETMSB  (uP1P2 ) ;
  Command.Header.P2 = GETLSB  (uP1P2 ) ;
  Command.Body.LC = 0x01 ;
  /* copy the data */
  memcpy(Command.Body.pData , &uSetOrReset, 0x01 );
  //Command.Body.LE = 0x00 ;
  /* build the I²C command */
  M24SR_BuildIBlockCommand ( M24SR_CMDSTRUCT_GPOSTATE,  Command, &NbByte , pBuffer);
  
  /* send the request */ 
  status = NFC_IO_WriteMultiple(DeviceAddr, pBuffer, NbByte);
  if (status != NFC_IO_STATUS_SUCCESS)
  {
    return M24SR_ERROR_TIMEOUT;
  } 
  status = M24SR_IsAnswerReady (DeviceAddr);
  if (status != M24SR_STATUS_SUCCESS)
  {
    return status;
  } 
  /* read the response */ 
  status = NFC_IO_ReadMultiple (DeviceAddr , pBuffer,  M24SR_STATUSRESPONSE_NBBYTE);
  if (status != NFC_IO_STATUS_SUCCESS)
  {
    return M24SR_ERROR_TIMEOUT;
  } 
  
  status = M24SR_IsCorrectCRC16Residue (pBuffer, M24SR_STATUSRESPONSE_NBBYTE); 
  return status;
}

/**
  * @brief  This function configure GPO purpose for I2C session
  * @param  DeviceAddr: I2C address of the device
  * @param  GPO_I2Cconfig: GPO configuration to set
  * @retval Status (SW1&SW2) : Status of the operation to complete.
  */
uint16_t M24SR_ManageI2CGPO(uint16_t DeviceAddr, uint8_t GPO_I2Cconfig)
{
  uint16_t status;
  uint8_t GPO_config;
  uint8_t DefaultPassword[16]={0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
  
  if( GPO_I2Cconfig > STATE_CONTROL)
  {  
    return M24SR_ERROR_PARAMETER;
  }
  
  /* we must not be in interrupt mode for I2C synchro as we will change GPO purpose */
  M24SR_SetI2CSynchroMode(M24SR_WAITINGTIME_POLLING);
  
  M24SR_SelectApplication(DeviceAddr);
  M24SR_SelectSystemfile(DeviceAddr);
  M24SR_ReadBinary (DeviceAddr, 0x0004 , 0x01 , &GPO_config );
  
  /* Update only GPO purpose for I2C */  
  GPO_config = (GPO_config & 0xF0) | GPO_I2Cconfig;
  M24SR_SelectSystemfile(DeviceAddr);
  M24SR_Verify(DeviceAddr, M24SR_I2C_PWD ,0x10 ,DefaultPassword );
  status = M24SR_UpdateBinary (DeviceAddr, 0x0004 ,0x01, &(GPO_config) );
  
  /* if we have set interrupt mode for I2C synchro we can enable interrupt mode */
  if (GPO_I2Cconfig == I2C_ANSWER_READY && status == M24SR_ACTION_COMPLETED)
  {
  if(uGpoMode == M24SR_GPO_SYNCHRO)
  {
    M24SR_SetI2CSynchroMode(M24SR_WAITINGTIME_GPO);
  }
  else
  {
    M24SR_SetI2CSynchroMode(M24SR_INTERRUPT_GPO);
  }
  }

  return status;
}

/**
  * @brief  This function configure GPO purpose for RF session
  * @param  DeviceAddr: I2C address of the device
  * @param  GPO_RFconfig: GPO configuration to set
  * @retval Status (SW1&SW2) : Status of the operation to complete.
  */
uint16_t M24SR_ManageRFGPO(uint16_t DeviceAddr, uint8_t GPO_RFconfig)
{
  uint16_t status;
  uint8_t GPO_config;
  uint8_t DefaultPassword[16]={0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
  
  if( GPO_RFconfig > STATE_CONTROL)
  {  
    return M24SR_ERROR_PARAMETER;
  }
  
  M24SR_SelectApplication(DeviceAddr);
  M24SR_SelectSystemfile(DeviceAddr);
  M24SR_ReadBinary (DeviceAddr, 0x0004 , 0x01 , &GPO_config );
  
  /* Update only GPO purpose for I2C */  
  GPO_config = (GPO_config & 0x0F) | (GPO_RFconfig<<4);
  M24SR_SelectSystemfile(DeviceAddr);
  M24SR_Verify(DeviceAddr, M24SR_I2C_PWD ,0x10 ,DefaultPassword );
  status = M24SR_UpdateBinary (DeviceAddr, 0x0004 ,0x01, &(GPO_config));
  
  return status;
}

/**
  * @brief  This functions returns M24SR_STATUS_SUCCESS when a response is ready
  * @param  DeviceAddr: I2C address of the device
  * @retval M24SR_STATUS_SUCCESS : a response of the M24LR is ready
  * @retval M24SR_ERROR_DEFAULT : the response of the M24LR is not ready
  */
static uint16_t M24SR_IsAnswerReady (uint16_t DeviceAddr)
{
  uint32_t retry = 0xFFFFF;
  uint8_t stable = 0;
  uint8_t PinState;

  switch (uSynchroMode)
  {
  case M24SR_WAITINGTIME_POLLING :
    if(NFC_IO_IsDeviceReady(DeviceAddr, NFC_IO_TRIALS) != NFC_IO_STATUS_SUCCESS)
    {
      return  M24SR_ERROR_TIMEOUT;
    } 
    break;
    
  case M24SR_WAITINGTIME_TIMEOUT :
    /* M24SR FWI=5 => (256*16/fc)*2^5=9.6ms but M24SR ask for extended time to program up to 246Bytes. */
    NFC_IO_Delay(M24SR_ANSWER_TIMEOUT);    
    break;
    
  case M24SR_WAITINGTIME_GPO :
    /* mbd does not support interrupt for the moment with nucleo board */
    do
    {
      NFC_IO_ReadState(&PinState);
      if( PinState == 0) /* RESET */
      {
        stable ++;                        
      }
      retry --;                        
    }
    while(stable < M24SR_ANSWER_STABLE && retry>0);
    if(!retry)
    {
      return M24SR_ERROR_TIMEOUT;   
    }          
    break;
    
  case M24SR_INTERRUPT_GPO :
    /* Check if the GPIO is not already low before calling this function */
    NFC_IO_ReadState(&PinState);
    if(PinState == 1) /* SET */
    {
      while (GPO_Low == 0);
    }
    GPO_Low = 0;
    break;
    
  default : 
    return M24SR_ERROR_DEFAULT;
  }
  
  return M24SR_STATUS_SUCCESS;
}

/**
  * @brief  This function enable or disable RF communication
  * @param  OnOffChoice: GPO configuration to set
  * @retval Status (SW1&SW2) : Status of the operation to complete.
  */
void M24SR_RFConfig( uint8_t OnOffChoice)
{
    /* Disable RF */
    if ( OnOffChoice != 0 )
    {    
        NFC_IO_RfDisable(0); /* PIN RESET */
    }
    else
    {    
        NFC_IO_RfDisable(1); /* PIN SET */
    }
}

/**
  * @brief  this functions configure I2C synchronization mode
  * @param  WaitingMode : interruption or polling
  * @retval None
  */
static void M24SR_SetI2CSynchroMode( uint8_t WaitingMode)
{
  if((uGpoMode == M24SR_GPO_SYNCHRO) || (uGpoMode == M24SR_GPO_INTERRUPT))
  {
    uSynchroMode = WaitingMode;
  } 
  else  /* GPO_POLLING */
  { 
    if(WaitingMode == M24SR_WAITINGTIME_GPO || WaitingMode == M24SR_INTERRUPT_GPO)
        uSynchroMode = M24SR_WAITINGTIME_POLLING;  /* Force Polling */
    else
        uSynchroMode = WaitingMode;
  }
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

/******************* (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
