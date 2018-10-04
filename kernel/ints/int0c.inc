;***
; XenonOS
; Copyright 2007-2008 Ryabov Alexey aka Sh@dy (alex_megasoft@mail.ru)
;***

; ############ Аппаратное ПРЕРЫВАНИЕ INT 0CH ##################
; ############         Stack fault           ##################
int0c:
  mov  ax,0ch
  jmp  show_dialog