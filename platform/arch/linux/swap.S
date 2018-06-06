#include "swap.h"

   .globl swapcontext_safe;
   .type  swapcontext_safe,@function;
   .align 16;
    swapcontext_safe:.cfi_startproc;

	movl	4(%esp), %eax
	movl	$0, EAX_x86(%eax)
	movl	%ecx, ECX_x86(%eax)
	movl	%edx, EDX_x86(%eax)
	movl	%edi, EDI_x86(%eax)
	movl	%esi, ESI_x86(%eax)
	movl	%ebp, EBP_x86(%eax)
	movl	(%esp), %ecx
	movl	%ecx, EIP_x86(%eax)
	leal	4(%esp), %ecx
	movl	%ecx, ESP_x86(%eax)
	movl	%ebx, EBX_x86(%eax)

	xorl	%edx, %edx
	movw	%fs, %dx
	movl	%edx, FS_x86(%eax)

	leal	FPREGSMEM(%eax), %ecx
	movl	%ecx, FPREGS(%eax)

	fnstenv	(%ecx)

	movl	8(%esp), %ecx

	pushl	%ebx
	leal	SIGMASK(%eax), %edx
	leal	SIGMASK(%ecx), %ecx
	movl	$SIG_SETMASK, %ebx
	movl	$0x7e, %eax
	int     $0x80
	popl	%ebx
	cmpl	$-4095, %eax
	jae	syscall_error_label

	movl	8(%esp), %eax

	movl	FPREGS(%eax), %ecx
	fldenv	(%ecx)

	movl	FS_x86(%eax), %edx
	movw	%dx, %fs

	movl	EIP_x86(%eax), %ecx

	movl	ESP_x86(%eax), %esp

	pushl	%ecx

	movl	EDI_x86(%eax), %edi
	movl	ESI_x86(%eax), %esi
	movl	EBP_x86(%eax), %ebp
	movl	EBX_x86(%eax), %ebx
	movl	EDX_x86(%eax), %edx
	movl	ECX_x86(%eax), %ecx
	movl	EAX_x86(%eax), %eax

    jmp    unlock_spin

	ret

   .cfi_endproc;
   .size swapcontext_safe,.-swapcontext_safe;

