;***
; XenonOS - Memory Manager
; Copyright 2007-2008 Ryabov Alexey aka Sh@dy (alex_megasoft@mail.ru)
;***

mem_start = 80h*2

mem_man_init:
  mov  edi,MEMORY_TABLE
  mov  cx,80h
 @@:
  mov  word [gs:edi],0ffffh
  add  edi,2
  loop @b
  ret

;***
; Выделить сегмент
;***
; in:  bx=необходимый размер памяти
; out: ax=статус
;      bx=адрес выделенного сегмента
;      dx=реальное кол-во выделенных байт
get_memory:
  push cx edi si ds es
  ;сначала вычислим кол-во необходимых блоков памяти
  ;разедлим dx:ax на bx (dword на word)
  xor  dx,dx
  mov  ax,bx
  mov  bx,200h
  div  bx
  cmp  dx,0
  je   @f
  inc  ax
@@:
  ;Теперь в ax кол-во блоков  (shl ax,9 = ax*512)
  ;Подготовимся к поиску ax-свободных подряд идущих блоков
  mov  edi,MEMORY_TABLE
  add  edi,mem_start  ;пропустим системные блоки
  xor  cx,cx
 .find:
  cmp  word [gs:edi],0
  je   .free_block
  xor  cx,cx	 ;сбросим счетчик
  push edi
   sub	edi,MEMORY_TABLE
   cmp	edi,1000h
  pop  edi
  jge  .not_free ;Нет блоков такого размера
  add  edi,2
  jmp  .find
 .free_block:
  ;нашли свободный блок
  inc  cx
  cmp  cx,ax
  je   .success
  add  edi,2
  jmp  .find
  ;выходим [cs:di-ax] указывают на начальный блок памяти
 .success:
  dec  ax
  push ax
   shl	ax,1
   and	eax,0ffffh
   sub	edi,eax
  pop  ax
  mov  word [gs:edi],cx  ;сохраним кол-во выделенных блоков

  push cx edi
   mov	cx,ax
  .mark:
    add  edi,2
    mov  word [gs:edi],0ffffh
   loop .mark
  pop  edi cx

  sub  edi,MEMORY_TABLE
  mov  dx,cx
  shl  dx,9  ;*512
  mov  ax,di
  shl  ax,4  ;*512
  mov  bx,ax ;<=адрес свободного блока

  pop  es ds si edi cx
  mov  ax,CM_NO_ERRORS
  ret

 .not_free:
  pop  es ds si edi cx
  mov  ax,CM_ERROR
  ret


;***
; Освободить выделенный блок памяти
;***
;in:  bx-адрес выделенной памяти
;out: ax-статус
free_memory:
  push edi cx
  ;Получим адрес в таблице памяти
  mov  edi,MEMORY_TABLE
  mov  ax,bx
  shr  ax,4
  and  eax,0ffffh
  add  edi,eax
  ;add  di,200h

  ;Узнаем сколько блоков было выделено
  mov  cx,word [gs:edi]
  cmp  cx,0xffff
  je   .end
  cmp  cx,0x0000
  je   .end
 .free_loop:
  mov  word [gs:edi],0
  add  edi,2
  loop .free_loop
  pop  cx edi
  mov  ax,CM_NO_ERRORS
  ret
 .end:
  pop  cx edi
  mov  ax,CM_ERROR
  ret


;***
; Получить кол-во свободных блоков памяти
;***
;out: ax=кол-во свободных блоков
get_free_memory_count:
  push edi cx
  mov  edi,MEMORY_TABLE
  mov  cx,500h
  xor  ax,ax
.lp:
  cmp  word [gs:edi],0
  jne  @f
  inc  ax
 @@:
  add  edi,2
  loop .lp
  pop  cx edi
  ret