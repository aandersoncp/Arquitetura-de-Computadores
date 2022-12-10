    org 0x7c00
    bits 16

    mov ax, 0
    mov ds, ax
    cli 

    mov al, 0x13
    int 0x10
        
    mov ax, 0xA000
    mov es, ax

inicio:
	mov cx, 64000
	mov di, 0
	mov ah, 0
	int 0x16

loop:
	mov [es:di], al
	inc di
	dec cx
	jz inicio	
	jmp loop

	hlt
	
	times 510 - ($-$$) db 0
	dw 0xaa55
