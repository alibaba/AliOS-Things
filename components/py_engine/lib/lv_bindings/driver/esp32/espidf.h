/**
 * This file defines the Micorpython API to ESP-IDF
 * It is used as input to gen_mpy.py to create a micropython module
 **/ 
#if __has_include("esp_idf_version.h")
#   include "esp_idf_version.h"
#endif

// Disable some macros and includes that make pycparser choke

#ifdef PYCPARSER
#define __attribute__(x)
#define _Static_assert(x,y)
#define __extension__
#define _SOC_IO_MUX_REG_H_
#define _SYS_REENT_H_
#define PORTMACRO_H
#define PORTABLE_H
#define INC_FREERTOS_H
#define QUEUE_H
#define SEMAPHORE_H
#define XTENSA_HAL_H
#define _SOC_I2S_STRUCT_H_
#define XTRUNTIME_H
#define _SOC_SPI_STRUCT_H_
#define _SOC_RTC_CNTL_STRUCT_H_
#define __XTENSA_API_H__
#define _SOC_GPIO_STRUCT_H_
#define _SOC_RTC_IO_STRUCT_H_
#define _SOC_PCNT_STRUCT_H_
#define _SYS_FCNTL_H_
#define __SYS_ARCH_H__
#define LIST_H
#define INC_TASK_H
#define LWIP_HDR_NETIF_H
#define ESP_EVENT_H_
#define __SNTP_H__
#define XTENSA_CONFIG_CORE_H
#define _SOC_SPI_MEM_STRUCT_H_

typedef int	BaseType_t;
typedef unsigned int	UBaseType_t;
typedef void* system_event_t;
typedef void *intr_handle_t;

// Exclude SOC just because it contains large structs that don't interest the user
#define _SOC_SPI_PERIPH_H_
typedef void *spi_dev_t;

// TODO: Check why lldesc_t causes inifinite recursion on gen_mpy.py 
#define _ROM_LLDESC_H_
typedef void *lldesc_t; 

// FreeRTOS definitions we want available on Micropython
#include <stdint.h>
typedef uint32_t TickType_t;
typedef void * TaskHandle_t;
static inline uint32_t xPortGetCoreID();
UBaseType_t uxTaskPriorityGet( TaskHandle_t xTask );

// Micropython specific types
typedef void *mp_obj_t;

static inline void SPH0645_WORKAROUND(int i2s_num);
static inline void get_ccount(int *ccount);

// Memory management helper functions
void * memcpy ( void * destination, const void * source, size_t num );
void * memset ( void * ptr, int value, size_t num );


#else // PYCPARSER


/////////////////////////////////////////////////////////////////////////////////////////////
// A workaround for SPH0645 I2S, see:
// - https://hackaday.io/project/162059-street-sense/log/160705-new-i2s-microphone/discussion-124677
// - https://www.esp32.com/viewtopic.php?t=4997#p45366
// Since reg access is based on macros, this cannot currently be directly implemented in Micropython

#include "soc/i2s_reg.h" // for SPH0645_WORKAROUND

static inline void SPH0645_WORKAROUND(int i2s_num)
{
    REG_SET_BIT( I2S_TIMING_REG(i2s_num), BIT(9));
    REG_SET_BIT( I2S_CONF_REG(i2s_num), I2S_RX_MSB_SHIFT);
}

/////////////////////////////////////////////////////////////////////////////////////////////
// Helper function to measure CPU cycles
//
static inline void get_ccount(int *ccount)
{
	asm volatile("rsr.ccount %0" : "=a"(*ccount));
}


#endif //PYCPARSER

// The following includes are the source of the esp-idf micropython module.
// All included files are API we want to include in the module

#if defined(ESP_IDF_VERSION_MAJOR) && ESP_IDF_VERSION_MAJOR >= 4
#   if CONFIG_IDF_TARGET_ESP32
#   include "esp32/clk.h"
#   elif CONFIG_IDF_TARGET_ESP32S2
#   include "esp32s2/clk.h"
#   elif CONFIG_IDF_TARGET_ESP32S3
#   include "esp32s3/clk.h"
#   elif CONFIG_IDF_TARGET_ESP32C3
#   include "esp32c3/clk.h"
#   elif CONFIG_IDF_TARGET_ESP32H2
#   include "esp32h2/clk.h"
#   else // CONFIG_IDF_TARGET_* not defined
#   include "esp32/clk.h"
#   endif
#else
#   include "esp_clk.h"
#endif

#include "driver/gpio.h"
#include "driver/spi_master.h"
#include "esp_heap_caps.h"
#include "esp_log.h"
#include "driver/adc.h"
#include "driver/i2s.h"
#include "driver/pcnt.h"
#include "mdns.h"
#include "esp_http_client.h"
#include "sh2lib.h"

/////////////////////////////////////////////////////////////////////////////////////////////
// Helper function to register HTTP event handler
// Needed to fulfill gen_mpy.py callback conventions
//
static inline void esp_http_client_register_event_handler(esp_http_client_config_t *config, http_event_handle_cb http_event_handler, void *user_data)
{
    config->event_handler = http_event_handler;
    config->user_data = user_data;
}

// We don't want the whole FreeRTOS, only selected functions

void task_delay_ms(int ms);

// The binding only publishes structs that are used in some function. We need spi_transaction_ext_t
// TOOD: Find some way to mark structs for binding export instead of new function.
static inline void set_spi_transaction_ext(
        spi_transaction_ext_t *ext_trans,
        spi_transaction_t *trans,
        uint8_t command_bits,
        uint8_t address_bits){
    ext_trans->base = *trans;
    ext_trans->command_bits = command_bits;
    ext_trans->address_bits = address_bits;
}

// Wrapper for safe ISR callbacks from micropython
// Need to call both spi_transaction_set_cb and set spi_pre/post_cb_isr!

// Use this to set pre/post callbacks for spi transaction.
// pre_cb/post_cb should be either a callable object or "None".
// Result should be set to spi_transaction_t user field.
// Allocates RAM.

void *spi_transaction_set_cb(mp_obj_t pre_cb, mp_obj_t post_cb);

// These functions can be set into pre_cb/post_cb of spi_device_interface_config_t

void ex_spi_pre_cb_isr(spi_transaction_t *trans);
void ex_spi_post_cb_isr(spi_transaction_t *trans);

// Useful constants

#define EXPORT_CONST_INT(int_value) enum {ENUM_##int_value = int_value}

#if defined(ESP_IDF_VERSION_MAJOR) && ESP_IDF_VERSION_MAJOR >= 4
// SPI HOST enum was changed to macros on v4
enum {
    ENUM_SPI_HOST = SPI_HOST,
    ENUM_HSPI_HOST = HSPI_HOST,
    ENUM_VSPI_HOST = VSPI_HOST,
};
#endif

enum {
    ENUM_portMAX_DELAY = portMAX_DELAY
};

enum {
    ENUM_I2S_PIN_NO_CHANGE = I2S_PIN_NO_CHANGE
};

enum {
    ENUM_SPI_DEVICE_TXBIT_LSBFIRST = SPI_DEVICE_TXBIT_LSBFIRST,
    ENUM_SPI_DEVICE_RXBIT_LSBFIRST = SPI_DEVICE_RXBIT_LSBFIRST,
    ENUM_SPI_DEVICE_BIT_LSBFIRST = SPI_DEVICE_BIT_LSBFIRST,
    ENUM_SPI_DEVICE_3WIRE = SPI_DEVICE_3WIRE,
    ENUM_SPI_DEVICE_POSITIVE_CS = SPI_DEVICE_POSITIVE_CS,
    ENUM_SPI_DEVICE_HALFDUPLEX = SPI_DEVICE_HALFDUPLEX,
    ENUM_SPI_DEVICE_NO_DUMMY = SPI_DEVICE_NO_DUMMY,
    ENUM_SPI_DEVICE_CLK_AS_CS = SPI_DEVICE_CLK_AS_CS,
};

enum {
    ENUM_SPI_TRANS_MODE_DIO = SPI_TRANS_MODE_DIO,
    ENUM_SPI_TRANS_MODE_QIO = SPI_TRANS_MODE_QIO,
    ENUM_SPI_TRANS_MODE_DIOQIO_ADDR = SPI_TRANS_MODE_DIOQIO_ADDR,
    ENUM_SPI_TRANS_USE_RXDATA = SPI_TRANS_USE_RXDATA,
    ENUM_SPI_TRANS_USE_TXDATA = SPI_TRANS_USE_TXDATA,
    ENUM_SPI_TRANS_VARIABLE_CMD = SPI_TRANS_VARIABLE_CMD,
    ENUM_SPI_TRANS_VARIABLE_ADDR = SPI_TRANS_VARIABLE_ADDR,
};

enum {
    ENUM_MALLOC_CAP_DMA = MALLOC_CAP_DMA,
    ENUM_MALLOC_CAP_INTERNAL = MALLOC_CAP_INTERNAL,
    ENUM_MALLOC_CAP_SPIRAM = MALLOC_CAP_SPIRAM,
};

enum {
    ENUM_ESP_TASK_PRIO_MAX = ESP_TASK_PRIO_MAX,
    ENUM_ESP_TASK_PRIO_MIN = ESP_TASK_PRIO_MIN,
};

/////////////////////////////////////////////////////////////////////////////////////////////
// ili9xxx flush and ISR in C
//
// disp_drv->user_data should be a dict that contains dc and spi, setup by micropython.
// like this: "self.disp_drv.user_data = {'dc': self.dc, 'spi': self.spi, 'dt': display_type}"


void ili9xxx_post_cb_isr(spi_transaction_t *trans);

void ili9xxx_flush(void *disp_drv, const void *area, void *color_p);


