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
#include "wdt_api.h"
#include "cmsis.h"

extern VOID WDGInitial(u32 Period);
extern VOID WDGStart(VOID);
extern VOID WDGStop(VOID);
extern VOID WDGRefresh(VOID);
extern VOID WDGIrqInitial(VOID);
extern VOID WDGIrqCallBackReg(VOID *CallBack, u32   Id);

/**
 *  @brief   Initial the watch dog time setting
 *
 *  @param   timeout_ms: the watch-dog timer timeout value, in ms.
 *           default action of timeout is to reset the whole system.
 *  @return  None
 *         
 */
void watchdog_init(uint32_t timeout_ms)
{
    WDGInitial(timeout_ms);
}

/**
 *  @brief   Start the watchdog counting
 *
 *  @param   None
 *  @return  None
 *         
 */
void watchdog_start(void)
{
    WDGStart();
}

/**
 *  @brief   Stop the watchdog counting
 *
 *  @param   None
 *  @return  None
 *         
 */
void watchdog_stop(void)
{
    WDGStop();
}

/**
 *  @brief   Refresh the watchdog counting to prevent WDT timeout
 *
 *  @param   None
 *  @return  None
 *         
 */
void watchdog_refresh(void)
{
    WDGRefresh();
}

/**
 *  @brief   Switch the watchdog timer to interrupt mode and
 *           register a watchdog timer timeout interrupt handler.
 *           The interrupt handler will be called when the watch-dog 
 *           timer is timeout.
 *
 *  @param   handler: the callback function for WDT timeout interrupt.
 *           id: the parameter for the callback function
 *  @return  None
 *         
 */
void watchdog_irq_init(wdt_irq_handler handler, uint32_t id)
{
    WDGIrqCallBackReg((VOID*)handler, (u32)id);
    WDGIrqInitial();
}


