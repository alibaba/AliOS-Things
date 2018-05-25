#include <stdio.h>

#ifdef __LINUX_UB2__
#include <stdint.h>
#endif

#include <unistd.h>

#include "joylink.h"
#include "joylink_list.h"
#include "joylink_utils.h"

typedef struct __s{
    int i;
    char name[24];
}ST_t;

void 
test(ST_t *st)
{
    ST_t t1 = {10, "wag"};
    ST_t t2;

    *st = t1;
    t2 = t1;

    printf(" test func -->:%d--:%s\n", st->i, st->name);
    printf(" test func -->:%d--:%s\n", t2.i, t2.name);

    printf(" test func -->:%p--:%p\n", &t2, &t1);
}
int main()
{
    int len;
    int index = 0x1234;    
    char buffer[100] = {0};
    
    len = joylink_util_byte2hexstr((uint8_t *) &index, sizeof(index), 
            (uint8_t*)buffer, sizeof(buffer));

    printf("--->:%s:len:%d\n", buffer, len);


    ST_t t2;

    test(&t2);

    printf("-->:%d--:%s\n", t2.i, t2.name);

    return 0;
}
