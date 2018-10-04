;***
; XenonOS - System calls
; Copyright 2007-2008 Ryabov Alexey aka Sh@dy (alex_megasoft@mail.ru)
;***

include 'font_ru.inc'

;***
; Вывод строки
;***
__sys_print:
  push ax
.loop:
  mov  al,[ds:si]
  inc  si
  cmp  al,00h
  je   .quit
  mov  ah,0eh
  int  10h
  jmp  .loop
.quit:
  pop  ax
  iret

;***
; Очистить экран
;***
__sys_clear:
  pusha
  push es
  mov  bx,0b800h
  mov  es,bx
  xor  bx,bx
  mov  di,0000
  mov  cx,80*25
  mov  ah,07h
  mov  al,20h
  rep  stosw
  xor  dx,dx
  mov  ah,01
  int  80h
  pop  es
  popa
  iret

;***
; Получение версии XenonOS
;***
__sys_getversion:
  mov  ah,byte [cs:os_ver_major]
  mov  al,byte [cs:os_ver_minor]
  iret

;***
; Установить русскую кодировку
;***
__sys_set_cp886:
   pusha
   push es
   push cs
   pop	es
   mov	bp, RU_FNT1		; RU_FNT1 - First part
   mov	bx, 1000h		; 768 bytes
   mov	cx, 30h 		; 48 symbols
   mov	dx, 80h 		; 128 - position of first symbol
   mov	ax, 1100h
   int	10h
   mov	bp, RU_FNT2		; RU_FNT2 -Second part
   mov	bx, 1000h		; 512 bytes
   mov	cx, 20h 		; 32 symbols
   mov	dx, 0E0h		; 224 - position of first symbol
   mov	ax, 1100h
   int	10h
   pop	es
   popa
   iret

;***
; Ф-ии для работы с памятью
;***
__sys_memory:
  cmp  al,00
  je   get_mem
  cmp  al,01
  je   free_mem
  cmp  al,02
  je   get_free_mem_count
  cmp  al,0ffh
  je   get_mem_table
  iret

get_mem_table:
  push cs
  pop  es
  mov  edi,MEMORY_TABLE
  iret

get_mem:
  call get_memory
  iret

free_mem:
  call free_memory
  iret

get_free_mem_count:
  call get_free_memory_count
  iret


;***
; Получить адрес вектора прерывания
;***
__sys_get_vector:
  push ax si
  push 0000h
  pop  es
  xor  ah,ah
  shl  ax,2			   ;Умножаем на 4
  mov  si,ax
  mov  di,word [es:si]
  mov  es,word [es:si+2]
  pop  si ax
  iret

;***
; Установить адрес вектора прерывания
;***
__sys_set_vector:
  push ax si ds
  push 0000h
  pop  ds
  xor  ah,ah
  shl  ax,2			   ;Умножаем на 4
  mov  si,ax
  pushf
  cli
  mov  word [si],di
  mov  word [si+2],es
  sti
  popf
  pop ds si ax
  iret


;***
; Файловый системный сервис
;***
__sys_fs:
  mov  ax,dx
  call file_system
  iret


;***
; Запустить программу (создать процесс)
;***
__sys_cr_proc:
  push cx dx ds es si di
   call create_process
  pop  di si es ds dx cx
  iret


;***
; Получить pid текущего процесса
;***
__sys_get_pid:
  call get_proc_id
  iret


;***
; Усыпить процесс, который вызвал это прерывание
;***
__sys_proc_sleep:
  pop  ax
  pop  ax
  pop  ax
  call proc_sleep


__sys_stand_by_proc:
  jmp  free_app
