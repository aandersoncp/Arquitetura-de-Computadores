    org 0x7c00
    bits 16

    mov ax, 0
    mov ds, ax
    cli 

    mov al, 0x13
    int 0x10
     
    mov ax, 0xA000
    mov es, ax
        
    mov al, 255 ;Primeira cor

inicio:
    mov cx, 64000
    mov di, 0

loop:
    mov [es:di], al 
    inc di
    dec cx
    jz inicio   
    dec al
    jmp loop

    hlt

    times 510 - ($-$$) db 0
    dw 0xaa55
