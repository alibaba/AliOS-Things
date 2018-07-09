/*
*********************************************************************************************************
*
*                                           EXCEPTION VECTORS
*
*                                            Renesas RX64M
*                                                on the
*                                      RSKRX64M Evaluation Board
*
* Filename      : bsp_vect_tbl.c
* Version       : V1.00
* Programmer(s) : DC
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                         FUNCTION PROTOTYPES
*********************************************************************************************************
*/

#pragma interrupt (Excep_SuperVisorInst)                        /* Supervisor Instruction Exception ISR Handler.        */
        void  Excep_SuperVisorInst (void);

#pragma interrupt (Excep_AccessInst)                            /* Access Instruction Exception ISR Handler.            */
        void  Excep_AccessInst     (void);

#pragma interrupt (Excep_UndefinedInst)                         /* Undefined Instruction Exception ISR Handler.         */
        void  Excep_UndefinedInst  (void);

#pragma interrupt (Excep_FloatingPoint)                         /* Floating Point Exception ISR Handler.                */
        void  Excep_FloatingPoint  (void);

#pragma interrupt (NonMaskableInterrupt)                        /* NMI Exception ISR Handler.                           */
        void  NonMaskableInterrupt (void);

#pragma interrupt (Dummy)                                       /* Generic "Dummy" ISR Handler.                         */
        void  Dummy                (void);

extern  void  PowerON_Reset_PC     (void);                      /* External: Reset Function.                            */


/*
*********************************************************************************************************
*                                  EXCEPTION / INTERRUPT VECTOR TABLE
*********************************************************************************************************
*/

#pragma section C EXCEPTVECT

void (*const Except_Vectors[])(void) = {
    Dummy,                                                      /* 0xffffff80  Reserved.                                */
    Dummy,                                                      /* 0xffffff84  Reserved.                                */
    Dummy,                                                      /* 0xffffff88  Reserved.                                */
    Dummy,                                                      /* 0xffffff8c  Reserved.                                */
    Dummy,                                                      /* 0xffffff90  Reserved.                                */
    Dummy,                                                      /* 0xffffff94  Reserved.                                */
    Dummy,                                                      /* 0xffffff98  Reserved.                                */
    Dummy,                                                      /* 0xffffff9c  Reserved.                                */
    Dummy,                                                      /* 0xffffffa0  Reserved.                                */
    Dummy,                                                      /* 0xffffffa4  Reserved.                                */
    Dummy,                                                      /* 0xffffffa8  Reserved.                                */
    Dummy,                                                      /* 0xffffffac  Reserved.                                */
    Dummy,                                                      /* 0xffffffb0  Reserved.                                */
    Dummy,                                                      /* 0xffffffb4  Reserved.                                */
    Dummy,                                                      /* 0xffffffb8  Reserved.                                */
    Dummy,                                                      /* 0xffffffbc  Reserved.                                */
    Dummy,                                                      /* 0xffffffc0  Reserved.                                */
    Dummy,                                                      /* 0xffffffc4  Reserved.                                */
    Dummy,                                                      /* 0xffffffc8  Reserved.                                */
    Dummy,                                                      /* 0xffffffcc  Reserved.                                */
    Excep_SuperVisorInst,                                       /* 0xffffffd0  Supervisor Instruction Exception.        */
    Excep_AccessInst,                                           /* 0xffffffd4  Access Instruction Exception.            */
    Dummy,                                                      /* 0xffffffd8  Reserved                                 */
    Excep_UndefinedInst,                                        /* 0xffffffdc  Undefined Instruction Exception.         */
    Dummy,                                                      /* 0xffffffe0  Reserved.                                */
    Excep_FloatingPoint,                                        /* 0xffffffe4  Floating Point Exception.                */
    Dummy,                                                      /* 0xffffffe8  Reserved.                                */
    Dummy,                                                      /* 0xffffffec  Reserved.                                */
    Dummy,                                                      /* 0xfffffff0  Reserved.                                */
    Dummy,                                                      /* 0xfffffff4  Reserved.                                */
    NonMaskableInterrupt,                                       /* 0xfffffff8  NMI Exception.                           */
};


/*
*********************************************************************************************************
*                                          RESET VECTOR TABLE
*********************************************************************************************************
*/

#pragma section C RESETVECT

void (*const Reset_Vectors[])(void) = {                         /* <<VECTOR DATA START (POWER ON RESET)>>               */
    PowerON_Reset_PC                                            /* (void*) Power On Reset PC                            */
};                                                              /* <<VECTOR DATA END (POWER ON RESET)>>                 */


/*
*********************************************************************************************************
*                                       STARTUP REGISTER DEFINES
*********************************************************************************************************
*/
#if 0
#pragma address __SPCCreg=0x00120040                            /* SPCC register                                        */
const unsigned long __SPCCreg = 0xffffffff;

#pragma address __OSISreg=0x00120050                            /* OSIC register (ID codes)                             */
const unsigned long __OSISreg[4] = {
        0xffffffff,
        0xffffffff,
        0xffffffff,
        0xffffffff,
};

#pragma address __MDEreg=0x00120064                             /* MDE register (Single Chip Mode)                      */
#ifdef __BIG
    const unsigned long __MDEreg = 0xfffffff8;                  /* Big                                                  */
#else
    const unsigned long __MDEreg = 0xffffffff;                  /* Little                                               */
#endif

#pragma address __OFS0reg=0x00120068                            /* OFS0 register                                        */
const unsigned long __OFS0reg = 0xffffffff;

#pragma address __OFS1reg=0x0012006c                            /* OFS1 register                                        */
const unsigned long __OFS1reg = 0xffffffff;
#endif

#pragma address __MDEreg=0xfe7f5d00           // MDE register (Single Chip Mode)
#ifdef __BIG
    const unsigned long __MDEreg = 0xfffffff8;    // big
#else
    const unsigned long __MDEreg = 0xffffff8f;    // little
#endif

#pragma address __OFS0reg=0xfe7f5d04              // OFS0 register
const unsigned long __OFS0reg = 0xffffffff;

#pragma address __OFS1reg=0xfe7f5d08              // OFS1 register
const unsigned long __OFS1reg = 0xffffffff;

#pragma address __TMINFreg=0xfe7f5d10         // TMINF register
const unsigned long __TMINFreg = 0xffffffff;

#pragma address __SPCCreg=0xfe7f5d40          // SPCC register
const unsigned long __SPCCreg = 0xffffffff;

#pragma address __TMEFreg=0xfe7f5d48          // TMEF register
const unsigned long __TMEFreg = 0xffffffff;

#pragma address __OSISreg=0xfe7f5d50          // OSIC register (ID codes)
const unsigned long __OSISreg[4] = {
        0xffffffff,
        0xffffffff,
        0xffffffff,
        0xffffffff,
};

#pragma address __FAWreg=0xfe7f5d64           // FAW register
const unsigned long __FAWreg = 0xffffffff;

#pragma address __RCPreg=0xfe7f5d70           // RCP register
const unsigned long __RCPreg = 0xffffffff;


/*
*********************************************************************************************************
*                                        EXCEPTION HANDLER ISRs
*********************************************************************************************************
*/

#pragma section IntPRG


/*
*********************************************************************************************************
*                                        Excep_SuperVisorInst()
*
* Description : Supervisor Instruction Exception ISR Handler.
*
* Argument(s) : none.
*
* Return(s)   : none.
*
* Caller(s)   : Application.
*
* Note(s)     : none.
*********************************************************************************************************
*/

void  Excep_SuperVisorInst (void)
{
    while(1);
}


/*
*********************************************************************************************************
*                                          Excep_AccessInst()
*
* Description : Access Instruction Exception ISR Handler.
*
* Argument(s) : none.
*
* Return(s)   : none.
*
* Caller(s)   : Application.
*
* Note(s)     : none.
*********************************************************************************************************
*/

void  Excep_AccessInst (void)
{
    while(1);
}


/*
*********************************************************************************************************
*                                        Excep_UndefinedInst()
*
* Description : Undefined Instruction Exception ISR Handler.
*
* Argument(s) : none.
*
* Return(s)   : none.
*
* Caller(s)   : Application.
*
* Note(s)     : none.
*********************************************************************************************************
*/

void  Excep_UndefinedInst (void)
{
    while(1);
}


/*
*********************************************************************************************************
*                                        Excep_FloatingPoint()
*
* Description : Floating Point Exception ISR Handler.
*
* Argument(s) : none.
*
* Return(s)   : none.
*
* Caller(s)   : Application.
*
* Note(s)     : none.
*********************************************************************************************************
*/

void  Excep_FloatingPoint (void)
{
    while(1);
}


/*
*********************************************************************************************************
*                                        NonMaskableInterrupt()
*
* Description : Non-Maskable Interrupt Exception ISR Handler.
*
* Argument(s) : none.
*
* Return(s)   : none.
*
* Caller(s)   : Application.
*
* Note(s)     : none.
*********************************************************************************************************
*/

void  NonMaskableInterrupt (void)
{
    while(1);
}


/*
*********************************************************************************************************
*                                               Dummy()
*
* Description : Generic "Dummy" ISR Handler.
*
* Argument(s) : none.
*
* Return(s)   : none.
*
* Caller(s)   : Application.
*
* Note(s)     : none.
*********************************************************************************************************
*/

void  Dummy (void)
{
    while(1);
}
