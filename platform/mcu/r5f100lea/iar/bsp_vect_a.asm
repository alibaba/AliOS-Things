
#include  "port_cpu.inc"

    NAME    ?bsp_vect

    EXTERN  _int_dummy
    EXTERN  _krhino_tick_proc
    PUBLIC  SOC_WDTI
    PUBLIC  SOC_LVI
    PUBLIC  SOC_P0
    PUBLIC  SOC_P1
    PUBLIC  SOC_P2
    PUBLIC  SOC_P3
    PUBLIC  SOC_P4
    PUBLIC  SOC_P5
    PUBLIC  SOC_ST2_CSI20_IIC20
    PUBLIC  SOC_SR2_CSI20_IIC21
    PUBLIC  SOC_SRE2_TM11H
    PUBLIC  SOC_DMA0
    PUBLIC  SOC_DMA1
    PUBLIC  SOC_ST0_CSI00_IIC00
    PUBLIC  SOC_ST0_CSI00_IIC00
    PUBLIC  SOC_SR0_CSI01_IIC01
    PUBLIC  SOC_SRE0_TM01H
    PUBLIC  SOC_ST1_CSI10_IIC10
    PUBLIC  SOC_SR1_CSI11_IIC11
    PUBLIC  SOC_IICA0
    PUBLIC  SOC_TM00
    PUBLIC  SOC_TM01
    PUBLIC  SOC_TM02
    PUBLIC  SOC_TM03
    PUBLIC  SOC_AD
    PUBLIC  SOC_RTC
    PUBLIC  SOC_IT
    PUBLIC  SOC_KR
    PUBLIC  SOC_ST3_CSI30_IIC30
    PUBLIC  SOC_SR3_CSI31_IIC31
    PUBLIC  SOC_TM13
    PUBLIC  SOC_TM04
    PUBLIC  SOC_TM05
    PUBLIC  SOC_TM06
    PUBLIC  SOC_TM07
    PUBLIC  SOC_P6
    PUBLIC  SOC_P7
    PUBLIC  SOC_P8
    PUBLIC  SOC_P9
    PUBLIC  SOC_P10
    PUBLIC  SOC_P11
    PUBLIC  SOC_TM10
    PUBLIC  SOC_TM11
    PUBLIC  SOC_TM12
    PUBLIC  SOC_SRE3_TM13H
    PUBLIC  SOC_MD
    PUBLIC  SOC_IICA1
    PUBLIC  SOC_FL
    PUBLIC  SOC_DMA2
    PUBLIC  SOC_DMA3
    PUBLIC  SOC_TM14
    PUBLIC  SOC_TM15
    PUBLIC  SOC_TM16
    PUBLIC  SOC_TM17

    PUBLIC  ___interrupt_0x04
    PUBLIC  ___interrupt_0x06
    PUBLIC  ___interrupt_0x08
    PUBLIC  ___interrupt_0x0A
    PUBLIC  ___interrupt_0x0C
    PUBLIC  ___interrupt_0x0E
    PUBLIC  ___interrupt_0x10
    PUBLIC  ___interrupt_0x12
    PUBLIC  ___interrupt_0x14
    PUBLIC  ___interrupt_0x16
    PUBLIC  ___interrupt_0x18
    PUBLIC  ___interrupt_0x1A
    PUBLIC  ___interrupt_0x1C
    PUBLIC  ___interrupt_0x1E
    PUBLIC  ___interrupt_0x20
    PUBLIC  ___interrupt_0x22
    PUBLIC  ___interrupt_0x24
    PUBLIC  ___interrupt_0x26
    PUBLIC  ___interrupt_0x28
    PUBLIC  ___interrupt_0x2A
    PUBLIC  ___interrupt_0x2C
    PUBLIC  ___interrupt_0x2E
    PUBLIC  ___interrupt_0x30
    PUBLIC  ___interrupt_0x32
    PUBLIC  ___interrupt_0x34
    PUBLIC  ___interrupt_0x36
    PUBLIC  ___interrupt_0x38
    PUBLIC  ___interrupt_0x3A
    PUBLIC  ___interrupt_0x3C
    PUBLIC  ___interrupt_0x3E
    PUBLIC  ___interrupt_0x40
    PUBLIC  ___interrupt_0x42
    PUBLIC  ___interrupt_0x44
    PUBLIC  ___interrupt_0x46
    PUBLIC  ___interrupt_0x48
    PUBLIC  ___interrupt_0x4A
    PUBLIC  ___interrupt_0x4C
    PUBLIC  ___interrupt_0x4E
    PUBLIC  ___interrupt_0x50
    PUBLIC  ___interrupt_0x52
    PUBLIC  ___interrupt_0x54
    PUBLIC  ___interrupt_0x56
    PUBLIC  ___interrupt_0x58
    PUBLIC  ___interrupt_0x5A
    PUBLIC  ___interrupt_0x5C
    PUBLIC  ___interrupt_0x5E
    PUBLIC  ___interrupt_0x60
    PUBLIC  ___interrupt_0x62
    PUBLIC  ___interrupt_0x64
    PUBLIC  ___interrupt_0x66
    PUBLIC  ___interrupt_0x68
    PUBLIC  ___interrupt_0x6A
    PUBLIC  ___interrupt_0x6C
    PUBLIC  ___interrupt_0x6E

;********************************************************************************************************
;                                      OPTION BYTES CONFIGURATIONS
;********************************************************************************************************

    SECTION .option_byte:CODE:ROOT(1)
    DB      0xEF, 0x57, 0xE8, 0x84

    SECTION .security_id:CODE:ROOT(1)
    DB      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00

;********************************************************************************************************
;                                         INTERRUPT HANDLERS
;********************************************************************************************************

    SECTION .text:CODE:ROOT(1)

SOC_WDTI:
___interrupt_0x04:
    sys_isr_enter
    MOVW   AX, #0
    CALL   _int_dummy
    sys_isr_exit

SOC_LVI:
___interrupt_0x06:
    sys_isr_enter
    MOVW   AX, #1
    CALL   _int_dummy
    sys_isr_exit

SOC_P0:
___interrupt_0x08:
    sys_isr_enter
    MOVW   AX, #2
    CALL   _int_dummy
    sys_isr_exit

SOC_P1:
___interrupt_0x0A:
    sys_isr_enter
    MOVW   AX, #3
    CALL   _int_dummy
    sys_isr_exit

SOC_P2:
___interrupt_0x0C:
    sys_isr_enter
    MOVW   AX, #4
    CALL   _int_dummy
    sys_isr_exit

SOC_P3:
___interrupt_0x0E:
    sys_isr_enter
    MOVW   AX, #5
    CALL   _int_dummy
    sys_isr_exit

SOC_P4:
___interrupt_0x10:
    sys_isr_enter
    MOVW   AX, #6
    CALL   _int_dummy
    sys_isr_exit

SOC_P5:
___interrupt_0x12:
    sys_isr_enter
    MOVW   AX, #7
    CALL   _int_dummy
    sys_isr_exit

SOC_ST2_CSI20_IIC20:
___interrupt_0x14:
    sys_isr_enter
    MOVW   AX, #8
    CALL   _int_dummy
    sys_isr_exit

SOC_SR2_CSI20_IIC21:
___interrupt_0x16:
    sys_isr_enter
    MOVW   AX, #9
    CALL   _int_dummy
    sys_isr_exit

SOC_SRE2_TM11H:
___interrupt_0x18:
    sys_isr_enter
    MOVW   AX, #10
    CALL   _int_dummy
    sys_isr_exit

SOC_DMA0:
___interrupt_0x1A:
    sys_isr_enter
    MOVW   AX, #11
    CALL   _int_dummy
    sys_isr_exit

SOC_DMA1:
___interrupt_0x1C:
    sys_isr_enter
    MOVW   AX, #12
    CALL   _int_dummy
    sys_isr_exit

SOC_ST0_CSI00_IIC00:
___interrupt_0x1E:
    sys_isr_enter
    MOVW   AX, #13
    CALL   _int_dummy
    sys_isr_exit

SOC_SR0_CSI01_IIC01:
___interrupt_0x20:
    sys_isr_enter
    MOVW   AX, #14
    CALL   _int_dummy
    sys_isr_exit

SOC_SRE0_TM01H:
___interrupt_0x22:
    sys_isr_enter
    MOVW   AX, #15
    CALL   _int_dummy
    sys_isr_exit

SOC_ST1_CSI10_IIC10:
___interrupt_0x24:
    sys_isr_enter
    MOVW   AX, #16
    CALL   _int_dummy
    sys_isr_exit

SOC_SR1_CSI11_IIC11:
___interrupt_0x26:
    sys_isr_enter
    MOVW   AX, #17
    CALL   _int_dummy
    sys_isr_exit

SOC_SRE1_TM03H:
___interrupt_0x28:
    sys_isr_enter
    MOVW   AX, #18
    CALL   _int_dummy
    sys_isr_exit

SOC_IICA0:
___interrupt_0x2A:
    sys_isr_enter
    MOVW   AX, #19
    CALL   _int_dummy
    sys_isr_exit

SOC_TM00:
___interrupt_0x2C:
    sys_isr_enter
    MOVW   AX, #20
    CALL   _krhino_tick_proc
    sys_isr_exit

SOC_TM01:
___interrupt_0x2E:
    sys_isr_enter
    MOVW   AX, #21
    CALL   _int_dummy
    sys_isr_exit

SOC_TM02:
___interrupt_0x30:
    sys_isr_enter
    MOVW   AX, #22
    CALL   _int_dummy
    sys_isr_exit

SOC_TM03:
___interrupt_0x32:
    sys_isr_enter
    MOVW   AX, #23
    CALL   _int_dummy
    sys_isr_exit

SOC_AD:
___interrupt_0x34:
    sys_isr_enter
    MOVW   AX, #24
    CALL   _int_dummy
    sys_isr_exit

SOC_RTC:
___interrupt_0x36:
    sys_isr_enter
    MOVW   AX, #25
    CALL   _int_dummy
    sys_isr_exit

SOC_IT:
___interrupt_0x38:
    sys_isr_enter
    MOVW   AX, #26
    CALL   _int_dummy
    sys_isr_exit

SOC_KR:
___interrupt_0x3A:
    sys_isr_enter
    MOVW   AX, #27
    CALL   _int_dummy
    sys_isr_exit

SOC_ST3_CSI30_IIC30:
___interrupt_0x3C:
    sys_isr_enter
    MOVW   AX, #28
    CALL   _int_dummy
    sys_isr_exit

SOC_SR3_CSI31_IIC31:
___interrupt_0x3E:
    sys_isr_enter
    MOVW   AX, #29
    CALL   _int_dummy
    sys_isr_exit

SOC_TM13:
___interrupt_0x40:
    sys_isr_enter
    MOVW   AX, #30
    CALL   _int_dummy
    sys_isr_exit

SOC_TM04:
___interrupt_0x42:
    sys_isr_enter
    MOVW   AX, #31
    CALL   _int_dummy
    sys_isr_exit

SOC_TM05:
___interrupt_0x44:
    sys_isr_enter
    MOVW   AX, #32
    CALL   _int_dummy
    sys_isr_exit

SOC_TM06:
___interrupt_0x46:
    sys_isr_enter
    MOVW   AX, #33
    CALL   _int_dummy
    sys_isr_exit

SOC_TM07:
___interrupt_0x48:
    sys_isr_enter
    MOVW   AX, #34
    CALL   _int_dummy
    sys_isr_exit

SOC_P6:
___interrupt_0x4A:
    sys_isr_enter
    MOVW   AX, #35
    CALL   _int_dummy
    sys_isr_exit

SOC_P7:
___interrupt_0x4C:
    sys_isr_enter
    MOVW   AX, #36
    CALL   _int_dummy
    sys_isr_exit

SOC_P8:
___interrupt_0x4E:
    sys_isr_enter
    MOVW   AX, #37
    CALL   _int_dummy
    sys_isr_exit

SOC_P9:
___interrupt_0x50:
    sys_isr_enter
    MOVW   AX, #38
    CALL   _int_dummy
    sys_isr_exit

SOC_P10:
___interrupt_0x52:
    sys_isr_enter
    MOVW   AX, #39
    CALL   _int_dummy
    sys_isr_exit

SOC_P11:
___interrupt_0x54:
    sys_isr_enter
    MOVW   AX, #40
    CALL   _int_dummy
    sys_isr_exit

SOC_TM10:
___interrupt_0x56:
    sys_isr_enter
    MOVW   AX, #41
    CALL   _int_dummy
    sys_isr_exit

SOC_TM11:
___interrupt_0x58:
    sys_isr_enter
    MOVW   AX, #42
    CALL   _int_dummy
    sys_isr_exit

SOC_TM12:
___interrupt_0x5A:
    sys_isr_enter
    MOVW   AX, #43
    CALL   _int_dummy
    sys_isr_exit

SOC_SRE3_TM13H:
___interrupt_0x5C:
    sys_isr_enter
    MOVW   AX, #44
    CALL   _int_dummy
    sys_isr_exit

SOC_MD:
___interrupt_0x5E:
    sys_isr_enter
    MOVW   AX, #45
    CALL   _int_dummy
    sys_isr_exit

SOC_IICA1:
___interrupt_0x60:
    sys_isr_enter
    MOVW   AX, #46
    CALL   _int_dummy
    sys_isr_exit

SOC_FL:
___interrupt_0x62:
    sys_isr_enter
    MOVW   AX, #47
    CALL   _int_dummy
    sys_isr_exit

SOC_DMA2:
___interrupt_0x64:
    sys_isr_enter
    MOVW   AX, #48
    CALL   _int_dummy
    sys_isr_exit

SOC_DMA3:
___interrupt_0x66:
    sys_isr_enter
    MOVW   AX, #49
    CALL   _int_dummy
    sys_isr_exit

SOC_TM14:
___interrupt_0x68:
    sys_isr_enter
    MOVW   AX, #50
    CALL   _int_dummy
    sys_isr_exit

SOC_TM15:
___interrupt_0x6A:
    sys_isr_enter
    MOVW   AX, #51
    CALL   _int_dummy
    sys_isr_exit

SOC_TM16:
___interrupt_0x6C:
    sys_isr_enter
    MOVW   AX, #52
    CALL   _int_dummy
    sys_isr_exit

SOC_TM17:
___interrupt_0x6E:
    sys_isr_enter
    MOVW   AX, #53
    CALL   _int_dummy
    sys_isr_exit

    END

