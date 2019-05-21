/* ***** BEGIN LICENSE BLOCK *****
 * Version: RCSL 1.0/RPSL 1.0 
 *  
 * Portions Copyright (c) 1995-2002 RealNetworks, Inc. All Rights Reserved. 
 *      
 * The contents of this file, and the files included with this file, are 
 * subject to the current version of the RealNetworks Public Source License 
 * Version 1.0 (the "RPSL") available at 
 * http://www.helixcommunity.org/content/rpsl unless you have licensed 
 * the file under the RealNetworks Community Source License Version 1.0 
 * (the "RCSL") available at http://www.helixcommunity.org/content/rcsl, 
 * in which case the RCSL will apply. You may also obtain the license terms 
 * directly from RealNetworks.  You may not use this file except in 
 * compliance with the RPSL or, if you have a valid RCSL with RealNetworks 
 * applicable to this file, the RCSL.  Please see the applicable RPSL or 
 * RCSL for the rights, obligations and limitations governing use of the 
 * contents of the file.  
 *  
 * This file is part of the Helix DNA Technology. RealNetworks is the 
 * developer of the Original Code and owns the copyrights in the portions 
 * it created. 
 *  
 * This file, and the files included with this file, is distributed and made 
 * available on an 'AS IS' basis, WITHOUT WARRANTY OF ANY KIND, EITHER 
 * EXPRESS OR IMPLIED, AND REALNETWORKS HEREBY DISCLAIMS ALL SUCH WARRANTIES, 
 * INCLUDING WITHOUT LIMITATION, ANY WARRANTIES OF MERCHANTABILITY, FITNESS 
 * FOR A PARTICULAR PURPOSE, QUIET ENJOYMENT OR NON-INFRINGEMENT. 
 * 
 * Technology Compatibility Kit Test Suite(s) Location: 
 *    http://www.helixcommunity.org/content/tck 
 * 
 * Contributor(s): 
 *  
 * ***** END LICENSE BLOCK ***** */ 

/**************************************************************************************
 * Fixed-point MP3 decoder
 * Jon Recker (jrecker@real.com), Ken Cooke (kenc@real.com)
 * June 2003
 *
 * mp3dec.c - platform-independent top level MP3 decoder API
 **************************************************************************************/
#include "include.h"    // J.Sz. 21/04/2006
#include "string.h"     // J.Sz. 21/04/2006
#include "mp3common.h"	/* includes mp3dec.h (public API) and internal, platform-independent API */
#include "layer21.h"
#include "uart_pub.h"

#if (CONFIG_APP_MP3PLAYER == 1)
#define MP3HEADERMASK           0x0F0CFFFF

int16  MP3FixDiv(int32 num,int32 denum)
{
    int16 div = 0;
    if(denum <= 0)
        return 0;
    if(num < denum)
        return 0;
    while(num >= denum)
    {
        num -= denum;
        div++;
    }
    return div;
    
}
/**************************************************************************************
 * Function:    MP3InitDecoder
 *
 * Description: allocate memory for platform-specific data
 *              clear all the user-accessible fields
 *
 * Inputs:      none
 *
 * Outputs:     none
 *
 * Return:      handle to mp3 decoder instance, 0 if malloc fails
 **************************************************************************************/
HMP3Decoder  MP3InitDecoder(void)
{
	MP3DecInfo *mp3DecInfo;

	mp3DecInfo = AllocateBuffers();
	return (HMP3Decoder)mp3DecInfo;
}

/**************************************************************************************
 * Function:    MP3FreeDecoder
 *
 * Description: free platform-specific data allocated by InitMP3Decoder
 *              zero out the contents of MP3DecInfo struct
 *
 * Inputs:      valid MP3 decoder instance pointer (HMP3Decoder)
 *
 * Outputs:     none
 *
 * Return:      none
 **************************************************************************************/
void  MP3FreeDecoder(HMP3Decoder hMP3Decoder)
{
	MP3DecInfo *mp3DecInfo = (MP3DecInfo *)hMP3Decoder;

	if (!mp3DecInfo)
		return;

	FreeBuffers(mp3DecInfo);
}

/**************************************************************************************
 * Function:    MP3FindSyncWord
 *
 * Description: locate the next byte-alinged sync word in the raw mp3 stream
 *
 * Inputs:      buffer to search for sync word
 *              max number of bytes to search in buffer
 *
 * Outputs:     none
 *
 * Return:      offset to first sync word (bytes from start of buf)
 *              -1 if sync not found after searching nBytes
 *              SYNCWORD_NOT_SURE means that this syncword is not sure, because
 *              there are no enough data in the buffer
 **************************************************************************************/
int  MP3FindSyncWord(HMP3Decoder hMP3Decoder,unsigned char *buf, int nBytes)
{
	int i;
	int frsz;
	uint32_t header0,header1;
    MP3DecInfo *mp3DecInfo = (MP3DecInfo *)hMP3Decoder;
	/* find byte-aligned syncword - need 12 (MPEG 1,2) or 11 (MPEG 2.5) matching bits */
	for (i = 0; i < nBytes - 5; i++) {
		if ( (buf[i+0] & SYNCWORDH) == SYNCWORDH && (buf[i+1] & SYNCWORDL) == SYNCWORDL )
		{
		    
		    frsz = UnpackFrameHeader(mp3DecInfo,buf+i);
		    if(frsz < 0)
		    {
		        //os_printf("MP3 Header Err...0\r\n");
		        continue;
		    }
		    frsz = mp3DecInfo->framesize;
		    if(frsz == 0)
		    {
		        os_printf("MP3 Header Err...1\r\n");
		        continue;
		    }
		    if((frsz + i) > (nBytes-5))
		    {
		        /* this syncword is not sure, because there are no enough data 
		        in the buffer */
		        return SYNCWORD_NOT_SURE | i; 
		    }
		    memcpy(&header0,buf+i,4);
		    memcpy(&header1,buf+i+frsz,4);
		    //os_printf("head01:%x,%x,i = %d, frsz = %d,nBytes = %d\r\n",header0,header1,i,frsz,nBytes);
		    if((header0 & MP3HEADERMASK) == (header1 & MP3HEADERMASK))
		    {
		        return i;
		    }
		    else
		    {
		        //os_printf("MP3 Header Err...2\r\n");
		        continue;
		    }
		}
			
	}
	return -1;
}

int  MP3FindSyncWord_Free(unsigned char *buf, int nBytes)
{
	int i;
	/* find byte-aligned syncword - need 12 (MPEG 1,2) or 11 (MPEG 2.5) matching bits */
	for (i = 0; i < nBytes - 1; i++) {
		if ( (buf[i+0] & SYNCWORDH) == SYNCWORDH && (buf[i+1] & SYNCWORDL) == SYNCWORDL )
		{
		    return i;
		}
			
	}
	
	return -1;
}

/**************************************************************************************
 * Function:    MP3FindFreeSync
 *
 * Description: figure out number of bytes between adjacent sync words in "free" mode
 *
 * Inputs:      buffer to search for next sync word
 *              the 4-byte frame header starting at the current sync word
 *              max number of bytes to search in buffer
 *
 * Outputs:     none
 *
 * Return:      offset to next sync word, minus any pad byte (i.e. nSlots)
 *              -1 if sync not found after searching nBytes
 *
 * Notes:       this checks that the first 22 bits of the next frame header are the
 *                same as the current frame header, but it's still not foolproof
 *                (could accidentally find a sequence in the bitstream which 
 *                 appears to match but is not actually the next frame header)
 *              this could be made more error-resilient by checking several frames
 *                in a row and verifying that nSlots is the same in each case
 *              since free mode requires CBR (see spec) we generally only call
 *                this function once (first frame) then store the result (nSlots)
 *                and just use it from then on
 **************************************************************************************/
static int  MP3FindFreeSync(unsigned char *buf, unsigned char firstFH[4], int nBytes)
{
	int offset = 0;
	unsigned char *bufPtr = buf;
	/* loop until we either: 
	 *  - run out of nBytes (FindMP3SyncWord() returns -1)
	 *  - find the next valid frame header (sync word, version, layer, CRC flag, bitrate, and sample rate
	 *      in next header must match current header)
	 */
	while (1) {
		offset = MP3FindSyncWord_Free(bufPtr, nBytes);
		bufPtr += offset;
		if (offset < 0) {
			return -1;
		} else if ( (bufPtr[0] == firstFH[0]) && (bufPtr[1] == firstFH[1]) && ((bufPtr[2] & 0xfc) == (firstFH[2] & 0xfc)) ) {
			/* want to return number of bytes per frame, NOT counting the padding byte, so subtract one if padFlag == 1 */
			if ((firstFH[2] >> 1) & 0x01)
				bufPtr--;
			return bufPtr - buf;
		}
		bufPtr += 3;
		nBytes -= (offset + 3);
	};

	return -1;
}

/**************************************************************************************
 * Function:    MP3GetLastFrameInfo
 *
 * Description: get info about last MP3 frame decoded (number of sampled decoded, 
 *                sample rate, bitrate, etc.)
 *
 * Inputs:      valid MP3 decoder instance pointer (HMP3Decoder)
 *              pointer to MP3FrameInfo struct
 *
 * Outputs:     filled-in MP3FrameInfo struct
 *
 * Return:      none
 *
 * Notes:       call this right after calling MP3Decode
 **************************************************************************************/
void  MP3GetLastFrameInfo(HMP3Decoder hMP3Decoder, MP3FrameInfo *mp3FrameInfo)
{
	MP3DecInfo *mp3DecInfo = (MP3DecInfo *)hMP3Decoder;

	//if (!mp3DecInfo || mp3DecInfo->layer != 3) {
	if(!mp3DecInfo)
	{
		mp3FrameInfo->bitrate = 0;
		mp3FrameInfo->nChans = 0;
		mp3FrameInfo->samprate = 0;
		mp3FrameInfo->bitsPerSample = 0;
		mp3FrameInfo->outputSamps = 0;
		mp3FrameInfo->layer = 0;
		mp3FrameInfo->version = 0;
	} else {
		mp3FrameInfo->bitrate = mp3DecInfo->bitrate;
		mp3FrameInfo->nChans = mp3DecInfo->nChans;
		mp3FrameInfo->samprate = mp3DecInfo->samprate;
		mp3FrameInfo->bitsPerSample = 16;
		mp3FrameInfo->outputSamps = mp3DecInfo->nChans * (int)samplesPerFrameTab[mp3DecInfo->version][mp3DecInfo->layer - 1];
		mp3FrameInfo->layer = mp3DecInfo->layer;
		mp3FrameInfo->version = mp3DecInfo->version;
	}
}

/**************************************************************************************
 * Function:    MP3GetNextFrameInfo
 *
 * Description: parse MP3 frame header
 *
 * Inputs:      valid MP3 decoder instance pointer (HMP3Decoder)
 *              pointer to MP3FrameInfo struct
 *              pointer to buffer containing valid MP3 frame header (located using 
 *                MP3FindSyncWord(), above)
 *
 * Outputs:     filled-in MP3FrameInfo struct
 *
 * Return:      error code, defined in mp3dec.h (0 means no error, < 0 means error)
 **************************************************************************************/
int  MP3GetNextFrameInfo(HMP3Decoder hMP3Decoder, MP3FrameInfo *mp3FrameInfo, unsigned char *buf)
{
	MP3DecInfo *mp3DecInfo = (MP3DecInfo *)hMP3Decoder;

	if (!mp3DecInfo)
		return ERR_MP3_NULL_POINTER;

	//if (UnpackFrameHeader(mp3DecInfo, buf) == -1 || mp3DecInfo->layer != 3)
	if (UnpackFrameHeader(mp3DecInfo, buf) == -1)
		return ERR_MP3_INVALID_FRAMEHEADER;

	MP3GetLastFrameInfo(mp3DecInfo, mp3FrameInfo);

	return ERR_MP3_NONE;
}

/**************************************************************************************
 * Function:    MP3ClearBadFrame
 *
 * Description: zero out pcm buffer if error decoding MP3 frame
 *
 * Inputs:      mp3DecInfo struct with correct frame size parameters filled in
 *              pointer pcm output buffer
 *
 * Outputs:     zeroed out pcm buffer
 *
 * Return:      none
 **************************************************************************************/
static void  MP3ClearBadFrame(MP3DecInfo *mp3DecInfo, short *outbuf)
{
}


#include "ff.h"
extern FIL *newFatfs;

#define   BUF_CUTDOWN_DEBUG    os_printf

const char ID3_HEADER[3] = "ID3";
int  MP3_Get_ID3_Offset(unsigned char *buf, int nBytes)
{
	int offset;
	/* the head of id3 should be large than 9 byte */
	if(nBytes < 10)
		return -1;
	/* in the start of the mp3 file, "ID3" stand for the id3 head */
	if ((buf[0]==ID3_HEADER[0]) && (buf[1]==ID3_HEADER[1]) && (buf[2]==ID3_HEADER[2]))
	{
		/* byte6-9 is size(28bit) of all title, each of them only use the low 7bit */
		offset = ((buf[6]&0x7f)<<21) + ((buf[7]&0x7f)<<14) + ((buf[8]&0x7f)<<7) + (buf[9]&0x7f);
		/* offset need add the head of ID3 */
		offset += 10; 

		return offset;
	}

	return -1;
}

static int   file_read(unsigned char *buf, int size)
{
	unsigned int readbytes=0;
	int ret;
    if (newFatfs == NULL)
        return -1;

	ret = f_read(newFatfs, buf, size, &readbytes);
	if( ret != FR_OK ) {
        BUF_CUTDOWN_DEBUG("file_read: ret%d\r\n", ret);
		return FR_DISK_ERR;
	}
	ret = f_EOF(newFatfs);
	
	if( ret == FR_FILE_END)
		return FR_FILE_END;
	
	if(readbytes != size)
		return FR_DISK_ERR;
	
	return FR_OK;
}

static int  file_seek(int pos)
{
	int ret;
	if (newFatfs == NULL)
        return FR_NO_FILE;

	ret = f_lseek(newFatfs, (uint32)pos);
	if(ret != FR_OK)
		return ret;
	
	return(f_EOF(newFatfs));
}

/**************************************************************************************
 * Function:    MP3Decode
 *
 * Description: decode one frame of MP3 data
 *
 * Inputs:      valid MP3 decoder instance pointer (HMP3Decoder)
 *              double pointer to buffer of MP3 data (containing headers + mainData)
 *              number of valid bytes remaining in inbuf
 *              pointer to outbuf, big enough to hold one frame of decoded PCM samples
 *              flag indicating whether MP3 data is normal MPEG format (useSize = 0)
 *                or reformatted as "self-contained" frames (useSize = 1)
 *
 * Outputs:     PCM data in outbuf, interleaved LRLRLR... if stereo
 *                number of output samples = nGrans * nGranSamps * nChans
 *              updated inbuf pointer, updated bytesLeft
 *
 * Return:      error code, defined in mp3dec.h (0 means no error, < 0 means error)
 *
 * Notes:       switching useSize on and off between frames in the same stream 
 *                is not supported (bit reservoir is not maintained if useSize on)
 **************************************************************************************/
#define  GET_MAINBUF_FREE_LEN(p)     ((int)((p->mainBuf+READBUF_SIZE)-(p->mainBuf_ptr+p->mainBuf_len)))
#define  GET_MAINBUF_DATA_LEN(p)     ((int)(p->mainBuf_len))

int  MP3FileGetData(MP3DecInfo *mp3DecInfo, unsigned char flag, 
                         unsigned char **inbuf, int *bytesLeft)
{

	int ret = 0;
	unsigned char *buf=NULL;
	int readsize = 0;

	buf = mp3DecInfo->mainBuf+BIT_RESVOR_SIZE;
	if(!flag)
	{ 
		readsize = READBUF_SIZE-BIT_RESVOR_SIZE;  // 1940 - 512 = 1428 = 357*4
		BUF_CUTDOWN_DEBUG("full rdsize:%d\r\n", readsize);
		ret = file_read(buf, readsize);
		BUF_CUTDOWN_DEBUG("full ret:%d\r\n", ret);
		if(ret != FR_OK)
			return ret;
		*bytesLeft = READBUF_SIZE-BIT_RESVOR_SIZE;
	}
	else 
	{
		uint8 *prt = (uint8 *)mp3DecInfo->HuffmanInfoPS;
		
		memmove(buf, *inbuf, mp3DecInfo->mainBuf_len);
		mp3DecInfo->mainBuf_ptr = mp3DecInfo->mainBuf+BIT_RESVOR_SIZE; 

		buf += mp3DecInfo->mainBuf_len;
		readsize = GET_MAINBUF_FREE_LEN(mp3DecInfo);

		readsize &= 0xfffffffc;
		ret = file_read(prt, readsize);
		if(ret != FR_OK)
			return ret;
		memmove(buf, prt, readsize);

		*bytesLeft += readsize;
	}
	
	mp3DecInfo->mainBuf_ptr = mp3DecInfo->mainBuf+BIT_RESVOR_SIZE;
	*inbuf = mp3DecInfo->mainBuf+BIT_RESVOR_SIZE;
	
	return 0;
}

#if CALC_PLAY_TIME
const char XING_HDR[4] = "Xing";
const char VBRI_HDR[4] = "VBRI";  
void MP3_Find_CBR_Header(MP3DecInfo *mp3DecInfo, unsigned char *buf, 
    int nBytes)
{
	/* the head of id3 should be large than 9 byte */
	if(nBytes < 8)
		return;    

    if((buf[0]==XING_HDR[0]) && (buf[1]==XING_HDR[1]) 
        && (buf[2]==XING_HDR[2]) && (buf[3]==XING_HDR[3])) {
        unsigned char flags = buf[7];
        os_printf("xing header\r\n");
        os_printf("flag:0x%x-0x%x-0x%x-0x%x\r\n", buf[4], buf[5],buf[6], buf[7]);
        mp3DecInfo->filetype = VBR_XING_FILE;
        buf += 8;
        nBytes -= 8;
        
        if((flags&0x1)){
            unsigned char *total_frame_ptr = buf;
            unsigned int total_frame;
            if(nBytes > 4){
                total_frame = (total_frame_ptr[0]<<24) 
                        + (total_frame_ptr[1]<<16) + (total_frame_ptr[2]<<8) 
                        + (total_frame_ptr[3]);
                os_printf("total frames:%d\r\n", total_frame);
                mp3DecInfo->totalframes = total_frame;
                buf += 4;
            } else {
                return;    
            }
        } 
    }
    else if((buf[0]==VBRI_HDR[0]) && (buf[1]==VBRI_HDR[1]) 
        && (buf[2]==VBRI_HDR[2]) && (buf[3]==VBRI_HDR[3])) {
        unsigned char *total_frame_ptr = buf + 14;
        unsigned int total_frame;
        os_printf("vbri header\r\n");
        mp3DecInfo->filetype = VBR_VBRI_FILE;
        nBytes -= 14;
        if(nBytes > 4){
            total_frame = (total_frame_ptr[0]<<24) 
                    + (total_frame_ptr[1]<<16) + (total_frame_ptr[2]<<8) 
                    + (total_frame_ptr[3]);
            os_printf("total frames:%d\r\n", total_frame);
            mp3DecInfo->totalframes = total_frame;
        } else {
            return;    
        }
        
    } else {
        os_printf("no a VBR file, maybe a CBR file\r\n");
        os_printf("totalframes is ready:%d\r\n", mp3DecInfo->totalframes);
       // mp3DecInfo->totalframes = fUseLen;
        mp3DecInfo->filetype = CBR_FILE;
    }
}

void MP3_Show_Play_Time(int time)
{
    int min, sec;

    min = time / 60;
    sec = time % 60;
    os_printf("%02d:%02d\r\n", min, sec);
}

void MP3_Calc_Play_Time(HMP3Decoder hMP3Decoder, MP3FrameInfo *mp3FrameInfo)
{
    int total_playtime;
	MP3DecInfo *mp3DecInfo = (MP3DecInfo *)hMP3Decoder; 

    int filetype = mp3DecInfo->filetype;
    mp3DecInfo->timePerframe = PRECISION * mp3FrameInfo->outputSamps
                        / (mp3FrameInfo->nChans * mp3FrameInfo->samprate);
    os_printf("timePerframe = %d\r\n",mp3DecInfo->timePerframe);

    switch(filetype){
        case CBR_FILE:
            total_playtime = mp3DecInfo->totalframes * 8 / mp3DecInfo->bitrate;
            os_printf("total time = %d\r\n",total_playtime);
            MP3_Show_Play_Time(total_playtime);
            break;

        case VBR_VBRI_FILE:
        case VBR_XING_FILE:{
                total_playtime = mp3DecInfo->totalframes * mp3DecInfo->timePerframe
                    / PRECISION;
                os_printf("total time = %d\r\n",total_playtime);
                MP3_Show_Play_Time(total_playtime);
            }
            break;  
        default:
            break;
    }
}

int MP3_Calc_Current_Play_Time(HMP3Decoder hMP3Decoder)
{
    int current_playtime;
	MP3DecInfo *mp3DecInfo = (MP3DecInfo *)hMP3Decoder;    

    current_playtime = mp3DecInfo->curFramecnt * mp3DecInfo->timePerframe 
        / PRECISION;

  //  MP3_Show_Play_Time(current_playtime);
    
    return current_playtime;
}
#endif
int bitOffset, mainBits, gr;
unsigned char *mainPtr;
int  MP3Decode(HMP3Decoder hMP3Decoder, short *outbuf, int *pcm_size)
{
	int offset,ch, fhBytes, siBytes, freeFrameBytes,result;
	int prevBitOffset, sfBlockBits, huffBlockBits;

	MP3DecInfo *mp3DecInfo = (MP3DecInfo *)hMP3Decoder;

	unsigned char **inbuf = NULL;
	int *bytesLeft = NULL;

	if (!mp3DecInfo)
		return ERR_MP3_NULL_POINTER;

	inbuf = &mp3DecInfo->mainBuf_ptr;
	bytesLeft = &mp3DecInfo->mainBuf_len;
	*pcm_size = 0;

start_decode:
	switch (mp3DecInfo->decode_state)
	{
		case MP3_DECODE_FIND_ID3_INFO:
		{
			int offset, off_4_lign;
//			if(MP3FileGetData(mp3DecInfo, 0, inbuf, bytesLeft))
//				return ERR_MP3_READ_DATA_FAILED;
			
			result = MP3FileGetData(mp3DecInfo, 0, inbuf, bytesLeft);
			if(result != FR_OK)
			{
				if(result == ERR_MP3_FILE_END)
					return ERR_MP3_FILE_END;
				else
					return ERR_MP3_READ_DATA_FAILED;
			}
				
			offset = MP3_Get_ID3_Offset(*inbuf, *bytesLeft);
			BUF_CUTDOWN_DEBUG("ID3: offset:%d\r\n", offset);

			if(offset<=0)
			{
				os_printf("can't found ID3 info\r\n");
				mp3DecInfo->decode_state = MP3_DECODE_FIND_SYNC_WORD;
				goto start_decode;
			}
            #if CALC_PLAY_TIME
            mp3DecInfo->totalframes -= offset;
            os_printf("change totalframes len:%d, offset:%d\r\n", mp3DecInfo->totalframes
                , offset);
            #endif

			off_4_lign = offset & 0xfffffffc;   // bk3253 file_seek must 4btye laign
			BUF_CUTDOWN_DEBUG("off_4_lign0:%d\r\n", off_4_lign);
			result = file_seek(off_4_lign);
			
			if(result != FR_OK)
			{
				if(result != ERR_MP3_FILE_END)
					return ERR_MP3_READ_DATA_FAILED;
				else
					return ERR_MP3_FILE_END;
			}

			*bytesLeft = 0; //give up all the date that ready read.
			
//			if(MP3FileGetData(mp3DecInfo, 0, inbuf, bytesLeft))
//				return ERR_MP3_READ_DATA_FAILED;
			result = MP3FileGetData(mp3DecInfo, 0, inbuf, bytesLeft);
			if(result != FR_OK)
			{
				if(result == ERR_MP3_FILE_END)
					return ERR_MP3_FILE_END;
				else
					return ERR_MP3_READ_DATA_FAILED;
			}

			off_4_lign = offset & 0x3; 
			BUF_CUTDOWN_DEBUG("off_4_lign1:%d\r\n", off_4_lign);
			if(off_4_lign)
				memset(*inbuf, 0, off_4_lign); // clear the unuse data
			
			mp3DecInfo->decode_state = MP3_DECODE_FIND_SYNC_WORD;
			//break;
		}

		case MP3_DECODE_FIND_SYNC_WORD:
		{
			int ofst = MP3FindSyncWord((HMP3Decoder)mp3DecInfo,*inbuf, *bytesLeft);
			if(ofst<0)
			{
//				if(MP3FileGetData(mp3DecInfo, 0, inbuf, bytesLeft))
//					return ERR_MP3_READ_DATA_FAILED;
				result = MP3FileGetData(mp3DecInfo, 0, inbuf, bytesLeft);
				if(result != FR_OK)
				{
					if(result == ERR_MP3_FILE_END)
						return ERR_MP3_FILE_END;
					else
						return ERR_MP3_READ_DATA_FAILED;
				}

				/* if err counter exceeds the MAX_DECODE_ERR_TIMES, we should exit the whole decode process */
				mp3DecInfo->err_cnt++;
				if(mp3DecInfo->err_cnt > MAX_DECODE_ERR_TIMES)
					return ERR_MP3_DECODE_MAX_ERR;
				goto start_decode;
			}
            
            offset = ofst & (~SYNCWORD_NOT_SURE); // get the offset
            ofst = (ofst & SYNCWORD_NOT_SURE)?1:0; // syncword is not sure?

			*inbuf += offset;
			*bytesLeft -= offset;
            
				result = MP3FileGetData(mp3DecInfo, 1, inbuf, bytesLeft);
				if(result != FR_OK)
				{
					if(result == ERR_MP3_FILE_END)
						return ERR_MP3_FILE_END;
					else
						return ERR_MP3_READ_DATA_FAILED;
				}
            
            if(ofst) // syncword is not sure
            {
                /* we isn't sure that the sync word is right or not, at this time,
                we have filled our buffer, if we have enough data in buffer,so 
                call MP3FindSyncWord again. buf if the whole buffer len is less 
                than one frame, let decode to slove this problem */
                if(GET_MAINBUF_DATA_LEN(mp3DecInfo) >= mp3DecInfo->framesize + 5)
                    goto start_decode;
            }
			mp3DecInfo->decode_state = MP3_DECODE_HEADER_SIDEINFO;
			//break;
		}

		case MP3_DECODE_HEADER_SIDEINFO:
		{
			/* unpack frame header */
			fhBytes = UnpackFrameHeader(mp3DecInfo, *inbuf);
			if (fhBytes < 0)
			{
				os_printf("I am sure this not a mp3 file\r\n");
				/* don't clear outbuf since we don't know size (failed to parse header) */
				return ERR_MP3_INVALID_FRAMEHEADER;		
			}

			if(mp3DecInfo->layer != 3)
			{
			    int32_t nL2FrmSize;
				int32_t header;
				memcpy(&header,*inbuf,4);
				nL2FrmSize = L2_GetFrameSize(mp3DecInfo->L2DecInfo,header);
				if(GET_MAINBUF_DATA_LEN(mp3DecInfo) < nL2FrmSize)
				{
//					if(MP3FileGetData(mp3DecInfo, 1, inbuf, bytesLeft))
//						return ERR_MP3_READ_DATA_FAILED;
					result = MP3FileGetData(mp3DecInfo, 1, inbuf, bytesLeft);
					if(result != FR_OK)
					{
						if(result == ERR_MP3_FILE_END)
							return ERR_MP3_FILE_END;
						else
							return ERR_MP3_READ_DATA_FAILED;
					}
				}	
                #if CALC_PLAY_TIME
                if(!mp3DecInfo->filetype) {
                    MP3_Find_CBR_Header(mp3DecInfo, *inbuf, *bytesLeft);
                }
                mp3DecInfo->curFramecnt++;
                #endif
				*pcm_size = L2_decode_frame(mp3DecInfo->L2DecInfo,outbuf,&ch,*inbuf,nL2FrmSize);
				//return ch;
				*inbuf += nL2FrmSize;
				*bytesLeft -= nL2FrmSize;
				mp3DecInfo->decode_state = MP3_DECODE_FIND_SYNC_WORD;
				return ERR_MP3_NONE;
			}
						
			*inbuf += fhBytes;
			
			/* unpack side info */
			siBytes = UnpackSideInfo(mp3DecInfo, *inbuf);
			if (siBytes < 0) 
			{
				MP3ClearBadFrame(mp3DecInfo, outbuf);
				return ERR_MP3_INVALID_SIDEINFO;
			}
			*inbuf += siBytes;
			*bytesLeft -= (fhBytes + siBytes);
            #if CALC_PLAY_TIME
            if(!mp3DecInfo->filetype) {
                MP3_Find_CBR_Header(mp3DecInfo, *inbuf, *bytesLeft);
            }
            mp3DecInfo->curFramecnt++;
            #endif

			/* if free mode, need to calculate bitrate and nSlots manually, based on frame size */
			if (mp3DecInfo->bitrate == 0 || mp3DecInfo->freeBitrateFlag) {
				if (!mp3DecInfo->freeBitrateFlag) {
					/* first time through, need to scan for next sync word and figure out frame size */
					mp3DecInfo->freeBitrateFlag = 1;
					mp3DecInfo->freeBitrateSlots = MP3FindFreeSync(*inbuf, *inbuf - fhBytes - siBytes, *bytesLeft);
					if (mp3DecInfo->freeBitrateSlots < 0) {
						MP3ClearBadFrame(mp3DecInfo, outbuf);
						return ERR_MP3_FREE_BITRATE_SYNC;
					}
					freeFrameBytes = mp3DecInfo->freeBitrateSlots + fhBytes + siBytes;
					mp3DecInfo->bitrate = (freeFrameBytes * mp3DecInfo->samprate * 8) / (mp3DecInfo->nGrans * mp3DecInfo->nGranSamps);
				}
				mp3DecInfo->nSlots = mp3DecInfo->freeBitrateSlots + CheckPadBit(mp3DecInfo);	/* add pad byte, if required */
			}
	
			mp3DecInfo->decode_state = MP3_DECODE_COPY_MAIN_DATE;
			//break;
		}

		case MP3_DECODE_COPY_MAIN_DATE:
		{
			{
				/* out of data - assume last or truncated frame */
				if (mp3DecInfo->nSlots > *bytesLeft) 
				{				
					MP3ClearBadFrame(mp3DecInfo, outbuf);
					return ERR_MP3_INDATA_UNDERFLOW;	
				}
			
				/* fill main data buffer with enough new data for this frame */
				if (mp3DecInfo->mainDataBytes >= mp3DecInfo->mainDataBegin) {

					unsigned char *p_des, *p_src;
					unsigned int len;
					p_des = mp3DecInfo->mainBuf+BIT_RESVOR_SIZE-mp3DecInfo->mainDataBegin;
					p_src = mp3DecInfo->bit_reservoir + BIT_RESVOR_SIZE - mp3DecInfo->mainDataBegin;
					len = mp3DecInfo->mainDataBegin;
					memmove(p_des, p_src, len);

					p_des = mp3DecInfo->mainBuf+BIT_RESVOR_SIZE;
					p_src = *inbuf;
					len = *bytesLeft;
					memmove(p_des, p_src, len);
					*inbuf = p_des;
									
					mp3DecInfo->mainDataBytes = mp3DecInfo->mainDataBegin + mp3DecInfo->nSlots;
					*inbuf += mp3DecInfo->nSlots;
					*bytesLeft -= (mp3DecInfo->nSlots);
					mainPtr = mp3DecInfo->mainBuf+BIT_RESVOR_SIZE-mp3DecInfo->mainDataBegin;

					p_des = mp3DecInfo->bit_reservoir;
					p_src = *inbuf - BIT_RESVOR_SIZE;
					len = BIT_RESVOR_SIZE;
					memcpy(p_des, p_src, len);					
					
				} else {
					/* not enough data in bit reservoir from previous frames (perhaps starting in middle of file) */
					unsigned char *p_des, *p_src;
					unsigned int len;

					p_des = mp3DecInfo->mainBuf+BIT_RESVOR_SIZE;
					p_src = *inbuf;
					len = *bytesLeft;
					memmove(p_des, p_src, len);
					*inbuf = p_des;

					mp3DecInfo->mainDataBytes += mp3DecInfo->nSlots;
					*inbuf += mp3DecInfo->nSlots;
					*bytesLeft -= (mp3DecInfo->nSlots);
					
					MP3ClearBadFrame(mp3DecInfo, outbuf);
					return ERR_MP3_MAINDATA_UNDERFLOW;
				}
			}

			bitOffset = 0;
			mainBits = mp3DecInfo->mainDataBytes * 8;

			mp3DecInfo->decode_state = MP3_DECODE_CALU_CRUCIAL_DAT;
			gr = 0;
			
			//break;	
		}

		case MP3_DECODE_CALU_CRUCIAL_DAT:
		{
			if(gr < mp3DecInfo->nGrans)
			{
				for (ch = 0; ch < mp3DecInfo->nChans; ch++)
				{
					/* unpack scale factors and compute size of scale factor block */
					prevBitOffset = bitOffset;
					offset = UnpackScaleFactors(mp3DecInfo, mainPtr, &bitOffset, mainBits, gr, ch);

					sfBlockBits = 8*offset - prevBitOffset + bitOffset;
					huffBlockBits = mp3DecInfo->part23Length[gr][ch] - sfBlockBits;
					mainPtr += offset;
					mainBits -= sfBlockBits;

					if (offset < 0 || mainBits < huffBlockBits) 
					{
						MP3ClearBadFrame(mp3DecInfo, outbuf);
						return ERR_MP3_INVALID_SCALEFACT;
					}

					/* decode Huffman code words */
					prevBitOffset = bitOffset;
					offset = DecodeHuffman(mp3DecInfo, mainPtr, &bitOffset, huffBlockBits, gr, ch);
					if (offset < 0) 
					{
						MP3ClearBadFrame(mp3DecInfo, outbuf);
						return ERR_MP3_INVALID_HUFFCODES;
					}

					mainPtr += offset;
					mainBits -= (8*offset - prevBitOffset + bitOffset);
				}
				/* dequantize coefficients, decode stereo, reorder short blocks */
				if (Dequantize(mp3DecInfo, gr) < 0) {
					MP3ClearBadFrame(mp3DecInfo, outbuf);
					return ERR_MP3_INVALID_DEQUANTIZE;			
				}

				/* alias reduction, inverse MDCT, overlap-add, frequency inversion */
				for (ch = 0; ch < mp3DecInfo->nChans; ch++)
					if (IMDCT(mp3DecInfo, gr, ch) < 0) 
					{
						MP3ClearBadFrame(mp3DecInfo, outbuf);
						return ERR_MP3_INVALID_IMDCT;			
					}
			}
			gr++;
			
			mp3DecInfo->decode_state = MP3_DECODE_CALU_SUBBAND;
		}
		
		case MP3_DECODE_CALU_SUBBAND:
		{						
			short *outbuf_ptr = outbuf;
			if (Subband(mp3DecInfo, &outbuf) < 0) 
			{
				MP3ClearBadFrame(mp3DecInfo, outbuf);
				return ERR_MP3_INVALID_SUBBAND;	
			}

			*pcm_size = outbuf - outbuf_ptr;
			
			if(gr < mp3DecInfo->nGrans)
			{
				mp3DecInfo->decode_state = MP3_DECODE_CALU_CRUCIAL_DAT;
			}
			else
			{
				mainPtr = mp3DecInfo->mainBuf_ptr;
				mp3DecInfo->decode_state = MP3_DECODE_FIND_SYNC_WORD;
			}
			break;
		}

		default:
			return ERR_UNKNOWN;
			break;
	}

	return ERR_MP3_NONE;
}
#endif /* CONFIG_APP_MP3PLAYER */

