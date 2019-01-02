	.section .text
	.globl main


main: 
    add t0,zero,zero
1:  addi t0,t0, 1
    j 1b
