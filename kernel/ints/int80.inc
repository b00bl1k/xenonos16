;***
; XenonOS - System service
; Copyright 2007-2008 Ryabov Alexey aka Sh@dy (alex_megasoft@mail.ru)
;***

; ############ ПРЕРЫВАНИЕ INT 80H ##################
int80:
  cmp  ah,00h
  je   __sys_print
  cmp  ah,01h
  je   __sys_clear
  cmp  ah,02h
  je   __sys_getversion
  cmp  ah,03h
  je   __sys_memory
  cmp  ah,04h
  je   __sys_get_vector
  cmp  ah,05h
  je   __sys_set_vector
  cmp  ah,06h
  je   __sys_set_cp886
  cmp  ah,07h
  je   __sys_fs
  cmp  ah,08h
  je   __sys_cr_proc
  cmp  ah,09h
  je   __sys_get_pid
  cmp  ah,0Ah
  je   __sys_proc_sleep
  cmp  ah,0Bh
  je   __sys_gui
  cmp  ah,0Ch
  je   __sys_stand_by_proc
  iret