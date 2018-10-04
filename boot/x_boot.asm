;--------------------------------------;
; ��������� ��� �� Xenon v 0.1         ;
; -------------------------------------;

  use16
  org  0x7c00
  jmp start			 ; ��������� �� ��� ����������
  nop
bName	   db 'XENON OS'      ; 8 ����
bSectSize  dw 512	      ; bytes/sector
bClustSize db 1 	      ; sectors/allocation unit
bResSect   dw 1 	      ; reserved sectors
bFatCnt    db 2 	      ; of fats
bRootSize  dw 224	      ; of root directories
bTotalSec  dw 2880	      ; sectors total in image
bMedia	   db 240	      ; media descrip: fd=2side9sec, etc...
bFatSize   dw 9 	      ; sectors in a fat
bTrackSec  dw 18	      ; sectors/track
bHeadCnt   dw 2 	      ; heads
bHiddenSec dd 0 	      ; hidden sectors
bHugeSec   dd 0 	      ; sectors if > 65536
bBootDrv   db 00	      ; drive number
bResNT	   db 0 	      ; ��������������� ��� Windows NT
bBootSig   db 29h	      ; ��������� ����������
bVolID	   dd 2a876CE1h       ; �������� ����� ����
bVolLab    db 'XENON  DISK'   ; 11 ����, ����� ����
bFStype    db 'FAT12   '      ; 8 ����, ��� ��

start:
  cli
  xor  ax,ax
  mov  ss,ax
  mov  es,ax
  mov  ds,ax
  mov  sp,0xffff
  sti
  ; ������� � ��� ��� �� �����������
  mov  si,sz_load
  call print
  call getrootdir
  call getfat
  mov  si,kernel_name
  mov  bx,1000h
  mov  es,bx
  xor  bx,bx
  call read_file
  jc   error
  jmp  0x1000:0x0000

error:
  mov  si,sz_error
  call print
  jmp  $


;***
; ������ �����
;***
; in:  ds:si=��� �����
;      es:bx=���� ������
; out: zf=1,���� ���� �� ������
read_file:
  push es bx
  call getrootdir
  call getfat
  ;es:di=root
  xor bx,bx
  mov es,bx
  mov di,500h
  ;�����
  mov  bx,bRootSize
.find:
  xor  cx,cx
  cmp  [di],ch
  jz   .n_found
  pusha
  mov  cx,11
  rep  cmpsb
  popa
  je   .found
  add  di,32
  dec  bx
  jz   .n_found
  jmp  .find
.found:
  mov  ax,word [es:di+1ah]
  pop  bx es
.read_loop:
  pusha
  mov  ax,0e2eh
  int  10h
  popa
  mov  cx,1
  push ax
  add  ax,31
  call readsectors
  pop  ax
  call getnextcluster
  cmp  ax,0fffh
  jne  .read_loop
.end:
  ret
.n_found:
  pop  bx es
  stc
  ret

;***
; ����� ��������� ������� ����� FAT
;***
; in:  ax=����� ��������
; out: ax=����� ���������� ��������
getnextcluster:
  push	es bx dx cx
  mov	cx,ax
  mov	dx,3
  imul	dx		       ; �������� ax �� 3
  shr	ax,1		       ;  � ����� �� 2
  xor	bx,bx		       ; bx=������� � FAT
  mov	es,bx		       ; es=bx
  mov	bx,2100h
  add	bx,ax		       ; ax=�������� � ������� FAT
  mov	dx,word [es:bx]        ; ������� word � dx
  test	cx,0x0001	       ; �������� ax �� ��������
  jnz	.odd
 .even: 		       ; ������� ������ =>
  and	dx,0fffh	       ; ������� 12 ���
  jmp	.done
 .odd:			       ; ������� �������� =>
  shr	dx,4		       ; ������� 12 ���
 .done:
  mov	ax,dx
  pop	cx dx bx es
ret

;***
;������ �������� ���������� � ������
;***
getrootdir:
  pusha
  push	ds es
  xor	cx,cx
  xor	dx,dx
  mov	ax,0x0020	       ; ax=((32*RootSize)/512)+2
  mul	word [bRootSize]
  div	word [bSectSize]
  xchg	ax,cx
  mov	al,byte [bFatCnt]
  mul	word [bFatSize]
  add	ax,word [bResSect]
  mov	word [datasector],ax
  add	word [datasector],cx
  xor	bx,bx
  mov	es,bx
  mov	bx,500h
  call	readsectors
  pop	es ds
  popa
ret

;***
; ������ FAT � ������
;***
getfat:
  pusha
  push	ds es
  xor	ax,ax
  mov	al,byte [bFatCnt]      ; Fat Count
  mul	word [bFatSize]        ; Fat Size
  mov	cx,ax
  mov	ax,word [bResSect]     ; reserved sectors
  xor	bx,bx
  mov	es,bx
  mov	bx,2100h
  call	readsectors
  pop	es ds
  popa
ret

;***
;�-�� ��� ������ ��������
;***
;in:  ax=����� �������
;     cx=���-�� �������� ��� ������
;     es:bx=���� ������
;out: es:bx=����� ������
readsectors:
.main:
  mov	di,0x0003	       ; try to read 5 times
.sloop:
  push	ax
  push	bx
  push	cx
  xor	dx,dx		       ; set dx:ax for operation
  div	word [bTrackSec]       ; calc
  inc	dl		       ; set sector 0
  mov	byte [absSector],dl
  xor	dx,dx		       ; set dx:ax for operation
  div	word [bHeadCnt]        ; calc
  mov	byte [absHead],dl
  mov	byte [absTrack],al
  mov	ax,0x201	       ; ������ ���� ������
  mov	ch,byte [absTrack]     ; �������
  mov	cl,byte [absSector]    ; ������
  mov	dh,byte [absHead]      ; �������
  mov	dl,byte [bBootDrv]     ; ����
  int	0x13		       ;
  jnc	.goal		       ; ������ ?
  xor	ax,ax		       ; ������� ����
  int	0x13		       ;
  dec	di		       ; ������� ������-1
  pop	cx
  pop	bx
  pop	ax
  jnz	.sloop		       ; ������ ��� ���
  int	0x18
.goal:
  pop	cx
  pop	bx
  pop	ax
  add	bx,word [bSectSize]    ; ��������� �����
  inc	ax		       ; ��������� ������
  loop	.main		       ; ������ ������
ret

; ����� ������ �� �����
; ds:si - ����� ������
print:
  pusha
print_char:
  lodsb
  test al, al
  jz   pr_exit
  mov  ah, 0eh
  mov  bl, 7
  int  10h
  jmp  print_char
pr_exit:
  popa
  ret



; DATA ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;\
sz_load     db 'Loading',00h
sz_error    db 'Read error!',0dh,0ah,00h
kernel_name db 'KERNEL  BIN'
absSector   db 0
absHead     db 0
absTrack    db 0
datasector  dw 0
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;/
times	    1feh-($-$$) db 0
BootMagic   dw 0xAA55	    ; ��������� ������������ �������

