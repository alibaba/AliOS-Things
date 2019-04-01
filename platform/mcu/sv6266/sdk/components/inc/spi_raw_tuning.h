#ifndef SPI_RAW_TUNING_H
#define SPI_RAW_TUNING_H

/*******************************************************/
/*               SPI_RAW_DATA OPTION                   */
/*******************************************************/
#define SPI_RAW_TUNING_BUS_PRIORITY             (1)
#define SPI_RAW_TUNING_CACHE_DYNAMIC            (1)
/*******************************************************/


#define SPI_RAW_TARGET_DLM                      (1)
#define SPI_RAW_TUNING_FLASH_HIGH_PROFORMANCE   (1)
#if (SPI_RAW_TUNING_FLASH_HIGH_PROFORMANCE == 1)
#define SPI_RAW_TUNING_FLASH_PREFETCH           (1)
#endif

#define __nds32__standby_wait_done() \
  (__builtin_nds32_standby_wait_done())
#define __nds32__standby_no_wake_grant() \
  (__builtin_nds32_standby_no_wake_grant())
#define __nds32__standby_wake_grant() \
  (__builtin_nds32_standby_wake_grant())

#endif /* end of include guard: SPI_RAW_TUNING_H */


