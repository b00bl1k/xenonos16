;***
; XenonOS - Keyboard interrupt
; Copyright 2007-2008 Ryabov Alexey aka Sh@dy (alex_megasoft@mail.ru)
;***


scancode_ru:
  db 0 ;zero
  db 0 ;esc
  db '1234567890-='
  db 8 ;bckspc
  db 0 ;tab
  db '@жгЄҐ­Јий§ек'
  db 0 ;enter
  db 0 ;ctrl
  db 'длў Їа®«¤¦нс'
  db 0 ;left shift
  db '\пзб¬ЁвмЎо.'
  db 0 ;right shift
  db '*' ;numpad
  db 0	 ;alt
  db ' '
  db 0	 ;caps lock
  db 0,0,0,0,0,0,0,0,0,0 ;f1-f10
  db 0,0,0,0,0 ;page up
  db '-' ;numpad
  db 0,0,0
  db '+' ;numpad
  db 0,0,0,0,0,0,0,0,0,0,0,0,0
  db 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0

scancode_ru_shifted:
  db 0 ;zero
  db 0 ;esc
  db '!"№;%:?*()_+'
  db 8 ;bckspc
  db 0 ;tab
  db '‰–“Љ…Ќѓ™‡•љ'
  db 0 ;enter
  db 0 ;ctrl
  db '”›‚ЂЏђЋ‹„†ќр'
  db 0 ;left shift
  db '|џ—‘Њ€’њЃћ,'
  db 0 ;right shift
  db '*' ;numpad
  db 0	 ;alt
  db ' '
  db 0	 ;caps lock
  db 0,0,0,0,0,0,0,0,0,0 ;f1-f10
  db 0,0,0,0,0 ;page up
  db '-' ;numpad
  db 0,0,0
  db '+' ;numpad
  db 0,0,0,0,0,0,0,0,0,0,0,0,0
  db 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0

scancode_en:
  db 0 ;zero
  db 0 ;esc
  db '1234567890-='
  db 8 ;bckspc
  db 0 ;tab
  db 'qwertyuiop[]'
  db 0 ;enter
  db 0 ;ctrl
  db "asdfghjkl;'`"
  db 0 ;left shift
  db '\zxcvbnm,./'
  db 0 ;right shift
  db '*' ;numpad
  db 0	 ;alt
  db ' '
  db 0	 ;caps lock
  db 0,0,0,0,0,0,0,0,0,0 ;f1-f10
  db 0,0,0,0,0 ;page up
  db '-' ;numpad
  db 0,0,0
  db '+' ;numpad
  db 0,0,0,0,0,0,0,0,0,0,0,0,0
  db 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0

scancode_en_shifted:
  db 0 ;zero
  db 0 ;esc
  db '!@#$%^&*()_+'
  db 8 ;bckspc
  db 0 ;tab
  db 'QWERTYUIOP{}'
  db 0 ;enter
  db 0 ;ctrl
  db 'ASDFGHJKL:"~'
  db 0 ;left shift
  db '|ZXCVBNM<>?'
  db 0 ;right shift
  db '*' ;numpad
  db 0	 ;alt
  db ' '
  db 0	 ;caps lock
  db 0,0,0,0,0,0,0,0,0,0 ;f1-f10
  db 0,0,0,0,0 ;page up
  db '-' ;numpad
  db 0,0,0
  db '+' ;numpad
  db 0,0,0,0,0,0,0,0,0,0,0,0,0
  db 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0


key_shift db  0
key_ctrl  db  0
key_alt   db  0
rus	  db  0

; Прерывание от клавиатуры
int09:
  pusha32
   ;Считаем скэн код
   xor	ax,ax
   in	al,60h
   test  al,0x80
   jz  .old_end     ;Выход, если отпущен
   jmp	.end

   ;Нажат левый shift?
   cmp	al,0x2a
   jnz	@f
   mov	[cs:key_shift],1
   cmp	[cs:key_ctrl],1
   jne	.end
   cmp	[cs:rus],1
   jne	.offl
   mov	[cs:rus],1
   jmp	.end
  .offl:
   mov	[cs:rus],0
   jmp	.end
  @@:
   ;Нажат правый shift?
   cmp	al,0x36
   jnz	@f
   mov	[cs:key_shift],1
   cmp	[cs:key_ctrl],1
   jne	.end
   cmp	[cs:rus],1
   jne	.offr
   mov	[cs:rus],1
   jmp	.end
  .offr:
   mov	[cs:rus],0
   jmp	.end
  @@:
   ;Отпущен левый shift?
   cmp	al,0x2a+0x80
   jnz	@f
   mov	[cs:key_shift],0
   jmp	.end
  @@:
   ;Отпущен правый shift?
   cmp	al,0x36+0x80
   jnz	@f
   mov	[cs:key_shift],0
   jmp	.end
  @@:
   ;Нажат ctrl?
   cmp	al,0x1d
   jnz	@f
   mov	[cs:key_ctrl],1
   jmp	.end
  @@:
   ;Отпущен ctrl?
   cmp	al,0x1d+0x80
   jnz	@f
   mov	[cs:key_ctrl],0
   jmp	.end
  @@:
   ;Простой символ


    ;найдем активное окно
    mov  cx,256
    mov  edi,WINDOW_STACK
   @@:
     cmp  byte [gs:edi],0
     je   @f
     inc  edi
    loop @b
    jmp  .end
   @@:
    cmp  cx,256
    je	 .old_end
    xor  ebx,ebx
    mov  bl,[gs:edi-1]
    mov  edi,WINDOW_BASE
    shl  bx,5
    add  edi,ebx


   cmp	[cs:rus],1
   je	.ru
  .en:
   mov	si,scancode_en
   cmp	[cs:key_shift],1
   jne	.get_ascii
   mov	si,scancode_en_shifted
   jmp	.get_ascii
  .ru:
   mov	si,scancode_en
   cmp	[cs:key_shift],1
   jne	.get_ascii
   mov	si,scancode_ru_shifted

  .get_ascii:
   add	si,ax
   mov	ah,al
   mov	al,[cs:si]

  .send:
    ;Отправим сообщение окну о нажатии клавиши
    cmp  [gs:edi+WINDOW.msg_pres],0
    jne  @f
      mov  [gs:edi+WINDOW.msg_pres],1
      mov  [gs:edi+WINDOW.lparam],GUI_WIN_KEY_PRESS
      mov  [gs:edi+WINDOW.rparam],ax
    @@:
 .end:
  in   al,61h
  mov  ah,al
  or   al,80h
  out  61h,al
  xchg ah,al
  out  61h,al
  mov  al,20h
  out  20h,al

  popa32
  iret
 .old_end:
  popa32
  push [cs:oldseg09]
  push [cs:oldofs09]
  retf