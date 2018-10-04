  org  100h

  include '..\consts.inc'
__start:
  ;вычисляем...

  ;Создаем окно
  mov  ax,0b03h
  mov  ebx,15*65536+200
  mov  ecx,29*65536+100
  mov  dx,GWF_MOVABLE or GWF_VISIBLE or GWF_RESIZABLE or GWF_SKINED
  mov  si,title
  int  80h
  mov  [hwnd1],ebx

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
  mov  ecx,2*65536+120
  mov  edx,14*65536+10
  mov  esi,0x888888
  int  80h

  mov  ax,0b0ah
  mov  ebx,[hwnd1]
  mov  ecx,2*65536+14
  mov  di,buffer
  mov  esi,0xff0000
  int  80h
  jmp  main_loop

key:
  ;dh=scan code dl=ascii code
  cmp  dh,1
  je   @f
  cmp  dh,0eh
  je   back_spc

  cmp  dl,0
  je   main_loop
  cmp  [position],20
  je   main_loop
  mov  di,buffer
  add  di,[position]
  mov  [di],dl
  inc  [position]
  jmp  redraw
@@:
  ;выход
  ret
back_spc:
  cmp  [position],0
  je   main_loop
  mov  di,buffer
  add  di,[position]
  mov  byte [di-1],'_'
  dec  [position]
  jmp  redraw


  title 	 db  'Keyboarder (ESC-Quit)',0
  hwnd1 	 dd  ?

  position	 dw  0
  buffer	 db  20 dup('_')
		 db  0
		 db  'asd',0
