#include <stdio.h>
#include <stdint.h>

typedef struct {
    unsigned long r4;
    unsigned long r5;
    unsigned long r6;
    unsigned long r7;
    unsigned long r8;
    unsigned long r9;
    unsigned long r10;
    unsigned long r11;
    unsigned long r0;
    unsigned long r1;
    unsigned long r2;
    unsigned long r3;
    unsigned long r12;
    unsigned long lr;
    unsigned long pc;
    unsigned long cpsr;
} contex_frame_t;

void invalid_svcall_handler(contex_frame_t *cf, int svcall_no)
{

    printf("invalid svcall: no %d\r\n", svcall_no);

    printf("=========== Registers ===========\r\n");
    printf(" r0 0x%08lx r1 0x%08lx  r2 0x%08lx   r3 0x%08lx\r\n",
           cf->r0, cf->r1, cf->r2, cf->r3);
    printf(" r4 0x%08lx r5 0x%08lx  r6 0x%08lx   r7 0x%08lx\r\n",
           cf->r4, cf->r5, cf->r6, cf->r7);
    printf(" r8 0x%08lx r9 0x%08lx r10 0x%08lx  r11 0x%08lx\r\n",
           cf->r8, cf->r9, cf->r10, cf->r11);
    printf("r12 0x%08lx lr 0x%08lx  pc 0x%08lx cpsr 0x%08lx\r\n",
           cf->r12, cf->lr, cf->pc, cf->cpsr);

    while (1);
}
extern const void * syscall_tbl[];
void invalid_syscall_handler(int syscall_no, void *syscall_arg, void *pc)
{

    unsigned long *addr = syscall_arg;

    printf("invalid syscall: no %d, pc 0x%08lx,addr:0x%x,paddr:0x%x\r\n",
           syscall_no, (unsigned long)pc, (uint32_t)syscall_tbl[syscall_no],
           (uint32_t)&(syscall_tbl[syscall_no]));

    printf("====== Dump syscall arg mem ======\r\n");

    for (int i = 0; i < 20; i++) {
        // return
        if ((i != 0) && (i % 4 == 0)) {
            printf("\r\n");
        }

        // print addr
        if ((i == 0) || (i % 4 == 0)) {
            printf("0x%08lx:", (unsigned long)(addr));
        }

        printf(" 0x%08lx", *addr);
        addr++;
    }

    while (1);
}

