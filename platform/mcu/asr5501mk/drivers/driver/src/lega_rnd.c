#include <stdio.h>
#include "lega_cm4.h"
#include "lega_common.h"
#include "lega_peripheral_reg.h"
#include "sns_silib.h"
#include "crys_rnd.h"
#include "lega_rnd.h"

int lega_rnd_init(lega_rnd_dev_t *rnd)
{
    if(NULL == rnd->p_workspace)
    {
        return -1;
    }
    //C310 clock enable
#ifdef LEGA_A0V2
    REG_WR(SYS_REG_BASE_C310_CLK, 0x1);
    delay(50);
#endif
    return SaSi_LibInit((CRYS_RND_Context_t *)rnd->p_workspace, (CRYS_RND_WorkBuff_t *)(rnd->p_workspace + sizeof(CRYS_RND_Context_t)));
}

int lega_rnd_generate(lega_rnd_dev_t *rnd)
{
    int ret;
    if(NULL == rnd->rnd_data || 0 == rnd->rnd_size)
    {
        return -1;
    }

    if(NULL != rnd->seed && 0 != rnd->seed_size)
    {
        /*Set additional input for RND seed*/
        ret = CRYS_RND_AddAdditionalInput((CRYS_RND_Context_t *)rnd->p_workspace, rnd->seed, rnd->seed_size);
        if(SA_SILIB_RET_OK != ret)
        {
            return ret;
        }
        /*Perform instantiation for new seed*/
        ret = CRYS_RND_Instantiation((CRYS_RND_Context_t *)rnd->p_workspace, (CRYS_RND_WorkBuff_t *)(rnd->p_workspace + sizeof(CRYS_RND_Context_t)));
        if(SA_SILIB_RET_OK != ret)
        {
            return ret;
        }
    }

    /*Try to create two vectors and check that the vectors are different*/
    return CRYS_RND_GenerateVector(&((CRYS_RND_Context_t *)rnd->p_workspace)->rndState, rnd->rnd_size, rnd->rnd_data);
}

int lega_rnd_finalize(lega_rnd_dev_t *rnd)
{
    return SaSi_LibFini((CRYS_RND_Context_t *)rnd->p_workspace);
}

