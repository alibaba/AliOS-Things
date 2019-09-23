/*
 * Copyright (c) 2018 Alibaba Group. All rights reserved.
 * License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the LicenseMcSessionType_t
 *
 */

#include "secure-element.h"
#include "LoRaMac.h"
#include "LoRaMacCommands.h"
#include "LoRaMacMulticast.h"
#include "LoRaMacFCntHandler.h"


typedef struct sLoRaMacMcCtx
{
    /* preset multicast or dynamic multicast */
    mcType_t type;
    /* only for dynamic multicast after group has been setup */
    bool isSetup;
    /*Set to true , if the mc channel can run*/
    bool isEnabled;

    mcChannel_t mcChannel;

    mcKey_t mcKey;

    mcFcnt_t mcFcnt;

    mcSessionCtx_t session;

} LoRaMacMcCtx_t;

static uint8_t mcCmdFport;
/* error code */
static LoRaMacMcStatus_t mcError;

static LoRaMacMcCtx_t gMcCtxList[LORAMAC_MAX_MC_CTX];

static LoRaMacMcStatus_t LoRaMacMcSendCmdAns( void );
static LoRaMacMcStatus_t LoRaMacMcPrepareFrame( uint8_t* fBuffer, uint16_t fBufferSize, size_t* size );
static void LoRaMacMcEnableChannel(uint8_t groupId);
static void LoRaMacMcDisableChannel(uint8_t groupId);
static LoRaMacMcStatus_t LoRaMacMcSetChannel( MulticastChannel_t channel );
static LoRaMacMcStatus_t LoRaMacMcSetMckey(Mib_t type, uint8_t *mckey);
static LoRaMacMcStatus_t LoRaMacMcSet(uint8_t groupId);
static AddressIdentifier_t LoRaMacMcConvertToAddrID(uint8_t groupId);
static LoRaMacMcStatus_t LoRaMacMcProcessCommands( uint8_t *payload, uint8_t macIndex, uint8_t commandsSize );
static LoRaMacMcStatus_t LoRaMacMcDeriveMcKEKey( void );

#define CHECK_MC_ID(x)  (x>(LORAMAC_MAX_MC_CTX-1) ? 0 : 1)

#define MC_PACKAGE_VERSION     0x01
#define MC_PACKAGE_IDENTIFIER  0x31

static uint8_t mcDataBuffer[MC_CMD_DATA_MAX_SIZE];

/*
 * parse the groupsetup command
 */
static LoRaMacMcStatus_t LoRaMacMcParserGroupSetupReq(McGroupSetupReqParames_t* reqParames, uint8_t* mcPayload )
{
    uint8_t macIndex = 0;

    if (mcPayload == 0)
    {
        return LORAMAC_MULTICAST_ERROR_PARSER;
    }

    reqParames->groupId = mcPayload[macIndex++];

    reqParames->mcAddr = mcPayload[macIndex++];
    reqParames->mcAddr |= ( ( uint32_t ) mcPayload[macIndex++] << 8 );
    reqParames->mcAddr |= ( ( uint32_t ) mcPayload[macIndex++] << 16 );
    reqParames->mcAddr |= ( ( uint32_t ) mcPayload[macIndex++] << 24 );

    memcpy1( reqParames->encryptMckey, &mcPayload[macIndex], 16 );
    macIndex = macIndex+16;

    reqParames->minMcFCount = mcPayload[macIndex++];
    reqParames->minMcFCount |= ( ( uint32_t ) mcPayload[macIndex++] << 8 );
    reqParames->minMcFCount |= ( ( uint32_t ) mcPayload[macIndex++] << 16 );
    reqParames->minMcFCount |= ( ( uint32_t ) mcPayload[macIndex++] << 24 );

    reqParames->maxMcFCount = mcPayload[macIndex++];
    reqParames->maxMcFCount |= ( ( uint32_t ) mcPayload[macIndex++] << 8 );
    reqParames->maxMcFCount |= ( ( uint32_t ) mcPayload[macIndex++] << 16 );
    reqParames->maxMcFCount |= ( ( uint32_t ) mcPayload[macIndex++] << 24 );

    return LORAMAC_MULTICAST_SUCCESS;
}

/*
 * parse the class-B session command
 */
static LoRaMacMcStatus_t LoRaMacMcParserClassBSessionReq( McClassBSessionReqParames_t* reqParames, uint8_t* mcPayload )
{
    uint8_t macIndex = 0;

    if (mcPayload == 0)
    {
        return LORAMAC_MULTICAST_ERROR_PARSER;
    }

    reqParames->groupId = mcPayload[macIndex++];

    reqParames->sessionTime = mcPayload[macIndex++] & 0x000000FF;
    reqParames->sessionTime |= ( ( uint32_t ) mcPayload[macIndex++] << 8 ) & 0x0000FF00;
    reqParames->sessionTime |= ( ( uint32_t ) mcPayload[macIndex++] << 16 ) & 0x00FF0000;
    reqParames->sessionTime |= ( ( uint32_t ) mcPayload[macIndex++] << 24 ) & 0xFF000000;

    reqParames->tp.timeout = (mcPayload[macIndex] & 0x0F);
    reqParames->tp.periodicity = (mcPayload[macIndex++] >> 4 );

    reqParames->DlFrequ = mcPayload[macIndex++];
    reqParames->DlFrequ |= ( ( uint32_t )mcPayload[macIndex++] << 8 );
    reqParames->DlFrequ |= ( ( uint32_t )mcPayload[macIndex++] << 16 );

    reqParames->DR = mcPayload[macIndex++];

    return LORAMAC_MULTICAST_SUCCESS;
}

/*
 * parse the class-C session command
 */
static LoRaMacMcStatus_t LoRaMacMcParserClassCSessionReq( McClassCSessionReqParames_t* reqParames, uint8_t* mcPayload )
{
    uint8_t macIndex = 0;

    if (mcPayload == 0)
    {
        return LORAMAC_MULTICAST_ERROR_PARSER;
    }

    reqParames->groupId = mcPayload[macIndex++];

    reqParames->sessionTime = mcPayload[macIndex++] & 0x000000FF;
    reqParames->sessionTime |= ( ( uint32_t ) mcPayload[macIndex++] << 8 ) & 0x0000FF00;
    reqParames->sessionTime |= ( ( uint32_t ) mcPayload[macIndex++] << 16 ) & 0x00FF0000;
    reqParames->sessionTime |= ( ( uint32_t ) mcPayload[macIndex++] << 24 ) & 0xFF000000;

    reqParames->sessionTimeOut = mcPayload[macIndex] & 0x0F;

    reqParames->DlFrequ = mcPayload[macIndex++];
    reqParames->DlFrequ |= ( ( uint32_t )mcPayload[macIndex++] << 8 );
    reqParames->DlFrequ |= ( ( uint32_t )mcPayload[macIndex++] << 16 );

    reqParames->DR = mcPayload[macIndex++];

    return LORAMAC_MULTICAST_SUCCESS;
}

/*
 * check if fcntdown out of range
 */
LoRaMacMcStatus_t LoRaMacMcCheckFCntDownRange( uint8_t groupId, uint32_t fcntDown )
{
    uint32_t minfcnt = gMcCtxList[groupId].mcFcnt.minMcFcnt;
    uint32_t maxfcnt = gMcCtxList[groupId].mcFcnt.maxMcFcnt;

    if ( (fcntDown < minfcnt) || (fcntDown >= maxfcnt) )
    {
        return LORAMAC_MULTICAST_FAIL_FCNT;
    }

	return LORAMAC_MULTICAST_SUCCESS;
}

/*
 * This command is used to get multicast group status.
 */
LoRaMacMcStatus_t LoRaMacMcGroupStatusReq(uint8_t cmd, uint8_t* cmdAns, uint8_t* size)
{
    uint8_t nbGroup = 0;
    uint8_t mask = 0;
    uint8_t ansSize = 1;
    uint8_t* buffer = (cmdAns+1);

    for(uint8_t i = 0; i < LORAMAC_MAX_MC_CTX; i++)
    {
        if (gMcCtxList[i].isSetup)
        {
            nbGroup++;
            if(cmd & (1 << i))
            {
                mask |= (1 << i);
                uint32_t addr = gMcCtxList[i].mcChannel.Address;
                *buffer++ = i;
                *buffer++ = ((uint8_t)(addr & 0xFF));
                *buffer++ = ((uint8_t)(addr>>8 & 0xFF));
                *buffer++ = ((uint8_t)(addr>>16 & 0xFF));
                *buffer++ = ((uint8_t)(addr>>24 & 0xFF));
                ansSize += 5;
            }
        }
    }

    *cmdAns = mask;
    *cmdAns |= ((nbGroup<<4) & 0xF0);
    *size = ansSize;

    return LORAMAC_MULTICAST_SUCCESS;
}


/*
 * This command is used to create or modify the parameters of a multicast group.
 */
LoRaMacMcStatus_t LoRaMacMcGroupSetupReq(McGroupSetupReqParames_t *reqParames)
{
    uint8_t groupid = reqParames->groupId;
    uint8_t mckeybuf[16];
    LoRaMacMcCtx_t mcCtx;

    if(CHECK_MC_ID(groupid) == 0)
    {
        return LORAMAC_MULTICAST_FAIL_NOT_SUPPORT_DYNAMIC;
    }
    /*check if support dynamic mc*/
    if( gMcCtxList[groupid].type != MC_TYPE_DYNAMIC )
    {
        PRINTF("\r\nThe mc id not support dynamic \r\n");
        return LORAMAC_MULTICAST_ERROR_INVALID_GROUP_ID;
    }

    mcCtx.mcChannel = gMcCtxList[groupid].mcChannel;
    mcCtx.mcChannel.Address = reqParames->mcAddr;

    /*decrypt mckey*/
    SecureElementAesEncrypt( reqParames->encryptMckey, 16, MC_KE_KEY, mckeybuf );

    Mib_t key = 0;
    switch (groupid)
    {
        case 0:key = MIB_MC_KEY_0;break;
        case 1:key = MIB_MC_KEY_1;break;
        case 2:key = MIB_MC_KEY_2;break;
        case 3:key = MIB_MC_KEY_3;break;
    }
    mcCtx.mcKey.key = key;
    memcpy1(mcCtx.mcKey.val, mckeybuf, 16);

    /*minMcFCount ≤ McFCount < maxMcFCount*/
    if (reqParames->minMcFCount >= reqParames->maxMcFCount)
    {
        return LORAMAC_MULTICAST_ERROR_INVALID_FCNT;
    }
    mcCtx.mcFcnt.minMcFcnt = reqParames->minMcFCount;
    mcCtx.mcFcnt.maxMcFcnt = reqParames->maxMcFCount;

    /*stop multicast*/
    LoRaMacMcDisableChannel(groupid);

    /*copy to global parameters*/
    gMcCtxList[groupid].mcKey.key = mcCtx.mcKey.key;
    memcpy1(gMcCtxList[groupid].mcKey.val, mcCtx.mcKey.val, 16);
    gMcCtxList[groupid].mcChannel.AddrID = LoRaMacMcConvertToAddrID(groupid);
    gMcCtxList[groupid].mcChannel.Address = mcCtx.mcChannel.Address;
    gMcCtxList[groupid].mcFcnt.minMcFcnt = mcCtx.mcFcnt.minMcFcnt;
    gMcCtxList[groupid].mcFcnt.maxMcFcnt = mcCtx.mcFcnt.maxMcFcnt;
    gMcCtxList[groupid].isSetup = true;

    return LORAMAC_MULTICAST_SUCCESS;
}

/*
 * delete a multicast group from an end-device
 */
LoRaMacMcStatus_t LoRaMacMcGroupDeleteReq(uint8_t groupId)
{
	uint8_t groupid = groupId;

	if ( gMcCtxList[groupid].isSetup != true )
    {
        PRINTF("DELETE GROUP ERROR\r\n");
        return LORAMAC_MULTICAST_ERROR_INVALID_GROUP_ID;
    }

	LoRaMacMcDisableChannel(groupid);

	memset1( ( uint8_t* ) &gMcCtxList[groupid].mcChannel, 0x00, sizeof(mcChannel_t) );
    memset1( ( uint8_t* ) &gMcCtxList[groupid].mcKey, 0x00, sizeof(mcKey_t) );
    memset1( ( uint8_t* ) &gMcCtxList[groupid].mcFcnt, 0x00, sizeof(mcFcnt_t) );
    memset1( ( uint8_t* ) &gMcCtxList[groupid].session, 0x00, sizeof(mcSessionCtx_t) );

    gMcCtxList[groupid].isSetup = 0;

	return LORAMAC_MULTICAST_SUCCESS;
}

/*
 * the command used to setup a temporary ClassC multicast session associated
 */
LoRaMacMcStatus_t LoRaMacMcClassCSessionReq(McClassCSessionReqParames_t *reqParames, uint8_t* cmdAns)
{
    uint8_t groupid = reqParames->groupId;
    mcSessionCtx_t session;
    sessionTime_t gpsEpochTime = { 0 };
    sessionTime_t mcTime = { 0 };
    sessionTime_t sysTimeCurrent = { 0 };
    uint8_t status = 0;
    uint32_t nbSec = 0;
    bool mcErr = false;

    gpsEpochTime.Seconds = reqParames->sessionTime;
    gpsEpochTime.SubSeconds = 0;

    mcTime = gpsEpochTime;
    // Add Unix to Gps epcoh offset
    mcTime.Seconds += UNIX_GPS_EPOCH_OFFSET;

    sysTimeCurrent = SysTimeGet( );

    /*the session is past*/
    if (sysTimeCurrent.Seconds >= mcTime.Seconds)
    {
        mcErr = true;
    }
    else
    {
        nbSec = (mcTime.Seconds - sysTimeCurrent.Seconds);
    }

    session.type = MC_CLASS_C_SESSION;
    session.time = mcTime.Seconds;
    session.timeout = reqParames->sessionTimeOut;
    session.frequency = reqParames->DlFrequ*100;
    session.datarate = reqParames->DR;

    *cmdAns = groupid;
    status = LoRaMacCheckFreqDr(session.frequency, session.datarate);

    //frequency
    if((status & 0x01) == 0)
    {
        *cmdAns |= (1<<3);
    }
    //datarate
    if((status & 0x02) == 0)
    {
        *cmdAns |= (1<<2);
    }

    if(gMcCtxList[groupid].isSetup == false)
    {
        *cmdAns |= (1<<4);
        return LORAMAC_MULTICAST_ERROR_SESSION_NOT_DEFINED;
    }
    cmdAns++;

    if(mcErr)
    {
        *cmdAns++ = 0x00;
        *cmdAns++ = 0x00;
        *cmdAns = 0x00;
        return LORAMAC_MULTICAST_ERROR_COMMAND;
    }
    else
    {
        //the number of seconds from the McClassBSessionAns uplink to the beginning of the multicast fragmentation session.
        *cmdAns++ = ((uint8_t) nbSec & 0xFF);
        *cmdAns++ =((uint8_t) ((nbSec >> 8) & 0xFF));
        *cmdAns = ((uint8_t)((nbSec >> 16) & 0xFF));
        PRINTF("nbsec to start beacon is %lu seconds \r\n", nbSec);
    }

    LoRaMacMcDisableChannel(groupid);

    if(status == 0x07)
    {
        gMcCtxList[groupid].mcChannel.Datarate = session.datarate;
        gMcCtxList[groupid].mcChannel.Frequency = session.frequency;
    }

    /* reset downlink fcnts */
    LoRaMacResetMulticastFCnts(LoRaMacMcConvertToAddrID(groupid));

    //!! add processing code
    //!! add processing code

    gMcCtxList[groupid].session = session;
    LoRaMacMcEnableChannel(groupid);

    return LORAMAC_MULTICAST_SUCCESS;
}

/*
 * the command used to setup a temporary ClassB multicast session associated
 */
LoRaMacMcStatus_t LoRaMacMcClassBSessionReq(McClassBSessionReqParames_t *reqParames, uint8_t* cmdAns)
{
    uint8_t groupid = reqParames->groupId;
    mcSessionCtx_t session;
    sessionTime_t gpsEpochTime = { 0 };
    sessionTime_t mcTime = { 0 };
    sessionTime_t sysTimeCurrent = { 0 };
    uint8_t status = 0;
    uint32_t nbSec = 0;
    bool mcErr = false;

    if ( reqParames->sessionTime % 128 != 0 )
    {
        mcErr = true;
    }

    gpsEpochTime.Seconds = reqParames->sessionTime;
    gpsEpochTime.SubSeconds = 0;

    mcTime = gpsEpochTime;
    // Add Unix to Gps epcoh offset
    mcTime.Seconds += UNIX_GPS_EPOCH_OFFSET;
    sysTimeCurrent = SysTimeGet( );
    /*the session is past*/
    if (sysTimeCurrent.Seconds >= mcTime.Seconds)
    {
        PRINTF("\r\n------session time wrong------\r\n");
        mcErr = true;
    }
    else
    {
        nbSec = (mcTime.Seconds - sysTimeCurrent.Seconds);
    }

    session.type = MC_CLASS_B_SESSION;
    session.time = mcTime.Seconds;
    session.timeout = session.time + 128*(2<<(reqParames->tp.timeout-1));
    session.periodicity = reqParames->tp.periodicity;
    session.frequency = reqParames->DlFrequ*100;
    session.datarate = reqParames->DR;

    *cmdAns = groupid;
    status = LoRaMacCheckFreqDr(session.frequency, session.datarate);

    //frequency ko when frequency not 0
    if(((status & 0x01) == 0) && (session.frequency != 0))
    {
        *cmdAns |= (1<<3);
        PRINTF("FREQUECY ERROR ======\r\n");
        mcErr = true;
    }
    //datarate ko
    if((status & 0x02) == 0)
    {
        *cmdAns |= (1<<2);
        PRINTF("DR ERROR ======\r\n");
        mcErr = true;
    }
    if(gMcCtxList[groupid].isSetup != true)
    {
        *cmdAns |= (1<<4);
        PRINTF("GROUPID  ERROR ======\r\n");
        mcErr = true;
    }
    cmdAns++;

    if(mcErr)
    {
        *cmdAns++ = 0x00;
        *cmdAns++ = 0x00;
        *cmdAns = 0x00;
        return LORAMAC_MULTICAST_ERROR_COMMAND;
    }
    else
    {
        //the number of seconds from the McClassBSessionAns uplink to the beginning of the multicast fragmentation session.
        *cmdAns++ = ((uint8_t) nbSec & 0xFF);
        *cmdAns++ =((uint8_t) ((nbSec >> 8) & 0xFF));
        *cmdAns = ((uint8_t)((nbSec >> 16) & 0xFF));
        PRINTF("Nbsec to start beacon is %lu seconds ====\r\n", nbSec);
    }

    LoRaMacMcDisableChannel(groupid);

    gMcCtxList[groupid].mcChannel.Datarate = session.datarate;
    gMcCtxList[groupid].mcChannel.Frequency = session.frequency;
    gMcCtxList[groupid].mcChannel.Periodicity = session.periodicity;
    gMcCtxList[groupid].mcChannel.IsEnabled = 1;

    LoRaMacMcStatus_t ret;
    ret = LoRaMacMcSet(groupid);
    if (ret != LORAMAC_MULTICAST_SUCCESS)
    {
        return ret;
    }

    /* reset downlink fcnts */
    LoRaMacResetMulticastFCnts(LoRaMacMcConvertToAddrID(groupid));

    gMcCtxList[groupid].session = session;

    LoRaMacMcEnableChannel(groupid);

    return LORAMAC_MULTICAST_SUCCESS;
}

/*
 * check the multicast command
 */
static LoRaMacMcStatus_t LoRaMacMcCheckCommands(uint8_t *payload, uint8_t commandsSize)
{
    uint8_t index = 0;

    while( index < commandsSize )
    {
        switch( payload[index++] )
        {
            case SRV_MAC_MC_PACKAGE_VERSION_REQ:
                break;
            case SRV_MAC_MC_GROUP_STATUS_REQ:
                index++;
                break;
            case SRV_MAC_MC_GROUP_SETUP_REQ:
                index += MC_CMD_GROUP_SETUP_REQ_LEN;
                break;
            case SRV_MAC_MC_GROUP_DELETE_REQ:
                index++;
                break;
            case SRV_MAC_MC_CLASS_C_SESSION_REQ:
                index += MC_CMD_CLASS_C_SESSION_REQ_LEN;
                break;
            case SRV_MAC_MC_CLASS_B_SESSION_REQ:
                index += MC_CMD_CLASS_B_SESSION_REQ_LEN;
                break;
            default:
                return LORAMAC_MULTICAST_ERROR_COMMAND;
        }
    }

    if (index > commandsSize)
    {
        return LORAMAC_MULTICAST_ERROR_COMMAND;
    }

    return LORAMAC_MULTICAST_SUCCESS;
}

/*
 * process the multicast command from server
 */
static LoRaMacMcStatus_t LoRaMacMcProcessCommands( uint8_t *payload, uint8_t macIndex, uint8_t commandsSize )
{
    LoRaMacMcStatus_t ret;
    uint8_t* cmdPayload;
    uint8_t mcBuf[21+1] = { 0 };

    ret = LoRaMacMcCheckCommands(payload, commandsSize);
    if (ret != LORAMAC_MULTICAST_SUCCESS)
    {
        return ret;
    }

    while( macIndex < commandsSize )
    {
        // Decode Frame MAC commands
        switch( payload[macIndex++] )
        {
            case SRV_MAC_MC_PACKAGE_VERSION_REQ:
            {
          	    uint8_t packageVersion = MC_PACKAGE_VERSION;
                uint8_t packageIdentifier = MC_PACKAGE_IDENTIFIER;
                mcBuf[0] = packageIdentifier;
                mcBuf[1] = packageVersion;
                PRINTF("====SRV_MAC_MC_PACKAGE_VERSION_REQ====\r\n");
                LoRaMacCommandsMcAddCmd(SRV_MAC_MC_PACKAGE_VERSION_ANS, mcBuf, 2);
                break;
            }
            case SRV_MAC_MC_GROUP_STATUS_REQ:
            {
                uint8_t size;
                PRINTF("====SRV_MAC_MC_GROUP_STATUS_REQ====\r\n");
                LoRaMacMcGroupStatusReq(payload[macIndex], mcBuf, &size);
                LoRaMacCommandsMcAddCmd(SRV_MAC_MC_GROUP_STATUS_ANS, mcBuf, size);
                macIndex++;
                break;
            }
            case SRV_MAC_MC_GROUP_SETUP_REQ:
            {
                McGroupSetupReqParames_t groupSetup;
                memset1( (uint8_t*)(&groupSetup), 0x00, sizeof(McGroupSetupReqParames_t) );
                cmdPayload = &payload[macIndex];

                LoRaMacMcParserGroupSetupReq(&groupSetup, cmdPayload);
                PRINTF("\r\n====SRV_MAC_MC_GROUP_SETUP_REQ====\r\n");
#if 0
                PRINTF("GROUP ID IS %d\r\n", groupSetup.groupId);
                PRINTF("DEVADDR IS %x\r\n", groupSetup.mcAddr);
                PRINTF("MINFCNT IS %x\r\n", groupSetup.minMcFCount);
                PRINTF("MAXFCNT IS %x\r\n\r\n", groupSetup.maxMcFCount);
#endif

                mcBuf[0] = groupSetup.groupId;
                if (LoRaMacMcGroupSetupReq(&groupSetup) != LORAMAC_MULTICAST_SUCCESS)
                {
                    mcBuf[0] |= (1<<2);
                }
                LoRaMacCommandsMcAddCmd(SRV_MAC_MC_GROUP_SETUP_ANS, mcBuf, 1);

                macIndex += MC_CMD_GROUP_SETUP_REQ_LEN;
                break;
            }
            case SRV_MAC_MC_GROUP_DELETE_REQ:
            {
                uint8_t groupId = payload[macIndex];
                mcBuf[0] = groupId;
                PRINTF("\r\n====SRV_MAC_MC_GROUP_DELETE_REQ====\r\n");
                if (LoRaMacMcGroupDeleteReq(groupId) != LORAMAC_MULTICAST_SUCCESS)
                {
                    mcBuf[0] |= (1<<2);
                }
                LoRaMacCommandsMcAddCmd(SRV_MAC_MC_GROUP_DELETE_ANS, mcBuf, 1);

                macIndex++;
                break;
            }
            case SRV_MAC_MC_CLASS_C_SESSION_REQ:
            {
                PRINTF("\r\n====SRV_MAC_MC_CLASS_C_SESSION_REQ====\r\n");
                McClassCSessionReqParames_t mcClassCsession;
                cmdPayload = &payload[macIndex];
                LoRaMacMcParserClassCSessionReq(&mcClassCsession, cmdPayload);

                LoRaMacMcClassCSessionReq(&mcClassCsession, mcBuf);
                LoRaMacCommandsMcAddCmd(SRV_MAC_MC_CLASS_C_SESSION_ANS, mcBuf, 4);

                macIndex += MC_CMD_CLASS_C_SESSION_REQ_LEN;
                break;
            }
            case SRV_MAC_MC_CLASS_B_SESSION_REQ:
           {
                McClassBSessionReqParames_t mcClassBsession;
                cmdPayload = &payload[macIndex];
                LoRaMacMcParserClassBSessionReq(&mcClassBsession, cmdPayload);

                PRINTF("\r\n====SRV_MAC_MC_CLASS_B_SESSION_REQ====\r\n");
#if 0
                PRINTF("SESSIONTIME IS %lu\r\n", mcClassBsession.sessionTime);
                PRINTF("TIMEOUT IS %d\r\n", mcClassBsession.tp.timeout);
                PRINTF("PERIODICITY IS %d\r\n", mcClassBsession.tp.periodicity);
                PRINTF("FREQUENCY IS %d\r\n", mcClassBsession.DlFrequ);
                PRINTF("DR IS %d\r\n", mcClassBsession.DR);
#endif
                if (LoRaMacMcClassBSessionReq(&mcClassBsession, mcBuf) == LORAMAC_MULTICAST_SUCCESS)
                {
                    PRINTF("\r\nSetup dynamic class-B mc success \r\n\r\n");
                }
                LoRaMacCommandsMcAddCmd(SRV_MAC_MC_CLASS_B_SESSION_ANS, mcBuf, 4);

                macIndex += MC_CMD_CLASS_B_SESSION_REQ_LEN;
                break;
           }
           default: break;
        }
    }

    return LORAMAC_MULTICAST_SUCCESS;
}

/*
 * set the mulitcast command fpot
 */
LoRaMacMcStatus_t  LoRaMacMcSetCmdFport(uint8_t mcFport)
{
    if (mcFport == 0 || mcFport >= 224)
    {
        return LORAMAC_MULTICAST_ERROR_INVALID_FPORT;
    }
    mcCmdFport = mcFport;

    return LORAMAC_MULTICAST_SUCCESS;
}

/*
 * prepare the command answer to send
 */
static LoRaMacMcStatus_t LoRaMacMcPrepareFrame( uint8_t* fBuffer, uint16_t fBufferSize, size_t* size )
{
    size_t cmdsize = 0;

    if( fBuffer == NULL || fBufferSize == 0)
    {
        return  LORAMAC_MULTICAST_FAIL_PARAM;
    }

    if ( LoRaMacCommandsMcGetSizeSerializedCmds(&cmdsize) != LORAMAC_COMMANDS_SUCCESS )
    {
        return LORAMAC_MULTICAST_ERROR_COMMAND;
    }

    if (cmdsize > 0)
    {
        if (cmdsize > fBufferSize)
        {
            return LORAMAC_MULTICAST_ERROR_BUF_SIZE;
        }
        if ( LoRaMacCommandsMcSerializeCmds(fBufferSize, fBuffer) != LORAMAC_COMMANDS_SUCCESS )
        {
            return LORAMAC_MULTICAST_ERROR_SERIALIZER;
        }
        *size = cmdsize;
    }
    else
    {
        return LORAMAC_MULTICAST_ERROR_COMMAND;
    }

    return LORAMAC_MULTICAST_SUCCESS;
}

/*
 * send the multicast command ans
 */
static LoRaMacMcStatus_t LoRaMacMcSendCmdAns( void )
{
    size_t cmdsize = 0;
    McpsReq_t mcpsReq;
    LoRaMacMcStatus_t ret;
    LoRaMacTxInfo_t txInfo;

    ret = LoRaMacMcPrepareFrame(mcDataBuffer, MC_CMD_DATA_MAX_SIZE, &cmdsize);

    if(ret != LORAMAC_MULTICAST_SUCCESS)
    {
        return ret;
    }

    // Send empty frame in order to flush mulitcast MAC commands
    if( LoRaMacQueryTxPossible( cmdsize, &txInfo ) != LORAMAC_STATUS_OK )
    {
        // Send empty frame in order to flush MAC commands
        mcpsReq.Type = MCPS_UNCONFIRMED;
        mcpsReq.Req.Unconfirmed.fBuffer = NULL;
        mcpsReq.Req.Unconfirmed.fBufferSize = 0;
        mcpsReq.Req.Unconfirmed.Datarate = LORAWAN_MC_DEFAULT_DATARATE;
    }
    else
    {
        mcpsReq.Type = MCPS_UNCONFIRMED;
        mcpsReq.Req.Unconfirmed.fPort = LoRaMacMcGetCmdFport( );
        mcpsReq.Req.Unconfirmed.fBuffer = mcDataBuffer;
        mcpsReq.Req.Unconfirmed.fBufferSize = cmdsize;
        mcpsReq.Req.Unconfirmed.Datarate = LORAWAN_MC_DEFAULT_DATARATE;
    }
#if 0
    PRINTF("Dynamic MC cmd ans : ");
    PrintHexBuffer(mcDataBuffer, cmdsize);
    PRINTF("\r\n");
#endif
    LoRaMacStatus_t status;
    status = LoRaMacMcpsRequest( &mcpsReq );

    if( status != LORAMAC_STATUS_OK )
    {
        return LORAMAC_MULTICAST_ERROR_COMMAND;
    }
    else
    {
        // Remove all multicast MAC commands
        LoRaMacCommandsMcRemoveCmds( );
    }

    return LORAMAC_MULTICAST_SUCCESS;
}

/*
 * multicast set mckey
 */
static LoRaMacMcStatus_t LoRaMacMcSetMckey(Mib_t type, uint8_t *mckey)
{
    int ret;
    MibRequestConfirm_t mibReq;

    mibReq.Type = type;

    switch (type)
    {
        case MIB_MC_KEY_0:
            mibReq.Param.McKey0 = mckey;
            break;
        case MIB_MC_KEY_1:
            mibReq.Param.McKey1 = mckey;
            break;
        case MIB_MC_KEY_2:
            mibReq.Param.McKey1 = mckey;
            break;
        case MIB_MC_KEY_3:
            mibReq.Param.McKey3 = mckey;
            break;
        default: break;
    }

    ret = LoRaMacMibSetRequestConfirm(&mibReq);

    if (ret != LORAMAC_STATUS_OK) {
        return LORAMAC_MULTICAST_ERROR_SET_MCKEY;
    }

    return LORAMAC_MULTICAST_SUCCESS;
}

/*
 * multicast set channel parameters
 */
static LoRaMacMcStatus_t LoRaMacMcSetChannel( MulticastChannel_t channel )
{
    if (LoRaMacMulticastChannelSet(channel) != LORAMAC_STATUS_OK)
    {
        return LORAMAC_MULTICAST_ERROR_SET_CHANNEL;
    }

    return LORAMAC_MULTICAST_SUCCESS;
}

/*
 * multicast set key pair McAppSKey McNetSKey
 */
static LoRaMacMcStatus_t LoRaMacMcDeriveKeyPair( AddressIdentifier_t addrID, uint32_t mcAddr )
{
    if (LoRaMacCryptoDeriveMcSessionKeyPair(addrID, mcAddr) != LORAMAC_CRYPTO_SUCCESS)
    {
        return LORAMAC_MULTICAST_ERROR_SET_CHANNEL;
    }

    return LORAMAC_MULTICAST_SUCCESS;
}

/*
 * check whether multicast channel is enable
 */
static bool LoRaMacMcIsChannelEnable(uint8_t groupId)
{
    if (gMcCtxList[groupId].isEnabled == 1)
    {
        return true;
    }
    else
    {
        return false;
    }
}

/*
 * enable multicast channel
 */
static void LoRaMacMcEnableChannel(uint8_t groupId)
{
    CRITICAL_SECTION_BEGIN();
    gMcCtxList[groupId].isEnabled = 1;
    CRITICAL_SECTION_END();
}

/*
 * disable multicast channel
 */
static void LoRaMacMcDisableChannel(uint8_t groupId)
{
    CRITICAL_SECTION_BEGIN();
    gMcCtxList[groupId].isEnabled = 0;
    CRITICAL_SECTION_END();
}

/*
 * convert uint8_t to AddressIdentifier_t
 */
static AddressIdentifier_t LoRaMacMcConvertToAddrID(uint8_t groupId)
{
    AddressIdentifier_t mcAddr = 0;

    switch(groupId)
    {
        case 0:
            mcAddr = MULTICAST_0_ADDR;
            break;
        case 1:
            mcAddr = MULTICAST_1_ADDR;
            break;
        case 2:
            mcAddr = MULTICAST_2_ADDR;
            break;
        case 3:
            mcAddr = MULTICAST_3_ADDR;
            break;
    }

    return mcAddr;
}

/*
 * check multicast channel is active
 */
bool LoRaMacMcCheckActive(uint8_t groupId)
{
    mcType_t type = gMcCtxList[groupId].type;
    uint32_t time = gMcCtxList[groupId].session.time;
    uint32_t timeout = gMcCtxList[groupId].session.timeout;

    if (CHECK_MC_ID(groupId) == 0)
    {
        return false;
    }

    if ( !LoRaMacMcIsChannelEnable(groupId) )
    {
        return false;
    }

    if ( gMcCtxList[groupId].session.type != MC_CLASS_B_SESSION )
    {
        return false;
    }

    sessionTime_t curTime = SysTimeGet( );
    switch(type)
    {
        case MC_TYPE_PRESET:
            break;
        case MC_TYPE_DYNAMIC:
        {
            if ( curTime.Seconds >= time && curTime.Seconds < timeout )
            {
                /* arrive the time to run */
                break;
            }
            else if (curTime.Seconds < time)
            {
                /* don't arrive the time to run */
                return false;
            }
            else
            {
                /*the session is timeout*/
                LoRaMacMcDisableChannel(groupId);
                return false;
            }
            break;
        }
        case MC_TYPE_OTHER:
            return false;
    }

    return true;
}

/*
 * get multicast comands fport num
 */
uint8_t LoRaMacMcGetCmdFport( void )
{
    return mcCmdFport;
}

/*
 * check multicast is dynamic
 */
bool LoRaMacMcIsDynamic(AddressIdentifier_t addrID)
{
    uint8_t i;

    for (i = 0; i < LORAMAC_MAX_MC_CTX; i++)
    {
        if(gMcCtxList[i].mcChannel.AddrID == addrID)
        {
            return (gMcCtxList[i].type == MC_TYPE_DYNAMIC) ? true : false;
        }
    }

    return false;
}

/*
 * set multicast parameters
 */
static LoRaMacMcStatus_t LoRaMacMcSet(uint8_t groupId)
{
    LoRaMacMcStatus_t ret;

    if (CHECK_MC_ID(groupId) == 0)
    {
        return LORAMAC_MULTICAST_ERROR_INVALID_ADDR_ID;
    }

    ret = LoRaMacMcSetChannel(gMcCtxList[groupId].mcChannel);
    if (ret != LORAMAC_MULTICAST_SUCCESS)
    {
        return ret;
    }

    ret = LoRaMacMcSetMckey(gMcCtxList[groupId].mcKey.key, gMcCtxList[groupId].mcKey.val);
    if (ret != LORAMAC_MULTICAST_SUCCESS)
    {
        return ret;
    }

    ret = LoRaMacMcDeriveKeyPair(gMcCtxList[groupId].mcChannel.AddrID, gMcCtxList[groupId].mcChannel.Address);
    if (ret != LORAMAC_MULTICAST_SUCCESS)
    {
        return ret;
    }

    return LORAMAC_MULTICAST_SUCCESS;
}

/*
 * derive mckekey from mc-genappkey
 */
static LoRaMacMcStatus_t LoRaMacMcDeriveMcKEKey( void )
{
    LoRaMacCryptoStatus_t ret;

    ret = LoRaMacCryptoDeriveMcKEKey( );

    if(ret != LORAMAC_CRYPTO_SUCCESS)
    {
        return LORAMAC_MULTICAST_ERROR_DERIVE_MCKEKEY;
    }

    return LORAMAC_MULTICAST_SUCCESS;
}


/********************** MULTICAST  API *************************/

/*
 * multicast initialize
 */
int McInitialization( uint8_t mcPort, uint8_t* mcKey )
{
    LoRaMacMcStatus_t ret;
    MibRequestConfirm_t mibReq;

    /*initialize multicast command list*/
    LoRaMacCommandsMcInit(NULL);

    for (int i = 0; i < LORAMAC_MAX_MC_CTX; i++)
    {
        memset1( (uint8_t*)(&gMcCtxList[i]), 0x00, sizeof(LoRaMacMcCtx_t) );
        gMcCtxList[i].type = MC_TYPE_OTHER;
    }

    ret = LoRaMacMcSetCmdFport(mcPort);
    if(ret != LORAMAC_MULTICAST_SUCCESS)
    {
        mcError = ret;
        return -1;
    }

    mibReq.Type = MIB_MC_GEN_APP_KEY;
    mibReq.Param.McGenAppKey = mcKey;
    LoRaMacMibSetRequestConfirm( &mibReq );

    /*MC_KE_KEY*/
    ret = LoRaMacMcDeriveMcKEKey( );
    if(ret != LORAMAC_MULTICAST_SUCCESS)
    {
        mcError = ret;
        return -1;
    }

    return 0;
}

/*
 * create a preset multicast channel
 */
int McCreatePresetChannel(uint8_t groupId, mcChannel_t* channel, mcKey_t* key)
{
    LoRaMacMcStatus_t ret;

    if(CHECK_MC_ID(groupId) == 0)
    {
        mcError = LORAMAC_MULTICAST_ERROR_INVALID_GROUP_ID;
        return -1;
    }

    gMcCtxList[groupId].type = MC_TYPE_PRESET;
    gMcCtxList[groupId].mcChannel = *channel;
    gMcCtxList[groupId].mcKey = *key;

    ret = LoRaMacMcSet(groupId);

    if(ret != LORAMAC_MULTICAST_SUCCESS)
    {
        mcError = ret;
        return -1;
    }

    gMcCtxList[groupId].isSetup = 0;
    LoRaMacMcEnableChannel(groupId);

    return 0;
}

/*
 * create a dynamic multicast channel
 */
int McCreateDynamicChannel(uint8_t groupId)
{
    if(CHECK_MC_ID(groupId) == 0)
    {
        mcError = LORAMAC_MULTICAST_ERROR_INVALID_GROUP_ID;
        return -1;
    }
    gMcCtxList[groupId].type = MC_TYPE_DYNAMIC;

    LoRaMacMcDisableChannel(groupId);

    return 0;
}

/*
 * send back mulitcast command answers
 */
int McSendCmdAns( void )
{
    LoRaMacMcStatus_t ret;

    ret = LoRaMacMcSendCmdAns( );

    if (ret != LORAMAC_MULTICAST_SUCCESS)
    {
        mcError = ret;
        return -1;
    }

    return 0;
}

/*
 * processing dynamic multicast commands
 */
int McProcessCmd(uint8_t *payload, uint8_t cmdSize)
{
    LoRaMacMcStatus_t ret;

    if (payload == NULL || cmdSize == 0)
    {
        mcError = LORAMAC_MULTICAST_FAIL_PARAM;
        return -1;
    }

    ret = LoRaMacMcProcessCommands(payload, 0, cmdSize);

    if(ret != LORAMAC_MULTICAST_SUCCESS)
    {
       mcError = ret;
       return -1;
    }

    return 0;
}

/*
 * get the last dynamic multicast error code
 */
int McGetLastError( void )
{
    return (int)mcError;
}

