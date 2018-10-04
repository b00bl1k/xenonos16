  org  100h
  jmp  __start
  include '..\consts.inc'
  include 'tty.inc'
__start:
  ;вычисляем...

  ;Создаем окно
  mov  ax,0b03h
  mov  ebx,20*65536+2+480+2
  mov  ecx,20*65536+14+225+2
  mov  dx,GWF_MOVABLE or GWF_VISIBLE or GWF_RESIZABLE or GWF_SKINED
  mov  si,title
  int  80h

  mov  [hwnd1],ebx

  call tty_init
  mov  dl,'a'
  call tty_put_char

  ;Цикл обработки сообщений окна
 main_loop:

  mov  ebx,[hwnd1]
  mov  ax,0b05h
  int  80h
  cmp  ax,0
  jne  translate_msg
  mov  ah,0ch
  int  80h
  jmp  main_loop

  ;Обработаем сообщение
 translate_msg:
  mov  ebx,[hwnd1]
  mov  ax,0b06h
  int  80h
  cmp  cx,101
  je   redraw
  cmp  cx,102
  je   key
  jmp  main_loop

redraw:
  mov  ax,0b08h
  mov  ebx,[hwnd1]
  mov  ecx,2*65536+480
  mov  edx,14*65536+225
  mov  esi,0x000000
  int  80h

  push [std_buff_seg]
  pop  es

  mov  ecx,14
  xor  di,di
 lp_y:
   and	ecx,0xffff
   add	ecx,2*65536
   lp_x:
     mov  ax,0b09h
     mov  ebx,[hwnd1]
     mov  dl,[es:di]
     mov  esi,0xffffff
     int  80h
     inc  di
     add  ecx,0x60000
     cmp  ecx,(480+2)*65536
   jle	lp_x
   add	cx,9
   cmp	cx,14+225
 jle  lp_y

  jmp  main_loop

key:
  ;dh=scan code dl=ascii code
  mov  bl,dl
  call tty_put_char
  jmp  redraw

  title 	 db  'MS-DOS Window :)',0
  hwnd1 	 dd  ?
