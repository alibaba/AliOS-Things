#include "airkiss.h" 
#include <stdio.h> 
#include <string.h> 
#include <stdlib.h> 

#include "mac_frame.h"
#include "mac_ie.h"

#define PASSWORD_MAX_LEN     32 
#define ESSID_MAX_LEN        32 
#define USR_DATA_BUFF_MAX_SIZE    (PASSWORD_MAX_LEN + 1 + ESSID_MAX_LEN) 

typedef enum 
{ 
    AIRKISS_STATE_STOPED = 0, 
    AIRKISS_STATE_IDLE, 
    AIRKISS_STATE_SRC_LOCKED,
    AIRKISS_STATE_MAGIC_CODE_COMPLETE, 
    AIRKISS_STATE_PREFIX_CODE_COMPLETE,
    AIRKISS_STATE_COMPLETE 
} AIR_KISS_STATE; 

#define MAX_SRC_NUM         10
typedef struct {
    u8 crc;
    u8 rev_cnt;
    u8 ap_sta;
    u8 seq_cnt;
    u16 len;
    u16 seq;    
} src_info_st;

typedef struct 
{ 
    src_info_st src_info[MAX_SRC_NUM]; 
    u8 src_num;     
}guide_code_record;

#define MAX_MAGIC_CODE_RECORD    4 
typedef struct 
{ 
    unsigned short record[MAX_MAGIC_CODE_RECORD + 1]; 
}magic_code_record; 

#define MAX_PREFIX_CODE_RECORD    4 
typedef struct 
{ 
    unsigned short record[MAX_PREFIX_CODE_RECORD + 1]; 
}prfix_code_record; 

#define MAX_SEQ_CODE_RECORD    6 
typedef struct 
{ 
    unsigned short record[MAX_SEQ_CODE_RECORD + 1]; 
}seq_code_record; 

union airkiss_data{ 
    guide_code_record guide_code; 
    magic_code_record magic_code; 
    prfix_code_record prefix_code; 
    seq_code_record  seq_code; 
}; 

typedef struct 
{ 
    char* pwd;                         
    char* ssid; 
    unsigned char pswd_len; 
    unsigned char ssid_len; 
    unsigned char random_num; 
    unsigned char ssid_crc; //reserved used as ssid_crc 
    //above is airkiss_context_t 
    unsigned char usr_data[USR_DATA_BUFF_MAX_SIZE]; 
    unsigned char airkiss_state;  
    unsigned char src_mac_crc; 
    unsigned char src_ap_sta;  
    unsigned char base_len;     
    unsigned char need_seq; 
    unsigned char total_len; 
    unsigned char pswd_lencrc; 
    unsigned short seq_success_map; 
    unsigned short seq_success_map_cmp; 
    union airkiss_data data;
}_airkiss_local_context; 

const char airkiss_vers[] = "V1.0, Design by beken corporation\0";
static airkiss_config_t *akconf = 0; 
static _airkiss_local_context *_akcontext; 

//crc8 
static unsigned char calcrc_1byte(unsigned char abyte)     
{     
    unsigned char i,crc_1byte;      
    crc_1byte=0;                 
    for(i = 0; i < 8; i++)     
    {     
        if(((crc_1byte^abyte)&0x01))     
        {     
            crc_1byte^=0x18;      
            crc_1byte>>=1;     
            crc_1byte|=0x80;     
        }           
        else     
        { 
            crc_1byte>>=1;  
        } 
        abyte>>=1;           
    }    
    return crc_1byte;    
}   

static unsigned char calcrc_bytes(unsigned char *p,unsigned int num_of_bytes)   
{   
    unsigned char crc=0;   
    while(num_of_bytes--)  
    {   
        crc=calcrc_1byte(crc^*p++);   
    }   
    return crc;    
}

static void airkiss_resest_data(void) 
{ 
    akconf->memset(&_akcontext->data, 0, sizeof(union airkiss_data)); 
}

const char* airkiss_version(void) 
{ 
    return airkiss_vers; 
} 

/* return 1 is valid  
 * return 0 is invalid 
 * */ 
static int airkiss_filter(const unsigned char *frame, int size) 
{ 
    int isvalid = 0; 
    u8 *mac_ptr = NULL;
    struct mac_hdr *fmac_hdr = (struct mac_hdr *)frame;

    if(MAC_FCTRL_DATA_T != (fmac_hdr->fctl & MAC_FCTRL_TYPE_MASK))
    {
        goto invalid; 
    }

    // after discover
    // at least base len, not more than 2^9;
    if(size < _akcontext->base_len || size > _akcontext->base_len + 512) 
    {
        goto invalid;
    }               

    if((fmac_hdr->fctl & MAC_FCTRL_TODS_FROMDS) == MAC_FCTRL_FROMDS) 
    {
        if(_akcontext->src_ap_sta != 1)
        {
            goto invalid;
        }
        mac_ptr = (u8*)&fmac_hdr->addr3;
    }
    else if((fmac_hdr->fctl & MAC_FCTRL_TODS_FROMDS) == MAC_FCTRL_TODS) 
    {
        if(_akcontext->src_ap_sta != 2)
        {
            goto invalid;
        }
        mac_ptr = (u8*)&fmac_hdr->addr2;
    }
    
    if(_akcontext->src_mac_crc != calcrc_bytes(mac_ptr, 6)) 
    {
        goto invalid;
    }
      
    isvalid = 1;
    
invalid: 
    return isvalid; 
} 

/* return 1 is valid  
 * return 0 is invalid 
 * */ 
static int airkiss_discover_filter(const unsigned char *frame, int size) 
{ 
    int isvalid = 0; 

    struct mac_hdr *fmac_hdr = (struct mac_hdr *)frame;
    
    if(MAC_FCTRL_DATA_T != (fmac_hdr->fctl & MAC_FCTRL_TYPE_MASK)) 
    {
        goto invalid; 
    }

    // at least mac head, not more than 2^9;
    if(size < 24 || size > 1024) 
    {
        goto invalid; 
    }

    isvalid = 1;
    
invalid: 
    
    return isvalid; 
} 

static void airkiss_record_move_ones(void *base_addr, int record_num) 
{ 
    int i;  
    unsigned short *record_base = base_addr; 

    for(i = 0; i < record_num; i++) 
    { 
        record_base[i] = record_base[i+1]; 
    } 
} 

static void airkiss_add_seq_data(const unsigned char *data, int seq) 
{ 
    if(seq < _akcontext->need_seq) 
    { 
        if((seq*4 + 4) <= USR_DATA_BUFF_MAX_SIZE) 
        { 
            if((_akcontext->seq_success_map & (1 << seq)) == 0)  
            { 
                akconf->memcpy(_akcontext->usr_data + seq*4, data, 4); 
                _akcontext->seq_success_map |= (1 << seq); 
            } 
        } 
    } 
} 

int airkiss_init(airkiss_context_t* context,  
                            const airkiss_config_t* config) 
{ 
    if(!context || !config) 
        return -1; 

    akconf = (airkiss_config_t*)config; 
    if(sizeof(_airkiss_local_context) > sizeof(airkiss_context_t)) 
    {
        akconf->printf("airkiss_local_context to large, size:%ld\r\n", 
            sizeof(_airkiss_local_context)); 
        return -1; 
    }   

    _akcontext = (_airkiss_local_context*)context;
    akconf->memset(_akcontext, 0, sizeof(_airkiss_local_context)); 
    
    _akcontext->airkiss_state = AIRKISS_STATE_IDLE; 

    akconf->printf("airkiss_local_context size:%ld\r\n", sizeof(_airkiss_local_context)); 
    return 0; 
} 

static int airkiss_add_src_macinfo(u8 mac_crc, u8 ap_sta, u16 f_len, u16 f_seq)
{
    int success = 0;
    int i;
    u8 src_num = _akcontext->data.guide_code.src_num;
    src_info_st *src_info_ptr = &_akcontext->data.guide_code.src_info[0];

    for(i=0; i<src_num; i++) 
    {
        src_info_ptr = &_akcontext->data.guide_code.src_info[i];
        if((src_info_ptr->crc == mac_crc) 
            && (src_info_ptr->ap_sta == ap_sta)) 
        {
            if(src_info_ptr->seq == f_seq)
                break;
            
            if(f_len == (src_info_ptr->len + 1))
                src_info_ptr->seq_cnt++;
            else 
                src_info_ptr->seq_cnt = 0;

            src_info_ptr->len = f_len;
            src_info_ptr->seq = f_seq;
            src_info_ptr->rev_cnt++;

            if(src_info_ptr->seq_cnt == 3) 
            {
                _akcontext->base_len = src_info_ptr->len - 4;
                _akcontext->src_ap_sta = src_info_ptr->ap_sta;
                _akcontext->src_mac_crc = src_info_ptr->crc;
                success = 1;
            }
            break;
        }
    }
    
    if(i == src_num) 
    {
        if(src_num == MAX_SRC_NUM)
        {
            // replace and least one
            u8 min_rev_cnt = 255;
            int min_idx = 0;
            for(i = 0; i < MAX_SRC_NUM; i++) 
            {
                src_info_ptr = &_akcontext->data.guide_code.src_info[i];
                if(min_rev_cnt > src_info_ptr->rev_cnt) 
                {
                    min_rev_cnt = src_info_ptr->rev_cnt;
                    min_idx = i;
                }
            }
            i = src_num = min_idx;
        }
        
        src_info_ptr = &_akcontext->data.guide_code.src_info[i];
        _akcontext->data.guide_code.src_num++;

        src_info_ptr->crc = mac_crc;
        src_info_ptr->len = f_len;
        src_info_ptr->seq = f_seq;
        src_info_ptr->rev_cnt = 1;
        src_info_ptr->seq_cnt = 0;
        src_info_ptr->ap_sta = ap_sta;
    }
    
    akconf->printf("%s/guide field:%2d ", (ap_sta == 2)?"STA":" AP", i);
    akconf->printf("mac_crc:0x%02x, len:%4d, seq_cnt:%3d, rev_cnt:%3d, seq:%4d\r\n",
        src_info_ptr->crc, src_info_ptr->len, src_info_ptr->seq_cnt,
        src_info_ptr->rev_cnt, src_info_ptr->seq); 

    return success;
}

// 1: locked
// 0: nothing happened, contine
static int airkiss_recv_discover(const void* frame, unsigned short length) 
{ 
    int success = 0;
    u16 frame_seq = 0;
    u8 mac_crc = 0, ap_sta = 0;
    u8 *mac_ptr = 0;
    struct mac_hdr *fmac_hdr = (struct mac_hdr *)frame;

    if((fmac_hdr->fctl & MAC_FCTRL_TODS_FROMDS) == MAC_FCTRL_FROMDS) 
    {
        mac_ptr = (u8*)&fmac_hdr->addr3;
        ap_sta = 1;  //from ap
    }
    else if((fmac_hdr->fctl & MAC_FCTRL_TODS_FROMDS) == MAC_FCTRL_TODS) 
    {
        mac_ptr = (u8*)&fmac_hdr->addr2;
        ap_sta = 2; // from sta
    }
    
    mac_crc = calcrc_bytes(mac_ptr, 6);
    frame_seq = (fmac_hdr->seq)>>4;

    success = airkiss_add_src_macinfo(mac_crc, ap_sta, length, frame_seq);

    if(success) 
    {    
        _akcontext->airkiss_state = AIRKISS_STATE_SRC_LOCKED;
        airkiss_resest_data(); 
        akconf->printf("airkiss_recv_discover success\r\n"); 
        akconf->printf("base len:%d\r\n", _akcontext->base_len); 
    } 

    return success;
} 

static void airkiss_process_magic_code(unsigned short length) 
{
    _akcontext->data.magic_code.record[MAX_MAGIC_CODE_RECORD] = length - _akcontext->base_len; 


    airkiss_record_move_ones(_akcontext->data.magic_code.record, MAX_MAGIC_CODE_RECORD); 

    
    if(((_akcontext->data.magic_code.record[0]&0x01f0)==0x0000)&& 
        ((_akcontext->data.magic_code.record[1]&0x01f0)==0x0010)&& 
            ((_akcontext->data.magic_code.record[2]&0x01f0)==0x0020)&& 
            ((_akcontext->data.magic_code.record[3]&0x01f0)==0x0030))     
    { 
        // attention: not airkiss encode stand for 0, but use 8 for instand 
        // we can't get 0x0 but get 0x8, here we and 0x0007 to get 0
        // this problem only happen in bssid + keylen+ 1 < 16
        _akcontext->total_len = ((_akcontext->data.magic_code.record[0] & 0x0007) << 4) + (_akcontext->data.magic_code.record[1] & 0x000F); 
        _akcontext->ssid_crc = ((_akcontext->data.magic_code.record[2] & 0x000F) << 4) + (_akcontext->data.magic_code.record[3] & 0x000F); 
        //TODO:double check magic code 
        _akcontext->airkiss_state = AIRKISS_STATE_MAGIC_CODE_COMPLETE; 
        airkiss_resest_data(); 
        akconf->printf("airkiss_process_magic_code success\r\n"); 
        akconf->printf("total_len:%d, ssid crc:%x\r\n", _akcontext->total_len, _akcontext->ssid_crc); 
    } 
} 

static void airkiss_process_prefix_code(unsigned short length) 
{ 
    _akcontext->data.prefix_code.record[MAX_PREFIX_CODE_RECORD] = length - _akcontext->base_len; 


    airkiss_record_move_ones(_akcontext->data.prefix_code.record, MAX_PREFIX_CODE_RECORD ); 
    

    if((_akcontext->data.prefix_code.record[0]&0x01f0)==0x0040&& 
        (_akcontext->data.prefix_code.record[1]&0x01f0)==0x0050&& 
            (_akcontext->data.prefix_code.record[2]&0x01f0)==0x0060&& 
            (_akcontext->data.prefix_code.record[3]&0x01f0)==0x0070) 
    { 
        _akcontext->pswd_len = ((_akcontext->data.prefix_code.record[0] & 0x000F) << 4) + (_akcontext->data.prefix_code.record[1] & 0x000F); 
        if(_akcontext->pswd_len > PASSWORD_MAX_LEN) 
            _akcontext->pswd_len = 0;
        
        _akcontext->pswd_lencrc = ((_akcontext->data.prefix_code.record[2] & 0x000F) << 4) + (_akcontext->data.prefix_code.record[3] & 0x000F); 
        if(calcrc_1byte(_akcontext->pswd_len)==_akcontext->pswd_lencrc) 
        { 
            _akcontext->airkiss_state = AIRKISS_STATE_PREFIX_CODE_COMPLETE; 
        } 
        else 
        { 
            akconf->printf("password length crc error.\r\n"); 
            airkiss_resest_data(); 
            return; 
        } 

        // receive password, random and ssid
        _akcontext->need_seq = ((_akcontext->total_len) + 3)/4;
        _akcontext->seq_success_map_cmp = (1 << _akcontext->need_seq) - 1; // EXAMPLE: need_seq = 5; seq_success_map_cmp = 0x1f; ?идииии?? 
             
        airkiss_resest_data(); 
        akconf->printf("airkiss_process_prefix_code success\r\n"); 
        akconf->printf(" pswd_len:%d, pswd_lencrc:%x, need seq:%d, seq map:%x\r\n",  
                _akcontext->pswd_len, _akcontext->pswd_lencrc, _akcontext->need_seq, _akcontext->seq_success_map_cmp); 
    } 
} 

static void airkiss_process_sequence(unsigned short length) 
{ 
     
    _akcontext->data.seq_code.record[MAX_SEQ_CODE_RECORD] = length - _akcontext->base_len; 

    airkiss_record_move_ones(_akcontext->data.seq_code.record, MAX_SEQ_CODE_RECORD); 

    if(((_akcontext->data.seq_code.record[0]&0x180)==0x80) && 
        ((_akcontext->data.seq_code.record[1]&0x180)==0x80)) 
    {
        unsigned char seq_crc, cur_seq, seq_byte, data[5]={0}; 
        unsigned short *pdata = &_akcontext->data.seq_code.record[0];
        int seq_idx; 
        
        seq_crc = pdata[0]&0x7F; //seq crc 
        seq_idx = cur_seq = data[0] = pdata[1]&0x7F; //seq index

        // aready received
        if((_akcontext->seq_success_map & (1 << seq_idx)))
            return;
        
        if(cur_seq <= ((_akcontext->total_len>>2)+1))
        {
            int i;
            seq_byte = _akcontext->total_len - (cur_seq << 2);
            seq_byte = (seq_byte>4)? 4 : seq_byte;

            for(i=0; i<seq_byte; i++) 
            {
                if((pdata[2 + i] & 0x0100) != 0x0100)
                    return;
                else
                    data[1+i] = pdata[2 + i] & 0xFF;
            }

            akconf->printf("[crc:%02x][index:%d]:%02x,%02x,%02x,%02x; ", 
                seq_crc, cur_seq, data[1], data[2], data[3], data[4]); 

            if(seq_crc == (calcrc_bytes(data, seq_byte+1)&0x7F))
            {
                airkiss_add_seq_data(&data[1], cur_seq); 

                akconf->printf("seq mapped:%x\r\n", _akcontext->seq_success_map);
                
                airkiss_resest_data(); 

                if(_akcontext->seq_success_map_cmp == _akcontext->seq_success_map) 
                { 
                    int i; 
                    akconf->printf("User data is :"); 
                    for(i=0; i<_akcontext->total_len; i++) 
                    { 
                        akconf->printf("%02x ",_akcontext->usr_data[i]); 
                    } 
                    akconf->printf("\r\n"); 
                    _akcontext->random_num = _akcontext->usr_data[_akcontext->pswd_len]; 
                    _akcontext->usr_data[_akcontext->pswd_len] = 0; 
                    _akcontext->pwd = (char*)_akcontext->usr_data;
                    _akcontext->ssid_len = _akcontext->total_len - _akcontext->pswd_len;

                    _akcontext->ssid = (char*)_akcontext->usr_data + _akcontext->pswd_len + 1; 
                    _akcontext->usr_data[_akcontext->pswd_len + 1 + _akcontext->ssid_len] = 0;
                    
                    _akcontext->airkiss_state = AIRKISS_STATE_COMPLETE; 
                } 
            }
            else 
            { 
                akconf->printf("crc check error. calc crc:[%02x-%02x]\r\n", 
                        seq_crc,
                        calcrc_bytes(data,seq_byte+1)&0x7F); 
            } 
        }
    }    
} 

int airkiss_recv(airkiss_context_t* context,  
                            const void* frame, unsigned short length) 
{ 
    if(_akcontext->airkiss_state != AIRKISS_STATE_IDLE) {
        if(!airkiss_filter(frame, length)) 
            return AIRKISS_STATUS_CONTINUE; 
    }

    switch(_akcontext->airkiss_state) 
    { 
        case AIRKISS_STATE_IDLE:
            if(airkiss_discover_filter(frame, length)) 
            {
                int is_locked = airkiss_recv_discover(frame, length);
                if(is_locked) 
                    return AIRKISS_STATUS_CHANNEL_LOCKED; 
            } 
            break; 
        case AIRKISS_STATE_SRC_LOCKED: 
            airkiss_process_magic_code(length); 
            break; 
        case AIRKISS_STATE_MAGIC_CODE_COMPLETE: 
            airkiss_process_prefix_code(length); 
            break;     
        case AIRKISS_STATE_PREFIX_CODE_COMPLETE: 
            airkiss_process_sequence(length); 
            if(_akcontext->airkiss_state == AIRKISS_STATE_COMPLETE) 
                return AIRKISS_STATUS_COMPLETE; 
            break; 

        default: 
            _akcontext->airkiss_state = AIRKISS_STATE_IDLE; 
            break; 
    } 
    return AIRKISS_STATUS_CONTINUE; 
} 

int airkiss_get_result(airkiss_context_t* context,  
                            airkiss_result_t* result) 
{ 
    memcpy(result, _akcontext, sizeof(airkiss_result_t)); 
    return 0; 
} 

int airkiss_change_channel(airkiss_context_t* context) 
{ 
    memset(_akcontext , 0, sizeof(_airkiss_local_context)); 
    _akcontext->airkiss_state = AIRKISS_STATE_IDLE; 
    return 0; 
} 

// EOF
