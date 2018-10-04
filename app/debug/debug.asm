  org  100h

  jmp  _start

regs:
  r_ax	dw  ?
  r_bx	dw  ?
  r_cx	dw  ?
  r_dx	dw  ?
  r_si	dw  ?
  r_di	dw  ?
  r_es	dw  ?
  r_ds	dw  ?
  r_bp	dw  ?
  r_ss	dw  ?
  r_sp	dw  ?

  registers  db  13,10,"ax=",0,"bx=",0,"cx=",0,"dx=",0,"si=",0,"di=",0,"es=",0,"ds=",0,"bp=",0,"ss=",0,"sp=",0

  hex_digits	  db '0123456789ABCDEF'
  new_line   db  13,10,0
_start:

  mov  [r_ax],ax
  mov  [r_bx],bx
  mov  [r_cx],cx
  mov  [r_dx],dx
  mov  [r_si],si
  mov  [r_di],di
  mov  [r_bp],bp
  mov  ax,es
  mov  [r_es],ax
  mov  ax,ds
  mov  [r_ds],ax
  mov  ax,ss
  mov  [r_ss],ax
  mov  ax,sp
  mov  [r_sp],ax


  mov  si,registers
  mov  di,regs
  mov  cx,11
  @@:
    xor  ax,ax
    int  80h
    push cx si di
     mov  cx,[di]
     xchg ch,cl
     call write_hex_byte
     xchg ch,cl
     call write_hex_byte
     xor  ax,ax
     mov  si,new_line
     int  80h
    pop  di si cx
    add  di,2
  loop @b
  ret

;-OTHER FUNC----------------------------------------------;
write_hex_byte:
  mov  ah,0eh
  xor  bx,bx
  shld di,cx,12
  and  di,15
  mov  al,[hex_digits + di]
  int  10h
  mov  di,cx
  and  di,15
  mov  al,[hex_digits + di]
  int  10h
  ret
;-OTHER FUNC----------------------------------------------;