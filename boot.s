.section .reset, "ax"
.org 0

// vetor de interrupções (deve estar em 0x000000000)
ldr pc, reset_addr
ldr pc, undefined_addr
ldr pc, swi_addr
ldr pc, abort1_addr
ldr pc, abort2_addr
nop
ldr pc, irq_addr
ldr pc, fiq_addr

reset_addr: .word _reset
undefined_addr: .word _undefined
swi_addr:   .word _swi
abort1_addr:   .word _abort1
abort2_addr:   .word _abort2
irq_addr:   .word _irq
fiq_addr:   .word _fiq

.text
_reset:
   // Configura sp do modo SVR:
   mov r0, #0b10011
   msr cpsr, r0
   ldr sp, =stack_svr

   // Zera segmento .bss:
   mov r0, #0
   ldr r1, =start_bss
   ldr r2, =end_bss
loop_zera:
   cmp r1, r2
   bge start
   str r0, [r1], #4
   b loop_zera

_undefined:
    mov r0, #0x11
    swi 10
    movs pc, lr

_swi:
    mov r0, #0x22
    @.word 0xF10FFF9F
    @ldr r4, [r3]
    movs pc, lr

_abort1:
    mov r0, #0x33
    @.word 0xF10FFF9F
    @swi 10
    movs pc, lr

_abort2:
    mov r0, #0x44
    bkpt
    sub lr, lr, #4
    movs pc, lr

_irq:
    mov r0, #0x55
    b _irq

_fiq:
    mov r0, #0x66
    b _fiq

start:
    ldr r0, =start_heap
    ldr r1, =end_heap
    mov r2, #0
    bl initialize
    bl main

