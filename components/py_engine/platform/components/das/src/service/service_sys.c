/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#include <das/platform.h>
#include <das/service.h>

#ifdef DAS_PLATFORM_FREERTOS
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#endif // DAS_PLATFORM_FREERTOS

#ifdef DAS_PLATFORM_AOS
#include "aos/kernel.h"
#endif // DAS_PLATFORM_AOS

#ifdef DAS_PLATFORM_LINUX
#include <sys/sysinfo.h>
#endif // DAS_PLATFORM_LINUX

#define SYS_INFO_KEY_SP "sp:"
#define SYS_INFO_KEY_UP "up_seconds:"

static size_t _current_sp(size_t v, ...)
{
    size_t sp;
    sp = (size_t)&v;
    return sp;
}

static int _sys_init(void)
{
    return 0;
}

static das_result_t _sys_info(char *buffer, size_t size, das_service_state_t *state)
{
    size_t len;
    uint64_t up_seconds = 0;
    size_t offset = 0;
    size_t sp = 0;

    len = strlen(SYS_INFO_KEY_SP);
    if (len >= size) {
        DAS_LOG("No buffer size for sys sp info: %lx\n", (long)size);
        return DAS_ERROR_SHORT_BUFFER;
    }

    memset(buffer, 0, size);
    
    memcpy(buffer + offset, SYS_INFO_KEY_SP, len);
    offset += len;

    sp = _current_sp(len);
    das_hex2string(buffer + offset, size - offset, (uint8_t*)&sp, sizeof(size_t));

    offset = strlen(buffer);
    len = strlen(SYS_INFO_KEY_UP);
    if (len >= size - offset - 18) {
        DAS_LOG("No buffer size for up_seconds info: %lx\n", (long)(size - offset));
        return DAS_ERROR_SHORT_BUFFER;
    }

    buffer[offset] = ',';
    offset++;
    memcpy(buffer + offset, SYS_INFO_KEY_UP, len);
    offset += len;

#ifdef DAS_PLATFORM_AOS
    up_seconds = HAL_UptimeMs();
#endif // DAS_PLATFORM_AOS

#ifdef DAS_PLATFORM_FREERTOS
    up_seconds = xTaskGetTickCount();
#endif // DAS_PLATFORM_FREERTOS

#ifdef DAS_PLATFORM_LINUX
    struct sysinfo info;
    if (sysinfo(&info) == 0) {
        up_seconds = info.uptime * 1000;
    }
#endif // DAS_PLATFORM_LINUX

#ifdef DAS_PLATFORM_MACOS
    up_seconds = 0;
#endif // DAS_PLATFORM_MACOS

    das_itoa((int)(up_seconds / 1000), buffer + offset, 10);
    state->status = DAS_SRV_STATUS_FINISH;

    return DAS_SUCCESS;
}

das_service_t service_sys = {
    .name = "SYS",
    .init = _sys_init,
    .info = _sys_info,
    .attest  = NULL,
    .measure = NULL,
};

////////////////////////////////////////////////////////////////

#if (DAS_SERVICE_STACK_PROTECTION_ENABLED)
#define STACK_PROTECTION_KEY 	"stack:"

/*
eg. 
    tag: c
    ret_addr: a->b->....->c  (DAS_SERVICE_SP_BACKTRACE_MAX_DEEP)

    tag: c
    ret_addr: d->c
    
    tag: c
    ret_addr: e->d->c
*/
typedef struct _das_stack_trace_report_buf {
    char tag[DAS_SERVICE_SP_TAG_MAX_LEN];
    uint32_t ret_addr[DAS_SERVICE_SP_BACKTRACE_MAX_DEEP];
} das_stack_trace_report_buf_t;

/*
eg. 
    tag: c
    call_stack_hash: 
        a->b->c:    (uint32_t)hash1
        d->c:       (uint32_t)hash2
        e->d->c:    (uint32_t)hash3
        ... 
        DAS_SERVICE_SP_CALL_STACK_MAX_HASH_CNT
*/
typedef struct _das_function_stack_hash {
    char tag[DAS_SERVICE_SP_TAG_MAX_LEN];
    uint64_t call_stack_hash[DAS_SERVICE_SP_CALL_STACK_MAX_HASH_CNT];
} das_function_stack_hash_t;

static das_stack_trace_report_buf_t das_stack_trace_report_buf[DAS_SERVICE_SP_TRACE_MAX_REPORT_BUF_CNT];
static das_function_stack_hash_t das_function_stack_hash[DAS_SERVICE_SP_TRACE_MAX_FUNCTION_CNT];

#ifdef DAS_PLATFORM_FREERTOS
static SemaphoreHandle_t stack_trace_mutex;
#endif // DAS_PLATFORM_FREERTOS

#ifdef DAS_PLATFORM_AOS
static aos_mutex_t stack_trace_mutex;
#endif // DAS_PLATFORM_AOS

static uint32_t report_info_index = 0;
static int sp_service_inited = 0;

static char *_fill_report_buf(char *buf, size_t len, uint32_t *stacks);

static int _stack_protection_init(void)
{
	memset(das_function_stack_hash, 0, sizeof(das_function_stack_hash_t) * DAS_SERVICE_SP_TRACE_MAX_FUNCTION_CNT);
    memset(das_stack_trace_report_buf, 0, sizeof(das_stack_trace_report_buf_t) * DAS_SERVICE_SP_TRACE_MAX_REPORT_BUF_CNT);

#ifdef DAS_PLATFORM_FREERTOS
    stack_trace_mutex = xSemaphoreCreateMutex();
	if (stack_trace_mutex == NULL) {
#endif // DAS_PLATFORM_FREERTOS
#ifdef DAS_PLATFORM_AOS
	if (0 != aos_mutex_new(&stack_trace_mutex)) {
#endif // DAS_PLATFORM_AOS
        DAS_LOG("Creating stack trace mutex failed.\n");
        return -1;
    }

    sp_service_inited = 1;
	return 0;
}

/*
    stack:core_func1|ret_addr1|ret_addr2|ret_addr3,stack:core_func2|ret_addr1|ret_addr2|,stack:...
*/
static das_result_t _stack_protection_info(char* buffer, size_t size, das_service_state_t* state)
{
	size_t offset, remain_len, pre_len;
	int lock = 1;
    size_t tag_len = 0;

	if (buffer == NULL || size == 0 || state == NULL) {
		return DAS_ERROR_BAD_PARAMETERS;
	}

	offset = 0;
	remain_len = size;
	pre_len = strlen(STACK_PROTECTION_KEY);

	if (pre_len >= size) {
		DAS_LOG("No buffer size for sys info: %lx\n", (long)pre_len);
		return DAS_ERROR_SHORT_BUFFER;
	}

#ifdef DAS_PLATFORM_FREERTOS
    if (xSemaphoreTake(stack_trace_mutex, (TickType_t)100) != pdTRUE) {
#endif // DAS_PLATFORM_FREERTOS
#ifdef DAS_PLATFORM_AOS
	if (aos_mutex_lock(&stack_trace_mutex, AOS_WAIT_FOREVER) != 0) {
#endif // DAS_PLATFORM_AOS
        lock = 0;
    }

	for (; report_info_index < DAS_SERVICE_SP_TRACE_MAX_REPORT_BUF_CNT; report_info_index++) {
		das_stack_trace_report_buf_t *pinfo = &(das_stack_trace_report_buf[report_info_index]);
        tag_len = strlen(pinfo->tag);
		if (tag_len == 0) {
			continue;
		} else if (pinfo->ret_addr[0] != 0) {
			char tmp_buf[100] = {0};
            int exit = 0;
			char *buf = _fill_report_buf(tmp_buf, 100, pinfo->ret_addr);

			if (buf && (strlen(buf) + pre_len + tag_len + 1) < remain_len) {
                if (strlen(buffer) > 0) {
                    buffer[offset] = INFOS_SEPARATE_TAG;
                    offset++;
                    remain_len--;
			    }
                
				memcpy(buffer + offset, STACK_PROTECTION_KEY, pre_len * sizeof(char));
				offset += pre_len;
                remain_len -= pre_len;

                memcpy(buffer + offset, pinfo->tag, tag_len * sizeof(char));
                offset += tag_len;
                remain_len -= tag_len;

                buffer[offset] = FIELDS_SEPARATE_TAG;
				offset++;
                remain_len--;

				memcpy(buffer + offset, buf, strlen(buf) * sizeof(char));
				offset += strlen(buf);
				remain_len -= strlen(buf);

                // clean the local report info store buffer
                memset(pinfo, 0, sizeof(das_stack_trace_report_buf_t));
			} else {
                exit = 1;
            }
            
			if (exit == 1 || remain_len < 60) {
				report_info_index++;
				state->status = DAS_SRV_STATUS_PUBLISH;
				break;
			}
		}
	}

	if (report_info_index >= DAS_SERVICE_SP_TRACE_MAX_REPORT_BUF_CNT) {
		report_info_index = 0;
		state->status = DAS_SRV_STATUS_FINISH;
	}

    if (lock) {
#ifdef DAS_PLATFORM_FREERTOS
        xSemaphoreGive(stack_trace_mutex);
#endif // DAS_PLATFORM_FREERTOS
#ifdef DAS_PLATFORM_AOS
	    aos_mutex_unlock(&stack_trace_mutex);
#endif // DAS_PLATFORM_AOS
    }

    if (strlen(buffer) == 0) {
        return DAS_ERROR_GENERIC;
    }

	return DAS_SUCCESS;
}

static char *_fill_report_buf(char *buf, size_t len, uint32_t *stacks)
{
	char *ret = NULL;
	int off = 0;
    int i = 0;

	if (buf == NULL || len == 0 || stacks == NULL) {
		return ret;
	}

    for (i = 0; i < DAS_SERVICE_SP_BACKTRACE_MAX_DEEP; i++) {
        char tmp_buf[16] = {0};

        if (stacks[i] == 0) {
            break;
        }

        das_itoa((int)stacks[i], tmp_buf, 16);
        if (off + strlen(tmp_buf) >= len) {
		    break;
	    }
        memcpy(buf + off, tmp_buf, strlen(tmp_buf) * sizeof(char));
        off += strlen(tmp_buf);
        buf[off] = FIELDS_SEPARATE_TAG;
        off++;
    }
	
    if (off > 0 && buf[--off] == FIELDS_SEPARATE_TAG) {
        buf[off] = 0;
    }

 	ret = buf;
	return ret;
}

extern uint32_t __flash_text_start__;
extern uint32_t __flash_text_end__;

#ifdef DAS_PLATFORM_AOS
extern void krhino_task_deathbed(void);
#define portTASK_RETURN_ADDRESS	krhino_task_deathbed
#endif // DAS_PLATFORM_AOS

#ifdef DAS_PLATFORM_FREERTOS
extern uint32_t __ram_image2_text_start__;
extern uint32_t __ram_image2_text_end__;

extern void prvTaskExitError( void );

#ifdef configTASK_RETURN_ADDRESS
    #define portTASK_RETURN_ADDRESS	configTASK_RETURN_ADDRESS
#else
    #define portTASK_RETURN_ADDRESS	prvTaskExitError
#endif
#endif // DAS_PLATFORM_FREERTOS

int das_strncmp(const char *s1 , const char *s2 , size_t n)
{
    if (n == 0)
        return 0;

    while (n-- != 0 && *s1 == *s2) {
        if (n == 0 || *s1 == '\0')
            break;
        s1++;
        s2++;
    }

    return (*(unsigned char *) s1) - (*(unsigned char *) s2);
}

int stack_protection(const char *tag) {
    int ret = -1;
    int *FP = NULL, *builtin_frame = NULL;
    das_function_stack_hash_t *pinfo = NULL;
    uint32_t i = 0, j = 0, x = 0; 
    uint32_t stack_deep[DAS_SERVICE_SP_BACKTRACE_MAX_DEEP] = {0};
    uint64_t call_stack_hash = 0;
    size_t tag_len = 0; 

    if (tag == NULL || strlen(tag) == 0 || sp_service_inited == 0) {
        return ret;
    }

    tag_len = strlen(tag);
    if (tag_len >= DAS_SERVICE_SP_TAG_MAX_LEN) {
        tag_len = DAS_SERVICE_SP_TAG_MAX_LEN - 1;
    }

    for (i = 0; i < DAS_SERVICE_SP_TRACE_MAX_FUNCTION_CNT; i++) {
        das_function_stack_hash_t *pinfo_tmp = &(das_function_stack_hash[i]);
        if (strlen(pinfo_tmp->tag) == 0 || das_strncmp(pinfo_tmp->tag, tag, tag_len) == 0) {
            pinfo = pinfo_tmp;
            break;
        }
    }

    if (pinfo == NULL) {
        return ret;
    } else if (strlen(pinfo->tag) == 0) {
        memcpy(pinfo->tag, tag, tag_len * sizeof(char));
    }

    asm volatile("mov %0, R7\n" : "=r"(FP));
    builtin_frame = __builtin_frame_address(0);

    /* 
        DAS_LOG("[c] FP: %p, builtin_frame: %p, *FP: %p, *(FP+1): %p, *(FP+2): %p, *(FP-1): %p, *(FP-2): %p\n", 
                FP, builtin_frame, *FP, *(FP+1), *(FP+2), *(FP-1), *(FP-2));
        DAS_LOG("portTASK_RETURN_ADDRESS: %p\n", portTASK_RETURN_ADDRESS);
    */

    if (FP == NULL || builtin_frame != FP) {
        return ret;
    } 

    // search stack size: 2048 * 2 * sizeof(uint32_t) 
    for (i = 0; i < 2048 * 2; i++) { 
        uint32_t *ret_addr = NULL;

        ret_addr = (uint32_t *)(*(FP + i));
        if (ret_addr == NULL) {
            continue;
        }

        if ((ret_addr > &__flash_text_start__ && 
            ret_addr < &__flash_text_end__) 
#ifdef DAS_PLATFORM_FREERTOS
            || (ret_addr > &__ram_image2_text_start__ && 
            ret_addr < &__ram_image2_text_end__)
#endif
            ) {
            int bl_code = 0;
            unsigned short blx_code = 0;

            if (ret_addr == (uint32_t *)portTASK_RETURN_ADDRESS) {
                //DAS_LOG("Find portTASK_RETURN_ADDRESS, search stack break.\n");
                break;
            }

            bl_code = *((int *)((char *)ret_addr - 1) - 1);
            blx_code = *((unsigned short *)((char *)ret_addr - 1) - 1);

            if ((bl_code & 0xf0000000) == 0xf0000000 || // BL  addr
                (blx_code & 0xff00) == 0x4700) {        // BLX register
                if (x > DAS_SERVICE_SP_BACKTRACE_MAX_DEEP) {
                    break;
                }

                // eg. call chain: main() -> a() -> b() -> das_attest() -> stack_protection()
                // skip das_attest & b 
                if (++j > 2) {
                    //DAS_LOG("copy stack_deep x: %d, ret_addr: %d\n", x, ret_addr);
                    stack_deep[x++] = ret_addr;
                }
            }
        }
    }
    
    if (x > 0) {
        int report = 1;

        for (i = 0; i < x; i++) {
            call_stack_hash += (uint64_t)(stack_deep[i]);
        }
        
        for (i = 0; i < DAS_SERVICE_SP_CALL_STACK_MAX_HASH_CNT; i++) {
            if (pinfo->call_stack_hash[i] == 0) {
                break;
            } else if (pinfo->call_stack_hash[i] == call_stack_hash) {
                report = 0;
                break;
            }
        }

        if (report == 1) {
            int lock = 1;
            if (i < DAS_SERVICE_SP_CALL_STACK_MAX_HASH_CNT) {
                pinfo->call_stack_hash[i] = call_stack_hash;
            }

#ifdef DAS_PLATFORM_FREERTOS
            if (xSemaphoreTake(stack_trace_mutex, (TickType_t)100) != pdTRUE) {
#endif // DAS_PLATFORM_FREERTOS
#ifdef DAS_PLATFORM_AOS
	        if (aos_mutex_lock(&stack_trace_mutex, AOS_WAIT_FOREVER) != 0) {
#endif // DAS_PLATFORM_AOS
                lock = 0;
            }

            for (i = 0; i < DAS_SERVICE_SP_TRACE_MAX_REPORT_BUF_CNT; i++) {
                das_stack_trace_report_buf_t *pstack_trace = &(das_stack_trace_report_buf[i]);
                if (strlen(pstack_trace->tag) == 0) {
                    memcpy(pstack_trace->tag, pinfo->tag, sizeof(char) * tag_len);
                    memcpy(pstack_trace->ret_addr, stack_deep, sizeof(uint32_t) * DAS_SERVICE_SP_BACKTRACE_MAX_DEEP);
                    DAS_LOG("index: %d, save tag: %s, ret0: %d\n", i, pstack_trace->tag, pstack_trace->ret_addr[0]);
                    break;
                }
            }

            if (lock) {
#ifdef DAS_PLATFORM_FREERTOS
                xSemaphoreGive(stack_trace_mutex);
#endif // DAS_PLATFORM_FREERTOS
#ifdef DAS_PLATFORM_AOS
	            aos_mutex_unlock(&stack_trace_mutex);
#endif // DAS_PLATFORM_AOS
            }
        }
    }

    ret = 0;
    return ret;
 }

 das_service_t service_stack_protection = {
    .name = "STACK_PROTECTION",
    .init  = _stack_protection_init,
    .info = _stack_protection_info,
    .attest = NULL,
    .measure = NULL,
};

#endif // DAS_SERVICE_STACK_PROTECTION_ENABLED