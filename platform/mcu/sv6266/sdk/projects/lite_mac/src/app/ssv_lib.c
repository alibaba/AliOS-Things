#include <config.h>
#include <stdarg.h>
#include <stdio.h>
#if 0
#include <hal.h>

#if (CONFIG_SIM_PLATFORM == 0) && defined(NO_STDIO)
#include <uart/drv_uart.h>
#include "lib-impl.h"
#endif

#include <log.h>
#endif

#include "ipv6/lwip/ip6_addr.h"


//#include "ssv_lib.h"
#include "soc_types.h"

#define OS_APIs
#define H_APIs
#define LIB_APIs
#define DRV_APIs

#if 0
#define CONSOLE_UART   SSV6XXX_UART0
#endif

#if 0

#ifdef NO_STDIO
// return  -1 : fail
//        >=0 : ok
LIB_APIs s32 ssv6xxx_strrpos(const char *str, char delimiter)
{
	const char *p;

	for (p = (str + strlen(str)) - 1; (s32)p>=(s32)str; p--)
	{
		if (*p == delimiter)
			return ((s32)p - (s32)str);
	}
	
	return -1;	// find no matching delimiter

}

LIB_APIs s32	ssv6xxx_isalpha(s32 c)
{
	if (('A' <= c) && (c <= 'Z'))
		return 1;
	if (('a' <= c) && (c <= 'z'))
		return 1;
	return 0;
}

LIB_APIs s32 ssv6xxx_str_toupper(s8 *s)
{
	while (*s)
	{
		*s = toupper(*s);
		s++;
	}
	return 0;
}

LIB_APIs s32 ssv6xxx_str_tolower(s8 *s)
{
	while (*s)
	{
		*s = tolower(*s);
		s++;
	}
	return 0;
}



#if (CONFIG_SIM_PLATFORM == 0)
LIB_APIs s8 toupper(s8 ch)
{
	if (('a' <= ch) && (ch <= 'z'))
		return ('A' + (ch - 'a'));

	// else, make the original ch unchanged
	return ch;
}

LIB_APIs s8 tolower(s8 ch)
{
	if (('A' <= ch) && (ch <= 'Z'))
		return ('a' + (ch - 'A'));

	// else, make the original ch unchanged
	return ch;
}


LIB_APIs s32 strcmp( const s8 *s0, const s8 *s1 )
{
    s32 c1, c2;

    do {
        c1 = (u8) *s0++;
        c2 = (u8) *s1++;
        if (c1 == '\0')
            return c1 - c2;
    } while (c1 == c2);

    return c1 - c2;
}



LIB_APIs s8 *strcat(s8 *s, const s8 *append)
{   
    s8 *save = s;

    while (*s) { s++; }
    while ((*s++ = *append++) != 0) { }
    return(save);
}



LIB_APIs s8 *strncpy(s8 *dst, const s8 *src, size_t n)
{
    register s8 *d = dst;
    register const s8 *s = src;
    
    if (n != 0) {
        do {
            if ((*d++ = *s++) == 0) {
                /* NUL pad the remaining n-1 bytes */
                while (--n != 0)
                *d++ = 0;
                break;
            }
        } while (--n != 0);
    }
    return (dst);
}


LIB_APIs size_t strlen(const s8 *s)
{
    const s8 *ptr = s;
    while (*ptr) ptr++;
    return (size_t)ptr-(size_t)s;
}

LIB_APIs void *memset(void *s, s32 c, size_t n)
{
    if ( NULL != s ) {
		u8 * ps= (u8 *)s;
		const u8 * pes= ps+n;
        while( ps != pes )
			*(ps++) = (u8) c; 
    }
    return s;
}


LIB_APIs void *memcpy(void *dest, const void *src, size_t n)
{
    s8 *d = dest;
    s8 const *s = src;
    
    while (n-- > 0)
      *d++ = *s++;
    return dest;
}


LIB_APIs s32 memcmp(const void *s1, const void *s2, size_t n)
{
    const u8 *u1 = (const u8 *)s1, *u2 = (const u8 *)s2;

    while (n--) {
        s32 d = *u1++ - *u2++;
        if (d != 0)
            return d;
    }
    /*    
    for ( ; n-- ; s1++, s2++) {
        u1 = *(u8 *)s1;
        u2 = *(u8 *)s2;
        if (u1 != u2)
            return (u1-u2);
    } */
    return 0;
}


LIB_APIs void *malloc(size_t size)
{
    extern u32 g_heap_start;
    extern u32 g_heap_size;
    void *mem=(void *)0;

    /* Make sure the size is the multiple of 4-byte */
    if (size & 0x03)
        size = ((size>>2)+1) << 2;
    if (size <= g_heap_size) {
        mem = (void *)g_heap_start;
        g_heap_start += size;
        g_heap_size  -= size;
#ifdef	BOOT_MALLOC_DEBUG
		sys_init_prnf("Mem Alloc: 0x%08x, %u byte(s)\n", (u32)mem, size);
#endif
    }
    else {
        sys_init_prnf("Out of memory !!\n");
        /*lint -save -e716 */
        while(1) {}
        /*lint -restore */
    }
    return mem;
}


LIB_APIs void free(void *ptr)
{
    sys_init_prnf("Please DO NOT use free() !!\n");
}


LIB_APIs s32 printf(const s8 *format, ...)
{
   va_list args;
   s32     ret;
   /*lint -save -e530 */
   va_start( args, format );
   /*lint -restore */
   ret = print( 0, 0, format, args );
   va_end(args);
   return ret;
}


LIB_APIs s32 snprintf(char *out, size_t size, const char *format, ...)
{
    va_list args;
    s32     ret;
    /*lint -save -e530 */
    va_start( args, format ); /*lint -restore */
    ret = print( &out, (out + size - 1), format, args );
    va_end(args);
    return ret;
}

LIB_APIs s32 vsnprintf(char *out, size_t size, const char *format, va_list args)
{
	return print( &out, (out + size - 1), format, args );
}


LIB_APIs s32 putstr (const char *s, size_t len)
{
    return  printstr(0, 0, s, len);
}


LIB_APIs s32 snputstr (char *out, size_t size, const char *s, size_t len)
{
    return  printstr( &out, (out + size - 1), s, len);
}

#endif

#endif // NO_STDIO

LIB_APIs s32 memcmp(const void *s1, const void *s2, size_t n)
{
    const u8 *u1 = (const u8 *)s1, *u2 = (const u8 *)s2;

    while (n--) {
        s32 d = *u1++ - *u2++;
        if (d != 0)
            return d;
    }
    /*    
    for ( ; n-- ; s1++, s2++) {
        u1 = *(u8 *)s1;
        u2 = *(u8 *)s2;
        if (u1 != u2)
            return (u1-u2);
    } */
    return 0;
}
#endif

LIB_APIs u32 ssv6xxx_atoi_base( const s8 *s, u32 base )
{
    u32  index, upbound=base-1;
    u32  value = 0, v;

    while( (v = (u8)*s) != 0 ) {
        index = v - '0';
        if ( index > 10 && base==16 ) {
            index = (v >= 'a') ? (v - 'a') : (v - 'A');
            index += 10;
        }
        if ( index > upbound )
            break;
        value = value * base + index;
        s++;
    }
    
    return value;
}


LIB_APIs s32 ssv6xxx_atoi( const s8 *s )
{
    u32 neg=0, value, base=10;

    if ( *s=='0' ) {
        switch (*++s) {
        case 'x':
        case 'X': base = 16; break;
        case 'b':
        case 'B': base = 2; break;
        default: return 0;
        }
        s++;
    }
    else if ( *s=='-' ) {
        neg = 1;
        s++;
    }

    value = ssv6xxx_atoi_base(s, base);
    
    if ( neg==1 )
        return -(s32)value;
    return (s32)value;

}



#if 0
LIB_APIs s32 fatal_printf(const s8 *format, ...)
{
   va_list args;
   s32     ret;

   hal_console_begin_fatal();
   /*lint -save -e530 */
   va_start( args, format );
   /*lint -restore */
   ret = vprintf(format, args);
   va_end(args);
   hal_console_end_fatal();
   return ret;
}


LIB_APIs void ssv6xxx_raw_dump(s8 *data, s32 len)
{
	ssv6xxx_raw_dump_ex(data, len, true, 10, 10, 16, LOG_LEVEL_ON, LOG_MODULE_EMPTY);
	return;
}


LIB_APIs bool ssv6xxx_raw_dump_ex(s8 *data, s32 len, bool with_addr, u8 addr_radix, s8 line_cols, u8 radix, u32 log_level, u32 log_module)
{
    s32 i;

	// check input parameters
	if ((addr_radix != 10) && (addr_radix != 16))
	{
		LOG_ERROR("%s(): invalid value 'addr_radix' = %d\n\r", __FUNCTION__, addr_radix);
		return false;
	}
	if ((line_cols != 8) && (line_cols != 10) && (line_cols != 16) && (line_cols != -1))
	{
		LOG_ERROR("%s(): invalid value 'line_cols' = %d\n\r", __FUNCTION__, line_cols);
		return false;
	}
	if ((radix != 10) && (radix != 16))
	{
		LOG_ERROR("%s(): invalid value 'radix' = %d\n\r", __FUNCTION__, radix);
		return false;
	}

	if (len == 0)	return true;

	// if ONLY have one line
	if (line_cols == -1)
	{
		LOG_TAG_SUPPRESS_ON();
		// only print addr heading at one time
		if ((with_addr == true))
		{
			if      (addr_radix == 10)	LOG_PRINTF_LM(log_level, log_module, "%08d: ", 0);
			else if (addr_radix == 16)	LOG_PRINTF_LM(log_level, log_module, "0x%08x: ", 0);
		}

		for (i=0; i<len; i++) 
		{
			// print data
			if	    (radix == 10)	LOG_PRINTF_LM(log_level, log_module, "%4d ",  (u8)data[i]);
			else if (radix == 16)	LOG_PRINTF_LM(log_level, log_module, "%02x ", (u8)data[i]);
		}
		LOG_PRINTF_LM(log_level, log_module, "\n\r");
		LOG_TAG_SUPPRESS_OFF();
		return true;
	}

	// normal case
	LOG_TAG_SUPPRESS_ON();
    for (i=0; i<len; i++) 
	{
		// print addr heading
		if ((with_addr == true) && (i % line_cols) == 0)
		{
			if      (addr_radix == 10)	LOG_PRINTF_LM(log_level, log_module, "%08d: ", i);
			else if (addr_radix == 16)	LOG_PRINTF_LM(log_level, log_module, "0x%08x: ", i);
		}
		// print data
		if	    (radix == 10)	LOG_PRINTF_LM(log_level, log_module, "%4d ",  (u8)data[i]);
		else if (radix == 16)	LOG_PRINTF_LM(log_level, log_module, "%02x ", (u8)data[i]);
		// print newline
        if (((i+1) % line_cols) == 0)
            LOG_PRINTF_LM(log_level, log_module, "\n\r");
    }
    LOG_PRINTF_LM(log_level, log_module, "\n\r");
	LOG_TAG_SUPPRESS_OFF();
	return true;
}


LIB_APIs void hex_dump (const void *addr, u32 size)
{
    u32 i, j;
    const u32 *data = (const u32 *)addr;

    //LOG_TAG_SUPPRESS_ON();
    LOG_PRINTF("        ");
    for (i = 0; i < 8; i++)
        LOG_PRINTF("       %02X", i*sizeof(u32));

    LOG_PRINTF("\n--------");
    for (i = 0; i < 8; i++)
        LOG_PRINTF("+--------");

    for (i = 0; i < size; i+= 8)
    {
        LOG_PRINTF("\n%08X:%08X", (s32)data, data[0]);
        for (j = 1; j < 8; j++)
        {
            LOG_PRINTF(" %08X", data[j]);
        }
        data = &data[8];
    }
    LOG_PRINTF("\n");
    //LOG_TAG_SUPPRESS_OFF();
    return;
}
#endif

#if 0
#include <ssv_pktdef.h>
LIB_APIs void hex_parser(u32 rx_data)
{
    struct cfg_host_rxpkt *rx_desc_from_soc = (struct cfg_host_rxpkt *)rx_data;
    struct ssv6006_rxphy_info *rxphy_info   
            = (struct ssv6006_rxphy_info *) (rx_data+ sizeof(struct ssv6006_rx_desc));

    //LOG_TAG_SUPPRESS_ON();
    LOG_PRINTF("        ");

     /* The definition of WORD_1: */
    LOG_PRINTF("len=0x%x\n",rx_desc_from_soc->len);
    LOG_PRINTF("c_type=0x%x\n",rx_desc_from_soc->c_type);
    /* The definition of WORD_2: */
    LOG_PRINTF("fCmd=0x%x\n",rx_desc_from_soc->fCmd);
    /* The definition of WORD_3: */
    LOG_PRINTF("hdr_offset=0x%x\n",rx_desc_from_soc->hdr_offset);
    LOG_PRINTF("RxResult=0x%x\n",rx_desc_from_soc->RxResult);
    LOG_PRINTF("reason=0x%x\n",rx_desc_from_soc->reason);
    LOG_PRINTF("unicast=0x%x\n",rx_desc_from_soc->unicast);
    LOG_PRINTF("bssid=0x%x\n",rx_desc_from_soc->bssid);

    /* The definition of WORD_4: */
    LOG_PRINTF("fCmdIdx=0x%x\n",rx_desc_from_soc->fCmdIdx);
    LOG_PRINTF("wsid=0x%x\n",rx_desc_from_soc->wsid);
    LOG_PRINTF("phy_rate=0x%x\n",rxphy_info->phy_rate);

    //LOG_TAG_SUPPRESS_OFF();
    return;
}
#endif

#if 0
LIB_APIs void scan_packet_buffer()
{
    u32 packet_base = 0x80000000;
    u32 data = 0x00;
    struct cfg_host_rxpkt *rx_desc_from_soc;
    struct ssv6006_rxphy_info *rxphy_info;   

    u8 *fc;
    u32 i;

    for(i = 0; i < 128; i++)
    {
        data = (packet_base + (i << 16));
        if((*((u32 *)data)) != 0xbabababa)
        {
            rx_desc_from_soc = (struct cfg_host_rxpkt *)data;
            rxphy_info   
            = (struct ssv6006_rxphy_info *) (data+ sizeof(struct ssv6006_rx_desc));
            fc = (u8 *)(data + GET_PB_OFFSET);

            LOG_PRINTF("Packet buffer[%x]\n",data);
            LOG_PRINTF("Frame type[%x]\n",*fc);
            LOG_PRINTF("len[%d]c_type[%d]fCmd[%x]fCmdIdx[%x]RxResult[%x]drate_idx[%d]\n"
                ,rx_desc_from_soc->len,rx_desc_from_soc->c_type,rx_desc_from_soc->fCmd
                ,rx_desc_from_soc->fCmdIdx,rx_desc_from_soc->RxResult,rxphy_info->phy_rate);
            LOG_PRINTF("\n");
            hex_dump((void*)data,256);
            hex_parser(data);
        }
    }
    return;
}
#endif // 0

void dbg_dump_mac(s8 *msg,u8 *pmac){
    printf("%s:0x%x-%x-%x-%x-%x-%x\n",msg,pmac[0],pmac[1],pmac[2],pmac[3],pmac[4],pmac[5]);
}

void dbg_dump_ip(s8 *msg,u8 *p_ip){
    printf("%s:%d.%d.%d.%d\n",msg, p_ip[0], p_ip[1], p_ip[2], p_ip[3]);
}


#define PP_HTONL(x) ((((x) & 0xff) << 24) | \
                     (((x) & 0xff00) << 8) | \
                     (((x) & 0xff0000UL) >> 8) | \
                     (((x) & 0xff000000UL) >> 24))


#if (LWIP_IPV6==1) 
//fixed this function to allow unaligned address
void dbg_dump_ip6(char *msg,ip6_addr_t * target_addr) {
    uint32_t u32addr0;
	uint32_t u32addr1;
	uint32_t u32addr2;
	uint32_t u32addr3;

	memcpy(&u32addr0,&target_addr->addr[0],sizeof(uint32_t));
	memcpy(&u32addr1,&target_addr->addr[1],sizeof(uint32_t));
	memcpy(&u32addr2,&target_addr->addr[2],sizeof(uint32_t));
	memcpy(&u32addr3,&target_addr->addr[3],sizeof(uint32_t));

	printf("%s 0x%x:%x:%x:%x:%x:%x:%x:%x\n",msg,((PP_HTONL(u32addr0)>>16)&0xffff),
												  ((PP_HTONL(u32addr0)>>0)&0xffff),
												  ((PP_HTONL(u32addr1)>>16)&0xffff),
												  ((PP_HTONL(u32addr1)>>0)&0xffff),
												  ((PP_HTONL(u32addr2)>>16)&0xffff),
												  ((PP_HTONL(u32addr2)>>0)&0xffff),
												  ((PP_HTONL(u32addr3)>>16)&0xffff),
												  ((PP_HTONL(u32addr3)>>0)&0xffff));
}
#endif


LIB_APIs void halt(void)
{
#if (CONFIG_SIM_PLATFORM)
	system("pause");
	exit(EXIT_FAILURE);
#else
	/*lint -save -e716 */
    while (1) ;
	/*lint -restore */
#endif
} 
