;***
; XenonOS - BIG Real mode
; Copyright 2007-2008 Ryabov Alexey aka Sh@dy (alex_megasoft@mail.ru)
;***
;
; Автор исходного текста Кулаков В.Г.
;


; Порт, управляющий запретом немаскируемых прерываний
cmos_addr      equ  0070h
cmos_data      equ  0071h

; Селекторы сегментов
sys_prot_cs    equ  0008h
sys_real_seg   equ  0010h
sys_mondo_seg  equ  0018h

; Область сохранения значений сегментных регистров
save_sp        dw   ?
save_ss        dw   ?
save_ds        dw   ?

gdt_ptr        dq   ?	; Указатель на GDT
; Таблица дескрипторов сегментов для входа в защищенный режим
gdt dw 00000h,00000h,00000h,00000h ;не используется
    dw 0FFFFh,00000h,09A00h,00000h ;сегмент кода CS
    dw 0FFFFh,00000h,09200h,00000h ;сегмент данных DS
    dw 0FFFFh,00000h,09200h,0008Fh ;сегмент GS

; Локальный стек для защищенного режима (организован внутри кодового сегмента)
gdt_end        db 255 dup(0FFh)
local_stk_top  db (0FFh)

;*********************************************
;*        ОТМЕНИТЬ ПРЕДЕЛ СЕГМЕНТА GS        *
;* Процедура изменяет содержимое теневого    *
;* регистра GS таким образом, что становится *
;* возможной линейная адресация через него   *
;* 4 Gb памяти в реальном режиме             *
;*********************************************
SetLAddrModeForGS:
  ; Вычислить линейный адрес кодового сегмента
  mov  ax,cs
  movzx eax,ax
  shl  eax,4   ;умножить номер параграфа на 16
  mov  ebx,eax ;сохранить линейный адрес в ebx, занести младшее слово линейного адреса в дескрипторы сегментов кода и данных
  mov  word [cs:gdt+10],ax
  mov  word [cs:gdt+18],ax
  ;Переставить местами старшее и младшее слова
  ror  eax,16
  mov  byte [cs:gdt+12],al
  mov  byte [cs:gdt+20],al
  ;Установить предел (Limit) и базу (Base) для GDTR
  add  ebx, gdt
  mov  word [CS:gdt_ptr],gdt_end-gdt-1
  mov  dword [CS:gdt_ptr+2],ebx
  ;Сохранить регистр флагов
  pushf
  ;Запретить прерывания, так как таблица прерываний IDT не сформирована для защищенного режима
  cli
  ;Запретить немаскируемые прерывания NMI
  in   al,cmos_addr
  mov  ah,al
  or   al,080h
  out  cmos_addr,al
  and  ah,080h
  ;Запомнить старое состояние маски NMI
  mov  ch,ah
  ;Перейти в защищенный режим
  lgdt fword [cs:gdt_ptr]
  mov  bx,CS
  mov  eax,CR0
  or   al,01b
  mov  CR0,eax
  ;Безусловный дальний переход на метку SetPMode (очистить очередь команд и перезагрузить CS)
  db   0EAh
  dw   set_pmode
  dw   sys_prot_cs
set_pmode:
  mov  ax,sys_real_seg
  mov  ss,ax
  mov  ds,ax
  mov  es,ax
  mov  fs,ax
  ;Снять ограничения с сегмента GS
  mov  ax,sys_mondo_seg
  mov  gs,ax
  ;Вернуться в реальный режим
  mov  eax,CR0
  and  al,11111110b
  mov  CR0,eax

  db   0EAh
  dw   set_rmode
Self_Mod_CS dw ?

set_rmode:
  ; Регистры стека и данных настроить на сегмент кода
  mov  ss,bx
  mov  ds,bx
  ; Обнулить дополнительные сегментные регистры данных (GS не трогать!)
  xor  ax,ax
  mov  es,ax
  mov  fs,ax
  ; Возврат в реальный режим, прерывания снова разрешены
  in   al,cmos_addr
  and  al,07Fh
  or   al,ch
  out  cmos_addr,al
  popf
  ret


;****************************************
;* Разрешить работу с памятью выше 1 Мб *
;****************************************
enable_a20:
  call wait_8042_buf_empty
  mov  al,0d1h ;команда управления линий A20
  out  64h,al
  call wait_8042_buf_empty
  mov  al,0dfh ;разрешить работу линии
  out  60h,al
  call wait_8042_buf_empty
  ret


;***********************************************
;*   ОЖИДАНИЕ ОЧИСТКИ ВХОДНОГО БУФЕРА I8042    *
;* При выходе из процедуры:                    *
;* флаг ZF установлен - нормальное завершение, *
;* флаг ZF сброшен - ошибка тайм-аута.         *
;***********************************************
wait_8042_buf_empty:
  push cx
  mov  cx,0FFFFh  ;задать число циклов
@@kb:
  in   al,64h	  ;получить статус
  test al,10b	  ;буфер i8042 свободен?
  loopnz  @@kb	  ;если нет, то цикл
  pop  cx
  ret

;***
; Переход в Big Real Mode
;***
big_init:
  pushad
; Сохранить значения сегментных регистров в
; реальном режиме (кроме GS)
  mov  [cs:save_sp],sp
  mov  ax,ss
  mov  [cs:save_ss],ax
  mov  ax,ds
  mov  [cs:save_ds],ax
; (работаем теперь только с кодовым сегментом)
  mov  ax,cs
  mov  word [CS:Self_Mod_CS],ax
  mov  ds,ax
  cli
  mov  ss,ax
  mov  sp,local_stk_top
  sti

; Установить режим линейной адресации
  call SetLAddrModeForGS

; Восстановить значения сегментных регистров
  cli
  mov  sp,[cs:save_sp]
  mov  ax,[cs:save_ss]
  mov  ss,ax
  mov  ax,[cs:save_ds]
  mov  ds,ax
  sti
; Разрешить работу линии A20
  call enable_a20
  popad
  ret