  org  100h

  include 'consts.inc'
__start:
  ;���������...

  mov  ax,[gs:SCR_WIDTH]
  mov  di,lb1_data1
  mov  cx,4
  call show_dec
  mov  ax,[gs:SCR_HEIGHT]
  mov  di,lb1_data2
  mov  cx,4
  call show_dec
  mov  ax,[gs:SCR_BPP]
  mov  di,lb2_data1
  mov  cx,2
  call show_dec
  mov  cx,[gs:RAM_SIZE]
  add  cx,2
  mov  ax,2
  shl  ax,cl
  mov  di,lb3_data1
  mov  cx,4
  call show_dec

  ;������� ����
  mov  ax,0b03h
  mov  ebx,235*65536+200
  mov  ecx,29*65536+100
  mov  dx,GWF_MOVABLE or GWF_VISIBLE or GWF_RESIZABLE or GWF_SKINED
  mov  si,title
  int  80h
  mov  [hwnd1],ebx

  ;���� ��������� ��������� ����
 main_loop:
  mov  ebx,[hwnd1]
  mov  ax,0b05h
  int  80h
  cmp  ax,0
  jne  translate_msg
  mov  ah,0ch
  int  80h
  jmp  main_loop

  ;���������� ���������
 translate_msg:
  mov  ebx,[hwnd1]
  mov  ax,0b06h
  int  80h
  cmp  cx,101
  je   redraw
  cmp  cx,102
  je   key
  jmp  main_loop

redraw:
  mov  ax,0b0ah
  mov  ebx,[hwnd1]
  mov  ecx,3*65536+15
  mov  di,label1
  mov  esi,0xffffff
  int  80h
  mov  ax,0b0ah
  mov  ebx,[hwnd1]
  mov  ecx,3*65536+24
  mov  di,label2
  mov  esi,0xffffff
  int  80h
  mov  ax,0b0ah
  mov  ebx,[hwnd1]
  mov  ecx,3*65536+33
  mov  di,label3
  mov  esi,0xffffff
  int  80h

  jmp  main_loop

key:
  jmp  main_loop


  ;����� �� ��������� (��������� ��� ���� ����� ��������)
  ret

  title 	 db  '���⥬��� ���ଠ��',0
  label1	 db  '����襭�� �࠭�: '
   lb1_data1	 db  '0000x'
   lb1_data2	 db  '0000',0

  label2	 db  '��㡨�� 梥�: '
   lb2_data1	 db  '00',0

  label3	 db  '������ RAM: '
   lb3_data1	 db  '0000 �����',0

  hwnd1 	 dd  ?

show_dec:
  push bx	  ;cx - ����������� ���-�� �����. �����
  mov  bx,10	  ;di - ������ ������ � ���-���.
  call num2ascii
  pop  bx
  ret

num2ascii:
  push dx
  push di
  push si
  xor  si,si
  jcxz NTA20
NTA10:
  xor  dx,dx
  div  bx
  call HexDigit
  push dx
  inc  si
  loop NTA10
NTA20:
  inc  cx
  or   ax,ax
  jnz  NTA10
  mov  cx,si
  jcxz NTA40
  cld
NTA30:
  pop  ax
  stosb
  loop NTA30
NTA40:
  ;mov  byte [di],0
  pop  si
  pop  di
  pop  dx
  ret

HexDigit:
  cmp  dl,10
  jb   HD10
  add  dl,"A"-10
  ret
HD10:
  or   dl,"0"
  ret