;***
; XenonOS - Video functions
; Copyright 2007-2008 Ryabov Alexey aka Sh@dy (alex_megasoft@mail.ru)
;***

;***
; Переход в граф. режим
;***
set_video_mode:
  ;Установим режим
  mov  ax,4F02h
  mov  bx,[gs:SCR_MODE]
  int  10h
  ret


;***
; Поставить точку
;***
;in:  bx - x
;     cx - y
;     edx - цвет (00RRGGBBh)
put_pixel:
  and  ebx,0ffffh
  and  ecx,0ffffh
  cmp  bx,0
  jl   .fin
  cmp  cx,0
  jl   .fin
  cmp  bx,[gs:SCR_WIDTH]
  jge  .fin
  cmp  cx,[gs:SCR_HEIGHT]
  jge  .fin
  cmp  word [gs:SCR_BPP],24
  je   .put_pixel24
.put_pixel32:
  pushad
   movsx eax,word [gs:SCR_BPL]
   imul eax,ecx
   lea	esi,[eax+ebx*4]
   add	esi,[gs:SCR_LFB]
   mov	[gs:esi],edx
  popad
 .fin:
  ret
.put_pixel24:
  pushad
   movsx eax,word [gs:SCR_BPL]
   imul eax,ecx
   lea	esi,[ebx+ebx*2]
   add	esi,[gs:SCR_LFB]
   add	esi,eax
   mov	[gs:esi],dx
   shr	edx,16
   mov	[gs:esi+2],dl
  popad
  ret


;***
; Считать точку с экрана
;***
;in:  bx - x
;     cx - y
;out: edx - цвет (00RRGGBBh)
get_pixel:
  and  ebx,0ffffh
  and  ecx,0ffffh
  cmp  bx,0
  jl   @f
  cmp  cx,0
  jl   @f
  cmp  bx,word [gs:SCR_WIDTH]
  jge  @f
  cmp  cx,word [gs:SCR_HEIGHT]
  jge  @f

 push eax ebx ecx esi
  mov  esi,[gs:SCR_LFB]

  movsx eax,word [gs:SCR_BPP]
  shr  eax,3	 ;делим на 8
  imul ebx,eax
  movsx eax,word [gs:SCR_BPL]
  imul eax,ecx
  add  eax,ebx
  add  esi,eax
  mov  edx,[gs:esi]
  and  ecx,0ffffffh
 pop  esi ecx ebx eax
 @@:
  ret

;***
; Вывод изображения в RAW-формате (RRGGBB)
;***
; ax=width
; bx=height
; cx=left
; dx=top
; es:di=image
put_image:
  pusha
  mov  bp,sp
  mov  cx,[bp+10] ;cx=dx
  .loop_y:
    mov  bx,[bp+12] ;bx=cx
    .loop_x:
      xor  edx,edx
      mov  dl,byte [es:di]
      shl  edx,16
      mov  dh,byte [es:di+1]
      mov  dl,byte [es:di+2]
      add  di,3
      call put_pixel
      inc  bx
      mov  dx,bx
      sub  dx,[bp+12]
      cmp  dx,[bp+14]
    jne  .loop_x
    inc  cx
    mov  dx,cx
    sub  dx,[bp+10]
    cmp  dx,[bp+8]
  jne  .loop_y
  popa
  ret


; eax=x*65536+width
; ebx=y*65536+height
; edx=color
draw_bar:
  push eax  ;10
  push ebx  ;6
  push edx  ;2
  push bp   ;0
  mov  bp,sp

   mov	cx,[bp+8] ;y
   .loop_y:
     mov  bx,[bp+12] ;x
     .loop_x:
       call put_pixel
       inc  bx
       mov  ax,bx
       sub  ax,[bp+12] ;x
       cmp  ax,[bp+10] ;width
     jne  .loop_x
     inc  cx
     mov  ax,cx
     sub  ax,[bp+8] ;y
     cmp  ax,[bp+6] ;height
   jne	.loop_y

  pop  bp
  pop  edx
  pop  ebx
  pop  eax
  ret

vga_fill:
  xor  eax,eax
  xor  ebx,ebx
  mov  esi,[gs:SCR_LFB]
  mov  edi,DISPLAY_INFO
  and  edx,0xffffff
  cmp  word [gs:SCR_BPP],32
  jne  .fill24
 .fill32:
  mov  ax,[gs:SCR_WIDTH]
  mov  bx,[gs:SCR_HEIGHT]
  imul eax,ebx
  mov  ecx,eax
 @@:
   mov	byte [gs:edi],0
   inc	edi
   mov	[gs:esi],edx
   add	esi,4
   dec	ecx
  cmp  ecx,0
  jne  @b
  ret
 .fill24:
  mov  ax,[gs:SCR_WIDTH]
  mov  bx,[gs:SCR_HEIGHT]
  imul eax,ebx
  mov  ecx,eax
 @@:
   mov	byte [gs:edi],0
   inc	edi
   mov	eax,edx
   mov	[gs:esi],dx
   shr	eax,16
   mov	[gs:esi+2],al
   add	esi,3
   dec	ecx
  cmp  ecx,0
  jne  @b
  ret

;  xor  cx,cx
;  .loop_y:
;    xor  bx,bx
;    .loop_x:
;      call put_pixel
;      inc  bx
;      cmp  bx,[gs:SCR_WIDTH]
;    jne  .loop_x
;    inc  cx
;    cmp  cx,[gs:SCR_HEIGHT]
;  jne  .loop_y
;  ret