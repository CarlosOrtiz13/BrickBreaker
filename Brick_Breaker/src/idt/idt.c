#include "idt.h"
#include "config.h"
#include "kernel.h"
#include "memory/memory.h"
#include "io/io.h"
struct idt_desc idt_descriptors[PEACHOS_TOTAL_INTERRUPTS];
struct idtr_desc idtr_descriptor;

extern void idt_load(struct idtr_desc* ptr);
extern void int21h();
extern void no_interrupt();
extern void irq0_handler();  // ADDED: Timer handler from idt.asm
extern void keyboard_handler();  // ADDED: From keyboard.c
extern void exception_halt();

void int21h_handler()
{
    keyboard_handler();  // CHANGED: Call proper keyboard handler
}

void no_interrupt_handler()
{
    outb(0x20, 0x20);
}

void idt_zero()
{
    while(1) {}
}

void idt_set(int interrupt_no, void* address)
{
    struct idt_desc* desc = &idt_descriptors[interrupt_no];
    desc->offset_1 = (uint32_t) address & 0x0000ffff;
    desc->selector = KERNEL_CODE_SELECTOR;
    desc->zero = 0x00;
    desc->type_attr = 0x8E;
    desc->offset_2 = (uint32_t) address >> 16;
}

static volatile uint8_t* const vga = (volatile uint8_t*)0xA0000;

static inline void vga_fill(uint8_t color)
{
    for (int i = 0; i < 64000; i++)
        vga[i] = color;
}

void idt_init(void)
{
    disable_interrupts();
    
    outb(0x21, 0xFF);
    outb(0xA1, 0xFF);
    
    memset(idt_descriptors, 0, sizeof(idt_descriptors));
    
    idtr_descriptor.limit = sizeof(idt_descriptors) - 1;
    idtr_descriptor.base = (uint32_t)idt_descriptors;
    
    for (int i = 0; i < PEACHOS_TOTAL_INTERRUPTS; i++)
        idt_set(i, no_interrupt);
    
    idt_set(0, idt_zero);
    idt_set(0x20, irq0_handler);
    idt_set(0x21, int21h);
    
    idt_load(&idtr_descriptor);
}