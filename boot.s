.section .reset, "ax"
.org 0

// vettor de interrupções (deve estar em 0x000000000)
b _reset
b _undefined
b _swi
b _abort1
b _abort2
nop
b _irq
b _fiq

_reset:
    b main

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

