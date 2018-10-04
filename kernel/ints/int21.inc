;***
; XenonOS - DOS service
; Copyright 2007-2008 Ryabov Alexey aka Sh@dy (alex_megasoft@mail.ru)
;***

; ############ ПРЕРЫВАНИЕ INT 21H ##################
int21:
  cmp  ah,00
  je   _dos_terminate
  cmp  ah,06h
  je   _dos_put_char
  cmp  ah,09h
  je   _dos_put_string
  cmp  ah,25h
  je   _dos_set_int_vector
  cmp  ah,31h
  je   _dos_term_and_stay_resident
  cmp  ah,35h
  je   _dos_get_int_vector
  cmp  ah,48h
  je   _dos_alloc_mem
  cmp  ah,49h
  je   _dos_release_mem
  cmp  ah,4ch
  je   _dos_terminate

  ;pusha
  ;mov  ch,ah
  ;call write_hex_byte
  ;popa

  iret


; Вывод символа
_dos_put_char:
  pusha
   mov	ax,dx
   mov	ah,0eh
   int	10h
  popa
  iret


; Вывод строки ds:dx, заканчивающейся '$'
_dos_put_string:
 pusha
  mov  di,dx
@@:
  mov  al,byte [di]
  cmp  al,'$'
  je   @f
  mov  ah,0eh
  int  10h
  inc  di
  jmp  @b
@@:
 popa
 iret


; Установить вектор прерывания
_dos_set_int_vector:
  push ax si es
  push 0000h
  pop  es
  xor  ah,ah
  shl  ax,2			   ;Умножаем на 4
  mov  si,ax
  pushf
  cli
  mov  word [es:si],dx
  mov  word [es:si+2],ds
  sti
  popf
  pop  es si ax
  iret


;Окончить выполнение и остаться резидентной
_dos_term_and_stay_resident:
  pop  ax
  pop  ax
  pop  ax
  call proc_sleep


;Получить адрес вектора прерывания
_dos_get_int_vector:
  push ax si es
  push 0000h
  pop  es
  xor  ah,ah
  shl  ax,2			   ;Умножаем на 4
  mov  si,ax
  mov  bx,word [es:si]
  mov  es,word [es:si+2]
  pop  es si ax
  iret


;Функция DOS 48h — Выделить память
;Ввод:  АН = 48h
;       ВХ = размер блока в 16-байтных параграфах
;Вывод: CF = 0, если блок выделен
;       АХ = сегментный адрес выделенного блока
;       CF = 1, если произошла ошибка:
;        АХ = 7 — блоки управления памятью разрушены
;        АХ = 8 — недостаточно памяти:
;        ВХ = размер максимального доступного блока
_dos_alloc_mem:
  push bx
  shl  bx,4  ;Умножаем на 16
  call get_memory
  cmp  ax,0
  je   @f
  pop  bx
  stc
  ;mov  ax,<ERROR_CODE>
  mov  bx,1
  iret
 @@:
  clc
  mov  ax,bx
  pop  bx
  iret

;Функция DOS 49h — Освободить память
;Ввод:  АН = 49h
;       ES = сегментный адрес освобождаемого блока
;Вывод: CF = 0, если блок освобожден
;       CF = 1:
;        АХ = 7, если блоки управления памятью разрушены,
;        АХ = 9, если в ES содержится неверный адрес
_dos_release_mem:
  push bx ax
  mov  bx,es
  call free_mem
  pop  ax bx
  iret


;Окончить программу
_dos_terminate:
  jmp  int20