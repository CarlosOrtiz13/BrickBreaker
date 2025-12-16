section .asm

extern int21h_handler
extern no_interrupt_handler

global int21h
global idt_load
global no_interrupt
global enable_interrupts
global disable_interrupts

enable_interrupts:
    sti
    ret

disable_interrupts:
    cli
    ret


idt_load:
    mov eax, [esp+4]     ; pointer to struct idtr_desc
    lidt [eax]           ; load 6 bytes: limit (2) + base (4)
    ret


int21h:
    cli
    pushad
    call int21h_handler
    popad
    iret

no_interrupt:
    cli
    pushad
    call no_interrupt_handler
    mov al, 0x20
    out 0x20, al
    popad
    iret

; ADDED: Timer interrupt handler (IRQ0 = interrupt 0x20)
extern timer_handler

global irq0_handler
irq0_handler:
    cli
    pushad
    call timer_handler
    mov al, 0x20
    out 0x20, al
    popad
    iret

global exception_halt
exception_halt:
    cli
.hang:
    hlt
    jmp .hang


    