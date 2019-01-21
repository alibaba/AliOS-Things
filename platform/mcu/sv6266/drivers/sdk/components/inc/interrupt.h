/*
 * Copyright (c) 2012-2015 Andes Technology Corporation
 * All rights reserved.
 *
 */
#ifndef INTERRUPT_H
#define INTERRUPT_H

	.macro gie_disable
		setgie.d
		dsb
	.endm

	.macro gie_enable
		setgie.e
	.endm

	/*
	 * push_misc/pop_misc
	 * The macros are used for saving/restoring IFC_LP and ZOL
	 * related system register.
	 */
	.macro push_misc R0="$r2", R1="$r3", R2="$r4", R3="$r5", ODD_ALIGN=0
#if defined(__NDS32_EXT_IFC__) && defined(CFG_HWZOL)
		mfusr	\R0, $IFC_LP
		mfusr	\R1, $LB
		mfusr	\R2, $LE
		mfusr	\R3, $LC
		pushm	\R0, \R3
		.if (1 == \ODD_ALIGN)
		push	\R0             // R0: Dummy
		.endif
#else
#ifdef __NDS32_EXT_IFC__
		mfusr   \R0, $IFC_LP
		.if (1 == \ODD_ALIGN)
		push	\R0
		.else
		pushm	\R0, \R1        // R1: dummy
		.endif
#endif
#ifdef CFG_HWZOL
		mfusr	\R0, $LB
		mfusr	\R1, $LE
		mfusr	\R2, $LC
		.if (1 == \ODD_ALIGN)
		pushm	\R0, \R2
		.else
		pushm	\R0, \R3        // R3: dummy
		.endif
#endif
#endif
	.endm

	.macro pop_misc R0="$r2", R1="$r3", R2="$r4", R3="$r5", ODD_ALIGN=0
#if defined(__NDS32_EXT_IFC__) && defined(CFG_HWZOL)
		.if (1 == \ODD_ALIGN)
		pop	\R0             // R0: summy
		.endif
		popm	\R0, \R3
		mtusr	\R0, $IFC_LP
		mtusr	\R1, $LB
		mtusr	\R2, $LE
		mtusr	\R3, $LC
#else
#ifdef __NDS32_EXT_IFC__
		.if (1 == \ODD_ALIGN)
		pop	\R0
		.else
		popm	\R0, \R1        // R1: dummy
		.endif
		mtusr   \R0, $IFC_LP
#endif
#ifdef CFG_HWZOL
		.if (1 == \ODD_ALIGN)
		popm	\R0, \R2
		.else
		popm	\R0, \R3        // R3: dummy
		.endif
		mtusr	\R0, $LB
		mtusr	\R1, $LE
		mtusr	\R2, $LC
#endif
#endif
	.endm

#ifdef __NDS32_ABI_2FP_PLUS__
	/* New FPU ABI */
	.macro SAVE_FPU_REGS_00
		addi    $sp, $sp, -8
		fsdi.bi $fd2, [$sp], -8
		fsdi.bi $fd1, [$sp], -8
		fsdi    $fd0, [$sp+0]
	.endm

	.macro SAVE_FPU_REGS_01
		SAVE_FPU_REGS_00
	.endm

	.macro SAVE_FPU_REGS_02
		addi    $sp, $sp, -8
		fsdi.bi $fd15, [$sp], -8
		fsdi.bi $fd14, [$sp], -8
		fsdi.bi $fd13, [$sp], -8
		fsdi.bi $fd12, [$sp], -8
		fsdi.bi $fd11, [$sp], -8
		fsdi.bi $fd2, [$sp], -8
		fsdi.bi $fd1, [$sp], -8
		fsdi    $fd0, [$sp+0]
	.endm

	.macro SAVE_FPU_REGS_03
		addi    $sp, $sp, -8
		fsdi.bi $fd23, [$sp], -8
		fsdi.bi $fd22, [$sp], -8
		fsdi.bi $fd21, [$sp], -8
		fsdi.bi $fd20, [$sp], -8
		fsdi.bi $fd19, [$sp], -8
		fsdi.bi $fd18, [$sp], -8
		fsdi.bi $fd17, [$sp], -8
		fsdi.bi $fd16, [$sp], -8
		fsdi.bi $fd15, [$sp], -8
		fsdi.bi $fd14, [$sp], -8
		fsdi.bi $fd13, [$sp], -8
		fsdi.bi $fd12, [$sp], -8
		fsdi.bi $fd11, [$sp], -8
		fsdi.bi $fd2, [$sp], -8
		fsdi.bi $fd1, [$sp], -8
		fsdi    $fd0, [$sp+0]
	.endm
#else
	.macro SAVE_FPU_REGS_00
		addi    $sp, $sp, -8
		fsdi.bi $fd2, [$sp], -8
		fsdi.bi $fd1, [$sp], -8
		fsdi    $fd0, [$sp+0]
	.endm

	.macro SAVE_FPU_REGS_01
		addi    $sp, $sp, -8
		fsdi.bi $fd6, [$sp], -8
		fsdi.bi $fd4, [$sp], -8
		fsdi.bi $fd2, [$sp], -8
		fsdi.bi $fd1, [$sp], -8
		fsdi    $fd0, [$sp+0]
	.endm

	.macro SAVE_FPU_REGS_02
		addi    $sp, $sp, -8
		fsdi.bi $fd14, [$sp], -8
		fsdi.bi $fd12, [$sp], -8
		fsdi.bi $fd10, [$sp], -8
		fsdi.bi $fd8, [$sp], -8
		fsdi.bi $fd6, [$sp], -8
		fsdi.bi $fd4, [$sp], -8
		fsdi.bi $fd2, [$sp], -8
		fsdi.bi $fd1, [$sp], -8
		fsdi    $fd0, [$sp+0]
	.endm

	.macro SAVE_FPU_REGS_03
		addi    $sp, $sp, -8
		fsdi.bi $fd30, [$sp], -8
		fsdi.bi $fd28, [$sp], -8
		fsdi.bi $fd26, [$sp], -8
		fsdi.bi $fd24, [$sp], -8
		fsdi.bi $fd22, [$sp], -8
		fsdi.bi $fd20, [$sp], -8
		fsdi.bi $fd18, [$sp], -8
		fsdi.bi $fd16, [$sp], -8
		fsdi.bi $fd14, [$sp], -8
		fsdi.bi $fd12, [$sp], -8
		fsdi.bi $fd10, [$sp], -8
		fsdi.bi $fd8, [$sp], -8
		fsdi.bi $fd6, [$sp], -8
		fsdi.bi $fd4, [$sp], -8
		fsdi.bi $fd2, [$sp], -8
		fsdi.bi $fd1, [$sp], -8
		fsdi    $fd0, [$sp+0]
	.endm
#endif

	.macro push_fpu
#if defined(__NDS32_EXT_FPU_CONFIG_0__)
		SAVE_FPU_REGS_00
#elif defined(__NDS32_EXT_FPU_CONFIG_1__)
		SAVE_FPU_REGS_01
#elif defined(__NDS32_EXT_FPU_CONFIG_2__)
		SAVE_FPU_REGS_02
#elif defined(__NDS32_EXT_FPU_CONFIG_3__)
		SAVE_FPU_REGS_03
#else
#endif
	.endm

#ifdef __NDS32_ABI_2FP_PLUS__
        /* New FPU ABI */
	.macro RESTORE_FPU_REGS_00
		fldi.bi $fd0, [$sp], 8
		fldi.bi $fd1, [$sp], 8
		fldi.bi $fd2, [$sp], 8
	.endm

	.macro RESTORE_FPU_REGS_01
		RESTORE_FPU_REGS_00
	.endm

	.macro RESTORE_FPU_REGS_02
		fldi.bi $fd0, [$sp], 8
		fldi.bi $fd1, [$sp], 8
		fldi.bi $fd2, [$sp], 8
		fldi.bi $fd11, [$sp], 8
		fldi.bi $fd12, [$sp], 8
		fldi.bi $fd13, [$sp], 8
		fldi.bi $fd14, [$sp], 8
		fldi.bi $fd15, [$sp], 8
	.endm

	.macro RESTORE_FPU_REGS_03
		fldi.bi $fd0, [$sp], 8
		fldi.bi $fd1, [$sp], 8
		fldi.bi $fd2, [$sp], 8
		fldi.bi $fd11, [$sp], 8
		fldi.bi $fd12, [$sp], 8
		fldi.bi $fd13, [$sp], 8
		fldi.bi $fd14, [$sp], 8
		fldi.bi $fd15, [$sp], 8
		fldi.bi $fd16, [$sp], 8
		fldi.bi $fd17, [$sp], 8
		fldi.bi $fd18, [$sp], 8
		fldi.bi $fd19, [$sp], 8
		fldi.bi $fd20, [$sp], 8
		fldi.bi $fd21, [$sp], 8
		fldi.bi $fd22, [$sp], 8
		fldi.bi $fd23, [$sp], 8
	.endm
#else
	.macro RESTORE_FPU_REGS_00
		fldi.bi $fd0, [$sp], 8
		fldi.bi $fd1, [$sp], 8
		fldi.bi $fd2, [$sp], 8
	.endm

	.macro RESTORE_FPU_REGS_01
		fldi.bi $fd0, [$sp], 8
		fldi.bi $fd1, [$sp], 8
		fldi.bi $fd2, [$sp], 8
		fldi.bi $fd4, [$sp], 8
		fldi.bi $fd6, [$sp], 8
	.endm

	.macro RESTORE_FPU_REGS_02
		fldi.bi $fd0, [$sp], 8
		fldi.bi $fd1, [$sp], 8
		fldi.bi $fd2, [$sp], 8
		fldi.bi $fd4, [$sp], 8
		fldi.bi $fd6, [$sp], 8
		fldi.bi $fd8, [$sp], 8
		fldi.bi $fd10, [$sp], 8
		fldi.bi $fd12, [$sp], 8
		fldi.bi $fd14, [$sp], 8
	.endm

	.macro RESTORE_FPU_REGS_03
		fldi.bi $fd0, [$sp], 8
		fldi.bi $fd1, [$sp], 8
		fldi.bi $fd2, [$sp], 8
		fldi.bi $fd4, [$sp], 8
		fldi.bi $fd6, [$sp], 8
		fldi.bi $fd8, [$sp], 8
		fldi.bi $fd10, [$sp], 8
		fldi.bi $fd12, [$sp], 8
		fldi.bi $fd14, [$sp], 8
		fldi.bi $fd16, [$sp], 8
		fldi.bi $fd18, [$sp], 8
		fldi.bi $fd20, [$sp], 8
		fldi.bi $fd22, [$sp], 8
		fldi.bi $fd24, [$sp], 8
		fldi.bi $fd26, [$sp], 8
		fldi.bi $fd28, [$sp], 8
		fldi.bi $fd30, [$sp], 8
	.endm
#endif

	.macro pop_fpu
#if defined(__NDS32_EXT_FPU_CONFIG_0__)
		RESTORE_FPU_REGS_00
#elif defined(__NDS32_EXT_FPU_CONFIG_1__)
		RESTORE_FPU_REGS_01
#elif defined(__NDS32_EXT_FPU_CONFIG_2__)
		RESTORE_FPU_REGS_02
#elif defined(__NDS32_EXT_FPU_CONFIG_3__)
		RESTORE_FPU_REGS_03
#else
#endif
	.endm

	/*
	 * The SAVE/RESTORE macros for syscall.
	 * Since we have the syscall handler to be reentrant, it is
	 * necessary to do $IPC/$IPSW saving/restoring and descending
	 * interrupt level. Because the reentrant may go beyond the
	 * HW nested interruption transition level.
	 */
	.macro SYSCALL_SAVE_ALL
		/* push caller-saved gpr and $r6 ~ $r9, except $r0 */
		pushm   $r1, $r9                    ! $r0 for return value,
		pushm   $r15,$r30                   ! $r0 ~ $r5 are arguments, and
		                                    ! $r6 ~ $r9 are clobber-list

		/* push $IFC_LP or zol($LE, $LB, $LC) with odd word align */
		push_misc "$r6","$r7","$r8","$r9",1

		/* push $IPC(+4), $IPSW */
		mfsr    $r6, $IPC
		addi    $r6, $r6, 4
		mfsr    $r7, $IPSW
		pushm   $r6, $r7                    ! save $ipc, $ipsw

		/* Descend interrupt level */
		mfsr    $r6, $PSW
#ifdef CFG_HWZOL
		/* Also enable ZOL (PSW.AEN) */
		xori    $r6, $r6, #((1 << 13) | (1 << 1))
#else
		addi    $r6, $r6, #-2
#endif
		mtsr    $r6, $PSW
		dsb

		/* push caller-saved fpu */
		push_fpu                            ! save fpu
	.endm

	.macro SYSCALL_RESTORE_ALL
		/* pop caller-saved fpu */
		pop_fpu                 ! restore fpu

		/* pop $IPC, $IPSW */
		popm    $r6, $r7        ! restore $ipc, $ipsw
		mtsr    $r6, $IPC
		mtsr    $r7, $IPSW

		/* pop $IFC_LP or zol($LE, $LB, $LC) with odd word align */
		pop_misc "$r6","$r7","$r8","$r9",1

		/* pop caller-saved gpr and $r6 ~ $r9, except $r0 */
		popm    $r15,$r30
		popm    $r1, $r9        ! $r0 for return value
	.endm

	/*
	 * The SAVE/RESTORE macros for non-nested interrupt.
	 * For non-nested interrupt, we only need to handle the GPRs
	 * and IFC_LP/ZOL related system registers. The automatically
	 * saving/restoring will perform on $PSW/$PC because of HW
	 * interruption stack level transition feature. They are
	 * stored in $IPSW/$IPC respectively.
	 */
	.macro IRQ_SAVE_ALL             !should not touch $r0 because it has been saved already
		/* push caller-saved gpr */
		pushm   $r1, $r5
		pushm   $r15,$r30

		/* push $IFC_LP or zol($LE, $LB, $LC) */
		push_misc

		/* push caller-saved fpu */
		push_fpu
	.endm

	.macro IRQ_RESTORE_ALL
		/* pop caller-saved fpu */
		pop_fpu

		/* pop $IFC_LP or zol($LE, $LB, $LC) */
		pop_misc

		/* pop caller-saved gpr */
		popm    $r15,$r30
		popm    $r1, $r5
		pop     $r0
	.endm

	/*
         * The SAVE/RESTORE macros for nested interrupt.
	 * Since the interrupt is nested and maybe go beyond
	 * the HW nested interruption transition level, it is
	 * necessary to do $IPC/$IPSW saving/restoring and
	 * descending interrupt level.
	 */
	.macro IRQ_NESTED_SAVE_ALL      !should not touch $r0 because it has been saved already
		/* push caller-saved gpr */
		pushm	$r1, $r5
		pushm	$r15, $r30

		/* push $IFC_LP or zol($LE, $LB, $LC) */
		push_misc

		/* push $IPC, $IPSW */
		mfsr    $r1, $IPC
		mfsr    $r2, $IPSW
		pushm   $r1, $r2

		/* You can use -1 if you want to
		 * descend interrupt level and enable gie or
		 * you can enable gie when you finish your top
		 * half isr. */
		/* Descend interrupt level */
		mfsr    $r1, $PSW
#ifdef CFG_HWZOL
		/* Also enable ZOL (PSW.AEN) */
		xori    $r1, $r1, #((1 << 13) | (1 << 1))
#else
		addi    $r1, $r1, #-2
#endif
		mtsr    $r1, $PSW

		/* push caller-saved fpu */
		push_fpu
	.endm

	.macro IRQ_NESTED_RESTORE_ALL
		/* pop caller-saved fpu*/
		pop_fpu

		/* pop $IPC, $IPSW*/
		popm    $r0, $r1
		gie_disable
		mtsr    $r0, $IPC
		mtsr    $r1, $IPSW

		/* pop $IFC_LP or zol($LE, $LB, $LC) */
		pop_misc

		/* pop caller-saved gpr */
		popm    $r15,$r30
		popm    $r1, $r5
		pop     $r0
	.endm

#endif //#ifndef INTERRUPT_H
