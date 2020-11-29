use16
org 0x7C00
start:
mov ax,cs
mov ds,ax
mov ss,ax
mov sp, start
chosing_mode:
mov ah,0x0e
mov al, 'P'
int 0x10
mov al, 'R'
int 0x10
mov al, 'E'
int 0x10
mov al, 'S'
int 0x10
int 0x10
mov al, ':' 
int 0x10
mov al, ' '
int 0x10

moduls:
green:
mov ah,0x0e
mov al, '1'
int 0x10
mov al, '-'
int 0x10
mov al, 'g'
int 0x10
mov al, 'r'
int 0x10
mov al, 'e'
int 0x10
int 0x10
mov al, 'n'
int 0x10
mov al, ' '
int 0x10

blue:
mov al, '2'
int 0x10
mov al, '-'
int 0x10
mov al, 'b'
int 0x10
mov al, 'l'
int 0x10
mov al, 'u'
int 0x10
mov al, 'e'
int 0x10
mov al, ' '
int 0x10

red:
mov al, '3'
int 0x10
mov al, '-'
int 0x10
mov al, 'r'
int 0x10
mov al, 'e'
int 0x10
mov al, 'd'
int 0x10
mov al, ' '
int 0x10

yellow:
mov al, '4'
int 0x10
mov al, '-'
int 0x10
mov al, 'y'
int 0x10
mov al, 'e'
int 0x10
mov al, 'l'
int 0x10
int 0x10
mov al, 'o'
int 0x10
mov al, 'w'
int 0x10
mov al, ' '
int 0x10

grey:
mov al, '5'
int 0x10
mov al, '-'
int 0x10
mov al, 'g'
int 0x10
mov al, 'r'
int 0x10
mov al, 'e'
int 0x10
mov al, 'y'
int 0x10
mov al, ' '
int 0x10


white:
mov al, '6'
int 0x10
mov al, '-'
int 0x10
mov al, 'w'
int 0x10
mov al, 'h'
int 0x10
mov al, 'i'
int 0x10
mov al, 't'
int 0x10
mov al, 'e'
int 0x10
mov al, ' '
int 0x10

choosing_loop:
mov ah,0x00
int 0x16
cmp al, '1'
je exit_choose
cmp al, '2'
je exit_choose
cmp al, '3'
je exit_choose
cmp al, '4'
je exit_choose
cmp al, '5'
je exit_choose
cmp al, '6'
je exit_choose

jne choosing_loop

exit_choose:
mov [0x9000],al ;диапазон загрузки 0ч7000..0ч9000

mov ax,0x1000 ;load the kernel code to this address
mov es,ax
mov bx,0x0000
mov dl,1
mov dh,0	;head
mov ch,0	;cilindr
mov cl,1	;sector
mov al,18
mov ah,0x02
int 0x13

mov bx,0x2400
mov dl,1
mov dh,1	;head
mov ch,0	;cilindr
mov cl,1	;sector
mov al,18
mov ah,0x02
int 0x13



cli		;switch off the interrupts

lgdt [gdt_info]	;descriptor table

in al,0x92	;switch on A20
or al,2
out 0x92,al

mov eax,cr0	;Set protected mode
or al,1
mov cr0, eax

jmp 0x8:protected_mode

use32
protected_mode:
mov ax,0x10
mov es,ax
mov ds,ax
mov ss,ax
call 0x10000

gdt:
db 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
db 0xff, 0xff, 0x00, 0x00, 0x00, 0x9A, 0xCF, 0x00
db 0xff, 0xff, 0x00, 0x00, 0x00, 0x92, 0xCF, 0x00
gdt_info:
dw gdt_info - gdt
dw gdt, 0

loading_str:
db "Loading...",0

times (512 - ($ - start) - 2) db 0
db 0x55, 0xAA












































