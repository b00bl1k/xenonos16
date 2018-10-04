;***
; XenonOS - Windows
; Copyright 2007-2008 Ryabov Alexey aka Sh@dy (alex_megasoft@mail.ru)
;***

cur_hwnd  dd  1

;***
; Создать окно
;***
;in:  eax=x*65535+ширина
;     ebx=y*65535+высота
;     cx=флаги
;     ds:dx=адрес обработчика событий
;     ds:si=заголовок окна
create_window:
  push cx
   mov	edi,WINDOW_BASE
   add	edi,WINDOW_size    ;типа 1 окно зарезервировано
   mov	cx,255
 @@:
   cmp	[gs:edi+WINDOW.hwnd],0	;hwnd
   je	@f
   add	edi,WINDOW_size
 loop @b
  pop  cx
  mov  ax,GUI_WINDOWS_OVERFLOW
  ret
 @@:
  pop  cx

  mov  dword [gs:edi+WINDOW.width],eax
  mov  dword [gs:edi+WINDOW.height],ebx
  mov  eax,[cs:cur_hwnd]
  inc  dword [cs:cur_hwnd]
  mov  [gs:edi+WINDOW.hwnd],eax
  call get_proc_id
  mov  [gs:edi+WINDOW.pid],eax
  mov  [gs:edi+WINDOW.caption],si
  mov  [gs:edi+WINDOW.flags],cx
  mov  ebx,[gs:edi+WINDOW.hwnd]
  call check_window_position

  ;добавим окно в window stack
  sub  edi,WINDOW_BASE
  shr  edi,5  ;2^5=32=WINDOW_size !!!! при изменении размера структуры окна - изменить !!!!
  mov  ax,di  ;slot number
  and  ax,0xff

  mov  cx,256		    ;проверку на свободное место
  mov  edi,WINDOW_STACK     ;в стеке можно убрать т.к.
 @@:			    ;она производится выше
   cmp	byte [gs:edi],0
   je	@f
   inc	edi
  loop @b
  jmp  .fin
 @@:
  mov  [gs:edi],al
 .fin:
   pusha32
    call hide_mouse
   popa32

   xor	ah,ah
   mov	edi,WINDOW_BASE
   shl	ax,5
   add	edi,eax
   call draw_window

   pusha32
    call show_mouse
   popa32

  xor  ax,ax
  ret


;***
; Убить окно
;***
;in:  ebx=hwnd
;out: ax=статус
kill_window:
  push edi cx
   mov	edi,WINDOW_BASE
   add	edi,WINDOW_size
   mov	cx,255
 @@:
   cmp	[gs:edi+WINDOW.hwnd],ebx
   je	@f
   add	edi,WINDOW_size
 loop @b
  pop  edi cx
  mov  ax,GUI_INVALID_HANDLE
 @@:
  push ebx edx
  mov  [gs:edi+WINDOW.hwnd],0
  sub  edi,WINDOW_BASE
  shr  edi,5  ;2^5=32=WINDOW_size
  mov  ax,di  ;slot number
  xor  ah,ah
  ;Теперь нужно убрать это окно из стека:
  ;сначала активируем,потом обнулим
  mov  cx,256
  mov  edi,WINDOW_STACK
 @@:
   cmp	byte [gs:edi],al
   je	@f
   inc	edi
  loop @b
  jmp  .fin2
 @@:
  ;нашли... передвигаем его вверх
  ;edi=позиция в стеке
   mov	bl,[gs:edi+1]
   cmp	bl,0
   je	.fin  ;оно и так уже активировано
   mov	[gs:edi+1],al
   mov	[gs:edi],bl
   inc	edi
  jmp  @b
 .fin:
  mov  byte [gs:edi],0
  call redraw_all
 .fin2:
  pop  edx ebx
  pop  edi cx
  ret


;послать сообщение окну
;in:  ebx=hwnd
;     cx=lparam
;     dx=rparam
send_message:
  pusha32
   push cx
   ;Найдем окно...
   mov	edi,WINDOW_BASE
   add	edi,WINDOW_size
   mov	cx,255
  @@:
    cmp  [gs:edi+WINDOW.hwnd],ebx
    je	 @f
    add  edi,WINDOW_size
  loop @b
   ;Не нашли
   pop	cx
   popa32
   ret
  @@:
   pop	cx
   ;Нашли
   ;А прочли ли предыдущее сообщение?
   cmp	[gs:edi+WINDOW.msg_pres],0
   jne	@f
    mov  [gs:edi+WINDOW.msg_pres],1
    mov  [gs:edi+WINDOW.lparam],cx
    mov  [gs:edi+WINDOW.rparam],dx
  @@:
  popa32
  ret

;проверить наличие сообщения
;in:  ebx=hwnd
;out: ax=0, если пусто
check_message:
  push ebx ecx edi
   mov	edi,WINDOW_BASE
   add	edi,WINDOW_size
   mov	cx,255
  @@:
    cmp  [gs:edi+WINDOW.hwnd],ebx
    je	 @f
    add  edi,WINDOW_size
  loop @b
   pop edi ecx ebx
   ret
  @@:
   xor	ax,ax
   cmp	[gs:edi+WINDOW.msg_pres],0
   je	@f
   inc	ax
  @@:
  pop edi ecx ebx
  ret

;получить сообщение
;in:  ebx=hwnd
;out: cx=lparam
;     dx=rparam
get_message:
  push eax ebx edi ecx
   mov	edi,WINDOW_BASE
   add	edi,WINDOW_size
   mov	cx,255
  @@:
    cmp  [gs:edi+WINDOW.hwnd],ebx
    je	 @f
    add  edi,WINDOW_size
  loop @b
   pop	ecx edi ebx eax
   ret
  @@:
   pop	ecx
   cmp	[gs:edi+WINDOW.msg_pres],0
   je	@f
    mov  cx,[gs:edi+WINDOW.lparam]
    mov  dx,[gs:edi+WINDOW.rparam]
    mov  [gs:edi+WINDOW.msg_pres],0
  @@:
  pop edi ebx eax
  ret


check_window_position:
;edi = слот окна
  pusha
   mov	ax,[gs:edi+WINDOW.x]
   mov	bx,[gs:edi+WINDOW.y]
   mov	cx,[gs:edi+WINDOW.width]
   mov	dx,[gs:edi+WINDOW.height]

   ;Проверка координаты X
   mov	si,ax
   add	si,cx
   cmp	si,[gs:SCR_WIDTH]
   jbe	x_pos_ok
    mov  [gs:edi+WINDOW.x],0
    xor  ax,ax
   x_pos_ok:

   ;Проверка координаты Y
   mov	si,bx
   add	si,dx
   cmp	si,[gs:SCR_HEIGHT]
   jbe	y_pos_ok
    mov  [gs:edi+WINDOW.y],0
    xor  bx,bx
   y_pos_ok:

   ;Проверка размера по X
   mov	si,ax
   add	si,cx
   cmp	si,[gs:SCR_WIDTH]
   jbe	x_size_ok
    mov  cx,[gs:SCR_WIDTH]
    mov  [gs:edi+WINDOW.width],cx
   x_size_ok:

   ;Проверка размера по Y
   mov	si,bx
   add	si,dx
   cmp	si,[gs:SCR_HEIGHT]
   jbe	y_size_ok
    mov  cx,[gs:SCR_HEIGHT]
    mov  [gs:edi+WINDOW.height],cx
   y_size_ok:

  popa
  ret


calculate_screen:
;edi = слот окна
  pusha32
   xor	ecx,ecx
   xor	eax,eax
   mov	esi,DISPLAY_INFO
   mov	cx,[gs:edi+WINDOW.y]
   movsx eax,word [gs:SCR_WIDTH]
   imul eax,ecx
   add	esi,eax
   movsx eax,word [gs:edi+WINDOW.x]
   add	esi,eax

   push edi
    sub  edi,WINDOW_BASE
    shr  edi,5	;2^5=32=WINDOW_size
    mov  ax,di	;slot number
    xor  ah,ah
   pop	edi

   mov	cx,[gs:edi+WINDOW.height]
  @@:
    mov  dx,[gs:edi+WINDOW.width]
   .fill:
     mov  [gs:esi],al
     inc  esi
     dec  dx
     cmp  dx,0
    jne  .fill
    xor  ebx,ebx
    mov  bx,[gs:SCR_WIDTH]
    sub  bx,[gs:edi+WINDOW.width]
    add  esi,ebx
   loop @b

  popa32
  ret



;Перерисовать все!
redraw_all:
  pusha32
   call show_scr_buff

   mov	edx,004e98h
   call vga_fill

   mov	esi,WINDOW_STACK
   mov	cx,256
  @@:
    xor  eax,eax
    mov  al,[gs:esi]
    cmp  al,0
    je	 @f
    mov  edi,WINDOW_BASE
    shl  ax,5
    add  edi,eax
    push esi
    call draw_window
    pop  esi
    inc  esi
   loop @b
  @@:
   call save_scr_buf
   call show_cursor
  popa32
  ret



;ax=window slot number
activate_window:
  ;ищем окно с этим номером в стеке
  mov  cx,256
  mov  edi,WINDOW_STACK
 @@:
   cmp	byte [gs:edi],al
   je	@f
   inc	edi
  loop @b
  jmp  .fin2
 @@:
  cmp  byte [gs:edi+1],0
  je   .fin2   ;оно и так уже активировано
 @@:
  ;нашли... передвигаем его вверх
  ;edi=позиция в стеке
   mov	bl,[gs:edi+1]
   cmp	bl,0
   je	.fin
   mov	[gs:edi+1],al
   mov	[gs:edi],bl
   inc	edi
  jmp  @b
 .fin:
   pusha32
    call hide_mouse
   popa32
   xor	ah,ah
   mov	edi,WINDOW_BASE
   shl	ax,5
   add	edi,eax
   call draw_window
   pusha32
    call show_mouse
   popa32
 .fin2:
  ret

; ebx=x*65536+width
; ecx=y*65536+height
show_drag:
  push edi
  push edx
  push ebx  ;6=width  8=x
  push ecx  ;2=height 4=y
  push bp   ;0
  mov  bp,sp
   call hide_mouse
   ;top
   mov	dx,[bp+6]
   dec	dx
   mov	cx,[bp+4]
   mov	bx,[bp+8]
  @@:
   push dx
    call get_pixel
    not  edx
    call put_pixel
    inc  bx
   pop	dx
   dec	dx
   jnz	@b

   ;bot
   mov	dx,[bp+6]
   dec	dx
   mov	cx,[bp+4]
   add	cx,[bp+2]
   dec	cx
   mov	bx,[bp+8]
  @@:
   push dx
    call get_pixel
    not  edx
    call put_pixel
    inc  bx
   pop	dx
   dec	dx
   jnz	@b

   ;left
   mov	dx,[bp+2]
   dec	dx
   mov	cx,[bp+4]
   inc	cx
   mov	bx,[bp+8]
  @@:
   push dx
    call get_pixel
    not  edx
    call put_pixel
    inc  cx
   pop	dx
   dec	dx
   jnz	@b

   ;right
   mov	dx,[bp+2]
   dec	dx
   mov	cx,[bp+4]
   inc	cx
   mov	bx,[bp+8]
   add	bx,[bp+6]
   dec	bx
  @@:
   push dx
    call get_pixel
    not  edx
    call put_pixel
    inc  cx
   pop	dx
   dec	dx
   jnz	@b
  call show_mouse
  pop  bp
  pop  ecx
  pop  ebx
  pop  edx
  pop  edi
  ret