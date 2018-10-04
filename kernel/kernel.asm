;***
; XenonOS - Kernel
; Copyright 2007-2008 Ryabov Alexey aka Sh@dy (alex_megasoft@mail.ru)
;***

  use16
  org  0x0
  jmp  __kernel_start

;-INCULDE------------------------------------------------------------;
  include  'misc\consts.inc'
  include  'misc\macros.inc'
  include  'misc\struct.inc'
  include  'misc\functions.inc'

  include  'sys16\ints.inc'
  include  'sys16\boot.inc'
  include  'sys16\big_real.inc'
  include  'sys16\mman.inc'
  include  'sys16\sheduler.inc'
  include  'sys16\proc.inc'
  include  'sys16\expects.inc'
  include  'sys16\syscalls.inc'

  include  'fs\fs.inc'
  include  'video\vesa20.inc' ; Vesa 2.0
  include  'gui\gui.inc' ; GUI of XenonOS
;-END----------------------------------------------------------------;

__kernel_start:
  cli
  push cs
  pop  bx
  mov  ds,bx
  mov  es,bx
  xor  bx,bx
  mov  ss,bx
  mov  sp,0fffeh
  sti

  call big_init

  mov  byte [gs:TASK_BASE+TASK.priority],1
  mov  byte [gs:TASK_BASE+TASK.cnt_prior],1
  mov  ax,cs
  mov  word [gs:TASK_BASE+TASK.pointer],0
  mov  word [gs:TASK_BASE+TASK.pointer+2],ax
  call init_tasks

  call get_vectors

  call set_vectors

  call boot_start
  ;call set_mouse

  mov  si,sz_ps1
  mov  di,sz_par
  ;call create_process

  mov  si,sz_ps2
  mov  di,sz_par
  call create_process

  mov  si,sz_ps3
  mov  di,sz_par
  ;call create_process

.os_loop:
  mov  ah,0ch
  int  80h
  jmp  .os_loop

.reboot:
  jmp  0xF000:0xFFF0

;-DATA---------------------------------------------------------------;
  sz_ps1	db '/fd/xenon/mousedrv.com',0
  sz_ps2	db '/fd/vgafade.com',0
  sz_ps3	db '/fd/test.com',0
  sz_par	db 0
  sz_newline	db 13,10,0
;-END----------------------------------------------------------------;

get_timer:
  push es
   xor	eax,eax
   mov	es,ax
   mov	eax,[es:046ch]
  pop  es
  ret

kernel_size = $