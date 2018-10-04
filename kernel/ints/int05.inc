;***
; XenonOS
; Copyright 2007-2008 Ryabov Alexey aka Sh@dy (alex_megasoft@mail.ru)
;***

; ############ Аппаратное ПРЕРЫВАНИЕ INT 05H ##################
; ############      Bound range fault        ##################
int05:
  mov  ax,5
  jmp  show_dialog