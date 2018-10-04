;***
; XenonOS
; Copyright 2007-2008 Ryabov Alexey aka Sh@dy (alex_megasoft@mail.ru)
;***

; ############ ПРЕРЫВАНИЕ INT 20H ##################
temp_cs dw ?
int20:
  ;Запрещаем переключать задачи
  mov  word [gs:TASK_CHANGE],0
  pop  ax		   ;\
  pop  ax		   ; >- flags,cs,ip
  pop  ax		   ;/

  push cs
  pop  ds
  mov  edi,TASK_BASE
  mov  ax,[gs:TASK_CURR]
  push dx
   mov	dx,TASK_size
   mul	dx
  pop  dx
  add  edi,eax
  mov  byte [gs:edi+TASK.state],255
  mov  bx,word [gs:edi+TASK.pointer+2]
  call free_memory

  ;/*** Здесь убиваем окна, принадлежащие этому процессу ***\;
  mov  eax,[gs:edi+TASK.pid]
  mov  esi,WINDOW_BASE
  add  esi,WINDOW_size
  mov  cx,255
 @@:
   cmp	[gs:esi+WINDOW.pid],eax
   jne	.not_my_win
    push eax
     mov  ebx,[gs:esi+WINDOW.hwnd]
     call kill_window
    pop  eax
  .not_my_win:
   add	esi,WINDOW_size
  loop @b

  ;Разрешаем переключать задачи
  mov  word [gs:TASK_CHANGE],1
  jmp  int08new