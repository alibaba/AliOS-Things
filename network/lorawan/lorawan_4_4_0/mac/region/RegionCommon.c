/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

/*
 / _____)             _              | |
( (____  _____ ____ _| |_ _____  ____| |__
 \____ \| ___ |    (_   _) ___ |/ ___)  _ \
 _____) ) ____| | | || |_| ____( (___| | | |
(______/|_____)_|_|_| \__)_____)\____)_| |_|
    (C)2013 Semtech
 ___ _____ _   ___ _  _____ ___  ___  ___ ___
/ __|_   _/_\ / __| |/ / __/ _ \| _ \/ __| __|
\__ \ | |/ _ \ (__| ' <| _| (_) |   / (__| _|
|___/ |_/_/ \_\___|_|\_\_| \___/|_|_\\___|___|
embedded.connectivity.solutions===============

Description: LoRa MAC common region implementation

License: Revised BSD License, see LICENSE.TXT file include in the project

Maintainer: Miguel Luis ( Semtech ), Gregory Cristian ( Semtech ) and Daniel Jaeckle ( STACKFORCE )
*/

#include <stdbool.h>
#include <string.h>
#include <stdint.h>
#include <math.h>

//#include "aos/hal/timer.h"
#include "system/timer.h"
#include "utilities.h"
#include "LoRaMac.h"
#include "RegionCommon.h"



#define BACKOFF_DC_1_HOUR       100
#define BACKOFF_DC_10_HOURS     1000
#define BACKOFF_DC_24_HOURS     10000



static uint8_t CountChannels( uint16_t mask, uint8_t nbBits )
{
    uint8_t nbActiveBits = 0;

    for( uint8_t j = 0; j < nbBits; j++ )
    {
        if( ( mask & ( 1 << j ) ) == ( 1 << j ) )
        {
            nbActiveBits++;
        }
    }
    return nbActiveBits;
}



uint16_t RegionCommonGetJoinDc( TimerTime_t elapsedTime )
{
    uint16_t dutyCycle = 0;

    if( elapsedTime < 3600000 )
    {
        dutyCycle = BACKOFF_DC_1_HOUR;
    }
    else if( elapsedTime < ( 3600000 + 36000000 ) )
    {
        dutyCycle = BACKOFF_DC_10_HOURS;
    }
    else
    {
        dutyCycle = BACKOFF_DC_24_HOURS;
    }
    return dutyCycle;
}

bool RegionCommonChanVerifyDr( uint8_t nbChannels, uint16_t* channelsMask, int8_t dr, int8_t minDr, int8_t maxDr, ChannelParams_t* channels )
{
    if( RegionCommonValueInRange( dr, minDr, maxDr ) == 0 )
    {
        return false;
    }

    for( uint8_t i = 0, k = 0; i < nbChannels; i += 16, k++ )
    {
        for( uint8_t j = 0; j < 16; j++ )
        {
            if( ( ( channelsMask[k] & ( 1 << j ) ) != 0 ) )
            {// Check datarate validity for enabled channels
                if( RegionCommonValueInRange( dr, ( channels[i + j].DrRange.Fields.Min & 0x0F ),
                                                  ( channels[i + j].DrRange.Fields.Max & 0x0F ) ) == 1 )
                {
                    // At least 1 channel has been found we can return OK.
                    return true;
                }
            }
        }
    }
    return false;
}

uint8_t RegionCommonValueInRange( int8_t value, int8_t min, int8_t max )
{
    if( ( value >= min ) && ( value <= max ) )
    {
        return 1;
    }
    return 0;
}

bool RegionCommonChanDisable( uint16_t* channelsMask, uint8_t id, uint8_t maxChannels )
{
    uint8_t index = id / 16;

    if( ( index > ( maxChannels / 16 ) ) || ( id >= maxChannels ) )
    {
        return false;
    }

    // Deactivate channel
    channelsMask[index] &= ~( 1 << ( id % 16 ) );

    return true;
}

uint8_t RegionCommonCountChannels( uint16_t* channelsMask, uint8_t startIdx, uint8_t stopIdx )
{
    uint8_t nbChannels = 0;

    if( channelsMask == NULL )
    {
        return 0;
    }

    for( uint8_t i = startIdx; i < stopIdx; i++ )
    {
        nbChannels += CountChannels( channelsMask[i], 16 );
    }

    return nbChannels;
}

void RegionCommonChanMaskCopy( uint16_t* channelsMaskDest, uint16_t* channelsMaskSrc, uint8_t len )
{
    if( ( channelsMaskDest != NULL ) && ( channelsMaskSrc != NULL ) )
    {
        for( uint8_t i = 0; i < len; i++ )
        {
            channelsMaskDest[i] = channelsMaskSrc[i];
        }
    }
}

void RegionCommonSetBandTxDone( bool joined, Band_t* band, TimerTime_t lastTxDone )
{
    if (joined == true) {
        band->LastTxDoneTime = lastTxDone;
    } else {
        band->LastTxDoneTime = lastTxDone;
        band->LastJoinTxDoneTime = lastTxDone;
    }
}

TimerTime_t RegionCommonUpdateBandTimeOff( bool joined, bool dutyCycle, Band_t* bands, uint8_t nbBands )
{
    TimerTime_t nextTxDelay = ( TimerTime_t )( -1 );

    // Update bands Time OFF
    for( uint8_t i = 0; i < nbBands; i++ )
    {
        if( joined == false )
        {
            uint32_t txDoneTime =  MAX( TimerGetElapsedTime( bands[i].LastJoinTxDoneTime ),
                                        ( dutyCycle == true ) ? TimerGetElapsedTime( bands[i].LastTxDoneTime ) : 0 );

            if( bands[i].TimeOff <= txDoneTime )
            {
                bands[i].TimeOff = 0;
            }
            if( bands[i].TimeOff != 0 )
            {
                nextTxDelay = MIN( bands[i].TimeOff - txDoneTime, nextTxDelay );
            }
        }
        else
        {
            if( dutyCycle == true )
            {
                if( bands[i].TimeOff <= TimerGetElapsedTime( bands[i].LastTxDoneTime ) )
                {
                    bands[i].TimeOff = 0;
                }
                if( bands[i].TimeOff != 0 )
                {
                    nextTxDelay = MIN( bands[i].TimeOff - TimerGetElapsedTime( bands[i].LastTxDoneTime ),
                                       nextTxDelay );
                }
            }
            else
            {
                nextTxDelay = 0;
                bands[i].TimeOff = 0;
            }
        }
    }
    return nextTxDelay;
}

uint8_t RegionCommonParseLinkAdrReq( uint8_t* payload, LinkAdrParams_t* linkAdrParams )
{
    uint8_t retIndex = 0;

    if( payload[0] == SRV_MAC_LINK_ADR_REQ )
    {
        // Parse datarate and tx power
        linkAdrParams->Datarate = payload[1];
        linkAdrParams->TxPower = linkAdrParams->Datarate & 0x0F;
        linkAdrParams->Datarate = ( linkAdrParams->Datarate >> 4 ) & 0x0F;
        // Parse ChMask
        linkAdrParams->ChMask = ( uint16_t )payload[2];
        linkAdrParams->ChMask |= ( uint16_t )payload[3] << 8;
        // Parse ChMaskCtrl and nbRep
        linkAdrParams->NbRep = payload[4];
        linkAdrParams->ChMaskCtrl = ( linkAdrParams->NbRep >> 4 ) & 0x07;
        linkAdrParams->NbRep &= 0x0F;

        // LinkAdrReq has 4 bytes length + 1 byte CMD
        retIndex = 5;
    }
    return retIndex;
}

DECIMAL RegionCommonComputeSymbolTimeLoRa( uint8_t phyDr, uint32_t bandwidth )
{
    return ( ( DECIMAL )( 1 << phyDr ) / ( DECIMAL )bandwidth ) * 1000;
}

DECIMAL RegionCommonComputeSymbolTimeFsk( uint8_t phyDr )
{
    return ( (DECIMAL)8.0 / ( DECIMAL )phyDr ); // 1 symbol equals 1 byte
}

void RegionCommonComputeRxWindowParameters( DECIMAL tSymbol, uint8_t minRxSymbols, uint32_t rxError, uint32_t wakeUpTime, uint32_t* windowTimeout, int32_t* windowOffset )
{
    *windowTimeout = MAX( ( uint32_t )ceilf( ( ( 2 * minRxSymbols - 8 ) * tSymbol + 2 * rxError ) / tSymbol ), minRxSymbols ); // Computed number of symbols
    *windowOffset = ( int32_t )ceilf( ( 4.0f * tSymbol ) - ( ( *windowTimeout * tSymbol ) / 2.0f ) - wakeUpTime );
}

int8_t RegionCommonComputeTxPower( int8_t txPowerIndex, float maxEirp, float antennaGain )
{
    int8_t phyTxPower = 0;

    phyTxPower = ( int8_t )floorf( ( maxEirp - ( txPowerIndex * 2U ) ) - antennaGain );

    return phyTxPower;
}
