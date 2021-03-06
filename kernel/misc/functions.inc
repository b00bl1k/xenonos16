;***
; XenonOS - Common functions
; Copyright 2007-2008 Ryabov Alexey aka Sh@dy (alex_megasoft@mail.ru)
;***
f_info	db  32 dup(0)
deel	dw 0x0000
varbuff dw 0x0000
zerow	db 0x00

DISPDEC:
    mov  BYTE [cs:zerow], 0x00
    mov  WORD [cs:varbuff], ax
    xor  ax, ax
    xor  cx, cx
    xor  dx, dx
    mov  bx, 10000
    mov  WORD [cs:deel], bx
   .mainl:
    mov  bx, WORD [cs:deel]
    mov  ax, WORD [cs:varbuff]
    xor  dx, dx
    xor  cx, cx
    div  bx
    mov  WORD [cs:varbuff], dx
    jmp .ydisp
   
   .vdisp:
    cmp  BYTE [cs:zerow], 0x00
    je .nodisp

   .ydisp:
    mov  ah, 0x0E			     ; BIOS teletype
    add  al, 48 			     ; lets make it a 0123456789 :D
    mov  bx, 1 
    int  0x10				     ; invoke BIOS
    mov  BYTE [cs:zerow], 0x01
   jmp .yydis

   .nodisp:

   .yydis:
    xor  dx, dx
    xor  cx, cx
    xor  bx, bx
    mov  ax, WORD [cs:deel]
    cmp  ax, 1
    je .bver
    cmp  ax, 0
    je .bver
    mov  bx, 10
    div  bx
    mov  WORD [cs:deel], ax
   jmp .mainl

   .bver:
   ret




;***
; ����� ������ �������� �����
;***
; in:  ds:si=������
;      cx=������ ������
; out: cx=0
;      si=si+cx
write:
  push ax
 .wr:
  mov  al,[si]
  mov  ah,0eh
  int  10h
  inc  si
  loop .wr
  pop  ax
  ret

;***
; �������� ������� �������
;***
; out: dl-y
;      dh-x
get_cursor_pos:
  push ax bx cx
  mov  ah,03
  mov  bh,00
  int  10h
  pop  cx bx ax
  ret

;***
; ���������� ������� �������
;***
; in: dl-y
;     dh-x
set_cursor_pos:
  push ax bx cx
  mov  ah,02
  mov  bh,00
  int  10h
  pop  cx bx ax
  ret

;***
; ������� ������ � ������� �������
;***
; in:  ds:di=������
; out: ds:di=������
str_ucase:
  push di ax
us0:
  mov  al,[ds:di]
  or   al,al
  je   us1
  cmp  al,'a'	      ; "a"
  jc   us2
  cmp  al,'z'	      ; "z"
  ja   us2
us00:
  sub  al,20h
  mov  [ds:di],al
us01:
  inc  di
  jmp  us0
us2:
  cmp  al,0A0h	      ; "a" �������
  jc   us3
  cmp  al,0AFh	      ; "�" �������
  ja   us3
  jmp  us00
us3:
  cmp  al,0E0h
  jc   us01
  cmp  al,0EFh
  ja   us01
  sub  al,'�'-'�'
  mov  [ds:di],al
  jmp  us01
us1:
  pop  ax di
  ret

;***
; ����� hex �����
;***
hex_digits	db '0123456789ABCDEF'
write_hex_byte:
  mov  ah,0eh
  xor  bx,bx
  shld di,cx,12
  and  di,15
  mov  al,[cs:hex_digits + di]
  int  10h
  mov  di,cx
  and  di,15
  mov  al,[cs:hex_digits + di]
  int  10h
  ret

;***
; ��������� ������ null-term ������
;***
;in:  ds:si=������
;out: cx=������ ������
;     ds:si=��������� �� ����� ������
len:
  push ax
  xor  cx,cx
.loop:
  mov  al,byte [si]
  inc  si
  cmp  al,00h
  je   .end
  inc  cx
  jmp  .loop
.end:
  pop  ax
  ret

;***
; ������ ������� ������
;***
; in:  ds:si=null-term ������
;      cx=������ ������
; out: ds:si=��������� null-term ������
rtrim:
  push di cx ax si
  dec  cx
  mov  di,si
.lp:
  mov  si,di
  add  si,cx   
  mov  al,byte [si]
  cmp  al,20h
  jne  .end
  mov  byte [si],00h
  dec  cx
  jmp  .lp  
.end:
  pop  si ax cx di
  ret