;***
; XenonOS - Processes
; Copyright 2007-2008 Ryabov Alexey aka Sh@dy (alex_megasoft@mail.ru)
;***

cur_pid     dd 1

;***
; Создать процесс
;***
;in:  ds:si - путь к запускаемому файлу
;     ds:di - указатель на параметры запуска
;out: ax <> 0 если ошибка
;     ebx - pid
create_process:
  push ds di

  mov  di,si
  call str_ucase

  push si
   call len
   cmp	dword [ds:si-5],'.COM'
  pop  si
  jne  .no_exec_file

  ;Вычислим размер файла
  push si
   push cs
   pop	es
   mov	bx,f_info
   mov	ax,2
   mov	cx,0
   call file_system
  pop  si

  ;bx=размер файла
  mov  bx,word [cs:f_info+1ch] ;максимальный размер - ffffh байт (64 кб)
  add  bx,200h+100h  ;200h байт отведем на стэк (!!! Возможно этого может нехватить !!!) и 100h байт на psp
  call get_memory
  cmp  ax,SS_NO_MEMORY
  je   .no_memory
  ;pop  di ds
  ;bx=сегмент
  ;dx=сколько памяти выделено

  mov  es,bx	 ;[es:bx]=сюда читаем программу
  mov  bx,100h

  push es bx dx
   xor	ax,ax
   mov	cx,ax
   call file_system
  pop  dx bx es

  cmp  ax,CM_NO_ERRORS
  pop  di ds
  jne  .err_return

  push ds di
   push cs
   pop	ds
   mov	si,f_info
   mov	di,78h
   mov	cx,8
   rep movsb
  pop  di ds


  mov  si,di
  mov  di,81h
  push si
   call len
  pop  si
  cmp  cx,0
  je   .skip_params
  mov  byte [es:80h],cl
  rep  movsb

.skip_params:

  mov  byte [es:di],0dh

  mov  word [gs:TASK_CHANGE],0	; выключим переключение задач
  mov  word [es:0000h],020CDh	; int 20h
  mov  edi,TASK_BASE+TASK_size
  mov  cx,256
 @@:
  cmp  byte [gs:edi+TASK.state],255
  je   @f
  add  edi,TASK_size
  loop @b
  ; Нет свободных дескрипторв
  jmp  .max_processes
  ; В DI имеем адрес свободного дескриптора
 @@:
  mov  ax,[gs:TASK_COUNT]
  inc  ax
  mov  [gs:TASK_COUNT],ax
  mov  byte [gs:edi+TASK.state],al   ;state=номер слота, должен будет быть не более 253!!!
  ; Сохраним точку входа в дескрипторе задач
  mov  eax,[cs:cur_pid]
  mov  dword [gs:edi+TASK.pid],eax
  inc  dword [cs:cur_pid]
  mov  word [gs:edi+TASK.pointer],100h
  mov  word [gs:edi+TASK.pointer+2],es

  push di
  mov  di,dx
  ; Подготовим оба стека
  mov  word [es:di-2],00000h  ; 0 - если программа закончится RET'ом, то CS:IP=CS:0000, а там int 20h
  mov  word [es:di-4],00232h  ; FLAGS
  mov  word [es:di-6],es      ; CS    PSP
  mov  word [es:di-8],00100h  ; IP    100h
  pop  di
  sub  dx,8    ;-stack

  mov dword [gs:edi+TASK.reg_eax],0
  mov dword [gs:edi+TASK.reg_ebx],0
  mov dword [gs:edi+TASK.reg_ecx],0
  mov dword [gs:edi+TASK.reg_edx],0
  mov dword [gs:edi+TASK.reg_esi],0
  mov dword [gs:edi+TASK.reg_edi],0
  mov  word [gs:edi+TASK.reg_ds],es
  mov  word [gs:edi+TASK.reg_es],es
  mov  word [gs:edi+TASK.reg_ss],es
  mov  word [gs:edi+TASK.reg_sp],dx
  mov dword [gs:edi+TASK.reg_ebp],0


  mov  byte [gs:edi+TASK.priority],1	     ; Задаем приоритет задачи
  mov  byte [gs:edi+TASK.cnt_prior],1	     ; Задаем приоритет задачи
  mov  word [gs:TASK_CHANGE],1

  mov  ebx,eax
  mov  ax,CM_NO_ERRORS
  ret

.max_processes:
  mov  ax,SS_NOT_RUN_PROCESS
  ret

.no_memory:
  pop  di ds
  mov  ax,SS_NO_MEMORY
  ret

.no_exec_file:
  pop  di ds
  mov  ax,FS_NO_EXEC_FILE
  ret

.err_return:
  ret


;***
; Получить PID текущего процесса
;***
;out: eax=pid
get_proc_id:
  push edi dx
  mov  word [gs:TASK_CHANGE],0
  mov  edi,TASK_BASE
  mov  ax,[gs:TASK_CURR]
  push dx
   mov	dx,TASK_size
   mul	dx
  pop  dx
  add  edi,eax
  mov  eax,dword [gs:edi+TASK.pid]
  mov  word [gs:TASK_CHANGE],1
  pop  dx edi
  ret

;***
; Создать поток
;***
;in:  ds:bx=cs:ip
;     es:di=ss:sp
create_thread:
  mov  word [gs:TASK_CHANGE],0	; выключим переключение задач
  ;mov  word [es:0000h],020CDh   ; int 20h
  mov  esi,TASK_BASE+TASK_size
  mov  cx,256
 @@:
  cmp  byte [gs:esi+TASK.state],255
  je   @f
  add  esi,TASK_size
  loop @b
  jmp  .max_processes
 @@:
  mov  ax,[gs:TASK_COUNT]
  inc  ax
  mov  [gs:TASK_COUNT],ax
  mov  byte [gs:esi+TASK.state],al   ;state=номер слота, должен будет быть не более 253!!!
  ; Сохраним точку входа в дескрипторе задач
  mov  eax,[cs:cur_pid]
  mov  dword [gs:esi+TASK.pid],eax
  inc  dword [cs:cur_pid]
  mov  word [gs:esi+TASK.pointer],bx
  mov  word [gs:esi+TASK.pointer+2],ds
  mov  word [es:di-2],00000h  ; 0 - если программа закончится RET'ом, то CS:IP=CS:0000, а там int 20h
  mov  word [es:di-4],00232h  ; FLAGS
  mov  word [es:di-6],ds      ; CS    PSP
  mov  word [es:di-8],bx      ; IP    000h
  sub  di,8    ;-stack
  mov dword [gs:esi+TASK.reg_eax],0
  mov dword [gs:esi+TASK.reg_ebx],0
  mov dword [gs:esi+TASK.reg_ecx],0
  mov dword [gs:esi+TASK.reg_edx],0
  mov dword [gs:esi+TASK.reg_esi],0
  mov dword [gs:esi+TASK.reg_edi],0
  mov  word [gs:esi+TASK.reg_ds],ds
  mov  word [gs:esi+TASK.reg_es],ds
  mov  word [gs:esi+TASK.reg_ss],es
  mov  word [gs:esi+TASK.reg_sp],di
  mov dword [gs:esi+TASK.reg_ebp],0
  mov  byte [gs:esi+TASK.priority],1	     ; Задаем приоритет задачи
  mov  byte [gs:esi+TASK.cnt_prior],1	     ; Задаем приоритет задачи
  mov  word [gs:TASK_CHANGE],1
  xor  ax,ax
  ret
.max_processes:
  mov  ax,SS_NOT_RUN_PROCESS
  ret

;***
; Получить адрес слота процесса
;***
;in:  eax=pid
;out: edi=адрес
get_proc_slot:
  push eax cx
  mov  edi,TASK_BASE
  mov  cx,254
 @@:
  cmp  [gs:edi],eax
  je   @f
  add  edi,TASK_size
  loop @b
 @@:
  pop  cx eax
  ret

;***
; Усыпить текущий процесс
;***
proc_sleep:
  pop  ax
  pop  ax
  pop  ax
  mov  word [gs:TASK_CHANGE],0
  push edi
  pusha
  mov  edi,TASK_BASE
  mov  ax,[gs:TASK_CURR]
  push dx
   mov	dx,TASK_size
   mul	dx
  pop  dx
  add  edi,eax
  mov  byte [gs:edi+TASK.state],254
  popa
  pop  edi
  mov  word [gs:TASK_CHANGE],1
  jmp  int08new
