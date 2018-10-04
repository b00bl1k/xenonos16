;***
; XenonOS - Draw skinable windows
; Copyright 2007-2008 Ryabov Alexey aka Sh@dy (alex_megasoft@mail.ru)
;***

skn_height	  dw  13

skn_cl_act_in	  dd  0xFFFFFF
skn_cl_act_out	  dd  0x000000
skn_cl_act_bgr	  dd  0x3B67A2

skn_std_bgrcl	  dd  0x888888
skn_btn_bgrcl	  dd  0x888888

btn_close_x	  dw  -12
btn_close_y	  dw  1

btn_max_x	  dw  -25
btn_max_y	  dw  1

btn_min_x	  dw  -39
btn_min_y	  dw  1

base_width	  dw  1
left_width	  dw  3
right_width	  dw  41

skin_left:
  file 'skin\left.raw'

skin_right:
  file 'skin\right.raw'

skin_base:
  file 'skin\base.raw'

;***
; Ф-ия отрисовки окна со скином
;***
;edi=offset slot of window
draw_window:
   pusha32
   push ax
     test [gs:edi+WINDOW.flags],GWF_VISIBLE
     jnz  @f
     pop ax
     ret
   @@:
   pop	ax

  push ds
  push edi
   call calculate_screen

   push bp
   mov	bp,sp
   test [gs:edi+WINDOW.flags],GWF_SKINED
   jz	.no_skin

   ;bgr
   mov	eax,dword [gs:edi+WINDOW.width]
   mov	ebx,dword [gs:edi+WINDOW.height]
   mov	edx,[cs:skn_std_bgrcl]
   call draw_bar


   mov	ax,[gs:edi+WINDOW.x]
   add	ax,[gs:edi+WINDOW.width]
   dec	ax
   shl	eax,16
   mov	ax,1
   mov	bx,[gs:edi+WINDOW.y]
   shl	ebx,16
   mov	bx,[gs:edi+WINDOW.height]
   mov	edx,[cs:skn_cl_act_out]
   call draw_bar


   mov	ax,[gs:edi+WINDOW.x]
   shl	eax,16
   mov	ax,[gs:edi+WINDOW.width]
   mov	bx,[gs:edi+WINDOW.y]
   add	bx,[gs:edi+WINDOW.height]
   dec	bx
   shl	ebx,16
   mov	bx,1
   mov	edx,[cs:skn_cl_act_out]
   call draw_bar

   mov	ax,[gs:edi+WINDOW.x]
   shl	eax,16
   mov	ax,1
   mov	bx,[gs:edi+WINDOW.y]
   shl	ebx,16
   mov	bx,[gs:edi+WINDOW.height]
   mov	edx,[cs:skn_cl_act_in]
   call draw_bar


   push cs
   pop	es

   ;base skin
   mov	ax,[cs:base_width]
   mov	bx,[cs:skn_height]
   mov	cx,[gs:edi+WINDOW.x]
   mov	dx,[gs:edi+WINDOW.y]
  @@:
   mov	di,skin_base
   call put_image
   add	cx,ax
   mov	edi,[bp+2]
   mov	si,[gs:edi+WINDOW.x]
   add	si,[gs:edi+WINDOW.width]
   cmp	cx,si
   jl	@b


   ;left skin
   mov	ax,[cs:left_width]
   mov	bx,[cs:skn_height]
   mov	cx,[gs:edi+WINDOW.x]
   mov	dx,[gs:edi+WINDOW.y]
   mov	di,skin_left
   call put_image
   mov	edi,[bp+2]

   ;right skin
   mov	ax,[cs:right_width]
   mov	bx,[cs:skn_height]
   mov	cx,[gs:edi+WINDOW.x]
   add	cx,[gs:edi+WINDOW.width]
   sub	cx,[cs:right_width]
   mov	dx,[gs:edi+WINDOW.y]
   mov	di,skin_right
   call put_image
   mov	edi,[bp+2]

   mov	si,[gs:edi+WINDOW.caption]
   mov	bx,[gs:edi+WINDOW.x]
   add	bx,[cs:left_width]
   add	bx,2
   mov	cx,[gs:edi+WINDOW.y]
   add	cx,3
   mov	edx,[cs:skn_cl_act_in]

   mov	eax,[gs:edi+WINDOW.pid]
   call get_proc_slot
   mov	ax,word [gs:edi+TASK.pointer+2]
   mov	ds,ax
   call draw_string

  .no_skin:

  pop  bp
  pop  edi
  pop  ds

    ;Отправим сообщение окну о перерисовке
    cmp  [gs:edi+WINDOW.msg_pres],0
    jne  @f
      mov  [gs:edi+WINDOW.msg_pres],1
      mov  [gs:edi+WINDOW.lparam],GUI_WIN_REDRAW
      mov  [gs:edi+WINDOW.rparam],0
    @@:

  popa32
  ret