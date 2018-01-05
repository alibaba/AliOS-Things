
/* Includes ------------------------------------------------------------------*/
#include "platform_logging.h"
#include "platform_peripheral.h"
#include "platform.h"
#include "platform_config.h"
#include "stdio.h"

#ifdef USE_MICO_SPI_FLASH
#include "spi_flash.h"
#endif

#ifdef USE_QUAD_SPI_FLASH
#include "spi_flash.h"
#include "spi_flash_internal.h"
#endif

/* Private constants --------------------------------------------------------*/
#define ADDR_FLASH_SECTOR_0     ((uint32_t)0x08000000) /* Base @ of Sector 0, 16 Kbyte */
#define ADDR_FLASH_SECTOR_1     ((uint32_t)0x08004000) /* Base @ of Sector 1, 16 Kbyte */
#define ADDR_FLASH_SECTOR_2     ((uint32_t)0x08008000) /* Base @ of Sector 2, 16 Kbyte */
#define ADDR_FLASH_SECTOR_3     ((uint32_t)0x0800C000) /* Base @ of Sector 3, 16 Kbyte */
#define ADDR_FLASH_SECTOR_4     ((uint32_t)0x08010000) /* Base @ of Sector 4, 64 Kbyte */
#define ADDR_FLASH_SECTOR_5     ((uint32_t)0x08020000) /* Base @ of Sector 5, 128 Kbyte */
#define ADDR_FLASH_SECTOR_6     ((uint32_t)0x08040000) /* Base @ of Sector 6, 128 Kbyte */
#define ADDR_FLASH_SECTOR_7     ((uint32_t)0x08060000) /* Base @ of Sector 7, 128 Kbyte */
#define ADDR_FLASH_SECTOR_8     ((uint32_t)0x08080000) /* Base @ of Sector 8, 128 Kbyte */
#define ADDR_FLASH_SECTOR_9     ((uint32_t)0x080A0000) /* Base @ of Sector 9, 128 Kbyte */
#define ADDR_FLASH_SECTOR_10    ((uint32_t)0x080C0000) /* Base @ of Sector 10, 128 Kbyte */
#define ADDR_FLASH_SECTOR_11    ((uint32_t)0x080E0000) /* Base @ of Sector 11, 128 Kbyte */

/* End of the Flash address */
#define FLASH_START_ADDRESS     (uint32_t)0x08000000  
#define FLASH_END_ADDRESS       (uint32_t)0x080FFFFF
#define FLASH_SIZE              (FLASH_END_ADDRESS -  FLASH_START_ADDRESS + 1)
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
#ifdef USE_MICO_SPI_FLASH
static sflash_handle_t sflash_handle = {0x0, 0x0, SFLASH_WRITE_NOT_ALLOWED};
#endif
#ifdef USE_QUAD_SPI_FLASH
#define sFLASH_SPI_PAGESIZE       0x100
static sflash_handle_t sflash_handle = {0x0, 0x0, SFLASH_WRITE_NOT_ALLOWED};
extern const platform_qspi_t platform_qspi_peripherals[];
__IO uint8_t *qspiaddr = (__IO uint8_t *)(0x90000000);
QSPI_ComConfig_InitTypeDef  QSPI_ComConfig_InitStructure;
#ifdef USE_QUAD_SPI_DMA
DMA_InitTypeDef DMA_InitStructure;
#endif
#endif
/* Private function prototypes -----------------------------------------------*/
static uint32_t _GetSector( uint32_t Address );
static OSStatus _GetAddress(uint32_t sector, uint32_t *startAddress, uint32_t *endAddress);
static OSStatus internalFlashInitialize( void );
static OSStatus internalFlashErase(uint32_t StartAddress, uint32_t EndAddress);
static OSStatus internalFlashWrite(volatile uint32_t* FlashAddress, uint32_t* Data ,uint32_t DataLength);
static OSStatus internalFlashByteWrite( volatile uint32_t* FlashAddress, uint8_t* Data ,uint32_t DataLength );
#ifdef MCU_EBANLE_FLASH_PROTECT
static uint32_t _GetWRPSector(uint32_t Address);
static OSStatus internalFlashProtect(uint32_t StartAddress, uint32_t EndAddress, bool enable);
#endif
#ifdef USE_MICO_SPI_FLASH
static OSStatus spiFlashErase(uint32_t StartAddress, uint32_t EndAddress);
#endif
#ifdef USE_QUAD_SPI_FLASH
static OSStatus qspiFlashErase( uint32_t StartAddress, uint32_t EndAddress );
static void QSPI_AutoPollingMemReady(void);
static void QSPI_WriteEnable(void);
static void QSPI_QEEnable(void);
static void QSPI_GPIO_Config(/*@out@*/ const platform_qspi_t* peripheral);
int init_qsflash( /*@out@*/ const platform_qspi_t* peripheral, uint32_t flash_length, /*@out@*/ sflash_handle_t* const handle, sflash_write_allowed_t write_allowed_in );
int qsflash_read_ID( sflash_handle_t* const handle, void* const data_addr );
int qsflash_write( const sflash_handle_t* const handle, unsigned long device_address, const void* const data_addr, unsigned int size );
int qsflash_read( const sflash_handle_t* const handle, unsigned long device_address, void* const data_addr, unsigned int size );
int qsflash_sector_erase ( unsigned long device_address );
int qsflash_write_page( const sflash_handle_t* const handle, unsigned long device_address, const void* const data_addr, unsigned int size );
int get_power(uint32_t number);
#endif


OSStatus platform_flash_init( const platform_flash_t *peripheral )
{
  OSStatus err = kNoErr;

  require_action_quiet( peripheral != NULL, exit, err = kParamErr);

  if( peripheral->flash_type == FLASH_TYPE_EMBEDDED ){
    err = internalFlashInitialize();
    require_noerr(err, exit);
  }
#ifdef USE_MICO_SPI_FLASH
  else if( peripheral->flash_type == FLASH_TYPE_SPI ){
    err = init_sflash( &sflash_handle, 0, SFLASH_WRITE_ALLOWED );
    require_noerr(err, exit);
  }
#else
  #ifdef USE_QUAD_SPI_FLASH
  else if( peripheral->flash_type == FLASH_TYPE_QSPI ){
    err = init_qsflash( &platform_qspi_peripherals[MICO_QSPI_1], peripheral->flash_length, &sflash_handle, SFLASH_WRITE_ALLOWED );
    require_noerr(err, exit);
  }
  #endif
#endif
  else{
    err = kTypeErr;
    goto exit;
  }
exit:
  return err;
}

OSStatus platform_flash_erase( const platform_flash_t *peripheral, uint32_t start_address, uint32_t end_address )
{
  OSStatus err = kNoErr;

  require_action_quiet( peripheral != NULL, exit, err = kParamErr);
  require_action( start_address >= peripheral->flash_start_addr 
               && end_address   <= peripheral->flash_start_addr + peripheral->flash_length - 1, exit, err = kParamErr);

  if( peripheral->flash_type == FLASH_TYPE_EMBEDDED ){
    err = internalFlashErase( start_address, end_address );    
    require_noerr(err, exit);
  }
#ifdef USE_MICO_SPI_FLASH
  else if( peripheral->flash_type == FLASH_TYPE_SPI ){
    err = spiFlashErase( start_address, end_address );
    require_noerr(err, exit);
  }
#else
  #ifdef USE_QUAD_SPI_FLASH
  else if( peripheral->flash_type == FLASH_TYPE_QSPI ){
    err = qspiFlashErase( start_address, end_address );
    require_noerr(err, exit);
  }
  #endif
#endif
  else{
    err = kTypeErr;
    goto exit;
  }

exit:
  return err;
}

OSStatus platform_flash_write( const platform_flash_t *peripheral, volatile uint32_t* start_address, uint8_t* data ,uint32_t length  )
{
  OSStatus err = kNoErr;

  require_action_quiet( peripheral != NULL, exit, err = kParamErr);
  require_action( *start_address >= peripheral->flash_start_addr 
               && *start_address + length <= peripheral->flash_start_addr + peripheral->flash_length, exit, err = kParamErr);
    
  if( peripheral->flash_type == FLASH_TYPE_EMBEDDED ){
    err = internalFlashWrite( start_address, (uint32_t *)data, length); 
    require_noerr(err, exit);
  }
#ifdef USE_MICO_SPI_FLASH
  else if( peripheral->flash_type == FLASH_TYPE_SPI ){
    err = sflash_write( &sflash_handle, *start_address, data, length );
    require_noerr(err, exit);
    *start_address += length;
  }
#else
  #ifdef USE_QUAD_SPI_FLASH
  else if( peripheral->flash_type == FLASH_TYPE_QSPI ){
    err = qsflash_write( &sflash_handle, *start_address, data, length );
    require_noerr(err, exit);
    *start_address += length;
  }
  #endif
#endif
  else{
    err = kTypeErr;
    goto exit;
  }

exit:
  return err;
}

OSStatus platform_flash_read( const platform_flash_t *peripheral, volatile uint32_t* start_address, uint8_t* data ,uint32_t length  )
{
  OSStatus err = kNoErr;

  require_action_quiet( peripheral != NULL, exit, err = kParamErr);
  require_action( (*start_address >= peripheral->flash_start_addr) 
               && (*start_address + length) <= ( peripheral->flash_start_addr + peripheral->flash_length), exit, err = kParamErr);

  if( peripheral->flash_type == FLASH_TYPE_EMBEDDED ){
    memcpy(data, (void *)(*start_address), length);
    *start_address += length;
  }
#ifdef USE_MICO_SPI_FLASH
  else if( peripheral->flash_type == FLASH_TYPE_SPI ){
    err = sflash_read( &sflash_handle, *start_address, data, length );
    require_noerr(err, exit);
    *start_address += length;
  }
#else
  #ifdef USE_QUAD_SPI_FLASH
  else if( peripheral->flash_type == FLASH_TYPE_QSPI ){
    err = qsflash_read( &sflash_handle, *start_address, data, length );
    require_noerr(err, exit);
    *start_address += length;
  }
  #endif
#endif
  else{
    err = kTypeErr;
    goto exit;
  }

exit:
  return err;
}

OSStatus platform_flash_enable_protect( const platform_flash_t *peripheral, uint32_t start_address, uint32_t end_address )
{
  OSStatus err = kNoErr;

  require_action_quiet( peripheral != NULL, exit, err = kParamErr);
  require_action( start_address >= peripheral->flash_start_addr 
               && end_address   <= peripheral->flash_start_addr + peripheral->flash_length - 1, exit, err = kParamErr);

  if( peripheral->flash_type == FLASH_TYPE_EMBEDDED ){
#ifdef MCU_EBANLE_FLASH_PROTECT
    err = internalFlashProtect( start_address, end_address, true );  
#endif  
    require_noerr(err, exit);
  }
#ifdef USE_MICO_SPI_FLASH
  else if( peripheral->flash_type == FLASH_TYPE_SPI ){
    err = kNoErr;
    goto exit;
  }
#else
  #ifdef USE_QUAD_SPI_FLASH
  else if( peripheral->flash_type == FLASH_TYPE_QSPI ){
    err = kNoErr;
    goto exit;
  }
  #endif
#endif
  else{
    err = kTypeErr;
    goto exit;
  }

exit:
  return err;  
}

OSStatus platform_flash_disable_protect( const platform_flash_t *peripheral, uint32_t start_address, uint32_t end_address )
{
  OSStatus err = kNoErr;

  require_action_quiet( peripheral != NULL, exit, err = kParamErr);
  require_action( start_address >= peripheral->flash_start_addr 
               && end_address   <= peripheral->flash_start_addr + peripheral->flash_length - 1, exit, err = kParamErr);

  if( peripheral->flash_type == FLASH_TYPE_EMBEDDED ){
#ifdef MCU_EBANLE_FLASH_PROTECT
    err = internalFlashProtect( start_address, end_address, false );   
#endif 
    require_noerr(err, exit);
  }
#ifdef USE_MICO_SPI_FLASH
  else if( peripheral->flash_type == FLASH_TYPE_SPI ){
    err = kNoErr;
    goto exit;
  }
#else
  #ifdef USE_QUAD_SPI_FLASH
  else if( peripheral->flash_type == FLASH_TYPE_QSPI ){
    err = kNoErr;
    goto exit;
  }
  #endif
#endif
  else{
    err = kTypeErr;
    goto exit;
  }

exit:
  return err;  
}

OSStatus internalFlashInitialize( void )
{ 
  platform_log_trace();
  FLASH_Unlock(); 
  /* Clear pending flags (if any) */
  FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_OPERR | FLASH_FLAG_WRPERR | 
                  FLASH_FLAG_PGAERR | FLASH_FLAG_PGPERR|FLASH_FLAG_PGSERR);
  return kNoErr;
}

OSStatus internalFlashErase(uint32_t StartAddress, uint32_t EndAddress)
{
  platform_log_trace();
  OSStatus err = kNoErr;
  uint32_t StartSector, EndSector, i = 0, j = 0;
  
  /* Get the sector where start the user flash area */
  StartSector = _GetSector(StartAddress);
  EndSector = _GetSector(EndAddress);
  
  for(i = StartSector; i <= EndSector; i += 8)
  {
    /* Device voltage range supposed to be [2.7V to 3.6V], the operation will
    be done by word */
    _GetAddress(i, &StartAddress, &EndAddress);
    for(j=StartAddress; j<=EndAddress; j+=4){
      if( (*(uint32_t *)(j))!=0xFFFFFFFF )
        break;
    }
    if( j>EndAddress ) 
      continue;
    require_action(FLASH_EraseSector(i, VoltageRange_3) == FLASH_COMPLETE, exit, err = kWriteErr); 
  }
  
exit:
  return err;
}

#ifdef MCU_EBANLE_FLASH_PROTECT
OSStatus internalFlashProtect(uint32_t StartAddress, uint32_t EndAddress, bool enable)
{
  OSStatus err = kNoErr;
  uint16_t WRP = 0x0;
  uint32_t StartSector, EndSector, i = 0;
  bool needupdate = false;
  
  /* Get the sector where start the user flash area */
  StartSector = _GetWRPSector(StartAddress);
  EndSector = _GetWRPSector(EndAddress);
  
  for(i = StartSector; i <= EndSector; i=i<<1)
  {
    WRP = FLASH_OB_GetWRP();

    if( ( enable == true && (WRP & i) == 0x0 ) ||
        ( enable == false && (WRP & i) ) ) {
      continue;
    }
    if( needupdate == false){
      FLASH_OB_Unlock( );
      needupdate = true;
    }
    if( enable == true )
      FLASH_OB_WRPConfig( i, ENABLE );
    else
      FLASH_OB_WRPConfig( i, DISABLE );
  }
  
  if( needupdate == true){
    FLASH_OB_Launch( );
    FLASH_OB_Lock( );
  }

  return err;
}
#endif

#ifdef USE_MICO_SPI_FLASH
OSStatus spiFlashErase(uint32_t StartAddress, uint32_t EndAddress)
{
  platform_log_trace();
  OSStatus err = kNoErr;
  uint32_t StartSector, EndSector, i = 0;
  
  /* Get the sector where start the user flash area */
  StartSector = StartAddress>>12;
  EndSector = EndAddress>>12;
  
  for(i = StartSector; i <= EndSector; i += 1)
  {
    /* Device voltage range supposed to be [2.7V to 3.6V], the operation will
    be done by word */
    require_action(sflash_sector_erase(&sflash_handle, i<<12) == kNoErr, exit, err = kWriteErr); 
  }
  
exit:
  return err;
}
#endif

#ifdef USE_QUAD_SPI_FLASH
static OSStatus qspiFlashErase( uint32_t StartAddress, uint32_t EndAddress )
{
  platform_log_trace();
  OSStatus err = kNoErr;
  uint32_t StartSector, EndSector, i = 0;

  /* Get the sector where start the user flash area */
  StartSector = StartAddress>>12;
  EndSector = EndAddress>>12;

  for(i = StartSector; i <= EndSector; i += 1)
  {
    require_action(qsflash_sector_erase(i<<12) == kNoErr, exit, err = kWriteErr);
  }  

exit:
  return err;
}
#endif


OSStatus internalFlashWrite(volatile uint32_t* FlashAddress, uint32_t* Data ,uint32_t DataLength)
{
  platform_log_trace();
  OSStatus err = kNoErr;
  uint32_t i = 0;
  uint32_t dataInRam;
  uint8_t startNumber;
  uint32_t DataLength32 = DataLength;
  
  /*First bytes that are not 32bit align*/
  if(*FlashAddress%4){
    startNumber = 4-(*FlashAddress)%4;
    err = internalFlashByteWrite(FlashAddress, (uint8_t *)Data, startNumber);
    require_noerr(err, exit);
    DataLength32 = DataLength - startNumber;
    Data = (uint32_t *)((uint32_t)Data + startNumber);
  }
  
  /*Program flash by words*/
  for (i = 0; (i < DataLength32/4) && (*FlashAddress <= (FLASH_END_ADDRESS-3)); i++)
  {
    /* Device voltage range supposed to be [2.7V to 3.6V], the operation will
    be done by word */ 
    dataInRam = *(Data+i);
    require_action(FLASH_ProgramWord(*FlashAddress, dataInRam) == FLASH_COMPLETE, exit, err = kWriteErr); 
    require_action(*(uint32_t*)*FlashAddress == dataInRam, exit, err = kChecksumErr); 
    /* Increment FLASH destination address */
    *FlashAddress += 4;
  }
  
  /*Last bytes that cannot be write by a 32 bit word*/
  err = internalFlashByteWrite(FlashAddress, (uint8_t *)Data + i*4, DataLength32-i*4);
  require_noerr(err, exit);
  
exit:
  return err;
}

OSStatus internalFlashByteWrite(__IO uint32_t* FlashAddress, uint8_t* Data ,uint32_t DataLength)
{
  uint32_t i = 0;
  uint32_t dataInRam;
  OSStatus err = kNoErr;
  
  for (i = 0; (i < DataLength) && (*FlashAddress <= (FLASH_END_ADDRESS)); i++)
  {
    /* Device voltage range supposed to be [2.7V to 3.6V], the operation will
    be done by word */ 
    dataInRam = *(uint8_t*)(Data+i);
    
    require_action(FLASH_ProgramByte(*FlashAddress, dataInRam) == FLASH_COMPLETE, exit, err = kWriteErr); 
    require_action(*(uint8_t*)*FlashAddress == dataInRam, exit, err = kChecksumErr); 
    *FlashAddress +=1;
  }
  
exit:
  return err;
}

/**
* @brief  Gets the sector of a given address
* @param  Address: Flash address
* @retval The sector of a given address
*/
static uint32_t _GetSector(uint32_t Address)
{
  uint32_t sector = 0;
  
  if((Address < ADDR_FLASH_SECTOR_1) && (Address >= ADDR_FLASH_SECTOR_0))
  {
    sector = FLASH_Sector_0;  
  }
  else if((Address < ADDR_FLASH_SECTOR_2) && (Address >= ADDR_FLASH_SECTOR_1))
  {
    sector = FLASH_Sector_1;  
  }
  else if((Address < ADDR_FLASH_SECTOR_3) && (Address >= ADDR_FLASH_SECTOR_2))
  {
    sector = FLASH_Sector_2;  
  }
  else if((Address < ADDR_FLASH_SECTOR_4) && (Address >= ADDR_FLASH_SECTOR_3))
  {
    sector = FLASH_Sector_3;  
  }
  else if((Address < ADDR_FLASH_SECTOR_5) && (Address >= ADDR_FLASH_SECTOR_4))
  {
    sector = FLASH_Sector_4;  
  }
  else if((Address < ADDR_FLASH_SECTOR_6) && (Address >= ADDR_FLASH_SECTOR_5))
  {
    sector = FLASH_Sector_5;  
  }
  else if((Address < ADDR_FLASH_SECTOR_7) && (Address >= ADDR_FLASH_SECTOR_6))
  {
    sector = FLASH_Sector_6;  
  }
  else if((Address < ADDR_FLASH_SECTOR_8) && (Address >= ADDR_FLASH_SECTOR_7))
  {
    sector = FLASH_Sector_7;  
  }
  else if((Address < ADDR_FLASH_SECTOR_9) && (Address >= ADDR_FLASH_SECTOR_8))
  {
    sector = FLASH_Sector_8;  
  }
  else if((Address < ADDR_FLASH_SECTOR_10) && (Address >= ADDR_FLASH_SECTOR_9))
  {
    sector = FLASH_Sector_9;  
  }
  else if((Address < ADDR_FLASH_SECTOR_11) && (Address >= ADDR_FLASH_SECTOR_10))
  {
    sector = FLASH_Sector_10;  
  }
  else/*(Address < FLASH_END_ADDR) && (Address >= ADDR_FLASH_SECTOR_11))*/
  {
    sector = FLASH_Sector_11;  
  }
  return sector;
}


#ifdef MCU_EBANLE_FLASH_PROTECT
/**
* @brief  Gets the sector of a given address
* @param  Address: Flash address
* @retval The sector of a given address
*/
static uint32_t _GetWRPSector(uint32_t Address)
{
  uint32_t sector = 0;
  
  if((Address < ADDR_FLASH_SECTOR_1) && (Address >= ADDR_FLASH_SECTOR_0))
  {
    sector = OB_WRP_Sector_0;  
  }
  else if((Address < ADDR_FLASH_SECTOR_2) && (Address >= ADDR_FLASH_SECTOR_1))
  {
    sector = OB_WRP_Sector_1;  
  }
  else if((Address < ADDR_FLASH_SECTOR_3) && (Address >= ADDR_FLASH_SECTOR_2))
  {
    sector = OB_WRP_Sector_2;  
  }
  else if((Address < ADDR_FLASH_SECTOR_4) && (Address >= ADDR_FLASH_SECTOR_3))
  {
    sector = OB_WRP_Sector_3;  
  }
  else if((Address < ADDR_FLASH_SECTOR_5) && (Address >= ADDR_FLASH_SECTOR_4))
  {
    sector = OB_WRP_Sector_4;  
  }
  else if((Address < ADDR_FLASH_SECTOR_6) && (Address >= ADDR_FLASH_SECTOR_5))
  {
    sector = OB_WRP_Sector_5;  
  }
  else if((Address < ADDR_FLASH_SECTOR_7) && (Address >= ADDR_FLASH_SECTOR_6))
  {
    sector = OB_WRP_Sector_6;  
  }
  else if((Address < ADDR_FLASH_SECTOR_8) && (Address >= ADDR_FLASH_SECTOR_7))
  {
    sector = OB_WRP_Sector_7;  
  }
  else if((Address < ADDR_FLASH_SECTOR_9) && (Address >= ADDR_FLASH_SECTOR_8))
  {
    sector = OB_WRP_Sector_8;  
  }
  else if((Address < ADDR_FLASH_SECTOR_10) && (Address >= ADDR_FLASH_SECTOR_9))
  {
    sector = OB_WRP_Sector_9;  
  }
  else if((Address < ADDR_FLASH_SECTOR_11) && (Address >= ADDR_FLASH_SECTOR_10))
  {
    sector = OB_WRP_Sector_10;  
  }
  else/*(Address < FLASH_END_ADDR) && (Address >= ADDR_FLASH_SECTOR_11))*/
  {
    sector = OB_WRP_Sector_11;  
  }
  return sector;
}
#endif

/**
* @brief  Gets the address of a given sector
* @param  Sector: The sector of a given address
* @retval Flash address if the sector start
*/
static OSStatus _GetAddress(uint32_t sector, uint32_t *startAddress, uint32_t *endAddress)
{
  OSStatus err = kNoErr; 
  if(sector == FLASH_Sector_0)
  {
    *startAddress = ADDR_FLASH_SECTOR_0;
    *endAddress = ADDR_FLASH_SECTOR_1 - 1;
  }
  else if(sector == FLASH_Sector_1)
  {
    *startAddress = ADDR_FLASH_SECTOR_1;
    *endAddress = ADDR_FLASH_SECTOR_2 - 1;
  }
  else if(sector == FLASH_Sector_2)
  {
    *startAddress = ADDR_FLASH_SECTOR_2;
    *endAddress = ADDR_FLASH_SECTOR_3 - 1;
  }
  else if(sector == FLASH_Sector_3)
  {
    *startAddress = ADDR_FLASH_SECTOR_3;
    *endAddress = ADDR_FLASH_SECTOR_4 - 1;
  }
  else if(sector == FLASH_Sector_4)
  {
    *startAddress = ADDR_FLASH_SECTOR_4;
    *endAddress = ADDR_FLASH_SECTOR_5 - 1;
  }
  else if(sector == FLASH_Sector_5)
  {
    *startAddress = ADDR_FLASH_SECTOR_5;
    *endAddress = ADDR_FLASH_SECTOR_6 - 1;
  }
  else if(sector == FLASH_Sector_6)
  {
    *startAddress = ADDR_FLASH_SECTOR_6;
    *endAddress = ADDR_FLASH_SECTOR_7 - 1;
  }
  else if(sector == FLASH_Sector_7)
  {
    *startAddress = ADDR_FLASH_SECTOR_7;
    *endAddress = ADDR_FLASH_SECTOR_8 - 1;
  }
  else if(sector == FLASH_Sector_8)
  {
    *startAddress = ADDR_FLASH_SECTOR_8;
    *endAddress = ADDR_FLASH_SECTOR_9 - 1;
  }
  else if(sector == FLASH_Sector_9)
  {
    *startAddress = ADDR_FLASH_SECTOR_9;
    *endAddress = ADDR_FLASH_SECTOR_10 - 1;
  }
  else if(sector == FLASH_Sector_10)
  {
    *startAddress = ADDR_FLASH_SECTOR_10;
    *endAddress = ADDR_FLASH_SECTOR_11 - 1;
  }
  else if(sector == FLASH_Sector_11)
  {
    *startAddress = ADDR_FLASH_SECTOR_11;
    *endAddress = FLASH_END_ADDRESS - 1;
  }
  else
    err = kNotFoundErr;
  
  return err;
}

#ifdef USE_QUAD_SPI_FLASH
/**
  * @brief Init qspi flash
  * @param None
  * @retval result
  */
int init_qsflash( /*@out@*/ const platform_qspi_t* peripheral, uint32_t flash_length, /*@out@*/ sflash_handle_t* const handle, sflash_write_allowed_t write_allowed_in )
{
  int status;
  device_id_t tmp_device_id;
  QSPI_InitTypeDef  QSPI_InitStructure;
  
  /* Configure QSPI GPIO */
  QSPI_GPIO_Config( peripheral );
  
  /* Initialize QuadSPI ------------------------------------------------------*/
  QSPI_StructInit(&QSPI_InitStructure);
  QSPI_InitStructure.QSPI_SShift    = QSPI_SShift_HalfCycleShift;
  QSPI_InitStructure.QSPI_Prescaler = 0x01; /* 90 MHZ */
  QSPI_InitStructure.QSPI_CKMode    = QSPI_CKMode_Mode0;
  QSPI_InitStructure.QSPI_CSHTime   = QSPI_CSHTime_2Cycle;
  QSPI_InitStructure.QSPI_FSize     = get_power(flash_length)-1; /* 2M Byte 0x14*/
  QSPI_InitStructure.QSPI_FSelect   = peripheral->FSelect;/* CHOOSE FLASH 2 */
  QSPI_InitStructure.QSPI_DFlash    = QSPI_DFlash_Disable;
  QSPI_Init(&QSPI_InitStructure);

#ifdef USE_QUAD_SPI_DMA
  /* Initialize DMA ----------------------------------------------------------*/
  DMA_StructInit( &DMA_InitStructure );
  DMA_DeInit( peripheral->dma.stream );
  
  /*DMA configuration*/
  DMA_InitStructure.DMA_BufferSize         = 0;
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
  DMA_InitStructure.DMA_MemoryDataSize     = DMA_MemoryDataSize_Byte;
  DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&(peripheral->port->DR) ;
  DMA_InitStructure.DMA_PeripheralInc      = DMA_PeripheralInc_Disable;
  DMA_InitStructure.DMA_MemoryInc          = DMA_MemoryInc_Enable;
  DMA_InitStructure.DMA_Mode               = DMA_Mode_Normal;
  DMA_InitStructure.DMA_Channel            = peripheral->dma.channel;
  DMA_InitStructure.DMA_Memory0BaseAddr = 0;
  DMA_InitStructure.DMA_DIR             = DMA_DIR_MemoryToPeripheral;
  DMA_InitStructure.DMA_Priority        = DMA_Priority_VeryHigh;          
  DMA_Init( peripheral->dma.stream, &DMA_InitStructure );
#endif
  
  /* Initialize Command Config -----------------------------------------------*/
  QSPI_ComConfig_StructInit(&QSPI_ComConfig_InitStructure);
  QSPI_ComConfig_InitStructure.QSPI_ComConfig_ADSize      = QSPI_ComConfig_ADSize_24bit;
  QSPI_ComConfig_InitStructure.QSPI_ComConfig_IMode       = QSPI_ComConfig_IMode_1Line;
  QSPI_ComConfig_InitStructure.QSPI_ComConfig_ABMode      = QSPI_ComConfig_ABMode_NoAlternateByte;
  QSPI_ComConfig_InitStructure.QSPI_ComConfig_DDRMode     = QSPI_ComConfig_DDRMode_Disable;
  QSPI_ComConfig_InitStructure.QSPI_ComConfig_SIOOMode    = QSPI_ComConfig_SIOOMode_Disable;
  QSPI_ComConfig_InitStructure.QSPI_ComConfig_DHHC        = QSPI_ComConfig_DHHC_Enable;
  QSPI_ComConfig_StructInit(&QSPI_ComConfig_InitStructure);
  QSPI_Cmd(ENABLE);

  if ( write_allowed_in == SFLASH_WRITE_ALLOWED )
  {
    /* Enable write operations ---------------------------------------------*/
    QSPI_WriteEnable();
    QSPI_QEEnable();
  }

  /*Delay wait to QE Mode*/

  handle->write_allowed = write_allowed_in;
  /* Read Flash ID */
  status = qsflash_read_ID( handle, &tmp_device_id );
  
  if( status != 0 )
  {
    return status;
  }

  handle->device_id = ( ((uint32_t) tmp_device_id.id[0]) << 16 ) +
                      ( ((uint32_t) tmp_device_id.id[1]) <<  8 ) +
                      ( ((uint32_t) tmp_device_id.id[2]) <<  0 );
  return 0;
}

/**
  * @brief  read block of data to the FLASH.
  *         
  * @param  data_addr: pointer to the buffer  containing the data to be written
  *         to the FLASH.
  * @param  device_address: FLASH's internal address to write to.
  * @param  size: number of bytes to write to the FLASH.
  * @retval None
  */
int qsflash_read( const sflash_handle_t* const handle, unsigned long device_address, void* const data_addr, unsigned int size )
{
  uint8_t* pAddr;
  
  /* Reading Sequence ----------------------------------------------------*/
  QSPI_Cmd(ENABLE);
  QSPI_TimeoutCounterCmd(DISABLE);
  QSPI_MemoryMappedMode_SetTimeout(0);
  QSPI_ComConfig_InitStructure.QSPI_ComConfig_ADSize       = QSPI_ComConfig_ADSize_24bit;
  QSPI_ComConfig_InitStructure.QSPI_ComConfig_FMode        = QSPI_ComConfig_FMode_Memory_Mapped;
  QSPI_ComConfig_InitStructure.QSPI_ComConfig_ADMode       = QSPI_ComConfig_ADMode_4Line;
  QSPI_ComConfig_InitStructure.QSPI_ComConfig_IMode        = QSPI_ComConfig_IMode_1Line;
  QSPI_ComConfig_InitStructure.QSPI_ComConfig_DMode        = QSPI_ComConfig_DMode_4Line;
  QSPI_ComConfig_InitStructure.QSPI_ComConfig_Ins          = SFLASH_QUAD_READ;
  QSPI_ComConfig_InitStructure.QSPI_ComConfig_DummyCycles  = DUMMY_CLOCK_CYCLES_READ_QUAD;
  QSPI_ComConfig_Init(&QSPI_ComConfig_InitStructure);  
  
  pAddr = (uint8_t*) qspiaddr + device_address;
  memcpy(data_addr, (void *)(pAddr), size);
  
  QSPI_AbortRequest();
  QSPI_Cmd(DISABLE);
  
  return 0;
}


/**
  * @brief  Writes block of data to the FLASH. In this function, the number of
  *         WRITE cycles are reduced, using Page WRITE sequence.
  * @param  data_addr: pointer to the buffer  containing the data to be written
  *         to the FLASH.
  * @param  device_address: FLASH's internal address to write to.
  * @param  size: number of bytes to write to the FLASH.
  * @retval None
  */
int qsflash_write( const sflash_handle_t* const handle, unsigned long device_address, const void* const data_addr, unsigned int size )
{
  int status = 0;
  uint8_t NumOfPage = 0, NumOfSingle = 0, Addr = 0, count = 0, temp = 0;
  unsigned char* data_addr_ptr = (unsigned char*) data_addr;

  Addr = device_address % sFLASH_SPI_PAGESIZE;
  count = sFLASH_SPI_PAGESIZE - Addr;
  NumOfPage =  size / sFLASH_SPI_PAGESIZE;
  NumOfSingle = size % sFLASH_SPI_PAGESIZE;

  if (Addr == 0) /*!< WriteAddr is sFLASH_PAGESIZE aligned  */
  {
    if (NumOfPage == 0) /*!< NumByteToWrite < sFLASH_PAGESIZE */
    {
      status = qsflash_write_page( handle, device_address, data_addr_ptr, size );
      //sFLASH_WritePage(pBuffer, WriteAddr, NumByteToWrite);
    }
    else /*!< NumByteToWrite > sFLASH_PAGESIZE */
    {
      while (NumOfPage--)
      {
        status = qsflash_write_page( handle, device_address, data_addr_ptr, sFLASH_SPI_PAGESIZE );
        //sFLASH_WritePage(pBuffer, WriteAddr, sFLASH_SPI_PAGESIZE);
        device_address +=  sFLASH_SPI_PAGESIZE;
        data_addr_ptr += sFLASH_SPI_PAGESIZE;
      }
      if( NumOfSingle != 0 )
      {
        status = qsflash_write_page( handle, device_address, data_addr_ptr, NumOfSingle);
      }
    }
  }
  else /*!< WriteAddr is not sFLASH_PAGESIZE aligned  */
  {
    if (NumOfPage == 0) /*!< NumByteToWrite < sFLASH_PAGESIZE */
    {
      if (NumOfSingle > count) /*!< (NumByteToWrite + WriteAddr) > sFLASH_PAGESIZE */
      {
        temp = NumOfSingle - count;

        status = qsflash_write_page( handle, device_address, data_addr_ptr, count );
        //sFLASH_WritePage(pBuffer, WriteAddr, count);
        device_address +=  count;
        data_addr_ptr += count;

        status = qsflash_write_page( handle, device_address, data_addr_ptr, temp);
      }
      else
      {
        status = qsflash_write_page(handle, device_address, data_addr_ptr, size);
      }
    }
    else /*!< NumByteToWrite > sFLASH_PAGESIZE */
    {
      size -= count;
      NumOfPage =  size / sFLASH_SPI_PAGESIZE;
      NumOfSingle = size % sFLASH_SPI_PAGESIZE;

      status = qsflash_write_page( handle, device_address, data_addr_ptr, count );
      //sFLASH_WritePage(pBuffer, WriteAddr, count);
      device_address +=  count;
      data_addr_ptr += count;

      while (NumOfPage--)
      {
        status = qsflash_write_page( handle, device_address, data_addr_ptr, sFLASH_SPI_PAGESIZE );
        //sFLASH_WritePage(pBuffer, WriteAddr, sFLASH_SPI_PAGESIZE);
        device_address +=  sFLASH_SPI_PAGESIZE;
        data_addr_ptr += sFLASH_SPI_PAGESIZE;
      }

      if (NumOfSingle != 0)
      {
        status = qsflash_write_page( handle, device_address, data_addr_ptr, NumOfSingle );
        //sFLASH_WritePage(pBuffer, WriteAddr, NumOfSingle);
      }
    }
  }
  return status;
}

/**
  * @brief  Writes block of data to the FLASH. In this function, the number of
  *         WRITE cycles are reduced, using Page WRITE sequence.
  * @param  data_addr: pointer to the buffer  containing the data to be written
  *         to the FLASH.
  * @param  device_address: FLASH's internal address to write to.
  * @param  size: number of bytes to write to the FLASH.
  * @retval None
  */
int qsflash_write_page( const sflash_handle_t* const handle, unsigned long device_address, const void* const data_addr, unsigned int size )
{

#ifdef USE_QUAD_SPI_DMA
  const platform_qspi_t* peripheral = &platform_qspi_peripherals[MICO_QSPI_1];
#else
  uint8_t* buffer = NULL;
#endif
  if ( handle->write_allowed == SFLASH_WRITE_ALLOWED )
  {
  }
  else
  {
    return -1;
  }
  /* Enable write operations ---------------------------------------------*/
  QSPI_Cmd(ENABLE);
  QSPI_WriteEnable();
#ifdef USE_QUAD_SPI_DMA
  QSPI_DMACmd(ENABLE);
#endif
      
  /* Writing Sequence ----------------------------------------------------*/
  QSPI_SetDataLength(size-1);
  
  QSPI_ComConfig_StructInit(&QSPI_ComConfig_InitStructure); 
  QSPI_ComConfig_InitStructure.QSPI_ComConfig_IMode       = QSPI_ComConfig_IMode_1Line;
  QSPI_ComConfig_InitStructure.QSPI_ComConfig_ADMode      = QSPI_ComConfig_ADMode_4Line;
  QSPI_ComConfig_InitStructure.QSPI_ComConfig_DMode       = QSPI_ComConfig_DMode_4Line;
  QSPI_ComConfig_InitStructure.QSPI_ComConfig_FMode       = QSPI_ComConfig_FMode_Indirect_Write;
  QSPI_ComConfig_InitStructure.QSPI_ComConfig_ADSize      = QSPI_ComConfig_ADSize_24bit; 
  QSPI_ComConfig_InitStructure.QSPI_ComConfig_Ins         = SFLASH_QUAD_WRITE;
  QSPI_ComConfig_InitStructure.QSPI_ComConfig_DummyCycles = 0;
  QSPI_ComConfig_Init(&QSPI_ComConfig_InitStructure); 
  QSPI_SetAddress(device_address);
  
#ifdef USE_QUAD_SPI_DMA
  /*DMA configuration*/
  peripheral->dma.stream->M0AR = (uint32_t)data_addr;
  peripheral->dma.stream->NDTR = size;
  DMA_Cmd( peripheral->dma.stream, ENABLE );

  /* Wait for the end of Transfer */

  while(DMA_GetFlagStatus(peripheral->dma.stream, peripheral->dma.complete_flags) == RESET)
  {}  
  DMA_ClearFlag(peripheral->dma.stream, peripheral->dma.complete_flags);
  
  DMA_Cmd(peripheral->dma.stream, DISABLE);
  
  QSPI_DMACmd(DISABLE);
#else
  /* Send data */
  buffer = (uint8_t *)data_addr;
  while(size--)
  {
    if(QSPI_GetFlagStatus(QSPI_FLAG_FT))
      QSPI_SendData8(*buffer++);
  }
#endif

  /* Configure automatic polling mode to wait for end of program ---------*/   
  QSPI_AutoPollingMemReady();
  QSPI_Cmd(DISABLE);

  return 0;
}

/**
  * @brief Erase the selected sector through device_address.
  * @param None
  * @retval None
  */
int qsflash_sector_erase ( unsigned long device_address )
{
  /* Enable write operations ---------------------------------------------*/
  QSPI_Cmd(ENABLE);
  QSPI_WriteEnable();
      
  /* Erasing Sequence ----------------------------------------------------*/
  QSPI_ComConfig_StructInit(&QSPI_ComConfig_InitStructure); 
  QSPI_ComConfig_InitStructure.QSPI_ComConfig_ADSize = QSPI_ComConfig_ADSize_24bit;
  QSPI_ComConfig_InitStructure.QSPI_ComConfig_IMode  = QSPI_ComConfig_IMode_1Line;
  QSPI_ComConfig_InitStructure.QSPI_ComConfig_ADMode = QSPI_ComConfig_ADMode_1Line;
  QSPI_ComConfig_InitStructure.QSPI_ComConfig_DMode  = QSPI_ComConfig_DMode_NoData;
  QSPI_ComConfig_InitStructure.QSPI_ComConfig_FMode  = QSPI_ComConfig_FMode_Indirect_Write;
  QSPI_ComConfig_InitStructure.QSPI_ComConfig_Ins    = SFLASH_SECTOR_ERASE;
  QSPI_ComConfig_Init(&QSPI_ComConfig_InitStructure);
      
  /* Set sector address to erase */
  QSPI_SetAddress(device_address);  
      
  while(QSPI_GetFlagStatus(QSPI_FLAG_TC) == RESET)
  {}

  /* Configure automatic polling mode to wait for end of erase -----------*/  
  QSPI_AutoPollingMemReady();
  QSPI_Cmd(DISABLE);
  
  return 0;
}

/**
  * @brief Read qspi flash ID
  * @param None
  * @retval result
  */
int qsflash_read_ID( sflash_handle_t* const handle, void* const data_addr )
{
  uint8_t* pAddr;
  /* Reading Sequence ----------------------------------------------------*/
  QSPI_TimeoutCounterCmd(DISABLE);
  QSPI_MemoryMappedMode_SetTimeout(0);
  QSPI_ComConfig_InitStructure.QSPI_ComConfig_ADSize       = QSPI_ComConfig_ADSize_24bit;
  QSPI_ComConfig_InitStructure.QSPI_ComConfig_FMode        = QSPI_ComConfig_FMode_Memory_Mapped;
  QSPI_ComConfig_InitStructure.QSPI_ComConfig_ADMode       = QSPI_ComConfig_ADMode_NoAddress;
  QSPI_ComConfig_InitStructure.QSPI_ComConfig_IMode        = QSPI_ComConfig_IMode_1Line;
  QSPI_ComConfig_InitStructure.QSPI_ComConfig_DMode        = QSPI_ComConfig_DMode_1Line;
  QSPI_ComConfig_InitStructure.QSPI_ComConfig_Ins          = SFLASH_READ_JEDEC_ID;
  QSPI_ComConfig_Init(&QSPI_ComConfig_InitStructure);  
      
  pAddr = (uint8_t*) qspiaddr;
  for( int i=0; i<3; i++ )
  {
    ((uint8_t*)data_addr)[i] = *pAddr++;
  } 
  QSPI_AbortRequest();
  QSPI_Cmd(DISABLE);
  
  return 0;
}

/**
  * @brief Configures the QSPI GPIO Pins.
  * @param None
  * @retval None
  */
static void QSPI_GPIO_Config(/*@out@*/ const platform_qspi_t* peripheral)
{
  
  /* Peripheral Clock Enable -------------------------------------------------*/
  /* Enable the QSPI clock */
  (peripheral->peripheral_clock_func)( peripheral->peripheral_clock_reg, ENABLE );
  
  /* Enable DMA1/2 AHB1 clock */
#ifdef USE_QUAD_SPI_DMA
  if( peripheral->dma.controller == DMA1 )
  {
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1, ENABLE);
  }
  else
  {
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2, ENABLE);
  }
#endif
  
  /* QSPI GPIO Configuration -------------------------------------------------*/
  platform_gpio_set_alternate_function( peripheral->pin_clock->port, peripheral->pin_clock->pin_number, GPIO_OType_PP, GPIO_PuPd_NOPULL, peripheral->gpio_af_clk );
  platform_gpio_set_alternate_function( peripheral->pin_d0->port, peripheral->pin_d0->pin_number, GPIO_OType_PP, GPIO_PuPd_NOPULL, peripheral->gpio_af_d0 );
  platform_gpio_set_alternate_function( peripheral->pin_d1->port, peripheral->pin_d1->pin_number, GPIO_OType_PP, GPIO_PuPd_NOPULL, peripheral->gpio_af_d1 );
  platform_gpio_set_alternate_function( peripheral->pin_d2->port, peripheral->pin_d2->pin_number, GPIO_OType_PP, GPIO_PuPd_NOPULL, peripheral->gpio_af_d2 );
  platform_gpio_set_alternate_function( peripheral->pin_d3->port, peripheral->pin_d3->pin_number, GPIO_OType_PP, GPIO_PuPd_NOPULL, peripheral->gpio_af_d3 );
  platform_gpio_set_alternate_function( peripheral->pin_cs->port, peripheral->pin_cs->pin_number, GPIO_OType_PP, GPIO_PuPd_UP, peripheral->gpio_af_cs );
  
  /* QSPI CS pin set high */
  platform_gpio_output_high( peripheral->pin_cs );
}

/**
  * @brief  This function read the SR of the memory and wait the end of Command.
  * @param  None
  * @retval None
  */
static void QSPI_AutoPollingMemReady(void)
{
  /* Wait for end of Cmd */
  while(QSPI_GetFlagStatus(QSPI_FLAG_BUSY) != RESET)
  {}
  QSPI_SetDataLength(0x00);
  QSPI_AutoPollingMode_Config(0x0000, 0x0101, QSPI_PMM_AND);
  QSPI_AutoPollingModeStopCmd(ENABLE);
  QSPI_SetDataLength(0);
  
  /* Reconfigure QSPI to automatic polling mode to wait for end of Command*/
  QSPI_ComConfig_InitStructure.QSPI_ComConfig_FMode  = QSPI_ComConfig_FMode_Auto_Polling;
  QSPI_ComConfig_InitStructure.QSPI_ComConfig_ADMode = QSPI_ComConfig_ADMode_NoAddress;
  QSPI_ComConfig_InitStructure.QSPI_ComConfig_DMode  = QSPI_ComConfig_DMode_1Line;
  QSPI_ComConfig_InitStructure.QSPI_ComConfig_Ins    = SFLASH_READ_STATUS_REGISTER;
  QSPI_ComConfig_Init(&QSPI_ComConfig_InitStructure); 
  
  while(QSPI_GetFlagStatus(QSPI_FLAG_SM) == RESET)
  {}
  
  QSPI_ClearFlag(QSPI_FLAG_SM);  
  QSPI_ClearFlag(QSPI_FLAG_TC);
}

/**
  * @brief  This function send a Write Enable and wait it is effective.
  * @param  None
  * @retval None
  */
static void QSPI_WriteEnable(void)
{
  /* Command Config for Write Enable Operation*/
  QSPI_ComConfig_InitStructure.QSPI_ComConfig_DHHC        = QSPI_ComConfig_DHHC_Disable;
  QSPI_ComConfig_InitStructure.QSPI_ComConfig_DDRMode     = QSPI_ComConfig_DDRMode_Disable;
  QSPI_ComConfig_InitStructure.QSPI_ComConfig_SIOOMode    = QSPI_ComConfig_SIOOMode_Disable;
  QSPI_ComConfig_InitStructure.QSPI_ComConfig_ABSize      = QSPI_ComConfig_ABSize_8bit;
  QSPI_ComConfig_InitStructure.QSPI_ComConfig_ADSize      = QSPI_ComConfig_ADSize_24bit;
  QSPI_ComConfig_InitStructure.QSPI_ComConfig_DMode       = QSPI_ComConfig_DMode_NoData;
  QSPI_ComConfig_InitStructure.QSPI_ComConfig_ADMode      = QSPI_ComConfig_ADMode_NoAddress;
  QSPI_ComConfig_InitStructure.QSPI_ComConfig_ABMode      = QSPI_ComConfig_ABMode_NoAlternateByte;
  QSPI_ComConfig_InitStructure.QSPI_ComConfig_IMode       = QSPI_ComConfig_IMode_1Line;
  QSPI_ComConfig_InitStructure.QSPI_ComConfig_FMode       = QSPI_ComConfig_FMode_Indirect_Write;
  QSPI_ComConfig_InitStructure.QSPI_ComConfig_Ins         = SFLASH_WRITE_ENABLE ;
  QSPI_ComConfig_InitStructure.QSPI_ComConfig_DummyCycles = 0;
  QSPI_ComConfig_Init(&QSPI_ComConfig_InitStructure);
  
  /* Switch to Autopolling mode for the end of the Command */
  while(QSPI_GetFlagStatus(QSPI_FLAG_BUSY) != RESET)
  {}
  QSPI_AutoPollingMode_SetInterval(0x10);
  QSPI_AutoPollingMode_Config(0x02, 0x02, QSPI_PMM_AND);
  QSPI_AutoPollingModeStopCmd(ENABLE);
  QSPI_SetDataLength(0x00);
  
  QSPI_ComConfig_InitStructure.QSPI_ComConfig_FMode  = QSPI_ComConfig_FMode_Auto_Polling;
  QSPI_ComConfig_InitStructure.QSPI_ComConfig_ADMode = QSPI_ComConfig_ADMode_NoAddress;
  QSPI_ComConfig_InitStructure.QSPI_ComConfig_DMode  = QSPI_ComConfig_DMode_1Line;
  QSPI_ComConfig_InitStructure.QSPI_ComConfig_Ins    = SFLASH_READ_STATUS_REGISTER;
  QSPI_ComConfig_Init(&QSPI_ComConfig_InitStructure); 
  
  while(QSPI_GetFlagStatus(QSPI_FLAG_SM) == RESET)
  {}
  QSPI_ClearFlag(QSPI_FLAG_SM);
  QSPI_ClearFlag(QSPI_FLAG_TC);
  while(QSPI_GetFlagStatus(QSPI_FLAG_BUSY) != RESET)
  {}  
}

/**
 * @brief  This function send a Quad Enable and wait it is effective.
 * @param  None
 * @retval None
 */
static void QSPI_QEEnable(void)
{
    uint8_t status_register = 0x40; //Quad Enable bit 6
    /* Command Config for Write Status Register Operation*/
    QSPI_ComConfig_InitStructure.QSPI_ComConfig_ADMode = QSPI_ComConfig_ADMode_NoAddress;
    QSPI_ComConfig_InitStructure.QSPI_ComConfig_ABMode = QSPI_ComConfig_ABMode_NoAlternateByte;
    QSPI_ComConfig_InitStructure.QSPI_ComConfig_DMode = QSPI_ComConfig_DMode_1Line;
    QSPI_ComConfig_InitStructure.QSPI_ComConfig_IMode = QSPI_ComConfig_IMode_1Line;
    QSPI_ComConfig_InitStructure.QSPI_ComConfig_FMode = QSPI_ComConfig_FMode_Indirect_Write;
    QSPI_ComConfig_InitStructure.QSPI_ComConfig_Ins = SFLASH_WRITE_STATUS_REGISTER;
    QSPI_ComConfig_InitStructure.QSPI_ComConfig_DummyCycles = 0;
    QSPI_ComConfig_Init(&QSPI_ComConfig_InitStructure);

    QUADSPI->DR = status_register;

    /* Switch to Autopolling mode for the end of the Command */
    while(QSPI_GetFlagStatus(QSPI_FLAG_BUSY) != RESET)
    {}
    QSPI_AutoPollingMode_SetInterval(0x10);
    QSPI_AutoPollingMode_Config(0x40, 0x40, QSPI_PMM_AND);
    QSPI_AutoPollingModeStopCmd(ENABLE);
    QSPI_SetDataLength(0x00);

    QSPI_ComConfig_InitStructure.QSPI_ComConfig_FMode = QSPI_ComConfig_FMode_Auto_Polling;
    QSPI_ComConfig_InitStructure.QSPI_ComConfig_ADMode = QSPI_ComConfig_ADMode_NoAddress;
    QSPI_ComConfig_InitStructure.QSPI_ComConfig_DMode = QSPI_ComConfig_DMode_1Line;
    QSPI_ComConfig_InitStructure.QSPI_ComConfig_Ins = SFLASH_READ_STATUS_REGISTER;
    QSPI_ComConfig_Init(&QSPI_ComConfig_InitStructure);

    while(QSPI_GetFlagStatus(QSPI_FLAG_SM) == RESET)
    {}
    QSPI_ClearFlag(QSPI_FLAG_SM);
    QSPI_ClearFlag(QSPI_FLAG_TC);
    while(QSPI_GetFlagStatus(QSPI_FLAG_BUSY) != RESET)
    {}
}


int get_power(uint32_t number)
{
  int count = 0;
  while(1)
  {
    if(number>>=1) count++;
    else break;
  }
  return count;
}



#endif
