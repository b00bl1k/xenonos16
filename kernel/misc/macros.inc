;***
; XenonOS - Macros
; Copyright 2007-2008 Ryabov Alexey aka Sh@dy (alex_megasoft@mail.ru)
;***

macro pusha32
{
  ;push eax ebx ecx edx edi esi ebp
  pushad
}

macro popa32
{
  ;pop  ebp esi edi edx ecx ebx eax
  popad
}


macro show_cx
{
 xchg ch,cl
 call write_hex_byte
 xchg ch,cl
 call write_hex_byte
}


macro debug str
{
  local .a
  jmp @f
   .a  db  str,13,10,0
  @@:
   push ds
   push cs
   pop	ds
   mov	si,.a
   xor	ax,ax
   int	80h
   pop	ds
}

cr_x  dw  442

macro gdebug_byte bt
{
  mov  cl,bt
  shld di,cx,12
  and  di,15
  mov  al,[cs:hex_digits + di]
  pusha
   mov	bx,[cs:cr_x]
   mov	cx,50
   mov	edx,0ffffffh
   call draw_char
   add	bx,6
   mov	[cs:cr_x],bx
  popa
  mov  di,cx
  and  di,15
  mov  al,[cs:hex_digits + di]
  pusha
   mov	bx,[cs:cr_x]
   mov	cx,50
   mov	edx,0ffffffh
   call draw_char
   add	bx,6
   mov	[cs:cr_x],bx
  popa
}