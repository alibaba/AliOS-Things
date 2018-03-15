/* mbed Microcontroller Library
 *******************************************************************************
 * Copyright (c) 2014, Realtek
 * All rights reserved.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *******************************************************************************
 */
#include "dma_api.h"
#include "cmsis.h"

extern BOOL HalGdmaMemCpyInit(PHAL_GDMA_OBJ pHalGdmaObj);
extern VOID HalGdmaMemCpyDeInit(PHAL_GDMA_OBJ pHalGdmaObj);
extern VOID* HalGdmaMemCpy(PHAL_GDMA_OBJ pHalGdmaObj, void* pDest, void* pSrc, u32 len);
extern VOID HalGdmaMemAggr(PHAL_GDMA_OBJ pHalGdmaObj, PHAL_GDMA_BLOCK pHalGdmaBlock);
extern BOOL HalGdmaMemCpyAggrInit(PHAL_GDMA_OBJ pHalGdmaObj);

/**
 *  @brief   Initial the GDMA
 *
 *  @param   dma_obj: the GDMA object
 *           handler: the callback function for a DMA transfer complete.
 *           id: the argument of the callback function.
 *  @return  None
 *         
 */
void dma_memcpy_aggr_init(gdma_t *dma_obj, dma_irq_handler handler, uint32_t id)
{
    dma_obj->gdma_obj.GdmaIrqHandle.IrqFun = (IRQ_FUN)handler;
    dma_obj->gdma_obj.GdmaIrqHandle.Data = (u32)id;
    dma_obj->gdma_allocated = HalGdmaMemCpyAggrInit(&(dma_obj->gdma_obj));
}


void dma_memcpy_init(gdma_t *dma_obj, dma_irq_handler handler, uint32_t id)
{
    dma_obj->gdma_obj.GdmaIrqHandle.IrqFun = (IRQ_FUN)handler;
    dma_obj->gdma_obj.GdmaIrqHandle.Data = (u32)id;
    dma_obj->gdma_allocated = HalGdmaMemCpyInit(&(dma_obj->gdma_obj));
}

/**
 *  @brief   De-Initial the GDMA
 *
 *  @param   dma_obj: the GDMA object
 *  @return  None
 *         
 */
void dma_memcpy_deinit(gdma_t *dma_obj)
{
    if (dma_obj->gdma_allocated) {
        HalGdmaMemCpyDeInit(&(dma_obj->gdma_obj));
    }
}

/**
 *  @brief   To do a memory copy by DMA
 *
 *  @param   None
 *  @return  None
 *         
 */
void dma_memcpy(gdma_t *dma_obj, void *dst, void* src, uint32_t len)
{
#if 0
    if (!dma_obj->gdma_allocated) {
        dma_irq_handler handler;
        
        _memcpy(dst, src, len);
        handler = dma_obj->GdmaIrqHandle.IrqFun;
        handler(dma_obj->GdmaIrqHandle.Data);
    }
#endif    
    HalGdmaMemCpy(&(dma_obj->gdma_obj), dst, src, len);
}

void dma_memcpy_aggr(gdma_t *dma_obj, PHAL_GDMA_BLOCK block_info)
{
    HalGdmaMemAggr(&(dma_obj->gdma_obj), block_info);
}
