;***
; XenonOS - Expections handler
; Copyright 2007-2008 Ryabov Alexey aka Sh@dy (alex_megasoft@mail.ru)
;***
sz_error    db	13,10,'Программа выполнила недопустимую инструкцию',13,10,'и вынуждена завершиться.',13,10,0

; in:  ax=эюьхЁ шёъы■ўхэш 
show_dialog:
  pop  dx ;ip
  pop  dx ;cs
  pop  dx ;FLAGS

  cmp  ax,0ch
  je   .reboot

  call set_video_mode
  int  20h

.reboot:
  jmp  0xF000:0xFFF0