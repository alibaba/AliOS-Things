#include <sys/stat.h>

////////////////////////////////////
int _fstat (int fd, struct stat *pStat)
{
     pStat->st_mode = S_IFCHR;
     return 0;
}
////////////////////////////////////
int _close(int i)
{
     return -1;
}
/////////////////////////////////////
int _write (int fd, char *pBuffer, int size)
 {
    int i;

    for ( i = 0; i < size; i++)
    {
         //USART_SendData(USART1, pBuffer[i]);
        //uart_put_char(pBuffer[i]);
    }

    return size;
}
///////////////////////////////////
int _isatty (int fd)
{
    return 1;
}
/////////////////////////////////
int _lseek(int i, int k, int j)
{
     return -1;
}
////////////////////////////////
int _read (int fd, char *pBuffer, int size)
{
    int i;

    for (i = 0; i < size; i++)
    {
        //pBuffer[i] = UART_ReceiveData(UART1);
    }
    return size;
}
////////////////////////////////
caddr_t _sbrk(int increment)
{
      extern char end asm("end");
      register char *pStack asm("sp");

      static char *s_pHeapEnd;

      if (!s_pHeapEnd)
            s_pHeapEnd = &end;

      if (s_pHeapEnd + increment > pStack)
           return (caddr_t)-1;

      char *pOldHeapEnd = s_pHeapEnd;
      s_pHeapEnd += increment;
      return (caddr_t)pOldHeapEnd;
 }

 /************end *************************/