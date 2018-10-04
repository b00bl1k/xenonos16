;***
; XenonOS - Interrupts
; Copyright 2007-2008 Ryabov Alexey aka Sh@dy (alex_megasoft@mail.ru)
;***


include 'ints\int00.inc'
include 'ints\int05.inc'
include 'ints\int06.inc'
include 'ints\int09.inc'
include 'ints\int0c.inc'
include 'ints\int20.inc'
include 'ints\int21.inc'
include 'ints\int80.inc'

oldofs00       dw    ?
oldseg00       dw    ?

oldofs05       dw    ?
oldseg05       dw    ?

oldofs06       dw    ?
oldseg06       dw    ?

oldofs09       dw    ?
oldseg09       dw    ?

oldofs0c       dw    ?
oldseg0c       dw    ?

oldofs20       dw    ?
oldseg20       dw    ?

oldofs21       dw    ?
oldseg21       dw    ?

oldofs27       dw    ?
oldseg27       dw    ?

oldofs80       dw    ?
oldseg80       dw    ?

;***
; Сохранение прерываний
;***
get_vectors:
  push ds
  xor  bx,bx
  mov  ds,bx
  ;int 00h
  mov  bx,[ds:00h*4]
  mov  es,[ds:00h*4+2]
  mov  [cs:oldseg00],es
  mov  [cs:oldofs00],bx
  ;int 05h
  mov  bx,[ds:05h*4]
  mov  es,[ds:05h*4+2]
  mov  [cs:oldseg05],es
  mov  [cs:oldofs05],bx
  ;int 06h
  mov  bx,[ds:06h*4]
  mov  es,[ds:06h*4+2]
  mov  [cs:oldseg06],es
  mov  [cs:oldofs06],bx
  ;int 08h
  mov  bx,[ds:08h*4]
  mov  es,[ds:08h*4+2]
  mov  [cs:oldseg08],es
  mov  [cs:oldofs08],bx
  ;int 09h
  mov  bx,[ds:09h*4]
  mov  es,[ds:09h*4+2]
  mov  [cs:oldseg09],es
  mov  [cs:oldofs09],bx
  ;int 0ch
  mov  bx,[ds:0ch*4]
  mov  es,[ds:0ch*4+2]
  mov  [cs:oldseg0c],es
  mov  [cs:oldofs0c],bx
  ;int 20h
  mov  bx,[ds:20h*4]
  mov  es,[ds:20h*4+2]
  mov  [cs:oldseg20],es
  mov  [cs:oldofs20],bx
  ;int 21h
  mov  bx,[ds:21h*4]
  mov  es,[ds:21h*4+2]
  mov  [cs:oldseg21],es
  mov  [cs:oldofs21],bx
  ;int 27h
  mov  bx,[ds:27h*4]
  mov  es,[ds:27h*4+2]
  mov  [cs:oldseg27],es
  mov  [cs:oldofs27],bx
  ;int 80h
  mov  bx,[ds:80h*4]
  mov  es,[ds:80h*4+2]
  mov  [cs:oldseg80],es
  mov  [cs:oldofs80],bx
  pop  ds
  ret

;***
; Установка прерываний
;***
set_vectors:
  cli
  push ds
  xor  bx,bx
  mov  ds,bx
  ;int 00h
  lea  bx,[cs:int00]
  mov  [ds:00h*4],bx
  mov  [ds:00h*4+2],cs
  ;int 05h
  lea  bx,[cs:int05]
  mov  [ds:05h*4],bx
  mov  [ds:05h*4+2],cs
  ;int 06h
  lea  bx,[cs:int06]
  mov  [ds:06h*4],bx
  mov  [ds:06h*4+2],cs
  ;int 08h
  lea  bx,[cs:sheduler]  ;sys16/sheduler.inc
  mov  [ds:08h*4],bx
  mov  [ds:08h*4+2],cs
  ;int 09h
  lea  bx,[cs:int09]
  mov  [ds:09h*4],bx
  mov  [ds:09h*4+2],cs
  ;int 0ch
  lea  bx,[cs:int0c]
  mov  [ds:0ch*4],bx
  mov  [ds:0ch*4+2],cs
  ;int 20h
  lea  bx,[cs:int20]
  mov  [ds:20h*4],bx
  mov  [ds:20h*4+2],cs
  ;int 21h
  lea  bx,[cs:int21]
  mov  [ds:21h*4],bx
  mov  [ds:21h*4+2],cs
  ;int 27h
  lea  bx,[cs:proc_sleep]
  mov  [ds:27h*4],bx
  mov  [ds:27h*4+2],cs
  ;int 80h
  lea  bx,[cs:int80]
  mov  [ds:80h*4],bx
  mov  [ds:80h*4+2],cs
  pop  ds
  sti
  ret