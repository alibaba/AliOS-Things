#include <string.h>
#include <stdint.h>
#include <stdlib.h>

uint32_t platform_random(uint32_t max)
{
    uint32_t seed =  * ((volatile uint32_t *) 0xe000e018) ;      //system tick current value
    srand(seed);

    return rand() % max;
}

