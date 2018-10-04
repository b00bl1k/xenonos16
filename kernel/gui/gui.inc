;***
; XenonOS - Gui of XenonOS
; Copyright 2007-2008 Ryabov Alexey aka Sh@dy (alex_megasoft@mail.ru)
;***

; WINDOW MANAGER
;**************************************
;  Менеджер окон очень  прост.  На каждое окно отводится
;структура в 32 байта. Ее описание можно изучить в файле
;misc/struct.inc. При  каждом событии,  окну  передается
;сообщение, в котором указывается тип события  и  данные
;(если нужно).
;  Всего можно  создать 256 окон:  255 для  пользователя
;и  1 для системы  (desktop).  Каждое  окно  имеет  свой
;уникальный ID, который никогда не повторяется (hwnd).

; MOUSE
;**************************************
;  Для нормальной работы мыши нужен драйвер.По-умолчанию
;грузится /fd/xenon/mousedrv.com, но вы  можете заменить
;его другим драйвером мыши для MS-DOS.

include 'font.inc'
include 'mouse.inc'
include 'skins.inc'
include 'windows.inc'
include 'user_video.inc'

__sys_gui:
  cmp  al,00h
  je   __gui_getmousepos
  cmp  al,01h
  je   __gui_setmousepos
  cmp  al,02h
  je   __gui_get_screen
  cmp  al,03h
  je   __gui_create_window
  cmp  al,04h
  je   __gui_send_message
  cmp  al,05h
  je   __gui_check_message
  cmp  al,06h
  je   __gui_get_message
  cmp  al,07h
  je   __gui_set_pixel
  cmp  al,08h
  je   __gui_draw_bar
  cmp  al,09h
  je   __gui_draw_char
  cmp  al,0ah
  je   __gui_draw_string
  iret


__gui_getmousepos:
   mov	cx,[cs:mouse_x]
   mov	dx,[cs:mouse_y]
  iret

__gui_setmousepos:
   ;Ф-ия чёта не работает :(
   pusha
    ;mov  [cs:mouse_x],cx
    ;mov  [cs:mouse_y],dx
    mov  al,4
    int  33h
    call show_mouse_grph
   popa
  iret

__gui_get_screen:
   mov	bx,[gs:SCR_WIDTH]
   mov	cx,[gs:SCR_HEIGHT]
   mov	dx,[gs:SCR_BPP]
  iret

__gui_create_window:
  push ecx edx edi esi ebp
   mov	eax,ebx
   mov	ebx,ecx
   mov	cx,dx
   mov	dx,di
   call create_window
  pop  ebp esi edi edx ecx

__gui_send_message:
  call send_message
  iret

__gui_check_message:
  call check_message
  iret

__gui_get_message:
  call get_message
  iret

__gui_set_pixel:
  call user_put_pixel
  iret

__gui_draw_bar:
  call user_draw_bar
  iret

__gui_draw_char:
  call user_draw_char
  iret

__gui_draw_string:
  call user_draw_string
  iret