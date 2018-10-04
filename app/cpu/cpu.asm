  org  100h

  include '..\consts.inc'

  BASE_CONST   equ     100000h
  TASK_COUNT   equ     (BASE_CONST+0100h)
  TASK_BASE    equ     (BASE_CONST+0110h)


__start:

  ;Создаем окно
  mov  ax,0b03h
  mov  ebx,35*65536+300
  mov  ecx,59*65536+150
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
  jmp  main_loop

key:
  cmp  dh,1
  jne  main_loop
  ;выход
  ret


  title 	 db  'CPU',0
  hwnd1 	 dd  ?

  label1
   lbl1_data1	 db  '             '
   lbl1_data2	 db  '             ',0
