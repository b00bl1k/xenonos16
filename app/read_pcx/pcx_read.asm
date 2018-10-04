  org  100h

  pcx_addr	 equ 300000h
  include '..\consts.inc'

__start:
   ;Получим инфу о файле
   mov	si,file_name
   mov	bx,file_info
   mov	ah,7
   mov	dx,2
   mov	cx,0
   int	80h
   cmp	ax,0
   je	@f
    ret
  @@:
   ;Выделим память для файла
   mov	bx,word [file_info+1ch]
   mov	ax,0300h
   int	80h
   cmp	ax,0
   je	@f
    ret
  @@:
   ;Прочитаем файл
   mov	[file_seg],bx
   mov	es,bx
   xor	bx,bx
   mov	ah,07
   xor	dx,dx
   mov	cx,dx
   mov	si,file_name
   int	80h
   je	@f
    ret
  @@:

  ;Обработка pcx-файла
  push word [file_seg]
  pop  es
  mov  si,128
  ;вычислим ширину и высоту изображения
  mov  ax,[es:42h]
  mov  [img_len],ax
  mov  ax,[es:0ah]
  sub  ax,[es:6]
  inc  ax
  mov  [img_height],ax
  ;смещение палитры
  mov  ax,word [file_info+1ch]
  sub  ax,768
  mov  [pal_ofs],ax
  ;
  mov  edi,pcx_addr
  ;перевод в нормальный вид
 .string_start:
  mov  dx,[img_len]
 .next_byte:
  mov  cx,1
  mov  al,[es:si]
  inc  si
  test al,10000000b
  jz   .real_color
  test al,1000000b
  jz   .real_color
  and  al,111111b
  mov  cl,al
  xor  ch,ch
  mov  al,[es:si]
  inc  si
 .real_color:
  xor  ah,ah
  mov  bx,ax
  shl  ax,1
  add  bx,ax
  add  bx,[pal_ofs]
  xor  eax,eax
  mov  ax,[es:bx]
  xchg al,ah
  shl  eax,8
  mov  al,[es:bx+2]
 .next_pixel:
  mov  [gs:edi],eax
  add  edi,3
  dec  dx
  jz   .next_string
  loop .next_pixel
  jmp  .next_byte
 .next_string:
  dec  [img_height]
  jnz  .string_start

  ;Освободим память,занятую файлом
  mov  bx,[file_seg]
  mov  ax,0301h
  int  80h

  ;Создаем окно
  mov  ax,0b03h
  mov  ebx,60*65536+325
  mov  ecx,60*65536+267
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
  mov  edi,pcx_addr
  push word 0
  pop  es
  mov  bx,46ch
  mov  eax,[es:bx]
  push eax

  mov  bx,14
  .loop_y:
    mov  cx,3
    .loop_x:
      mov  edx,[gs:edi]
      ;and  edx,0ffffffh
      add  edi,3
      push cx bx
       shl  ecx,16
       mov  cx,bx
       mov  ax,0b07h
       mov  ebx,[hwnd1]
       int  80h
      pop  bx cx
      inc  cx
      cmp  cx,320+3
    jne  .loop_x
    inc  bx
    cmp  bx,240+13
  jne  .loop_y

  push word 0
  pop  es
  mov  bx,46ch
  mov  eax,[es:bx]
  pop  ebx
  sub  eax,ebx

  push cs
  pop  es

  mov  di,lbl1_data
  mov  cx,8
  call show_dec

  mov  ax,0b0ah
  mov  ebx,[hwnd1]
  mov  ecx,3*65536+256
  mov  di,label1
  mov  esi,0xffffff
  int  80h
  jmp  main_loop

key:
  jmp  main_loop

  title 	 db  'PCX File reader',0
  hwnd1 	 dd  ?
  file_name	 db  '/fd/bg.pcx',0
  file_info	 db  32 dup(0)
  file_seg	 dw  ?
  img_len	 dw  ?
  img_height	 dw  ?
  pal_ofs	 dw  ?
  label1	 db  "Draw tick's: "
   lbl1_data	 db  '00000000',0

show_dec:
  push bx	  ;cx - Минимальное кол-во вывод. чисел
  mov  bx,10	  ;di - Адресс строки с рез-том.
  call num2ascii
  pop  bx
  ret

num2ascii:
  push dx
  push di
  push si
  xor  si,si
  jcxz NTA20
NTA10:
  xor  dx,dx
  div  bx
  call HexDigit
  push dx
  inc  si
  loop NTA10
NTA20:
  inc  cx
  or   ax,ax
  jnz  NTA10
  mov  cx,si
  jcxz NTA40
  cld
NTA30:
  pop  ax
  stosb
  loop NTA30
NTA40:
  ;mov  byte [di],0
  pop  si
  pop  di
  pop  dx
  ret

HexDigit:
  cmp  dl,10
  jb   HD10
  add  dl,"A"-10
  ret
HD10:
  or   dl,"0"
  ret
