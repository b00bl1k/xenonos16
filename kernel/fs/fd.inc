;***
; XenonOS - FAT12 File System
; Copyright 2007-2008 Ryabov Alexey aka Sh@dy (alex_megasoft@mail.ru)
;***


;***
; Данные для FAT12
;***
bSectSize  dw	   512		   ; bytes/sector
bClustSize db	   1		   ; sectors/allocation unit
bResSect   dw	   1		   ; reserved sectors
bFatCnt    db	   2		   ; of fats
bRootSize  dw	   224		   ; of root directories
bTotalSec  dw	   2880 	   ; sectors total in image
bMedia	   db	   240		   ; media descrip: fd=2side9sec, etc...
bFatSize   dw	   9		   ; sectors in a fat
bTrackSec  dw	   18		   ; sectors/track
bHeadCnt   dw	   2		   ; heads
bHiddenSec dd	   0		   ; hidden sectors
bHugeSec   dd	   0		   ; sectors if > 65536
bBootDrv   db	   00		   ; drive number


;***
; Дополнительные данные
;***
absSector   db 0
absHead     db 0
absTrack    db 0
datasector  dw 0
readclustno dw 0



;***
;Ф-ия для чтения секторов
;***
;in:  dx=номер сектора
;     cx=кол-во секторов для чтения
;     es:bx=куда читать
;out: es:bx=конец буфера
fd_read_sectors:
  mov	ax,dx
.main:
  mov	di,0x0003	       ; try to read 5 times
.sloop:
  push	ax
  push	bx
  push	cx
  xor	dx,dx		       ; set dx:ax for operation
  div	word [cs:bTrackSec]	  ; calc
  inc	dl		       ; set sector 0
  mov	byte [cs:absSector],dl
  xor	dx,dx		       ; set dx:ax for operation
  div	word [cs:bHeadCnt]	  ; calc
  mov	byte [cs:absHead],dl
  mov	byte [cs:absTrack],al
  mov	ax,0x201	       ; читаем один сектор
  mov	ch,byte [cs:absTrack]	  ; дорожка
  mov	cl,byte [cs:absSector]	  ; сектор
  mov	dh,byte [cs:absHead]	  ; головка
  mov	dl,byte [cs:bBootDrv]	  ; диск
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
  add	bx,word [cs:bSectSize] ; следующий буфер
  inc	ax		       ; следующий сектор
  loop	.main		       ; читаем дальше
  ret


;***
; Читаем FAT в память
;***
fd_read_fat:
  pusha
  push ds es
  xor  ax,ax
  mov  al,byte [cs:bFatCnt]	 ; Fat Count
  mul  word [cs:bFatSize]	 ; Fat Size
  mov  cx,ax
  mov  ax,word [cs:bResSect]	 ; reserved sectors
  xor  bx,bx
  mov  es,bx
  mov  bx,fat_pos
  mov  dx,ax
  call fd_read_sectors
  pop  es ds
  popa
  ret


;***
; Найти следующий кластер через FAT
;***
; in:  dx=номер кластера
; out: dx=номер следующего кластера
fd_get_next_clust:
  push	ax es bx cx
  mov	ax,dx
  mov	cx,ax
  mov	dx,3
  imul	dx		       ; Умножаем ax на 3
  shr	ax,1		       ;  и делим на 2
  xor	bx,bx		       ; bx=сегмент с FAT
  mov	es,bx		       ; es=bx
  mov	bx,fat_pos
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
  pop	cx bx es ax
  ret


;***
; Читаем ROOT в память
;***
fd_read_root:
  pusha
  push ds es
  xor  cx,cx
  xor  dx,dx
  mov  ax,0x0020	      ; ax=((32*RootSize)/512)+2
  mul  word [cs:bRootSize]
  div  word [cs:bSectSize]
  xchg ax,cx
  mov  al,byte [cs:bFatCnt]
  mul  word [cs:bFatSize]
  add  ax,word [cs:bResSect]
  mov  word [cs:datasector],ax
  add  word [cs:datasector],cx
  xor  bx,bx
  mov  es,bx
  mov  bx,root_pos
  mov  dx,ax
  call fd_read_sectors
  pop  es ds
  popa
  ret


;***
; Чтение файла по начальному клстеру
;***
; in:  dx=начальный номер кластера
;      es:bx=куда читать
; out:
fd_read_clusters:
  push es bx dx cx
.read_loop:
  mov  cx,1
  push dx
  add  dx,31
  call fd_read_sectors
  pop  dx
  call fd_get_next_clust
  cmp  dx,0fffh
  jne  .read_loop
.end:
  pop  cx dx bx es
  ret


;***
; Читать файл с дискеты
;***
;in:  ds:si - имя файла
;     es:bx - буфер, куда читать
;out: ax - статус
filename  db 11 dup(0)
fd_read_file:
  push bx cx dx di si ds es
  push es bx
   push cs
   pop	es
   mov	bx,f_info
   call fd_get_file_info
   cmp	ax,CM_NO_ERRORS
   jne	.return_err
   mov	bx,f_info
   mov	dx,word [es:bx+1ah]
  pop  bx es
  call fd_read_clusters
  mov  ax,CM_NO_ERRORS
  pop  es ds si di dx cx bx
  ret
.return_err:
  pop  bx es
  pop  es ds si di dx cx bx
  ret


;***
; Читать каталог с дискеты
;***
;in:  ds:si - имя файла
;     es:bx - буфер, куда читать
;out: ax - статус
fd_read_folder:
  pusha
  cmp  word [ds:si],002fh
  jne  @f

  xor  cx,cx
  xor  dx,dx
  mov  ax,0x0020	      ; ax=((32*RootSize)/512)+2
  mul  word [cs:bRootSize]
  div  word [cs:bSectSize]
  xchg ax,cx
  mov  al,byte [cs:bFatCnt]
  mul  word [cs:bFatSize]
  add  ax,word [cs:bResSect]
  mov  word [cs:datasector],ax
  add  word [cs:datasector],cx
  mov  dx,ax
  call fd_read_sectors
  popa
  mov  ax,CM_NO_ERRORS
  ret

 @@:
  push es bx
   push cs
   pop	es
   mov	bx,f_info
   call fd_get_file_info
   cmp	ax,CM_NO_ERRORS
   jne	.return_err
   mov	bx,f_info
   mov	dx,word [es:bx+1ah]
  pop  bx es
  call fd_read_clusters
  popa
  mov  ax,CM_NO_ERRORS
  ret
.return_err:
  pop  es bx
  popa
  mov  ax,CM_ERROR
  ret


;***
; Получить информационную структуру файла
;***
;in:  ds:si - имя файла
;     es:bx - буфер, куда скопировать структуру (32 байта)
;     ax - статус
fd_get_file_info:
  push bx cx dx si es ds
  call fd_read_fat
  call fd_read_root

  pusha
   mov	di,si
   call str_ucase
  popa
.read_loop:
  cmp  byte [ds:si],'/'
  jne  .file_not_found
  inc  si
  ; Очистим filename пробелами
  mov  di,filename
  push es bx ds si di
   mov	di,filename
   push cs
   pop	es
   cld
   mov	cx,11
   mov	ax,20h
   rep	stosb
  pop  di si ds      ;!!!! bx es

  xor  cl,cl
.next_char:		   ; Теперь будем побайтово копировать
  mov  al,byte [ds:si]
  inc  cl
  cmp  al,00h		   ; Конец
  je   .find
  cmp  al,'/'		   ; Конец
  je   .find
  cmp  al,0ah		   ; Конец
  je   .find
  cmp  al,0dh		   ; Конец
  je   .find
  cmp  al,'.'		   ; Расширение
  je   .extens
  mov  byte [cs:di],al
  inc  si
  inc  di
  cmp  cl,12
  jne  .next_char
  jmp  .find
.extens:		   ; Если расширение
  mov  di,filename	   ; di='DIR1       '
  add  di,8
  mov  cl,8
  inc  si
  jmp  .next_char
.find:

  push ds si

   xor	ax,ax
   mov	es,ax
   mov	di,root_pos

   mov	ax,cs
   mov	ds,ax
   mov	si,filename

   mov	cx,244
.loop_f:
   push cx di si
    mov  cx,11
    rep  cmpsb
   pop	si di cx
   jz	.found
   add	di,32
   cmp	byte [es:di],00h
   je	.not_found
   loop .loop_f
.not_found:
  pop  si ds bx es
  jmp  .file_not_found
.found:
  mov  dx,word [es:di+1ah]
  pop  si ds bx es

  cmp  byte [ds:si],0
  jne  .read_again

  ;конец, скопируем информацию в es:bx  из  0:di
  mov  si,di
  xor  ax,ax
  mov  ds,ax

  mov  di,bx
  mov  cx,32
  rep  movsb

  jmp  .file_is_read
.read_again:
  ;pop  si ds
  push es bx
  xor  ax,ax
  mov  es,ax
  mov  bx,root_pos
  call fd_read_clusters
  pop  bx es
  jmp  .read_loop

.file_is_read:
  pop  ds es si dx cx bx
  mov  ax,CM_NO_ERRORS
  ret

.file_not_found:
  pop  ds es si dx cx bx
  mov  ax,FS_FILE_NOT_FOUND
  ret