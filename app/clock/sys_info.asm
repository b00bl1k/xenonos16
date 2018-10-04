  org  100h

  counter_ticks  equ  5000

  include '..\consts.inc'
__start:

  ;Создаем окно
  mov  ax,0b03h
  mov  ebx,235*65536+200
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
  je   get_time

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
  jmp  get_time

key:
  cmp  dh,1
  jne  main_loop
  ret

get_time:
  mov  ah,02h	       ;Узнаем время RTC
  int  1Ah
  jc   main_loop
  dec  [counter]
  jnz  main_loop
  mov  [counter],counter_ticks

  mov  di,lbl1_data
  call bcd2asc
  mov  byte [di+0],ah
  mov  byte [di+1],al
  mov  al,cl
  call bcd2asc
  mov  byte [di+3],ah
  mov  byte [di+4],al
  mov  al,dh
  call bcd2asc
  mov  byte [di+6],ah
  mov  byte [di+7],al

  mov  ax,0b08h
  mov  ebx,[hwnd1]
  mov  ecx,3*65536+84
  mov  edx,15*65536+9
  mov  esi,0x888888
  int  80h

  mov  ax,0b0ah
  mov  ebx,[hwnd1]
  mov  ecx,3*65536+15
  mov  di,label1
  mov  esi,0xffffff
  int  80h

  mov  ah,0ch
  int  80h
  jmp  main_loop

bcd2asc:
  mov  ah,al
  and  al,0Fh			   ;оставить младшие 4 бита в AL
  shr  ah,4			   ;сдвинуть старшие 4 бита в AH
  or   ax,3030h 		   ;преобразовать в ASCII-число
  ret

  title 	 db  'World clock :)',0
  label1	 db  'Time: '
   lbl1_data	 db  'HH:MM:SS',0

  hwnd1 	 dd  ?
  counter	 dw  1