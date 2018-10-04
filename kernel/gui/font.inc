;***
; XenonOS - Graphical font
; Copyright 2007-2008 Ryabov Alexey aka Sh@dy (alex_megasoft@mail.ru)
;***

;***
; Вывод буквы
;***
;in: al - char
;    bx - x
;    cx - y
;    edx - color (0x00RRGGBB)
draw_char:
  pusha
  push es
  xor  ah,ah

  push edx
   mov	dx,word [gs:FONT_SEG]
   mov	es,dx
   imul ax,9
   mov	di,ax
  pop  edx

 mov  bp,9
 .loop_1:
  mov  al,byte [es:di]
  push bx
  .loop_2:
   sar al,1
   jnc @f
    call put_pixel
   @@:
   inc	bx
   cmp	al,0
   jne	.loop_2
  pop  bx
  inc  cx
  inc  di
  dec  bp
  cmp  bp,0
  jne  .loop_1

  pop  es
  popa
  ret

;***
; Вывод строки
;***
;in: ds:si - адрес строки
;    bx - x
;    cx - y
;    edx - color (0x00RRGGBB)
draw_string:
  push ax si bx
@@:
  lodsb
  cmp  al,00h
  je   .quit
  call draw_char
  add  bx,6
  jmp  @b
.quit:
  pop  bx si ax
  ret