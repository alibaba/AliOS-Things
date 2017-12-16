/**
******************************************************************************
* @file    BSP/Src/qspi.c 
* @author  MCD Application Team
* @brief   This example code shows how to use the QSPI Driver
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
#include "sensors.h"

/** @addtogroup STM32L4xx_HAL_Examples
* @{
*/

/** @addtogroup BSP
* @{
*/ 

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define QSPI_BUFFER_SIZE    ((uint32_t)0x0100)
#define WRITE_READ_ADDR     ((uint32_t)0x0050)
#define HEADBAND_HEIGHT     64
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
uint8_t qspi_aTxBuffer[QSPI_BUFFER_SIZE];
uint8_t qspi_aRxBuffer[QSPI_BUFFER_SIZE];

/* Private function prototypes -----------------------------------------------*/
static void     Fill_Buffer (uint8_t *pBuffer, uint32_t uwBufferLength, uint32_t uwOffset);
static uint8_t  Buffercmp   (uint8_t* pBuffer1, uint8_t* pBuffer2, uint32_t BufferLength);

/* Private functions ---------------------------------------------------------*/

/**
* @brief  QSPI Demo
* @param  None
* @retval None
*/
void QSPI_demo(void)
{ 
  /* QSPI info structure */
  static QSPI_Info pQSPI_Info;
  uint8_t status;

  printf("\n");
  printf("*****************************************************************\n");
  printf("****************************** QSPI Test ************************\n");
  printf("*****************************************************************\n"); 

  BSP_QSPI_DeInit();
  
  /*##-1- Configure the QSPI device ##########################################*/
  /* QSPI device configuration */ 
  status = BSP_QSPI_Init();
  
  if ((status == QSPI_NOT_SUPPORTED) || (status == QSPI_ERROR))
  {
    printf("QSPI Init : FAILED, QSPI Test Aborted \n");
  }
  else
  {
    printf("QSPI Init : OK \n");
    
    /*##-2- Read & check the QSPI info #######################################*/
    /* Initialize the structure */
    pQSPI_Info.FlashSize        = (uint32_t)0x00;
    pQSPI_Info.EraseSectorSize    = (uint32_t)0x00;
    pQSPI_Info.EraseSectorsNumber = (uint32_t)0x00;
    pQSPI_Info.ProgPageSize       = (uint32_t)0x00;
    pQSPI_Info.ProgPagesNumber    = (uint32_t)0x00;
    
    /* Read the QSPI memory info */
    BSP_QSPI_GetInfo(&pQSPI_Info);
    
    /* Test the correctness */
    if((pQSPI_Info.FlashSize != 0x800000) || (pQSPI_Info.EraseSectorSize != 0x1000)  ||
       (pQSPI_Info.ProgPageSize != 0x100)  || (pQSPI_Info.EraseSectorsNumber != 2048) ||
       (pQSPI_Info.ProgPagesNumber != 32768))
    {
      printf("QSPI GET INFO : FAILED, QSPI Test Aborted \n");
    }
    else
    {
      printf("QSPI GET INFO : OK \n");
      
      /*##-3- Erase QSPI memory ################################################*/ 
      if(BSP_QSPI_Erase_Block(WRITE_READ_ADDR) != QSPI_OK)
      {
        printf("QSPI ERASE : FAILED, QSPI Test Aborted \n");
      }
      else
      {
        printf("QSPI ERASE : OK \n");
        
        /*##-4- QSPI memory read/write access  #################################*/   
        /* Fill the buffer to write */
        Fill_Buffer(qspi_aTxBuffer, QSPI_BUFFER_SIZE, 0xD20F);   
        
        /* Write data to the QSPI memory */
        if(BSP_QSPI_Write(qspi_aTxBuffer, WRITE_READ_ADDR, QSPI_BUFFER_SIZE) != QSPI_OK)
        {
          printf("QSPI WRITE : FAILED, QSPI Test Aborted \n");
        }
        else
        {
          printf("QSPI WRITE : OK \n");
          
          /* Read back data from the QSPI memory */
          if(BSP_QSPI_Read(qspi_aRxBuffer, WRITE_READ_ADDR, QSPI_BUFFER_SIZE) != QSPI_OK)
          {
            printf("QSPI READ : FAILED, QSPI Test Aborted \n");
          }
          else
          {
            printf("QSPI READ : OK \n");
            
            /*##-5- Checking data integrity ############################################*/  
            if(Buffercmp(qspi_aRxBuffer, qspi_aTxBuffer, QSPI_BUFFER_SIZE) > 0)
            {
              printf("QSPI COMPARE : FAILED, QSPI Test Aborted \n");
            }
            else
            {    
              printf("QSPI Test : OK \n");
            }  
          }
        }
      }
    }
  }
  
  while (1)
  {    
  printf("\n*** Type q to quit test ***\n\n"); 
  if(Serial_Scanf(255) == 'q')
  {
    return;
  }  
  }
}

/**
* @brief  QSPI Demo
* @param  None
* @retval None
*/
void QSPI_MemoryMapped_demo(void)
{ 
  /* QSPI info structure */
  static QSPI_Info pQSPI_Info;
  uint8_t status, read_status = 0;
  uint16_t index = 0;
  uint8_t *mem_addr;

  printf("\n");
  printf("*****************************************************************\n");
  printf("******************* QSPI Memory Mapped Test *********************\n");
  printf("*****************************************************************\n"); 
  
  BSP_QSPI_DeInit();
  
  /*##-1- Configure the QSPI device ##########################################*/
  /* QSPI device configuration */ 
  status = BSP_QSPI_Init();
  
  if (status == QSPI_NOT_SUPPORTED)
  {
    printf("QSPI Initialization : FAILED.\n");
    printf("QSPI Test Aborted.\n");
    printf("Check the hardware configuration :\n");
    printf("  refer to the UM of the board\n");
    printf("  for the hardware modifications\n");
    printf("  to connect the QSPI memory\n");
  }
  else if (status == QSPI_ERROR)
  {
    printf("QSPI Initialization : FAILED.\n");
    printf("QSPI Test Aborted.\n");
  }
  else
  {
    printf("QSPI Initialization : OK.\n");
    
    /*##-2- Read & check the QSPI info #######################################*/
    /* Initialize the structure */
    pQSPI_Info.FlashSize        = (uint32_t)0x00;
    pQSPI_Info.EraseSectorSize    = (uint32_t)0x00;
    pQSPI_Info.EraseSectorsNumber = (uint32_t)0x00;
    pQSPI_Info.ProgPageSize       = (uint32_t)0x00;
    pQSPI_Info.ProgPagesNumber    = (uint32_t)0x00;
    
    /* Read the QSPI memory info */
    BSP_QSPI_GetInfo(&pQSPI_Info);
    
    /* Test the correctness */
    if((pQSPI_Info.FlashSize != 0x800000) || (pQSPI_Info.EraseSectorSize != 0x1000)  ||
       (pQSPI_Info.ProgPageSize != 0x100)  || (pQSPI_Info.EraseSectorsNumber != 2048) ||
         (pQSPI_Info.ProgPagesNumber != 32768))
    {
      printf("QSPI GET INFO : FAILED.\n");
      printf("QSPI Test Aborted.\n");
    }
    else
    {
      printf("QSPI GET INFO : OK.   \n");
      
      /*##-3- Erase QSPI memory ################################################*/ 
      if(BSP_QSPI_Erase_Block(WRITE_READ_ADDR) != QSPI_OK)
      {
        printf("QSPI ERASE : FAILED.\n");
        printf("QSPI Test Aborted.\n");
      }
      else
      {
        printf("QSPI ERASE : OK.   \n");
        
        /*##-4- QSPI memory read/write access  #################################*/   
        /* Fill the buffer to write */
        Fill_Buffer(qspi_aTxBuffer, QSPI_BUFFER_SIZE, 0xD20F);   
        
        //        for(uint32_t i =0; i < 32768; i++)
        for(uint32_t i =0; i < 32; i++)  
        {
          if(BSP_QSPI_Write(qspi_aTxBuffer, (i * 0x100), QSPI_BUFFER_SIZE) != QSPI_OK)
          {
            printf("QSPI WRITE : FAILED.\n");
            printf("QSPI Test Aborted.\n");
          }
        }
        
        /* Read back data from the QSPI memory */
        BSP_QSPI_EnableMemoryMappedMode();
        
        //        for(uint32_t i =0; i < 32768; i++)
        for(uint32_t i =0; i < 32; i++) 
        {          
          mem_addr = (uint8_t *)0x90000000 + (i * 0x100);
          for (index = 0; index < QSPI_BUFFER_SIZE; index++)
          {
            if (*mem_addr != qspi_aTxBuffer[index])
            {
              read_status = index + 1;
            }
            mem_addr++;
          }          
          
        }
        
        if(read_status != 0)
        {
          printf("QSPI READ : FAILED.\n");
          printf("QSPI Test Aborted.\n");
        }
        else
        {
          printf("QSPI READ :  OK.    \n");
          printf("QSPI Test : OK.     \n");
          
        }
      }
    }
  }
  
  while (1)
  {    
    printf("\n*** Type q to quit test ***\n\n"); 
    if(Serial_Scanf(255) == 'q')
    {
      return;
    } 
  }
}



/**
* @brief  Fills buffer with user predefined data.
* @param  pBuffer: pointer on the buffer to fill
* @param  uwBufferLenght: size of the buffer to fill
* @param  uwOffset: first value to fill on the buffer
* @retval None
*/
static void Fill_Buffer(uint8_t *pBuffer, uint32_t uwBufferLenght, uint32_t uwOffset)
{
  uint32_t tmpIndex = 0;
  
  /* Put in global buffer different values */
  for (tmpIndex = 0; tmpIndex < uwBufferLenght; tmpIndex++ )
  {
    pBuffer[tmpIndex] = tmpIndex + uwOffset;
  }
}


/**
* @brief  Compares two buffers.
* @param  pBuffer1, pBuffer2: buffers to be compared.
* @param  BufferLength: buffer's length
* @retval 1: pBuffer identical to pBuffer1
*         0: pBuffer differs from pBuffer1
*/
static uint8_t Buffercmp(uint8_t* pBuffer1, uint8_t* pBuffer2, uint32_t BufferLength)
{
  while (BufferLength--)
  {
    if (*pBuffer1 != *pBuffer2)
    {
      return 1;
    }
    
    pBuffer1++;
    pBuffer2++;
  }
  
  return 0;
}
/**
* @}
*/ 

/**
* @}
*/ 
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
