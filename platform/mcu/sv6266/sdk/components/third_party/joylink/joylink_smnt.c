/*************************************

Copyright (c) 2015-2050, JD Smart All rights reserved. 

*************************************/
#include <stdio.h>
#include <string.h>
#include "joylink_smnt.h"
#include "joylink_auth_aes.h"
#include "cryptoApi.h"

#ifdef USING_TINY_AES
#include "tiny_aes.h"
#endif

#define STEP_MULTICAST_HOLD_CHANNEL				5
#define STEP_BROADCAST_HOLD_CHANNEL				4
#define STEP_BROADCAST_ERROR_HOLE_CHANNEL		2
#define PAYLOAD_MIN								(3)
#define PAYLOAD_MAX								(48+1)				
extern void smart_stop(void);
typedef struct{
	unsigned char type;	                        // 0:NotReady, 1:ControlPacketOK, 2:BroadcastOK, 3:MulticastOK
	unsigned char encData[1+32+32+32];            // length + EncodeData
}smnt_encrypt_data_t;

typedef enum{
	SMART_CH_INIT 		=	0x1,
	SMART_CH_LOCKING 	=	0x2,
	SMART_CH_LOCKED 	=	0x4,
	SMART_FINISH 		= 	0x8
}smnt_status_t;


typedef struct{
	smnt_status_t state;	
	uint16 syncFirst;				
	uint8 syncCount;				
	uint8 syncStepPoint;			
	uint8 syncFirst_downlink;		
	uint8 syncCount_downlink;		
	uint8 syncStepPoint_downlink;	
	uint8 directTimerSkip;			
	uint8 broadcastVersion;			
	uint8 muticastVersion;			
	uint8  broadIndex;				
	uint8  broadBuffer[5];			
	uint16 lastLength;			
	uint16 lastUploadSeq;		
	uint16 lastDownSeq;		
	uint8  syncAppMac[6];		
	uint8  syncBssid[6];		
	uint16 syncIsUplink;		
	uint8 chCurrentIndex;		
	uint8 chCurrentProbability;	
	uint8 isProbeReceived;	
	uint8 payload_multicast[128];	
	uint8 payload_broadcast[128];
	smnt_encrypt_data_t result;	
}joylinkSmnt_t;

joylinkSmnt_t* pSmnt = NULL;
static int  joylink_smnt_payLoadcheck(uint8 *payload);
static void joylink_smnt_muticastadd(uint8* pAddr);
static void joylink_smnt_broadcastadd(int ascii);
static uint8 joylink_smnt_crc(uint8 *ptr, uint8 len);
static void joylink_smnt_bufferprintf(uint8* p, int split, int len);




void joylink_smnt_init(joylink_smnt_param_t param)
{
	pSmnt = (joylinkSmnt_t *)malloc(sizeof(joylinkSmnt_t));
	
    memset(pSmnt, 0, sizeof(joylinkSmnt_t) );
	memset(pSmnt->payload_multicast, 0xFF, sizeof(pSmnt->payload_multicast));	
	memset(pSmnt->payload_broadcast, 0xFF, sizeof(pSmnt->payload_broadcast));
	
	memcpy(joylink_smnt_gobal.secretkey,param.secretkey,16);
	
	if(param.get_result_callback != NULL){
		joylink_smnt_gobal.get_result_callback = param.get_result_callback;
	}else{
		printf("get_result_callback is NULL\n");
	}

	if(param.switch_channel_callback != NULL){
		joylink_smnt_gobal.switch_channel_callback = param.switch_channel_callback;
	}else{
		printf("switch_channel_callback is NULL\n");
	}
	return;
}

void joylink_smnt_release(void)
{
	printf("%s\n", __func__);
	memset(joylink_smnt_gobal.secretkey,0,sizeof(joylink_smnt_gobal.secretkey));
	joylink_smnt_gobal.get_result_callback 		= NULL;
	joylink_smnt_gobal.switch_channel_callback 	= NULL;

	if(pSmnt != NULL){
		free(pSmnt);
		pSmnt = NULL;
	}
}

void joylink_smnt_reset(void)
{
	if(pSmnt != NULL){
		memset(pSmnt,0,sizeof(pSmnt));
	}
}

#ifdef USING_TINY_AES
static int joylinkUnPadding1(uint8_t *data, uint32_t len)
{
    int i;
    uint8_t padding;

    if ( len % 16 )
        return -1;    

    padding = data[len - 1]; 
    for (i=0; i<padding; i++)
    {   
        if (data[len - 1 - i] != padding)
            return -1;
    }   

    return padding;
}
#endif

static void  joylink_smnt_finish(void)
{
#ifdef USING_TINY_AES
    int enc_len, padding;
    uint8 *out = (uint8 *) malloc((sizeof(uint8)*(pSmnt->result.encData[0])));
#else
    int ret = 128;
#endif
	uint8 iv[16] = {0};
	joylink_smnt_result_t smnt_result;
	memset(&smnt_result,0,sizeof(smnt_result));
	if (pSmnt && (pSmnt->state== SMART_FINISH) ){

		//memset(&smnt_result,0,sizeof(smnt_result));
#ifdef USING_TINY_AES
        enc_len = pSmnt->result.encData[0];
        AES_CBC_decrypt_buffer(out, pSmnt->result.encData+1, enc_len, joylink_smnt_gobal.secretkey, iv);
        padding = joylinkUnPadding1(out, enc_len);
        printf("enc_len=%d, padding=%d\n", enc_len, padding);
        smnt_result.smnt_result_status = smnt_result_decrypt_error;
        if (padding > 0 && padding < enc_len) {
            memcpy(pSmnt->result.encData+1, out, enc_len);
		    joylink_smnt_bufferprintf(pSmnt->result.encData, 1, pSmnt->result.encData[0]);
            padding = padding + 1 + 6;
			smnt_result.jd_password_len = pSmnt->result.encData[1];
			smnt_result.jd_ssid_len	 = (enc_len - padding - smnt_result.jd_password_len);

             //max ssid_len=32 + max pswd_len = 64
            if ((smnt_result.jd_password_len + smnt_result.jd_ssid_len) <= 96) {
                memcpy(smnt_result.jd_password,pSmnt->result.encData+2,smnt_result.jd_password_len);
			    memcpy(smnt_result.jd_ssid,pSmnt->result.encData + 2 + smnt_result.jd_password_len +6,smnt_result.jd_ssid_len);	
			    smnt_result.smnt_result_status = smnt_result_ok;
            }
        }
#else
		ret = JLdevice_aes_decrypt(joylink_smnt_gobal.secretkey,16,iv,pSmnt->result.encData+1,pSmnt->result.encData[0],pSmnt->result.encData+1,128);

		if((ret> 0) && (ret <= 96)){
			smnt_result.jd_password_len = pSmnt->result.encData[1];
			smnt_result.jd_ssid_len	 = ret - smnt_result.jd_password_len -1 -6;
			memcpy(smnt_result.jd_password,pSmnt->result.encData+2,smnt_result.jd_password_len);
			memcpy(smnt_result.jd_ssid,pSmnt->result.encData + 2 + smnt_result.jd_password_len +6,smnt_result.jd_ssid_len);	

			smnt_result.smnt_result_status = smnt_result_ok;
		}else{
			smnt_result.smnt_result_status = smnt_result_decrypt_error;
		}

#endif
		if(joylink_smnt_gobal.get_result_callback == NULL){
			printf("ERROR:joylink_smnt_finish->get_result_callback NULL\n");
			goto RET;
		}
		
		joylink_smnt_gobal.get_result_callback(smnt_result);

#ifdef USING_TINY_AES
        if (out) {
            free(out);
            out = NULL;
        }
#endif
	}
RET:
	return;
}


int joylink_smnt_cyclecall(void)
{
	if(pSmnt == NULL){
    	return 50;
  	}

	if(joylink_smnt_gobal.switch_channel_callback == NULL){
		printf("switch channel function NULL\n");
		return 50;
	}
	
    if (pSmnt->directTimerSkip){
		pSmnt->directTimerSkip--;
		return 50;
	}
	
	if (pSmnt->state == SMART_FINISH){
		printf("-------------------->Finished\n");
		pSmnt->directTimerSkip = 10000/50;
		return 50;
	}
	
	if (pSmnt->isProbeReceived >0 ){
		printf("-------------------->Probe Stay(CH:%d) %d\n", pSmnt->chCurrentIndex, pSmnt->isProbeReceived);
		pSmnt->isProbeReceived = 0;
		pSmnt->directTimerSkip = 5000 / 50;
		return 50;
	}
	
	if (pSmnt->chCurrentProbability > 0){
		pSmnt->chCurrentProbability--;
		printf("------------------->SYNC (CH:%d) %d\n", pSmnt->chCurrentIndex, pSmnt->chCurrentProbability);
		return 50;
	}

	//pSmnt->chCurrentIndex = (pSmnt->chCurrentIndex + 1) % 13;
	pSmnt->chCurrentIndex = (pSmnt->chCurrentIndex % 13) + 1;
	
	if(joylink_smnt_gobal.switch_channel_callback != NULL){
		joylink_smnt_gobal.switch_channel_callback(pSmnt->chCurrentIndex);
	}
	
	pSmnt->state 			= SMART_CH_LOCKING;
	pSmnt->syncStepPoint = 0;
	pSmnt->syncCount = 0;
	pSmnt->chCurrentProbability = 0;
	printf("CH=%d, T=%d\n", pSmnt->chCurrentIndex, 50);
	return 50;
}

static void  joylink_smnt_broadcastadd(int ascii)
{
	uint8 isFlag = (uint8)((ascii >> 8) & 0x1);
	uint8 is_finishpacket = 0;

	uint8 *broadbuffer=pSmnt->broadBuffer,*broadindex = &(pSmnt->broadIndex);
	if (isFlag){
		*broadindex = 0;
		*broadbuffer = (uint8)ascii;
	}else{
		*broadindex = *broadindex + 1;
		broadbuffer[*broadindex] = (uint8)ascii;

		if((((pSmnt->payload_broadcast[1] +2) / 4 + 1) == ( (*broadbuffer) >> 3))  && (pSmnt->payload_broadcast[1] != 0) && (pSmnt->payload_broadcast[1] != 0xFF))
		{
			if(*broadindex == 2){
				is_finishpacket = 1;
				*(broadbuffer + 3) = 0;
				*(broadbuffer + 4) = 0;
			}
		}
	
		if (*broadindex >= 4 || is_finishpacket)
		{
			*broadindex = 0;
			uint8 crc = (*broadbuffer) & 0x7;
			uint8 index = (*broadbuffer) >> 3;
			uint8 rCrc = joylink_smnt_crc(broadbuffer + 1, 4) & 0x7;
			
			/*not to check the last pacet crc,It is a patch for the last packet is a lot wrong which maybe leaded by the phone*/
			if (((index>0) && (index<33) && (rCrc == crc)))
			{
				memcpy(pSmnt->payload_broadcast + (index - 1) * 4, broadbuffer + 1, 4);
					
				printf("B(%x=%x)--%02x,%02x,%02x,%02x\n", index, broadbuffer[0], broadbuffer[1], broadbuffer[2], broadbuffer[3], broadbuffer[4]);
				index = joylink_smnt_payLoadcheck(pSmnt->payload_broadcast);

				if (pSmnt->chCurrentProbability < 30){ 
					pSmnt->chCurrentProbability += STEP_BROADCAST_HOLD_CHANNEL;
				}
				
				if (index == 0){	
					pSmnt->result.type = 2;
				}
				
				//joylink_smnt_bufferprintf(pSmnt->payload, 1, 80);
				
			}else{
				if (pSmnt->chCurrentProbability < 30){ 
					pSmnt->chCurrentProbability += STEP_BROADCAST_ERROR_HOLE_CHANNEL;
				}
			}
		}else if (*broadindex == 2){
			uint8 index = broadbuffer[0] >> 3;
			if (index == 0){
				*broadindex = 0;
				uint8 crc = broadbuffer[0] & 0x7;
				uint8 rCrc = joylink_smnt_crc(broadbuffer + 1, 2) & 0x7;
				if (rCrc == crc){
					pSmnt->broadcastVersion = broadbuffer[1];
					printf("Version RX:%x\n",pSmnt->broadcastVersion);
				}
			}
		}
	} 
}

/*
Input: Muticast Addr
Output: -1:Unkown Packet, 0:Parse OK, 1:Normal Process
*/
static void joylink_smnt_muticastadd(uint8* pAddr)
{
	int8 index = 0;
	
	if ((pAddr[3] >> 6) == ((pAddr[4] ^ pAddr[5]) & 0x1)){
		index = pAddr[3] & 0x3F;
	}else
		return;
	
	if ((index >= 1) && (index < PAYLOAD_MAX))		//avoid overstep leaded by error
	{
		uint8 payloadIndex = index - 1;
		if (payloadIndex > 64)
			return;

		if (pSmnt->chCurrentProbability < 20) 
			pSmnt->chCurrentProbability += STEP_MULTICAST_HOLD_CHANNEL;			// Delay CH switch!
		
		printf("M%02d(CH=%d)--%02X:(%02X,%02X)\n", index, pSmnt->chCurrentIndex, pAddr[3], pAddr[4], pAddr[5]); 
		pSmnt->payload_multicast[payloadIndex * 2]     = pAddr[4];
		pSmnt->payload_multicast[payloadIndex * 2 + 1] = pAddr[5];

		if (joylink_smnt_payLoadcheck(pSmnt->payload_multicast) == 0){
			pSmnt->result.type = 3;
			return;
		}
	}
	return;
}

void joylink_smnt_datahandler(PHEADER_802_11 pHeader, int length)
{
	uint8 isUplink = 1;				
	uint8 packetType = 0;					// 1-multicast packets 2-broadcast packets 0-thers
	uint8 isDifferentAddr = 0;
	uint8 *pDest, *pSrc, *pBssid;
	uint16 lastLength = 0;
	uint16 lastSeq_uplink = 0,lastSeq_downlink = 0;
	static uint8 past_channel = 0xFF;
	
	if (pSmnt == NULL)
		return;
	if (((length > 100) && (pSmnt->state != SMART_CH_LOCKED)) || pSmnt->state == SMART_FINISH)	
		return;
	
	if (pHeader->FC.ToDs){
		isUplink = 1;				
		pBssid = pHeader->Addr1;
		pSrc = pHeader->Addr2;
		pDest = pHeader->Addr3;	

		if (!((memcmp(pDest, "\xFF\xFF\xFF\xFF\xFF\xFF", 6) == 0) || (memcmp(pDest, "\x01\x00\x5E", 3) == 0))){
			return;
		}		
		lastSeq_uplink = pSmnt->lastUploadSeq;
		pSmnt->lastUploadSeq = pHeader->Sequence;
	}else{
		pDest = pHeader->Addr1;	
		pBssid = pHeader->Addr2;
		pSrc  = pHeader->Addr3;
		
		isUplink = 0;
		//not broadcast nor multicast package ,return
		if (!((memcmp(pDest, "\xFF\xFF\xFF\xFF\xFF\xFF", 6) == 0) || (memcmp(pDest, "\x01\x00\x5E", 3) == 0))){
			return;
		}
		lastSeq_downlink = pSmnt->lastDownSeq;
		pSmnt->lastDownSeq = pHeader->Sequence;
	}
	lastLength = pSmnt->lastLength;
	pSmnt->lastLength = length;

	if (memcmp(pDest, "\xFF\xFF\xFF\xFF\xFF\xFF", 6) == 0)
	{
		if (pSmnt->state == SMART_CH_LOCKING)
		{
			if(isUplink == 1)
				printf("uplink:(%02x-%04d)->length:%2x, =length-synfirst:(0x%02x),synfirst:%2x\n", *((uint8*)pHeader) & 0xFF, pHeader->Sequence, length, (uint8)(length - pSmnt->syncFirst +1),pSmnt->syncFirst);
			else
				printf("downlink:(%02x-%04d)->length:%2x, =length-synfirst:(0x%02x),synfirst:%2x\n", *((uint8*)pHeader) & 0xFF, pHeader->Sequence, length, (uint8)(length - pSmnt->syncFirst_downlink +1),pSmnt->syncFirst_downlink);
			
		}
		packetType = 2;
	}
	else if (memcmp(pDest, "\x01\x00\x5E", 3) == 0)
	{
		if (pSmnt->state == SMART_CH_LOCKING)
			printf("(%02x-%04d):%02x:%02x:%02x->%d\n", *((uint8*)pHeader) & 0xFF, pHeader->Sequence, pDest[3], pDest[4], pDest[5], (uint8)length);
		packetType = 1;
	}

	if (memcmp(pSrc, pSmnt->syncAppMac, 6) != 0)
	{
		isDifferentAddr = 1;
	}

	if(pSmnt->state == SMART_CH_LOCKING)
	{
		if (packetType == 0) return;
		if ((isUplink==1) && (pHeader->Sequence == lastSeq_uplink)) return;
		if ((isUplink==0) && (pHeader->Sequence == lastSeq_downlink)) return;
		if(!isDifferentAddr)
		{
			if (packetType != 0)
			{
				if (packetType == 1)
				{
					if (((pDest[3] >> 6) == ((pDest[4] ^ pDest[5]) & 0x1)) && (pDest[3] != 0) && ((pDest[3]&0x3F) <=  PAYLOAD_MAX))
					{
						/*if receive multicast right message for two times,lock the channel*/
						if(past_channel == pSmnt->chCurrentIndex + 1)
						{
							past_channel = 0xFF;
							if (pSmnt->chCurrentProbability < 20) 
								pSmnt->chCurrentProbability = 10;

							memcpy(pSmnt->syncBssid, pBssid, 6);
							pSmnt->state = SMART_CH_LOCKED;
						}
						else
						{
							past_channel = pSmnt->chCurrentIndex + 1;
						}

					}
					joylink_smnt_muticastadd(pDest); // Internal state machine could delay the ch switching
					return;
				}

				if(isUplink == 1)
				{
					if (lastLength == length)	return;

					int expectLength = 1 + pSmnt->syncFirst + pSmnt->syncCount%4 - (pSmnt->syncStepPoint?4:0);
					int isStep = (pSmnt->syncStepPoint == 0 && length == (expectLength - 4));	

					if ( ( length == expectLength ) || isStep)
					{
						pSmnt->syncCount++;
						pSmnt->chCurrentProbability++;

						if (isStep)		pSmnt->syncStepPoint = pSmnt->syncCount;

						if (pSmnt->syncCount >= 3)	// Achive SYNC count!
						//if (pSmnt->syncCount >= 4)	// Achive SYNC count!
						{
							pSmnt->syncFirst = pSmnt->syncFirst + pSmnt->syncStepPoint - (pSmnt->syncStepPoint ? 4 : 0);	// Save sync world
							memcpy(pSmnt->syncBssid, pBssid, 6);

							pSmnt->state 		= SMART_CH_LOCKED;

							printf("SYNC:(%02X%02X%02X%02X%02X%02X-%02X%02X%02X%02X%02X%02X)------->:CH=%d, WD=%d\n",
								pSrc[0], pSrc[1], pSrc[2], pSrc[3], pSrc[4], pSrc[5],
								pBssid[0], pBssid[1], pBssid[2], pBssid[3], pBssid[4], pBssid[5],
								pSmnt->chCurrentIndex, pSmnt->syncFirst);
							
							pSmnt->syncIsUplink = isUplink;
							
							if(pSmnt->chCurrentProbability < 20)
								pSmnt->chCurrentProbability = 20;
							printf("--->locked by uplink\n");
						}
						return;
					}
					if (pSmnt->syncCount)
					{
						pSmnt->syncStepPoint = 0;
						pSmnt->syncCount = 0;
						memcpy(pSmnt->syncAppMac, pSrc, 6);
						pSmnt->syncFirst = length;
						printf("SYNC LOST\n");
					}
				}
				else
				{
					if (lastLength == length)	return;

					int expectLength = 1 + pSmnt->syncFirst_downlink+ pSmnt->syncCount_downlink%4 - (pSmnt->syncStepPoint_downlink?4:0);
					int isStep = (pSmnt->syncStepPoint_downlink == 0 && length == (expectLength - 4));	

					if ( ( length == expectLength ) || isStep)
					{
						pSmnt->syncCount_downlink++;
						pSmnt->chCurrentProbability++;
						
						if (isStep)			pSmnt->syncStepPoint_downlink = pSmnt->syncCount_downlink;

						if (pSmnt->syncCount_downlink>= 3)	// Achive SYNC count!
						//if (pSmnt->syncCount_downlink>= 4)	// Achive SYNC count!
						{
							pSmnt->syncFirst_downlink = pSmnt->syncFirst_downlink + pSmnt->syncStepPoint_downlink - (pSmnt->syncStepPoint_downlink? 4 : 0);	// Save sync world
							memcpy(pSmnt->syncBssid, pBssid, 6);
							pSmnt->state = SMART_CH_LOCKED;
							printf("SYNC:(%02X%02X%02X%02X%02X%02X-%02X%02X%02X%02X%02X%02X)------->:CH=%d, WD=%d\n",
								pSrc[0], pSrc[1], pSrc[2], pSrc[3], pSrc[4], pSrc[5],
								pBssid[0], pBssid[1], pBssid[2], pBssid[3], pBssid[4], pBssid[5],
								pSmnt->chCurrentIndex, pSmnt->syncFirst_downlink);

							pSmnt->syncIsUplink = isUplink;
							if(pSmnt->chCurrentProbability < 20)
								pSmnt->chCurrentProbability = 20;
							printf("--->locked by downlink\n");
						}
						return;
					}
					pSmnt->syncStepPoint_downlink = 0;
					pSmnt->syncCount_downlink = 0;
					memcpy(pSmnt->syncAppMac, pSrc, 6);
					pSmnt->syncFirst_downlink = length;
					printf("SYNC LOST\n");
				}
			} 
			return;	
		}
		memcpy(pSmnt->syncAppMac, pSrc, 6);
		pSmnt->syncFirst = length;
		pSmnt->syncFirst_downlink = length;
		printf("Try to SYNC!\n");
		return;
	}
	else if (pSmnt->state == SMART_CH_LOCKED)
	{
		if (isDifferentAddr) return;

		if (packetType == 1){
			joylink_smnt_muticastadd(pDest);
			return;
		}
		
		if ( (packetType != 1)&&(memcmp(pDest, pSmnt->syncBssid, 6) != 0) ){
			packetType = 2;
		}
		
		if(isUplink == 1)
		{
			if (length < (pSmnt->syncFirst - 1)) return;
			if ( pHeader->Sequence == lastSeq_uplink) return;
		}
		else
		{
			if (length < (pSmnt->syncFirst_downlink - 1)) return;
			if ( pHeader->Sequence == lastSeq_downlink) return;
		}
		if (packetType == 2)
		{
			int ascii;
			if(isUplink == 1)
				ascii = length - pSmnt->syncFirst + 1;
			else
				ascii = length - pSmnt->syncFirst_downlink + 1;

			if((((ascii >> 8) & 0x01) == 1) && (((ascii >> 3)&0x1F) == 0))
			{
				;
			}
			else
			{
				joylink_smnt_broadcastadd(ascii);
			}
			if (((length + 4 - lastLength) % 4 == 1)&& (length - pSmnt->syncFirst)<4)  // There are SYNC packets even ch locked.
			{
				if (pSmnt->chCurrentProbability < 20) pSmnt->chCurrentProbability++;
			}
		}
	}
	else if (pSmnt->state == SMART_FINISH)
	{
		printf("SMART_FINISH-1\n");
	}
	else
	{
		pSmnt->state = SMART_CH_LOCKING;
		memcpy(pSmnt->syncAppMac, pSrc, 6);
		pSmnt->syncFirst = length;
		pSmnt->syncStepPoint = 0;
		pSmnt->syncCount = 0;
		printf("Reset All State\n");
	}
	return;
}

static int joylink_smnt_payLoadcheck(uint8 *payload)
{
	uint8 crc = joylink_smnt_crc(payload + 1, payload[1]+1);
	if ((payload[1] > (PAYLOAD_MIN*2)) &&
		(payload[1] < (PAYLOAD_MAX*2)) &&
        (payload[0] == crc)){

		smnt_encrypt_data_t* pRet = &pSmnt->result;
		memcpy(pRet->encData, payload+1, payload[1]+1);
		pRet->type = pSmnt->result.type;
        pSmnt->state = SMART_FINISH;
        joylink_smnt_finish();
        return 0;
	}
	return 1;
}


static uint8 joylink_smnt_crc(uint8 *ptr, uint8 len)
{
	unsigned char crc;
	unsigned char i;
	crc = 0;
	while (len--)
	{
		crc ^= *ptr++;
		for (i = 0; i < 8; i++)
		{
			if (crc & 0x01)
			{
				crc = (crc >> 1) ^ 0x8C;
			}
			else
				crc >>= 1;
		}
	}
	return crc;
}
static void joylink_smnt_bufferprintf(uint8* p, int split, int len)
{
#ifdef DEBUG_PRINT
	int i;
	char buf[512];
	int index = 0;
	for (i = 0; i < len; i++)
	{
		if (split != 0 && ((i + 1) % split) == 0)
		{
			index += sprintf(buf + index, "%02x,", p[i]);
		}
		else
			index += sprintf(buf + index, "%02x ", p[i]);
	}
	printf("Len=%d:%s\n",len, buf);
#endif
}

