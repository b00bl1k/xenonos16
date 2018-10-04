;***
; XenonOS - System structures
; Copyright 2007-2008 Ryabov Alexey aka Sh@dy (alex_megasoft@mail.ru)
;***

TASK_size    equ    30h
struc TASK
{
  .pid	     dd ?   ;4 Processs ID
  .state     db ?   ;1 состояние процесса
  .pointer   dd ?   ;4 точка входа в задачу
  .priority  db ?   ;1 приоритет задачи
  .cnt_prior db ?   ;1 счетчик приоритета
  .tid	     db ?   ;1 Task ID (Thread ID)
  .reg_eax   dd ?   ;4
  .reg_ebx   dd ?   ;4
  .reg_ecx   dd ?   ;4
  .reg_edx   dd ?   ;4
  .reg_esi   dd ?   ;4
  .reg_edi   dd ?   ;4
  .reg_ds    dw ?   ;2
  .reg_es    dw ?   ;2
  .reg_ss    dw ?   ;2
  .reg_sp    dw ?   ;2
  .reg_ebp   dd ?   ;4
}

WINDOW_size  equ    20h
struc WINDOW
{
  .hwnd      dd  ?   ;4 ID окна (0-слот свободен)
  .pid	     dd  ?   ;4 ID процесса, которому пренадлежит окно (при убивании процесса, его окна убиваются тоже)
  .width     dw  ?   ;2 Ширина окна
  .x	     dw  ?   ;2 Координата X
  .height    dw  ?   ;2 Высота окна
  .y	     dw  ?   ;2 Координата Y
  .caption   dw  ?   ;2 Адрес на строку заголовка окна, сегмент строки = сегменту процесса
  .flags     dw  ?   ;2 Флаги окна
  .msg_pres  dw  ?   ;2 Сообщение присутствует?
  .lparam    dw  ?   ;2 первый параметр сообщения
  .rparam    dw  ?   ;2 второй параметр сообщения
  .buttons   dw  ?   ;2 id текущей кнопки
  .reserved2 dd  ?   ;4 Зарезервировано
}

virtual at 0
 TASK TASK
end virtual

virtual at 0
 WINDOW WINDOW
end virtual