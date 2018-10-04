;***
; XenonOS - Mouse/Cursor
; Copyright 2007-2008 Ryabov Alexey aka Sh@dy (alex_megasoft@mail.ru)
;***
;
; При выводе буфера и курсора не
; используются ф-ии put_pixel/get_pixel
;
;***

mouse_x       dw   0
mouse_y       dw   0
mouse_x_old   dw   0
mouse_y_old   dw   0
mouse_btn_st  db   0
mouse_st_old  db   0

;Курсор
cursor:
  file 'cursor.raw'

set_mouse:
  call save_scr_buf
  ;Диапазон перемещения по горизонтали
  mov  ax,7
  mov  cx,0
  mov  dx,[gs:SCR_WIDTH]
  int  33h
  ;Диапазон перемещения по вертикали
  mov  ax,8
  mov  cx,0
  mov  dx,[gs:SCR_HEIGHT]
  int  33h
  ;handler
  mov  ax,000ch
  mov  cx,11111b
  push cs
  pop  es
  mov  dx,mouse_hanler
  int  33h
  ret

mouse_hanler:
  push es
  pusha32

  mov  al,[cs:mouse_btn_st]
  mov  [cs:mouse_st_old],al
  mov  [cs:mouse_btn_st],bl
  mov  [cs:mouse_x],cx
  mov  [cs:mouse_y],dx
  call show_mouse_grph
  call check_mouse

  popa32
  pop  es
  retf

hide_mouse:
   call show_scr_buff
   ;call save_scr_buf
  ret

show_mouse:
   call save_scr_buf
   call show_cursor
  ret

; Вывод курсора мыши в графическом режиме
show_mouse_grph:
  call hide_mouse
  mov  ax,[cs:mouse_x]
  mov  [cs:mouse_x_old],ax
  mov  ax,[cs:mouse_y]
  mov  [cs:mouse_y_old],ax
  call show_mouse
  ret


;Сохранить место под мышью в буфер
save_scr_buf:
  movsx ebx,[cs:mouse_x_old]
  movsx ecx,[cs:mouse_y_old]

  mov  esi,[gs:SCR_LFB]
  movsx eax,word [gs:SCR_BPP]
  shr  eax,3	 ;делим на 8
  imul ebx,eax
  movsx eax,word [gs:SCR_BPL]
  imul eax,ecx
  add  eax,ebx
  add  esi,eax

  mov  edi,MOUSE_UNDER
  mov  cx,cursor_height
 .draw_y:
   mov	bx,cursor_width
   push esi
  .draw_x:
    ;GET_PIXEL
    mov  edx,[gs:esi]
    cmp  word [gs:SCR_BPP],24
    jne  @f
    ;get_24bpp
     and  edx,0ffffffh
     add  esi,3
     jmp  .ok
    @@:
    add  esi,4
   .ok:
    mov  [gs:edi],edx
    add  edi,4
    dec  bx
   jnz	.draw_x
   pop	esi
   movsx eax,word [gs:SCR_BPL]
   add	esi,eax
   dec	cx
  jnz  .draw_y
 .fin:
  ret

;Отрисовать буфер
show_scr_buff:
  ;xor  ebx,ebx
  ;xor  ecx,ecx
  movsx ebx,[cs:mouse_x_old]
  movsx ecx,[cs:mouse_y_old]

  mov  esi,[gs:SCR_LFB]
  movsx eax,word [gs:SCR_BPP]
  shr  eax,3	 ;делим на 8
  imul ebx,eax
  movsx eax,word [gs:SCR_BPL]
  imul eax,ecx
  add  eax,ebx
  add  esi,eax

  mov  ax,[cs:mouse_x_old]
  mov  bx,cursor_width

  add  ax,cursor_width
  cmp  ax,[gs:SCR_WIDTH]
  jle  .check_ok
  sub  ax,[gs:SCR_WIDTH]
  mov  bx,cursor_width
  sub  bx,ax
  cmp  bx,0
  je   .fin

 .check_ok:
  mov  edi,MOUSE_UNDER
  mov  cx,cursor_height
 .draw_y:
   push bx edi esi
  .draw_x:
    mov  edx,[gs:edi]
    add  edi,4
    ;PUT_PIXEL
    cmp  word [gs:SCR_BPP],24
    jne  @f
    ;put_24bpp
     mov  [gs:esi],dx
     shr  edx,16
     mov  [gs:esi+2],dl ;r
     add  esi,3
     jmp  .ok
    @@:
    ;put_32bpp
    and  edx,0ffffffh
    mov  [gs:esi],edx
    add  esi,4
   .ok:
    dec  bx
   jnz	.draw_x
   pop	esi edi bx
   add	edi,cursor_width*4
   movsx eax,word [gs:SCR_BPL]
   add	esi,eax
   dec	cx
  jnz  .draw_y
 .fin:
  ret


;Отрисовать курсор мыши
show_cursor:
  xor  ebx,ebx
  xor  ecx,ecx
  mov  bx,[cs:mouse_x]
  mov  cx,[cs:mouse_y]

  mov  esi,[gs:SCR_LFB]
  movsx eax,word [gs:SCR_BPP]
  shr  eax,3	 ;делим на 8
  imul ebx,eax
  movsx eax,word [gs:SCR_BPL]
  imul eax,ecx
  add  eax,ebx
  add  esi,eax

  mov  ax,[cs:mouse_x]
  mov  bx,cursor_width

  add  ax,cursor_width
  cmp  ax,[gs:SCR_WIDTH]
  jle  .check_ok
  sub  ax,[gs:SCR_WIDTH]
  mov  bx,cursor_width
  sub  bx,ax
  cmp  bx,0
  je   .fin

 .check_ok:
  mov  cx,cursor_height
  mov  di,cursor
 .draw_y:
   push bx
   push di
   push esi

  .draw_x:
    xor  edx,edx
    mov  dl,byte [cs:di]
    shl  edx,16
    mov  dh,byte [cs:di+1]
    mov  dl,byte [cs:di+2]
    add  di,3
    cmp  edx,0xff00ff
    je	 .skip
    ;PUT_PIXEL
    cmp  word [gs:SCR_BPP],24
    jne  @f
    ;put_24bpp
     mov  [gs:esi],dx
     shr  edx,16
     mov  [gs:esi+2],dl ;r
     add  esi,3
     jmp  .ok
    @@:
    ;put_32bpp
    and  edx,0ffffffh
    mov  [gs:esi],edx
    add  esi,4
   .ok:
    dec  bx
   jnz	.draw_x
   pop	esi
   pop	di
   pop	bx
   add	di,cursor_width*3
   movsx eax,word [gs:SCR_BPL]
   add	esi,eax
   dec	cx
  jnz  .draw_y
 .fin:
  ret
 .skip:
  cmp  word [gs:SCR_BPP],24
  jne  @f
   ;put_24bpp
   add	esi,3
   jmp	.ok
  @@:
   ;put_32bpp
   add	esi,4
   jmp	.ok


cur_drag_win  db  0
cur_drag_x    dw  0
cur_drag_y    dw  0
drag_moved    dw  0

check_mouse:
  ;Пропустим нажатие средней и правой кнопок
  cmp  [cs:mouse_btn_st],2
  je   .finish
  cmp  [cs:mouse_btn_st],3
  je   .finish

  ;Нажали левую кнопку?
  cmp  byte [cs:mouse_btn_st],1
  jne  .mouse_up
  ;кнопка уже была нажата?
   cmp	byte [cs:mouse_st_old],1
   jne	.no_drag
   ;drag&drop window
   xor	eax,eax
   mov	al,[cs:cur_drag_win]
   cmp	al,0
   je	.finish
   mov	edi,WINDOW_BASE
   shl	ax,5
   add	edi,eax
   mov	ebx,dword [gs:edi+WINDOW.width]
   mov	ecx,dword [gs:edi+WINDOW.height]
   call show_drag

   xor	edx,edx
   mov	dx,[cs:mouse_x]
   sub	dx,[cs:cur_drag_x]
   add	[gs:edi+WINDOW.x],dx
   mov	dx,[cs:mouse_x]
   mov	[cs:cur_drag_x],dx
   ;shl  edx,16
   ;add  ebx,edx

   xor	edx,edx
   mov	dx,[cs:mouse_y]
   sub	dx,[cs:cur_drag_y]
   add	[gs:edi+WINDOW.y],dx
   mov	dx,[cs:mouse_y]
   mov	[cs:cur_drag_y],dx
   ;shl  edx,16
   ;add  ecx,edx

   mov	ebx,dword [gs:edi+WINDOW.width]
   mov	ecx,dword [gs:edi+WINDOW.height]
   call show_drag
   mov	[cs:drag_moved],1
   jmp	.finish

  .no_drag:
   ;нажали перевый раз: захватим окно...
   mov	esi,DISPLAY_INFO
   xor	eax,eax
   xor	ebx,ebx
   xor	edx,edx

   mov	ax,[cs:mouse_x]
   mov	bx,[cs:mouse_y]
   mov	dx,[gs:SCR_WIDTH]
   imul ebx,edx
   add	ebx,eax
   add	esi,ebx

   xor	eax,eax
   mov	al,[gs:esi]
   mov	[cs:cur_drag_win],al
   cmp	al,0
   je	.finish
   push ax
    call activate_window
   pop	ax
   mov	edi,WINDOW_BASE
   shl	ax,5
   add	edi,eax
   test [gs:edi+WINDOW.flags],GWF_MOVABLE
   jz	.no_move
   test [gs:edi+WINDOW.flags],GWF_SKINED
   jz	.drag_win
   ;Если окно со скином, то перемещаем за заголовок
   mov	ax,[cs:mouse_y]
   sub	ax,[gs:edi+WINDOW.y]
   cmp	ax,[cs:skn_height]
   jg	.no_move
  .drag_win:
   mov	ebx,dword [gs:edi+WINDOW.width]
   mov	ecx,dword [gs:edi+WINDOW.height]
   call show_drag
   mov	ax,[cs:mouse_x]
   mov	[cs:cur_drag_x],ax
   mov	ax,[cs:mouse_y]
   mov	[cs:cur_drag_y],ax
   mov	[cs:drag_moved],0
   jmp	.finish
  .no_move:
   mov	[cs:cur_drag_win],0
   jmp	.finish

 ;Отпускаем...
 .mouse_up:
   xor	eax,eax
   mov	al,[cs:cur_drag_win]
   cmp	al,0
   je	.finish
   mov	edi,WINDOW_BASE
   shl	ax,5
   add	edi,eax
   mov	ebx,dword [gs:edi+WINDOW.width]
   mov	ecx,dword [gs:edi+WINDOW.height]
   call show_drag
   mov	[cs:cur_drag_win],0
   cmp	[cs:drag_moved],0
   je	.finish
  @@:
   call redraw_all
   mov	[cs:drag_moved],0
 .finish:
  ret