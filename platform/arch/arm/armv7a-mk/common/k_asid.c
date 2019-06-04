#include "k_api.h"
#include "k_asid.h"

#define ASID_WORD ((ARM_MAX_ASIDS+31)/32)

static uint32_t g_asid[ASID_WORD] = {0};

int32_t k_asid_reserve(int32_t asid)
{
    return 0;
}

int32_t k_asid_alloc(void)
{
    return 0;
}

int32_t k_asid_dealloc(int32_t asid)
{
    return 0;
}

