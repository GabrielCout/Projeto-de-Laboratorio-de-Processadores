.section .reset, "ax"
.org 0

// vettor de interrupções (deve estar em 0x000000000)
b _reset
b _undefined
b _swi
b _abort1
b _reset
nop
b _irq
b _fiq

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
    bl initialize

