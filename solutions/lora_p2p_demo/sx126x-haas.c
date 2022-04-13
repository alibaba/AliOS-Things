/*!
 * \file      sx1261mbxbas-board.c
 *
 * \brief     Target board SX1261MBXBAS shield driver implementation
 *
 * \copyright Revised BSD License, see section \ref LICENSE.
 *
 * \code
 *                ______                              _
 *               / _____)             _              | |
 *              ( (____  _____ ____ _| |_ _____  ____| |__
 *               \____ \| ___ |    (_   _) ___ |/ ___)  _ \
 *               _____) ) ____| | | || |_| ____( (___| | | |
 *              (______/|_____)_|_|_| \__)_____)\____)_| |_|
 *              (C)2013-2017 Semtech
 *
 * \endcode
 *
 * \author    Miguel Luis ( Semtech )
 *
 * \author    Gregory Cristian ( Semtech )
 */
#include <stdlib.h>
#include <string.h>
#include "board.h"
#include "radio.h"
#include "ulog/ulog.h"
#include "sx126x-board.h"
#include <drivers/u_ld.h>
#include <vfsdev/spi_dev.h>
#include <vfsdev/gpio_dev.h>
#include <devicevfs/devicevfs.h>
#include <drivers/char/u_device.h>
#include "aos/hal/gpio.h"
/*!
 * \brief Holds the internal operating mode of the radio
 */
static RadioOperatingModes_t OperatingMode;

static int g_gpio_fd = 0;

#define NSS_CRT(up) if (up == 1) {                                                      \
                        SX126x.NSS.data = 1;                                            \
                        ioctl(g_gpio_fd, IOC_GPIO_SET, (unsigned long)&SX126x.NSS);     \
                    } else if (up == 0) {                                               \
                        SX126x.NSS.data = 0;                                            \
                        ioctl(g_gpio_fd, IOC_GPIO_SET, (unsigned long)&SX126x.NSS);     \
                    } else {                                                            \
                        LOG("invalid nss ctr\n");                                       \
                    }

#define SPI_SANITY_CHECK(ret) if(ret) {LOG("[%s][%d]spi err(%d)", __func__, __LINE__, ret); return;}
#define CRITICAL_SECTION_BEGIN()
#define CRITICAL_SECTION_END()

void SX126xIoInit( void )
{
    memset(&SX126x, 0, sizeof(SX126x_t));

    SX126x.Spi = open("/dev/spi0", 0);
    if (SX126x.Spi <= 0) {
        LOG("[%s][%d]faild to open spi device!", __FUNCTION__, __LINE__);
        return;
    } else {
        LOG("[%s][%d]succeed to open spi device(%d)!", __FUNCTION__, __LINE__, SX126x.Spi);
    }

    int ret = ioctl(SX126x.Spi, IOC_SPI_SET_CFLAG, SPI_NO_CS | SPI_MODE_3 | SPI_MSB | SPI_TRANSFER_NORMAL_MODE | SPI_DATA_8BIT);
    SPI_SANITY_CHECK(ret);

    ret = ioctl(SX126x.Spi, IOC_SPI_SET_FREQ, 2000000);
    SPI_SANITY_CHECK(ret);

    g_gpio_fd = open("/dev/gpio", 0);

    SX126x.NSS.id = 22; //p2-6
    SX126x.NSS.config = GPIO_IO_OUTPUT | GPIO_IO_OUTPUT_PP;

    SX126x.Reset.id = 33; //p4-1
    SX126x.Reset.config=GPIO_IO_OUTPUT | GPIO_IO_OUTPUT_PP;

    SX126x.DIO1.id = 32; //p4-0

    SX126x.BUSY.id = 39; //p4-7
    SX126x.BUSY.data = 0;
    SX126x.BUSY.config=GPIO_IO_INPUT | GPIO_IO_INPUT_PD;
}

void SX126xIoIrqInit( DioIrqHandler dioIrq )
{
    SX126x.DIO1.config = GPIO_IRQ_CLEAR;

    int ret = ioctl(g_gpio_fd, IOC_GPIO_SET_IRQ, &SX126x.DIO1);
    if (ret) {
        LOG("clear gpio irq failed, ret %d", ret);
        return;
    }
    SX126x.DIO1.config = GPIO_IRQ_DISABLE;
    ret = ioctl(g_gpio_fd, IOC_GPIO_SET_IRQ, &SX126x.DIO1);
    if (ret) {
        LOG("clear gpio irq failed, ret %d", ret);
        return;
    }

    SX126x.DIO1.config = GPIO_IRQ_ENABLE | GPIO_IRQ_EDGE_RISING;
    SX126x.DIO1.cb = dioIrq;
    SX126x.DIO1.arg = NULL;

    ret = ioctl(g_gpio_fd, IOC_GPIO_SET_IRQ, &SX126x.DIO1);
    if (ret) {
        LOG("enable gpio irq failed, ret %d", ret);
        return -1;
    }
}

void SX126xIoDeInit( void )
{
}

void SX126xIoDbgInit( void )
{
}

void SX126xIoTcxoInit( void )
{
    // No TCXO component available on this board design.
}

uint32_t SX126xGetBoardTcxoWakeupTime( void )
{
    return 0;
}

void SX126xIoRfSwitchInit( void )
{
    SX126xSetDio2AsRfSwitchCtrl( true );
}

RadioOperatingModes_t SX126xGetOperatingMode( void )
{
    return OperatingMode;
}

void SX126xSetOperatingMode( RadioOperatingModes_t mode )
{
    OperatingMode = mode;
}

void SX126xReset( void )
{
    SX126x.Reset.data = 0;
    ioctl(g_gpio_fd, IOC_GPIO_SET, (unsigned long)&SX126x.Reset);
    usleep(500000);
    SX126x.Reset.data = 1;
    ioctl(g_gpio_fd, IOC_GPIO_SET, (unsigned long)&SX126x.Reset);
}

void SX126xWaitOnBusy( void )
{
    int32_t ret = 0;
    uint32_t busy = 1;
    while(0 == ret) {
        if (busy) {
            usleep(5000);
        } else {
            break;
        }
        busy = ioctl(g_gpio_fd, IOC_GPIO_GET,  (unsigned long)&SX126x.BUSY);
    }
}

uint8_t SpiInOut(uint8_t cmd) {
    uint8_t ack = 0;
    ioc_spi_transfer_t t = {.rx_buf = &ack, .rx_size = 1, .tx_buf = &cmd, .tx_size = 1};
    int ret = ioctl(SX126x.Spi, IOC_SPI_SEND_RECV, &t);
    SPI_SANITY_CHECK(ret)
    return ack;
}

void SpiOut(uint8_t cmd) {
    write(SX126x.Spi, &cmd, 1);
    usleep(10);
}

void SpiIn(uint8_t *ack) {
    read(SX126x.Spi, ack, 1);
}

void SX126xWakeup( void )
{
    CRITICAL_SECTION_BEGIN( );
    int32_t ret = 0;

    NSS_CRT( 0 );
    SpiOut( RADIO_GET_STATUS );
    RadioStatus_t status = (RadioStatus_t)SpiInOut( 0x00 );
    // LOG("[%s]chip mode:%d  command status:%d\n", __func__, status.Fields.ChipMode, status.Fields.CmdStatus);
    NSS_CRT( 1 );
    // Wait for chip to be ready.
    SX126xWaitOnBusy( );
    // Update operating mode context variable
    SX126xSetOperatingMode( MODE_STDBY_RC );

    CRITICAL_SECTION_END( );
}

void SX126xWriteCommand( RadioCommands_t command, uint8_t *buffer, uint16_t size )
{
    int32_t ret = 0;

    SX126xCheckDeviceReady( );

    NSS_CRT( 0 );

    SpiOut( ( uint8_t )command );

    for( uint16_t i = 0; i < size; i++ )
    {
        SpiOut( buffer[i] );
    }

    NSS_CRT( 1 );

    if( command != RADIO_SET_SLEEP )
    {
        SX126xWaitOnBusy( );
    }
}

uint8_t SX126xReadCommand( RadioCommands_t command, uint8_t *buffer, uint16_t size )
{
    int32_t ret = 0;
    uint8_t status = 0;

    SX126xCheckDeviceReady( );

    NSS_CRT( 0 );

    SpiOut( ( uint8_t )command );
    if (command == RADIO_GET_STATUS)
    {
        status = SpiInOut( 0x00 );
    }
    else
    {
        SpiOut( 0x00 );
    }
    for( uint16_t i = 0; i < size; i++ )
    {
        if (command == RADIO_GET_RXBUFFERSTATUS) {
            SpiIn( buffer+i );
        } else {
            buffer[i] = SpiInOut( 0 );
        }
    }

    NSS_CRT( 1 );

    SX126xWaitOnBusy( );

    return status;
}

void SX126xWriteRegisters( uint16_t address, uint8_t *buffer, uint16_t size )
{
    int32_t ret = 0;
    SX126xCheckDeviceReady( );

    NSS_CRT( 0 );

    SpiOut( RADIO_WRITE_REGISTER );
    SpiOut( ( address & 0xFF00 ) >> 8 );
    SpiOut( address & 0x00FF );

    for( uint16_t i = 0; i < size; i++ )
    {
        SpiOut( buffer[i] );
    }

    NSS_CRT( 1 );

    SX126xWaitOnBusy( );
}

void SX126xWriteRegister( uint16_t address, uint8_t value )
{
    SX126xWriteRegisters( address, &value, 1 );
}

void SX126xReadRegisters( uint16_t address, uint8_t *buffer, uint16_t size )
{
    int32_t ret = 0;
    SX126xCheckDeviceReady( );

    NSS_CRT( 0 );

    SpiOut( RADIO_READ_REGISTER );
    SpiOut( ( address & 0xFF00 ) >> 8 );
    SpiOut( address & 0x00FF );
    SpiOut( 0 );
    for( uint16_t i = 0; i < size; i++ )
    {
        //buffer[i] = SpiInOut( 0 );
        SpiIn(buffer+i);
    }

    NSS_CRT( 1 );

    SX126xWaitOnBusy( );
}

uint8_t SX126xReadRegister( uint16_t address )
{
    uint8_t data;
    SX126xReadRegisters( address, &data, 1 );
    return data;
}

void SX126xWriteBuffer( uint8_t offset, uint8_t *buffer, uint8_t size )
{
    int32_t ret = 0;
    SX126xCheckDeviceReady( );

    NSS_CRT( 0 );

    SpiOut( RADIO_WRITE_BUFFER );
    SpiOut( offset );
    for( uint16_t i = 0; i < size; i++ )
    {
        SpiOut( buffer[i] );
    }

    NSS_CRT( 1 );

    SX126xWaitOnBusy( );
}

void SX126xReadBuffer( uint8_t offset, uint8_t *buffer, uint8_t size )
{
    int32_t ret = 0;
    SX126xCheckDeviceReady( );

    NSS_CRT( 0 );

    SpiOut( RADIO_READ_BUFFER );
    SpiOut( offset );
    SpiOut( 0 );
    for( uint16_t i = 0; i < size; i++ )
    {
        SpiIn(buffer+i);
    }

    NSS_CRT( 1 );

    SX126xWaitOnBusy( );
}

void SX126xSetRfTxPower( int8_t power )
{
    SX126xSetTxParams( power, RADIO_RAMP_40_US );
}

uint8_t SX126xGetDeviceId( void )
{
    return SX1262;
}

void SX126xAntSwOn( void )
{

}

void SX126xAntSwOff( void )
{

}

bool SX126xCheckRfFrequency( uint32_t frequency )
{
    // Implement check. Currently all frequencies are supported
    return true;
}
