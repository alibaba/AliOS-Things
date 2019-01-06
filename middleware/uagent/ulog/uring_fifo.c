#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include "uring_fifo.h"

//syslog protocol, facility local use 0
const uint8_t SYSLOG_FACILITY = 128U;

/* log struct */
typedef struct{
    uint8_t      fifo_len;
    char         fifo_text[SYSLOG_SIZE];
}fifo_t;

extern osi_uring_fifo os_related;

static uint16_t msg_in_ptr  = 0;    //input point of buffer
static uint16_t msg_out_ptr = 0;    //output point of buffer
//static uint16_t peak_used   = 0;    //peak used of buffer

static fifo_t* log_buffer = NULL;
//comes from customer
static void* fifo_entry_mutex;

static uint16_t syslog_depth = MIN_SYSLOG_DEPTH;

static char module_name[MOD_SIZE][4] = {"UN","OS","NT","CD","AP"};


bool uring_fifo_init(const uint16_t queue_depth)
{
    bool rc = false;
    //the depth too small is not allowed
    syslog_depth = queue_depth>MIN_SYSLOG_DEPTH?queue_depth:MIN_SYSLOG_DEPTH;
    log_buffer = (fifo_t*)aos_malloc(syslog_depth*sizeof(fifo_t));
    if(log_buffer != NULL){
        fifo_entry_mutex = os_related._mutex_init();
        if(fifo_entry_mutex==NULL){
            aos_free(log_buffer);
            log_buffer = NULL;
        }else{
            rc = true;
        }
    }

    return rc;
}

//Thread Safe to operate this routine.
bool uring_fifo_push_s(const void* buf, const uint16_t len)
{
	bool rc = false;
	if(buf!=NULL && len!=0 && len<=SYSLOG_SIZE)	{
		if(0==os_related._mutex_lock(fifo_entry_mutex)) {
    		if ( (msg_in_ptr+1)%syslog_depth==msg_out_ptr )//record buffer is overflow
    		{
    			rc = false;
    		}
    		else
    		{
    			memcpy(log_buffer[msg_in_ptr].fifo_text,buf,len);
    			log_buffer[msg_in_ptr].fifo_len=len;

    			msg_in_ptr = (msg_in_ptr+1)%syslog_depth;
                if(NULL != os_related._log_push_cb)
                {
                    os_related._log_push_cb(NULL);
                }

    			rc = true;
    		}
            os_related._mutex_unlock(fifo_entry_mutex);
		}
	}

	return rc;
}

//Thread Safe to operate this routine.
bool post_log(const MOD_TYPE m, const uint8_t s, const char* f, const uint32_t l, const char *fmt, ...)
{
	bool rc = false;
    if(0==os_related._mutex_lock(fifo_entry_mutex)) {
        if ( (msg_in_ptr+1)%syslog_depth==msg_out_ptr )//record buffer is overflow
        {
            rc = false;
        }
        else
        {
            log_buffer[msg_in_ptr].fifo_len = 0;
			//using type char instead of int8_t which align with the prototype of string operation function
            char* p = (char*)log_buffer[msg_in_ptr].fifo_text;
			//ZERO the buffer is not necessary as null terminated is considering
			//when using snprintf strncat vsnprintf

#if EXTREAM_LOG_TEXT == 0

            //PRI
            snprintf(p, SYSLOG_SIZE,"<%d>",SYSLOG_FACILITY+s);

            //HEADER
            os_related._syslog_time(&p[strlen(p)], SYSLOG_SIZE-strlen(p));//-->Total 23~24 Byte
            if(os_related._host_name!=NULL){
                strncat(p, os_related._host_name, SYSLOG_SIZE - strlen(p) - 1);
                strncat(p," ", SYSLOG_SIZE - strlen(p) - 1);
            }                                                             //-->Total 23~33 Byte

            //MSG
            log_buffer[msg_in_ptr].fifo_len = strlen(p);//MUST < 36
            p += log_buffer[msg_in_ptr].fifo_len;
            int32_t empty_room_size = SYSLOG_SIZE-log_buffer[msg_in_ptr].fifo_len;

            int32_t len = snprintf(p,empty_room_size,"%s_%s[%d]: ",m<MOD_SIZE?module_name[m]:module_name[MOD_UNKNOWN],
                os_related._trim_file_path(f),l);
            log_buffer[msg_in_ptr].fifo_len += len<empty_room_size?len:empty_room_size-1;
            p += len;

            empty_room_size = SYSLOG_SIZE-log_buffer[msg_in_ptr].fifo_len;
            if(empty_room_size>1){
                va_list argptr;
                va_start(argptr, fmt);
                len = vsnprintf(p, empty_room_size, fmt, argptr);
                log_buffer[msg_in_ptr].fifo_len += len<empty_room_size?len:empty_room_size-1;
                va_end(argptr);
            }
#else
            va_list argptr;
			va_start(argptr, fmt);
			const int len = vsnprintf(p, SYSLOG_SIZE, fmt, argptr);
            log_buffer[msg_in_ptr].fifo_len +=  len<SYSLOG_SIZE?len:SYSLOG_SIZE-1;
			va_end(argptr);
#endif /*EXTREAM_LOG_TEXT == 0*/
            p[SYSLOG_SIZE-1] = '\0';

            msg_in_ptr = (msg_in_ptr+1)%syslog_depth;
            if(NULL != os_related._log_push_cb)
            {
                os_related._log_push_cb(NULL);
            }else{
                //TBD, error happen, shall report to system
            }

            rc = true;
        }
        os_related._mutex_unlock(fifo_entry_mutex);
    }

	return rc;
}

int32_t uring_fifo_pop( void* const buf, const uint16_t _len)
{
	int32_t rc = -1;
	if( (buf!=NULL) && (_len!=0) ) {
        if (msg_in_ptr!=msg_out_ptr){
            rc = log_buffer[msg_out_ptr].fifo_len<_len?log_buffer[msg_out_ptr].fifo_len:_len;
            memcpy(buf,log_buffer[msg_out_ptr].fifo_text,rc);
            msg_out_ptr = (msg_out_ptr+1)%syslog_depth;
        } else {
            rc = 0;
        }
	}
    return rc;
}

int32_t uring_fifo_pop_cb(pop_callback cb, void* cb_arg)
{
	int32_t rc = -1;
	if( (cb!=NULL)&&(msg_in_ptr!=msg_out_ptr) ) {
        cb(cb_arg, log_buffer[msg_out_ptr].fifo_text, log_buffer[msg_out_ptr].fifo_len);
        msg_out_ptr = (msg_out_ptr+1)%syslog_depth;
	}
    return rc;
}


