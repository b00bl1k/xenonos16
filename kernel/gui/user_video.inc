;***
; XenonOS - User video functions
; Copyright 2007-2008 Ryabov Alexey aka Sh@dy (alex_megasoft@mail.ru)
;***

;in:  ebx=hwnd
;     ecx=x*65536+y
;     edx=color
ms_disabled  db  0
user_put_pixel:
  pusha32
   mov	eax,ebx
   mov	ebx,ecx
   shr	ebx,16	   ;bx=x
   and	ecx,0xffff ;cx=y

   push cx
    mov  edi,WINDOW_BASE
    add  edi,WINDOW_size
    mov  cx,255
  @@:
    cmp  [gs:edi+WINDOW.hwnd],eax
    je	 @f
    add  edi,WINDOW_size
  loop @b
   pop	cx
   popa32
   mov	ax,GUI_INVALID_HANDLE
   ret
  @@:
   pop	cx

   add	bx,[gs:edi+WINDOW.x]
   add	cx,[gs:edi+WINDOW.y]

  push bx cx edx
   sub	edi,WINDOW_BASE
   shr	edi,5
   mov	ax,di

   push ax
    xor  edx,edx
    mov  esi,DISPLAY_INFO
    and  ebx,0ffffh
    and  ecx,0ffffh
    xor  eax,eax
    mov  ax,[gs:SCR_WIDTH]
    mul  ecx
    add  eax,ebx
    add  esi,eax
   pop	ax

   cmp	byte [gs:esi],al
   je	.ok
    pop  edx cx bx
    popa32
    xor  ax,ax
    ret
  .ok:
  pop  edx cx bx

  mov  ax,[cs:mouse_x]
 .check_x:
  cmp  bx,ax
  jl   .check_ok
 .check_width:
  add  ax,cursor_width
  cmp  bx,ax
  jg   .check_ok
 .check_y:
  mov  ax,[cs:mouse_y]
  cmp  cx,ax
  jl   .check_ok
 .check_height:
  add  ax,cursor_height
  cmp  cx,ax
  jg   .check_ok
  push bx cx edx
   call hide_mouse
   mov	[cs:ms_disabled],1
  pop  edx cx bx
 .check_ok:

   call put_pixel
 @@:
  cmp  [cs:ms_disabled],1
  jne  @f
  call show_mouse
  mov  [cs:ms_disabled],0
 @@:
  ;xor  ax,ax
  popa32
  ret


; ebx=hwnd
; ecx=x*65536+width
; edx=y*65536+height
; esi=color
user_draw_bar:
  pusha32

   push ebx ;14
   push ecx ;10
   push edx ;6
   push esi ;2
   push bp  ;0

   mov	bp,sp
   mov	cx,[bp+8] ;y
   .loop_y:
     mov  bx,[bp+12] ;x
     .loop_x:
       push cx bx
	xchg bx,cx
	shl  ecx,16
	mov  cx,bx
	mov  edx,[bp+2]
	mov  ebx,[bp+14]
	call user_put_pixel
       pop  bx cx
       inc  bx
       mov  ax,bx
       sub  ax,[bp+12] ;x
       cmp  ax,[bp+10] ;width
     jne  .loop_x
     inc  cx
     mov  ax,cx
     sub  ax,[bp+8] ;y
     cmp  ax,[bp+6] ;height
   jne	.loop_y

   pop	bp
   pop	esi
   pop	edx
   pop	ecx
   pop	ebx

  popa32
  ret


;ebx=hwnd
;ecx=x*65536+y
;dl=char
;esi=color
user_draw_char:
  pusha32
  push es
  xor  dh,dh

  mov  ax,word [gs:FONT_SEG]
  mov  es,ax
  imul dx,9
  mov  di,dx
  mov  edx,esi

  mov  bp,9
 .loop_y:
  mov  al,byte [es:di]
  push ecx
  .loop_x:
    sar  al,1
    jnc @f
     call user_put_pixel
    @@:
    add  ecx,10000h
    cmp  al,0
   jne .loop_x
  pop  ecx
  inc  cx
  inc  di
  dec  bp
  cmp  bp,0
  jne  .loop_y

  pop  es
  popa32
  ret

;ebx=hwnd
;ecx=x*65536+y
;ds:di=string
;esi=color
user_draw_string:
  pusha32
@@:
  mov  dl,[di]
  inc  di
  cmp  dl,00h
  je   @f
  call user_draw_char
  add  ecx,60000h
  jmp  @b
@@:
  popa32
  ret