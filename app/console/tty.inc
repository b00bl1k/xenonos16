std_buff_seg	dw   0
std_buff_pos	dw   0


tty_init:
  mov  ax,0300h
  mov  bx,80*25
  int  80h
  cmp  ax,CM_NO_ERRORS
  jne  @f
  mov  [std_buff_seg],bx
  mov  word [std_buff_pos],0
  clc
  ret
 @@:
  stc
  ret


tty_free:
  ret


;dh,dl=x,y
tty_set_cursor:
  cmp  dh,80
  jg   @f
  cmp  dh,25
  jg   @f
  pusha
  xor  ah,ah
  mov  al,dl
  mov  bx,80
  mul  bx
  mov  dl,dh
  xor  dh,dh
  add  dx,ax
  mov  [std_buff_pos],dx
  popa
 @@:
  ret

;in:  bl=char code
tty_put_char:
  pusha
  push es
  ;Перевод строки
  cmp  bl,13 ;CR?
  jne  .simple_char
  add  word [std_buff_pos],80
  xor  dx,dx
  mov  ax,[std_buff_pos]
  mov  bx,80
  div  bx
  sub  [std_buff_pos],dx
  jmp  .done_put
.simple_char:
  ;Обычный символ
  mov  ax,[std_buff_seg]
  mov  es,ax
  mov  di,[std_buff_pos]
  mov  [es:di],bl
  inc  word [std_buff_pos]
.done_put:
  cmp  word [std_buff_pos],80*25
  jle  .erroff
  ;Если курсор вышел за границу - сдвинем экран вверх на 1 строку
  mov  cx,80
 @@:
  mov  di,cx
  mov  al,[es:di]
  mov  [es:di-80],al
  inc  cx
  cmp  cx,80*25+80
  jle  @b
  sub  word [std_buff_pos],80
.erroff:
  pop  es
  popa
  ret