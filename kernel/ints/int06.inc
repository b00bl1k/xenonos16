;***
; XenonOS
; Copyright 2007-2008 Ryabov Alexey aka Sh@dy (alex_megasoft@mail.ru)
;***

; ############ Аппаратное ПРЕРЫВАНИЕ INT 06H ##################
; ############       Invalid  opcode         ##################
int06:
  mov  ax,6
  jmp  show_dialog