$INCLUDE(port_cpu.inc)

    ;.NAME    ?bsp_vect
	
    .EXTERN  _int_dummy
    .EXTERN  _krhino_tick_proc
    .PUBLIC  _SOC_WDTI
    .PUBLIC  _SOC_LVI
    .PUBLIC  _SOC_P0
    .PUBLIC  _SOC_P1
    .PUBLIC  _SOC_P2
    .PUBLIC  _SOC_P3
    .PUBLIC  _SOC_P4
    .PUBLIC  _SOC_P5
    .PUBLIC  _SOC_ST2_CSI20_IIC20
    .PUBLIC  _SOC_SR2_CSI20_IIC21
    .PUBLIC  _SOC_SRE2_TM11H
    .PUBLIC  _SOC_DMA0
    .PUBLIC  _SOC_DMA1
    .PUBLIC  _SOC_ST0_CSI00_IIC00
    .PUBLIC  _SOC_ST0_CSI00_IIC00
    .PUBLIC  _SOC_SR0_CSI01_IIC01
    .PUBLIC  _SOC_SRE0_TM01H
    .PUBLIC  _SOC_ST1_CSI10_IIC10
    .PUBLIC  _SOC_SR1_CSI11_IIC11
    .PUBLIC  _SOC_IICA0
    .PUBLIC  _SOC_TM00
    .PUBLIC  _SOC_TM01
    .PUBLIC  _SOC_TM02
    .PUBLIC  _SOC_TM03
    .PUBLIC  _SOC_AD
    .PUBLIC  _SOC_RTC
    .PUBLIC  _SOC_IT
    .PUBLIC  _SOC_KR
    .PUBLIC  _SOC_ST3_CSI30_IIC30
    .PUBLIC  _SOC_SR3_CSI31_IIC31
    .PUBLIC  _SOC_TM13
    .PUBLIC  _SOC_TM04
    .PUBLIC  _SOC_TM05
    .PUBLIC  _SOC_TM06
    .PUBLIC  _SOC_TM07
    .PUBLIC  _SOC_P6
    .PUBLIC  _SOC_P7
    .PUBLIC  _SOC_P8
    .PUBLIC  _SOC_P9
    .PUBLIC  _SOC_P10
    .PUBLIC  _SOC_P11
    .PUBLIC  _SOC_TM10
    .PUBLIC  _SOC_TM11
    .PUBLIC  _SOC_TM12
    .PUBLIC  _SOC_SRE3_TM13H
    .PUBLIC  _SOC_MD
    .PUBLIC  _SOC_IICA1
    .PUBLIC  _SOC_FL
    .PUBLIC  _SOC_DMA2
    .PUBLIC  _SOC_DMA3
    .PUBLIC  _SOC_TM14
    .PUBLIC  _SOC_TM15
    .PUBLIC  _SOC_TM16
    .PUBLIC  _SOC_TM17

    _SOC_WDTI				.VECTOR	0x0004
    _SOC_LVI                 .VECTOR 0x0006
    _SOC_P0                  .VECTOR 0x0008
    _SOC_P1                  .VECTOR 0x000A
    _SOC_P2                  .VECTOR 0x000C
    _SOC_P3                  .VECTOR 0x000E
    _SOC_P4                  .VECTOR 0x0010
    _SOC_P5                  .VECTOR 0x0012
    _SOC_ST2_CSI20_IIC20     .VECTOR 0x0014
    _SOC_SR2_CSI20_IIC21     .VECTOR 0x0016
    _SOC_SRE2_TM11H          .VECTOR 0x0018
    _SOC_DMA0                .VECTOR 0x001A
    _SOC_DMA1                .VECTOR 0x001C
    _SOC_ST0_CSI00_IIC00     .VECTOR 0x001E
    _SOC_ST0_CSI00_IIC00     .VECTOR 0x0020
    _SOC_SR0_CSI01_IIC01     .VECTOR 0x0022
    _SOC_SRE0_TM01H          .VECTOR 0x0024
    _SOC_ST1_CSI10_IIC10     .VECTOR 0x0026
    _SOC_SR1_CSI11_IIC11     .VECTOR 0x0028
    _SOC_IICA0               .VECTOR 0x002A
    _SOC_TM00                .VECTOR 0x002C
    _SOC_TM01                .VECTOR 0x002E
    _SOC_TM02                .VECTOR 0x0030
    _SOC_TM03                .VECTOR 0x0032
    _SOC_AD                  .VECTOR 0x0034
    _SOC_RTC                 .VECTOR 0x0036
    _SOC_IT                  .VECTOR 0x0038
    _SOC_KR                  .VECTOR 0x003A
    _SOC_ST3_CSI30_IIC30     .VECTOR 0x003C
    _SOC_SR3_CSI31_IIC31     .VECTOR 0x003E
    _SOC_TM13                .VECTOR 0x0040
    _SOC_TM04                .VECTOR 0x0042
    _SOC_TM05                .VECTOR 0x0044
    _SOC_TM06                .VECTOR 0x0046
    _SOC_TM07                .VECTOR 0x0048
    _SOC_P6                  .VECTOR 0x004A
    _SOC_P7                  .VECTOR 0x004C
    _SOC_P8                  .VECTOR 0x004E
    _SOC_P9                  .VECTOR 0x0050
    _SOC_P10                 .VECTOR 0x0052
    _SOC_P11                 .VECTOR 0x0054
    _SOC_TM10                .VECTOR 0x0056
    _SOC_TM11                .VECTOR 0x0058
    _SOC_TM12                .VECTOR 0x005A
    _SOC_SRE3_TM13H          .VECTOR 0x005C
    _SOC_MD                  .VECTOR 0x005E
    _SOC_IICA1               .VECTOR 0x0060
    _SOC_FL                  .VECTOR 0x0062
    _SOC_DMA2                .VECTOR 0x0064
    _SOC_DMA3                .VECTOR 0x0066
    _SOC_TM14                .VECTOR 0x0068
    _SOC_TM15                .VECTOR 0x006A
    _SOC_TM16                .VECTOR 0x006C
    _SOC_TM17                .VECTOR 0x006E



	.SECTION .text, TEXT
	
_SOC_WDTI:
    sys_isr_enter
    MOVW   AX, #0
    CALL   !!_int_dummy
    sys_isr_exit

_SOC_LVI:
    sys_isr_enter
    MOVW   AX, #1
    CALL   !!_int_dummy
    sys_isr_exit

_SOC_P0:
    sys_isr_enter
    MOVW   AX, #2
    CALL   !!_int_dummy
    sys_isr_exit

_SOC_P1:
    sys_isr_enter
    MOVW   AX, #3
    CALL   !!_int_dummy
    sys_isr_exit

_SOC_P2:
    sys_isr_enter
    MOVW   AX, #4
    CALL   !!_int_dummy
    sys_isr_exit

_SOC_P3:
    sys_isr_enter
    MOVW   AX, #5
    CALL   !!_int_dummy
    sys_isr_exit

_SOC_P4:
    sys_isr_enter
    MOVW   AX, #6
    CALL   !!_int_dummy
    sys_isr_exit

_SOC_P5:
    sys_isr_enter
    MOVW   AX, #7
    CALL   !!_int_dummy
    sys_isr_exit

_SOC_ST2_CSI20_IIC20:
    sys_isr_enter
    MOVW   AX, #8
    CALL   !!_int_dummy
    sys_isr_exit

_SOC_SR2_CSI20_IIC21:
    sys_isr_enter
    MOVW   AX, #9
    CALL   !!_int_dummy
    sys_isr_exit

_SOC_SRE2_TM11H:
    sys_isr_enter
    MOVW   AX, #10
    CALL   !!_int_dummy
    sys_isr_exit

_SOC_DMA0:
    sys_isr_enter
    MOVW   AX, #11
    CALL   !!_int_dummy
    sys_isr_exit

_SOC_DMA1:
    sys_isr_enter
    MOVW   AX, #12
    CALL   !!_int_dummy
    sys_isr_exit

_SOC_ST0_CSI00_IIC00:
    sys_isr_enter
    MOVW   AX, #13
    CALL   !!_int_dummy
    sys_isr_exit

_SOC_SR0_CSI01_IIC01:
    sys_isr_enter
    MOVW   AX, #14
    CALL   !!_int_dummy
    sys_isr_exit

_SOC_SRE0_TM01H:
    sys_isr_enter
    MOVW   AX, #15
    CALL   !!_int_dummy
    sys_isr_exit

_SOC_ST1_CSI10_IIC10:
    sys_isr_enter
    MOVW   AX, #16
    CALL   !!_int_dummy
    sys_isr_exit

_SOC_SR1_CSI11_IIC11:
    sys_isr_enter
    MOVW   AX, #17
    CALL   !!_int_dummy
    sys_isr_exit

_SOC_SRE1_TM03H:
    sys_isr_enter
    MOVW   AX, #18
    CALL   !!_int_dummy
    sys_isr_exit

_SOC_IICA0:
    sys_isr_enter
    MOVW   AX, #19
    CALL   !!_int_dummy
    sys_isr_exit

_SOC_TM00:
    sys_isr_enter
    MOVW   AX, #20
    CALL   !_krhino_tick_proc
    sys_isr_exit

_SOC_TM01:
    sys_isr_enter
    MOVW   AX, #21
    CALL   !!_int_dummy
    sys_isr_exit

_SOC_TM02:
    sys_isr_enter
    MOVW   AX, #22
    CALL   !!_int_dummy
    sys_isr_exit

_SOC_TM03:
    sys_isr_enter
    MOVW   AX, #23
    CALL   !!_int_dummy
    sys_isr_exit

_SOC_AD:
    sys_isr_enter
    MOVW   AX, #24
    CALL   !!_int_dummy
    sys_isr_exit

_SOC_RTC:
    sys_isr_enter
    MOVW   AX, #25
    CALL   !!_int_dummy
    sys_isr_exit

_SOC_IT:
    sys_isr_enter
    MOVW   AX, #26
    CALL   !!_int_dummy
    sys_isr_exit

_SOC_KR:
    sys_isr_enter
    MOVW   AX, #27
    CALL   !!_int_dummy
    sys_isr_exit

_SOC_ST3_CSI30_IIC30:
    sys_isr_enter
    MOVW   AX, #28
    CALL   !!_int_dummy
    sys_isr_exit

_SOC_SR3_CSI31_IIC31:
    sys_isr_enter
    MOVW   AX, #29
    CALL   !!_int_dummy
    sys_isr_exit

_SOC_TM13:
    sys_isr_enter
    MOVW   AX, #30
    CALL   !!_int_dummy
    sys_isr_exit

_SOC_TM04:
    sys_isr_enter
    MOVW   AX, #31
    CALL   !!_int_dummy
    sys_isr_exit

_SOC_TM05:
    sys_isr_enter
    MOVW   AX, #32
    CALL   !!_int_dummy
    sys_isr_exit

_SOC_TM06:
    sys_isr_enter
    MOVW   AX, #33
    CALL   !!_int_dummy
    sys_isr_exit

_SOC_TM07:
    sys_isr_enter
    MOVW   AX, #34
    CALL   !!_int_dummy
    sys_isr_exit

_SOC_P6:
    sys_isr_enter
    MOVW   AX, #35
    CALL   !!_int_dummy
    sys_isr_exit

_SOC_P7:
    sys_isr_enter
    MOVW   AX, #36
    CALL   !!_int_dummy
    sys_isr_exit

_SOC_P8:
    sys_isr_enter
    MOVW   AX, #37
    CALL   !!_int_dummy
    sys_isr_exit

_SOC_P9:
    sys_isr_enter
    MOVW   AX, #38
    CALL   !!_int_dummy
    sys_isr_exit

_SOC_P10:
    sys_isr_enter
    MOVW   AX, #39
    CALL   !!_int_dummy
    sys_isr_exit

_SOC_P11:
    sys_isr_enter
    MOVW   AX, #40
    CALL   !!_int_dummy
    sys_isr_exit

_SOC_TM10:
    sys_isr_enter
    MOVW   AX, #41
    CALL   !!_int_dummy
    sys_isr_exit

_SOC_TM11:
    sys_isr_enter
    MOVW   AX, #42
    CALL   !!_int_dummy
    sys_isr_exit

_SOC_TM12:
    sys_isr_enter
    MOVW   AX, #43
    CALL   !!_int_dummy
    sys_isr_exit

_SOC_SRE3_TM13H:
    sys_isr_enter
    MOVW   AX, #44
    CALL   !!_int_dummy
    sys_isr_exit

_SOC_MD:
    sys_isr_enter
    MOVW   AX, #45
    CALL   !!_int_dummy
    sys_isr_exit

_SOC_IICA1:
    sys_isr_enter
    MOVW   AX, #46
    CALL   !!_int_dummy
    sys_isr_exit

_SOC_FL:
    sys_isr_enter
    MOVW   AX, #47
    CALL   !!_int_dummy
    sys_isr_exit

_SOC_DMA2:
    sys_isr_enter
    MOVW   AX, #48
    CALL   !!_int_dummy
    sys_isr_exit

_SOC_DMA3:
    sys_isr_enter
    MOVW   AX, #49
    CALL   !!_int_dummy
    sys_isr_exit

_SOC_TM14:
    sys_isr_enter
    MOVW   AX, #50
    CALL   !!_int_dummy
    sys_isr_exit

_SOC_TM15:
    sys_isr_enter
    MOVW   AX, #51
    CALL   !!_int_dummy
    sys_isr_exit

_SOC_TM16:
    sys_isr_enter
    MOVW   AX, #52
    CALL   !!_int_dummy
    sys_isr_exit

_SOC_TM17:
    sys_isr_enter
    MOVW   AX, #53
    CALL   !!_int_dummy
    sys_isr_exit


