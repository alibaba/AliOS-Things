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
 * limitations under the License.
 *
 */

#include <stdint.h>
#include <stddef.h>
#include "LoRaMacTypes.h"


#define   MC_KEY_SIZE         16 

#define   LORAWAN_MC_DEFAULT_DATARATE   DR_4

#define   MC_CMD_GROUP_SETUP_REQ_LEN   29
#define   MC_CMD_CLASS_C_SESSION_REQ_LEN   10
#define   MC_CMD_CLASS_B_SESSION_REQ_LEN   10

#define   MC_CMD_DATA_MAX_SIZE   33              //2+21+1+1+4+4+6(CMDS)

typedef SysTime_t sessionTime_t;

typedef AddressIdentifier_t  mcId_t;

typedef MulticastChannel_t mcChannel_t;

typedef enum eLoRaMacMcType
{
   MC_TYPE_PRESET = 0,
   
   MC_TYPE_DYNAMIC,
   
   MC_TYPE_OTHER,
} mcType_t;

/*!
 * multicast server commands
 */
typedef enum eLoRaMacMcSrvCmd
{
    /*!
     * request the package version
     */
    SRV_MAC_MC_PACKAGE_VERSION_REQ       = 0x00,
    /*!
     * list the multicast groups currently configured
     */
    SRV_MAC_MC_GROUP_STATUS_REQ            = 0x01,
    /*!
     * create or modify the parameters of a multicast group
     */
    SRV_MAC_MC_GROUP_SETUP_REQ              = 0x02,
    /*!
     * delete a multicast group
     */
    SRV_MAC_MC_GROUP_DELETE_REQ             = 0x03,
    /*!
     * setup a temporary classC multicast session
     */
    SRV_MAC_MC_CLASS_C_SESSION_REQ        = 0x04,
    /*!
     * setup a temporary classB multicast session
     */
    SRV_MAC_MC_CLASS_B_SESSION_REQ        = 0x05,

}LoRaMacSrvMcCmd_t;

/*!
 * multicast nde end response commands
 */
typedef enum eLoRaMacMoteMcCmd
{
    SRV_MAC_MC_PACKAGE_VERSION_ANS      = 0x00,

    SRV_MAC_MC_GROUP_STATUS_ANS           = 0x01,

    SRV_MAC_MC_GROUP_SETUP_ANS             = 0x02,
  
    SRV_MAC_MC_GROUP_DELETE_ANS            = 0x03,

    SRV_MAC_MC_CLASS_C_SESSION_ANS      = 0x04,

    SRV_MAC_MC_CLASS_B_SESSION_ANS      = 0x05,
} LoRaMacMoteMcCmd_t;


typedef struct sLoRaMacMcKey
{
    Mib_t key;
    
    /*mckey value*/
    uint8_t val[MC_KEY_SIZE];
} mcKey_t;


typedef  struct sLoRaMacMcFcnt
{
    /*min multicast frame down count*/
    uint32_t  minMcFcnt;
    
	/*max multicast fram count down*/
    uint32_t  maxMcFcnt;
} mcFcnt_t;


typedef struct sMcGroupSetupReqParames
{
	/*RFU :6 ID :2*/
    uint8_t  groupId;
	
	/*mc group address*/
    uint32_t  mcAddr;
	
	/*McKey that is encrypted,l len is 16 bytes*/
    uint8_t  encryptMckey[16];
	
	/*minimum frame count*/
    uint32_t minMcFCount;
	
	/*maximum frame count*/
    uint32_t maxMcFCount;
} McGroupSetupReqParames_t;


/* multicast session type */
typedef enum eMcSessionType
{  
    /* class b session */
    MC_CLASS_B_SESSION = 0,
	
    /* class c session */
    MC_CLASS_C_SESSION,
} McSessionType_t;

/*multicast classB or classC session status*/
typedef enum eMcSessionStatus
{
	MC_SESSION_WAITING = 0,
	
	MC_SESSION_RUNNING,
	
	MC_SESSION_TIMEOUT, 
	
	MC_SESSION_END,
} McSessionStatus_t;


typedef struct sMcSessionCtx
{    
    McSessionType_t type;
	
	uint32_t time;
	
	uint32_t timeout;

	uint8_t periodicity;
	
	uint32_t frequency;
	
	uint8_t datarate;
	
	//McSessionStatus_t state;
} mcSessionCtx_t;

/* ================  CLASS B SESSION  ===================  */
typedef struct sMcClassBSessionReqParames
{
	/*RFU :6 ID :2*/
    uint8_t  groupId;
	
	/*session time start of the GPS epoch*/
    uint32_t  sessionTime;
    
    struct {
       uint8_t timeout:4;
       uint8_t periodicity:3;
    } tp;
	
    /*frequency for multicast , DlFrequ=0 signals default frequency hopping*/ 
    uint32_t DlFrequ;   
    /*datarate*/
    uint8_t DR;
} McClassBSessionReqParames_t;


/* ================  CLASS C SESSION  ===================  */
typedef struct sMcClassCSessionReqParames
{
	/*RFU :6 ID :2*/
    uint8_t  groupId;
	
	/*session time start of the GPS epoch*/
    uint32_t  sessionTime;
    
    uint8_t  sessionTimeOut;
	
    /*frequency for multicast , DlFrequ=0 signals default frequency hopping*/ 
    uint32_t  DlFrequ; 
    
    /*datarate*/
    uint8_t  DR;
} McClassCSessionReqParames_t;


typedef enum eLoRaMacMcStatus
{
    /*!
     * No error occurred
     */
    LORAMAC_MULTICAST_SUCCESS = 0,
    /*!
     * Not allowed group ID
     */
    LORAMAC_MULTICAST_ERROR_INVALID_GROUP_ID,
    /*!
     * Not support dynamic group
     */
    LORAMAC_MULTICAST_FAIL_NOT_SUPPORT_DYNAMIC,
    /*!
     * Address does not match
     */
    LORAMAC_MULTICAST_FAIL_ADDRESS,
    /*!
     * FCntUp/Down check failed Out of boundary
     */
    LORAMAC_MULTICAST_FAIL_FCNT,
    /*!
     * Not allowed parameter value
     */
    LORAMAC_MULTICAST_FAIL_PARAM,
    /*!
     * Null pointer exception
     */
    LORAMAC_MULTICAST_ERROR_NPE,
    /*!
     * Invalid multicast command fport
     */
    LORAMAC_MULTICAST_ERROR_INVALID_FPORT,
    /*!
     * Invalid key identifier exception
     */
    LORAMAC_MULTICAST_ERROR_INVALID_KEY_ID,
    /*!
     * Invalid address identifier exception
     */
    LORAMAC_MULTICAST_ERROR_INVALID_ADDR_ID,
       /*!
     * Invalid address identifier exception
     */
    LORAMAC_MULTICAST_ERROR_INVALID_FCNT,
    /*!
     * Invalid LoRaWAN specification version
     */
    LORAMAC_MULTICAST_ERROR_INVALID_VERSION,
    /*!
     * Incompatible buffer size
     */
    LORAMAC_MULTICAST_ERROR_BUF_SIZE,
    /*!
     * Error from parser reported
     */
    LORAMAC_MULTICAST_ERROR_PARSER,
    /*!
     * Error from serializer reported
     */
    LORAMAC_MULTICAST_ERROR_SERIALIZER,
    /*!
     * multicast command error
     */
    LORAMAC_MULTICAST_ERROR_COMMAND,
    /*!
     * multicast set channel error
     */
    LORAMAC_MULTICAST_ERROR_SET_CHANNEL,
     /*!
     * multicast set mckey error
     */
    LORAMAC_MULTICAST_ERROR_SET_MCKEY,
    /*!
     * multicast derive mckekey error
     */
    LORAMAC_MULTICAST_ERROR_DERIVE_MCKEKEY,
	/*!
     * malloc new slot failed 
     */
	LORAMAC_MULTICAST_ERROR_MEMORY,
	/*!
     * multicast session time error
     */
	LORAMAC_MULTICAST_ERROR_SESSION_TIME,
    /*!
     * multicast session not defined
     */
	LORAMAC_MULTICAST_ERROR_SESSION_NOT_DEFINED,
    /*!
     * Undefined Error occurred
     */
    LORAMAC_MULTICAST_ERROR,
	
} LoRaMacMcStatus_t;


/* API  for  loramac */

bool LoRaMacMcIsDynamic(AddressIdentifier_t addrID);

LoRaMacMcStatus_t LoRaMacMcCheckFCntDownRange( uint8_t groupId, uint32_t fcntDown );

uint8_t LoRaMacMcGetCmdFport(void);

/*API  for  lora  control*/
int McInitialization( uint8_t mcPort, uint8_t* mcKey );

int McCreatePresetChannel(uint8_t groupId, mcChannel_t* channel, mcKey_t* key);

int McCreateDynamicChannel(uint8_t groupId);

int McSendCmdAns(void);

int McProcessCmd(uint8_t *payload, uint8_t cmdSize);

int McGetLastError( void );
