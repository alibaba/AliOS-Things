#include "k_api.h"
#include "k_asid.h"

#define ASID_WORD ((ARM_MAX_ASIDS+31)/32)

static uint32_t g_asid[ASID_WORD] = {0};

int32_t k_asid_reserve(int32_t asid)
{
    int     word;
    int     bit;

    CPSR_ALLOC();

    if (asid >= ARM_MAX_ASIDS) {
        return -1;
    }

    RHINO_CRITICAL_ENTER();

    word = asid / 32;
    bit  = asid % 32;
    g_asid[word] |= bit << 1;

    RHINO_CRITICAL_EXIT();

    return 0;
}

int32_t k_asid_alloc(void)
{
    int     word;
    int     bit;
    int     asid;

    CPSR_ALLOC();

    RHINO_CRITICAL_ENTER();
    for (word = 0; word < ASID_WORD; word++) {
        for (bit = 0; bit < 32; bit++) {
            if (g_asid[word] & (1<<bit) == 0) {
                asid = word*32+bit;
                if (asid >= ARM_MAX_ASIDS) {
                    asid = -1;
                } else {
                    g_asid[word] |= (1<<bit);
                }
                goto out;
            }
        }
    }

out:
    RHINO_CRITICAL_EXIT();

    return asid;
}

int32_t k_asid_dealloc(int32_t asid)
{
    int     word;
    int     bit;

    CPSR_ALLOC();

    if (asid >= ARM_MAX_ASIDS) {
        return -1;
    }

    word = asid / 32;
    bit = asid % 32;

    RHINO_CRITICAL_ENTER();

    g_asid[word] &= ~(bit<<1);

    RHINO_CRITICAL_EXIT();

    return 0;
}

