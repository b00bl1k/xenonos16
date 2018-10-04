;***
; XenonOS
; Copyright 2007-2008 Ryabov Alexey aka Sh@dy (alex_megasoft@mail.ru)
;***

; ############ Аппаратное ПРЕРЫВАНИЕ INT 00H ##################
; ############      Divisioin by zero        ##################
int00:
  mov  ax,0
  jmp  show_dialog