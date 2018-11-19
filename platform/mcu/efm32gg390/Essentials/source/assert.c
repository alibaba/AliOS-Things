#include "BCDS_Assert.h"


void Assert_Dynamic(const unsigned long line, const unsigned char * const file)
{
    printf("Assert_Dynamic line=%d,file=%s\n",line,file);
    for (;;)
    {
        ; /* block application */
    }

}

void Assert_dynamic(const unsigned long line, const unsigned char * const file)
{
    Assert_Dynamic(line,file);
}

