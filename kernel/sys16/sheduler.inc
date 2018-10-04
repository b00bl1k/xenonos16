;***
; XenonOS - Sheduler
; Copyright 2007-2008 Ryabov Alexey aka Sh@dy (alex_megasoft@mail.ru)
;***

; Планировщик
sheduler:
  ;вызовем старый обработчик
  pushf
  db  9ah
  oldofs08	 dw    ?
  oldseg08	 dw    ?

  ;Можно ли переключать задачи
  cmp  word [gs:TASK_CHANGE],0
  je   end_shed

free_app:

  push edi
  push edx
  push eax

  mov  edi,TASK_BASE
  xor  eax,eax
  xor  edx,edx
  mov  ax,word [gs:TASK_CURR]
  mov  dx,TASK_size
  imul eax,edx
  add  edi,eax

  ;сохраняем регистры
  pop  [gs:edi+TASK.reg_eax]
  mov  [gs:edi+TASK.reg_ebx],ebx
  mov  [gs:edi+TASK.reg_ecx],ecx
  pop  [gs:edi+TASK.reg_edx]
  mov  [gs:edi+TASK.reg_esi],esi
  pop  [gs:edi+TASK.reg_edi]
  mov  [gs:edi+TASK.reg_ds],ds
  mov  [gs:edi+TASK.reg_es],es
  mov  [gs:edi+TASK.reg_ss],ss
  mov  [gs:edi+TASK.reg_sp],sp
  mov  [gs:edi+TASK.reg_ebp],ebp

  ; С учетом приоритетов
  dec  byte [gs:edi+TASK.cnt_prior]	   ;Счетчик приоритета
  cmp  byte [gs:edi+TASK.cnt_prior],0
  jne  end_shed2
  mov  al,byte [gs:edi+TASK.priority]	   ;Начальное значение приоритета
  mov  byte [gs:edi+TASK.cnt_prior],al	   ;...восстановим

.sheduler_01:
  add  edi,TASK_size
int08new:				   ;сюда переходим из int  20h
  xor  eax,eax
  mov  ax,word [gs:TASK_CURR]
nexttask:
  inc  ax
  mov  edi,TASK_BASE
  cmp  ax,254
  jne  int201
  xor  eax,eax
  xor  edx,edx
int201:
  push ax
   mov	dx,TASK_size
   mul	dx
   add	edi,eax
  pop  ax
  cmp  byte [gs:edi+TASK.state],255
  je   nexttask
  cmp  byte [gs:edi+TASK.state],254
  je   nexttask
  mov  [gs:TASK_CURR],ax
end_shed2:


  ;восстанавлимваем регистры
  mov  eax,[gs:edi+TASK.reg_eax]
  mov  ebx,[gs:edi+TASK.reg_ebx]
  mov  ecx,[gs:edi+TASK.reg_ecx]
  mov  edx,[gs:edi+TASK.reg_edx]
  mov  esi,[gs:edi+TASK.reg_esi]
  mov  ds,[gs:edi+TASK.reg_ds]
  mov  es,[gs:edi+TASK.reg_es]
  mov  ss,[gs:edi+TASK.reg_ss]
  mov  sp,[gs:edi+TASK.reg_sp]
  mov  ebp,[gs:edi+TASK.reg_ebp]
  mov  edi,[gs:edi+TASK.reg_edi]
  ;push dword [gs:edi+TASK.reg_edi]
  ;pop  edi

end_shed:

  push ax
  mov  al,20h
  out  20h,al
  pop  ax

  iret

; Инициализация задач
init_tasks:
  mov  byte [gs:TASK_BASE],0
  mov  byte [gs:TASK_COUNT],0
  mov  byte [gs:TASK_CURR],0
  mov  esi,TASK_BASE+TASK_size	;нацеливаем на первую задачу, 0-ос
  mov  cx,254
.it1:
  mov  byte [gs:esi+TASK.state],255
  mov  byte [gs:esi+TASK.pointer],254
  add  esi,TASK_size
  loop .it1
  ret