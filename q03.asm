    org 0x7c00
    bits 16
    mov ax, 0
    mov ds, ax
    mov dx, 0
    mov ax, 0x7e00
    call gets
    call prints
    jmp fim

gets:
    mov di, ax
.loop2: 
    mov ah, 0 
    int 0x16
    cmp al, 13
    je .ret2
    mov [ds:di], al
    inc di
    inc dx
    mov ah, 0x0e
    int 0x10
    jmp .loop2

.ret2:  
    mov ah, 0x0e    
    int 0x10
    mov al, 10
    int 0x10
    mov [ds:di], byte 0     
    ret

prints: 
    mov ah, 0x0e
    dec di

.loop1: 
    mov al, [ds:di]
    int 0x10
    dec di
    dec dx
    jz fim
    jmp .loop1 

fim:    
    hlt

    times 510 - ($-$$) db 0
    dw 0xaa55
