section .asm
global gdt_load

; void gdt_load(void* gdt, uint16_t size)
gdt_load:
    mov eax, [esp+4]            ; base
    mov [gdt_descriptor + 2], eax

    mov ax,  [esp+8]            ; size
    dec ax                      ; limit = size - 1
    mov [gdt_descriptor], ax

    lgdt [gdt_descriptor]

    ; Reload data segments (kernel data selector = 0x10)
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax

    ; Far jump to reload CS (kernel code selector = 0x08)
    jmp 0x08:flush_cs

flush_cs:
    ret

section .data
gdt_descriptor:
    dw 0x00
    dd 0x00
