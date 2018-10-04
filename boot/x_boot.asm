;--------------------------------------;
; Загрузчик для ОС Xenon v 0.1         ;
; -------------------------------------;

  use16
  org  0x7c00
  jmp start			 ; Переходим на код загрузчика
  nop
bName	   db 'XENON OS'      ; 8 байт
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
bResNT	   db 0 	      ; Зарезервировано для Windows NT
bBootSig   db 29h	      ; Сигнатура расширения
bVolID	   dd 2a876CE1h       ; Серийный номер тома
bVolLab    db 'XENON  DISK'   ; 11 байт, метка тома
bFStype    db 'FAT12   '      ; 8 байт, тип ФС

start:
  cli
  xor  ax,ax
  mov  ss,ax
  mov  es,ax
  mov  ds,ax
  mov  sp,0xffff
  sti
  ; Сообщим о том что мы загружаемся
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
; Чтение файла
;***
; in:  ds:si=имя файла
;      es:bx=куда читать
; out: zf=1,если файл не найден
read_file:
  push es bx
  call getrootdir
  call getfat
  ;es:di=root
  xor bx,bx
  mov es,bx
  mov di,500h
  ;поиск
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
; Найти следующий кластер через FAT
;***
; in:  ax=номер кластера
; out: ax=номер следующего кластера
getnextcluster:
  push	es bx dx cx
  mov	cx,ax
  mov	dx,3
  imul	dx		       ; Умножаем ax на 3
  shr	ax,1		       ;  и делим на 2
  xor	bx,bx		       ; bx=сегмент с FAT
  mov	es,bx		       ; es=bx
  mov	bx,2100h
  add	bx,ax		       ; ax=смещение в таблице FAT
  mov	dx,word [es:bx]        ; Считаем word в dx
  test	cx,0x0001	       ; Проверим ax на четность
  jnz	.odd
 .even: 		       ; Кластер четный =>
  and	dx,0fffh	       ; младшие 12 бит
  jmp	.done
 .odd:			       ; Кластер нечетный =>
  shr	dx,4		       ; старшие 12 бит
 .done:
  mov	ax,dx
  pop	cx dx bx es
ret

;***
;Читаем корневую директорию в память
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
; Читаем FAT в память
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
;Ф-ия для чтения секторов
;***
;in:  ax=номер сектора
;     cx=кол-во секторов для чтения
;     es:bx=куда читать
;out: es:bx=конец буфера
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
  mov	ax,0x201	       ; читаем один сектор
  mov	ch,byte [absTrack]     ; дорожка
  mov	cl,byte [absSector]    ; сектор
  mov	dh,byte [absHead]      ; головка
  mov	dl,byte [bBootDrv]     ; диск
  int	0x13		       ;
  jnc	.goal		       ; ошибка ?
  xor	ax,ax		       ; сбросим диск
  int	0x13		       ;
  dec	di		       ; счетчик ошибок-1
  pop	cx
  pop	bx
  pop	ax
  jnz	.sloop		       ; читаем еще раз
  int	0x18
.goal:
  pop	cx
  pop	bx
  pop	ax
  add	bx,word [bSectSize]    ; следующий буфер
  inc	ax		       ; следующий сектор
  loop	.main		       ; читаем дальше
ret

; Вывод строки на экран
; ds:si - адрес строки
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
BootMagic   dw 0xAA55	    ; Сигнатура загрузочного сектора

