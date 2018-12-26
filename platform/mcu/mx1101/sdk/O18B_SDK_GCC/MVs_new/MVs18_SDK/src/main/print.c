/*
 * definde logical I/O interface including serial console input/output,
 * character string redirective input/oupt -huangyucai20110610
 */
#ifndef	IN_DOWNLOADER_DEBUG
#include	"os.h"
#include	"uart.h"


#endif	//IN_DOWNLOADER_DEBUG

// begin added by lean_xiong @2013-12-19
static unsigned char sOsDebugFlag = 1;
void SetDebugFlag(unsigned char Flag)
{
    sOsDebugFlag = Flag;
}
// end added by lean_xiong @2013-12-19

typedef char* va_list;
#define _INTSIZEOF(n) \
		((sizeof(n)+sizeof(int)-1)&~(sizeof(int) - 1) )

#define va_start(ap,v) ( ap = (va_list)&v + _INTSIZEOF(v) )

#define va_arg(ap,t) \
		( *(t *)((ap += _INTSIZEOF(t)) - _INTSIZEOF(t)) )

#define va_end(ap) ( ap = (va_list)0 )

#ifdef O18_COMM_CPU //used for HF COMM CPU case ONLY, pls DO NOT remove it!
static unsigned char IsBuartActedAsFuartFlag = 0;
#include "driver/uart.h"
void EnableBuartAsFuart(char EnableFlag)
{
    IsBuartActedAsFuartFlag = EnableFlag;
}
#endif



//This is used as dummy function in case that appilcation dont define this function.
void SwUartSend(unsigned char* Buf,unsigned int BufLen) __attribute__((weak));
void SwUartSend(unsigned char* Buf,unsigned int BufLen)
{

}


void printchar(char **str,int c)
{            
	if (str) {
		**str = (char)c;
		++ (*str);
	}
	else
	{
// begin added by lean_xiong @2013-12-19
       /* if (!sOsDebugFlag)
        {
            return;
        }*/
// end added by lean_xiong @2013-12-19
		/*
		 * putchar is link to uart driver etc.
		 */
#ifdef O18_COMM_CPU //used for HF COMM CPU case ONLY, pls DO NOT remove it!	
        if(IsBuartActedAsFuartFlag)   //HF101 Buart act as fuart functions.
        {
            BuartSend((unsigned char*)&c,1);            
        }
        else
#endif //#ifdef O18_COMM_CPU 
       /* if(IsSwUartActedAsFuartFlag)
        {
			if((unsigned char)c == '\n'){
				const char lfca[2] = "\r\n";
				SwUartSend((unsigned char*)lfca,2);

			}
			else
				SwUartSend((unsigned char*)&c,1);
        }
        else*/
        {
			if((unsigned char)c == '\n'){
				const char lfca[2] = "\r\n";
				FuartSend((unsigned char*)lfca,2);
			}
			else
				FuartSend((unsigned char*)&c,1);
        }
	}
}

#define PAD_RIGHT 1
#define PAD_ZERO 2

int prints(char **out, const char *string, int width, int pad)
{
	register int pc = 0, padchar = ' ';

	if (width > 0) {
		register int len = 0;
		register const char *ptr;
		for (ptr = string; *ptr; ++ptr) ++len;
		if (len >= width) width = 0;
		else width -= len;
		if (pad & PAD_ZERO) padchar = '0';
	}
	if (!(pad & PAD_RIGHT)) {
		for ( ; width > 0; --width) {
			printchar (out, padchar);
			++pc;
		}
	}
	for ( ; *string ; ++string) {
		printchar (out, *string);
		++pc;
	}
	for ( ; width > 0; --width) {
		printchar (out, padchar);
		++pc;
	}

	return pc;
}

/* the following should be enough for 32 bit int */
#define PRINT_BUF_LEN 12

int printi(char **out, int i, int b, int sg, int width, int pad, int letbase)
{
	char print_buf[PRINT_BUF_LEN];
	register char *s;
	register int t, neg = 0, pc = 0;
	register unsigned int u = (unsigned int)i;

	if (i == 0) {
		print_buf[0] = '0';
		print_buf[1] = '\0';
		return prints (out, print_buf, width, pad);
	}

	if (sg && b == 10 && i < 0) {
		neg = 1;
		u = (unsigned int)-i;
	}

	s = print_buf + PRINT_BUF_LEN-1;
	*s = '\0';

	while (u) {
		t = (unsigned int)u % b;
		if( t >= 10 )
			t += letbase - '0' - 10;
		*--s = (char)(t + '0');
		u /= b;
	}

	if (neg) {
		if( width && (pad & PAD_ZERO) ) {
			printchar (out, '-');
			++pc;
			--width;
		}
		else {
			*--s = '-';
		}
	}

	return pc + prints (out, s, width, pad);
}

int print( char **out, const char *format, va_list args )
{
	register int width, pad;
	register int pc = 0;
	char scr[2];

	for (; *format != 0; ++format) {
		if (*format == '%') {
			++format;
			width = pad = 0;
			if (*format == '\0') break;
			if (*format == '%') goto out;
			if (*format == '-') {
				++format;
				pad = PAD_RIGHT;
			}
			while (*format == '0') {
				++format;
				pad |= PAD_ZERO;
			}
			for ( ; *format >= '0' && *format <= '9'; ++format) {
				width *= 10;
				width += *format - '0';
			}
			if( *format == 's' ) {
				register char *s = (char *)va_arg( args, int );
				pc += prints (out, s?s:"(null)", width, pad);
				continue;
			}
			if( *format == 'd' ) {
				pc += printi (out, va_arg( args, int ), 10, 1, width, pad, 'a');
				continue;
			}
			if( *format == 'x' ) {
				pc += printi (out, va_arg( args, int ), 16, 0, width, pad, 'a');
				continue;
			}
			if( *format == 'X' ) {
				pc += printi (out, va_arg( args, int ), 16, 0, width, pad, 'A');
				continue;
			}
			if( *format == 'u' ) {
				pc += printi (out, va_arg( args, int ), 10, 0, width, pad, 'a');
				continue;
			}
			if( *format == 'c' ) {
				/* 
				 * char are converted to int then pushed on the stack 
				 */
				scr[0] = (char)va_arg( args, int );
				scr[1] = '\0';
				pc += prints (out, scr, width, pad);
				continue;
			}
		}
		else{
		out:

			printchar (out, *format);
			++pc;
		}
	}
	if (out) **out = '\0';
	va_end( args );
	return pc;
}

int mvprintf(const char *format, ...)
{
        va_list args;
        
        va_start(args,format);
        return print(0,format,args);
}

int mvsprintf(char *out, const char *format, ...)
{
        va_list args;
        
        va_start( args, format );
        return print(&out,format,args);
}


int mvsnprintf(char *buf, unsigned int count, const char *format, ... )
{
        va_list args;
        (void) count;
        
        va_start( args, format );
        return print( &buf, format, args );
}

#ifndef	IN_DOWNLOADER_DEBUG
/*
 * back trace
 */
void trapfault_handler_dumpstack(unsigned long* irqs_regs,unsigned long* user_regs)  
{  
	extern void show_regs(unsigned long* irqs_regs, unsigned long* user_regs);
	extern void dump_backtrace(unsigned long* irqs_regs,unsigned long* user_regs);

	//show_regs(irqs_regs, user_regs);
	//dump_backtrace(irqs_regs, user_regs);
	/*
	OSPrintf("\r\n>>>>>>>>>>>>>>[");  
	switch(__get_IPSR())
	{
	case	3:
			OSPrintf("Hard Fault");  
			break;

	case	4:
			OSPrintf("Memory Manage");  
			break;

	case	5:
			OSPrintf("Bus Fault");  
			break;

	case	6:
			OSPrintf("Usage Fault");  
			break;

	default:
			OSPrintf("Unknown Fault %d",__get_IPSR());
			break;
	}
	OSPrintf(",corrupt,dump registers]>>>>>>>>>>>>>>>>>>\r\n");
	
	OSPrintf("R0  = 0x%08X\r\n", irqs_regs[0]);  
	OSPrintf("R1  = 0x%08X\r\n", irqs_regs[1]);  
	OSPrintf("R2  = 0x%08X\r\n", irqs_regs[2]);  
	OSPrintf("R3  = 0x%08X\r\n", irqs_regs[3]);  
	
	OSPrintf("R4  = 0x%08X\r\n", user_regs[0]);  
	OSPrintf("R5  = 0x%08X\r\n", user_regs[1]);  
	OSPrintf("R6  = 0x%08X\r\n", user_regs[2]);  
	OSPrintf("R7  = 0x%08X\r\n", user_regs[3]);  
	OSPrintf("R8  = 0x%08X\r\n", user_regs[4]);  
	OSPrintf("R9  = 0x%08X\r\n", user_regs[5]);  
	OSPrintf("R10 = 0x%08X\r\n", user_regs[6]);  
	OSPrintf("R11 = 0x%08X\r\n", user_regs[7]);  
	
	OSPrintf("R12 = 0x%08X\r\n", irqs_regs[4]);  
	OSPrintf("SP  = 0x%08X\r\n", &irqs_regs[8]);  
	OSPrintf("LR  = 0x%08X\r\n", irqs_regs[5]);  
	OSPrintf("PC  = 0x%08X\r\n", irqs_regs[6]);  
	OSPrintf("PSR = 0x%08X\r\n", irqs_regs[7]);  
	
	OSPrintf("BFAR = 0x%08X\r\n", (*((volatile unsigned long *)(0xE000ED38))));
	OSPrintf("CFSR = 0x%08X\r\n", (*((volatile unsigned long *)(0xE000ED28))));
	OSPrintf("HFSR = 0x%08X\r\n", (*((volatile unsigned long *)(0xE000ED2C))));
	OSPrintf("DFSR = 0x%08X\r\n", (*((volatile unsigned long *)(0xE000ED30))));
	OSPrintf("AFSR = 0x%08X\r\n", (*((volatile unsigned long *)(0xE000ED3C))));
	*/
	//OSPrintf("Terminated@%u ms\r\n", auxtmr_count_get());
	/*
#ifdef	DEBUG
	if(*(unsigned long*)0x20000000 != 0xA5A5A5A5){
		OSPrintf("Error:System Stack Overflow\r\n");
		return;
	}
#endif //DEBUG
	*/
	/*
	 * indefinitely deadloop
	 */
	while(1);;;
}
#endif	//IN_DOWNLOADER_DEBUG



#ifdef TEST_PRINTF
int main(void)
{
	char *ptr = "Hello world!";
	char *np = 0;
	int i = 5;
	unsigned int bs = sizeof(int)*8;
	int mi;
	char buf[80];

	mi = (1 << (bs-1)) + 1;
	printf("%s\n", ptr);
	printf("printf test\n");
	printf("%s is null pointer\n", np);
	printf("%d = 5\n", i);
	printf("%d = - max int\n", mi);
	printf("char %c = 'a'\n", 'a');
	printf("hex %x = ff\n", 0xff);
	printf("hex %02x = 00\n", 0);
	printf("signed %d = unsigned %u = hex %x\n", -3, -3, -3);
	printf("%d %s(s)%", 0, "message");
	printf("\n");
	printf("%d %s(s) with %%\n", 0, "message");
	sprintf(buf, "justif: \"%-10s\"\n", "left"); printf("%s", buf);
	sprintf(buf, "justif: \"%10s\"\n", "right"); printf("%s", buf);
	sprintf(buf, " 3: %04d zero padded\n", 3); printf("%s", buf);
	sprintf(buf, " 3: %-4d left justif.\n", 3); printf("%s", buf);
	sprintf(buf, " 3: %4d right justif.\n", 3); printf("%s", buf);
	sprintf(buf, "-3: %04d zero padded\n", -3); printf("%s", buf);
	sprintf(buf, "-3: %-4d left justif.\n", -3); printf("%s", buf);
	sprintf(buf, "-3: %4d right justif.\n", -3); printf("%s", buf);

	return 0;
}

/*
 * if you compile this file with
 *   gcc -Wall $(YOUR_C_OPTIONS) -DTEST_PRINTF -c printf.c
 * you will get a normal warning:
 *   printf.c:214: warning: spurious trailing `%' in format
 * this line is testing an invalid % at the end of the format string.
 *
 * this should display (on 32bit int machine) :
 *
 * Hello world!
 * printf test
 * (null) is null pointer
 * 5 = 5
 * -2147483647 = - max int
 * char a = 'a'
 * hex ff = ff
 * hex 00 = 00
 * signed -3 = unsigned 4294967293 = hex fffffffd
 * 0 message(s)
 * 0 message(s) with %
 * justif: "left      "
 * justif: "     right"
 *  3: 0003 zero padded
 *  3: 3    left justif.
 *  3:    3 right justif.
 * -3: -003 zero padded
 * -3: -3   left justif.
 * -3:   -3 right justif.
 */

#endif

#if		0
/* To keep linker happy. */
int	write(int i,char* c, int n)
{
	(void)i;
	(void)n;
	(void)c;
	return 0;
}
#endif //0

