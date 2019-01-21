    .extern _end
    .global _sbrk
    .extern __fence__

    .text
    .equ    HEAP_BARRIER    , 128

    .align  4
_sbrk:
    l.w     $r1, heap_end        ! get addr of heap top
    bnez    $r1, L0             ! is it initialized?
        ! Note: leave 1024 byte to do low memory action.
    la      $r1, _end + HEAP_BARRIER    ! no, init it. 
    s.w     $r1, heap_end        ! save it

L0:
    addi    $r0, $r0, 7         ! heap is aligned on 8-byte boundary
    srli    $r0, $r0, 3
    slli    $r0, $r0, 3

    add     $r0, $r1, $r0        ! r0 is new heap top
    la      $r2, __fence__          ! load __fence__ address to $r2
    !slt    $r2, $sp, $r0       ! compare heap top and stack top
    slt     $r2, $r2, $r0        ! compare heap top and stack end. (__fence__ address)
    beqz    $r2, L1             ! collision?

    movi    $r0, -1             ! Set Return Value (-1), can't alloc memory
    ret

L1:
    s.w     $r0, heap_end        ! no collision, save new heap top

    addi    $r0, $r1, 0         ! return old heap top

    ret

    .section    .bss
    .align  2
heap_end:
    .word   0                  ! put 4 bytes in bss section.

    .end
