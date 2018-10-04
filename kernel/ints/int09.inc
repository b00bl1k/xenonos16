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


; Прерывание от клавиатуры
int09:
  pusha32
   xor	ax,ax
   in	al,60h
   test al,10000000b
   jne	.end
   ;найдем активное окно
   mov	cx,256
   mov	edi,WINDOW_STACK
  @@:
    cmp  byte [gs:edi],0
    je	 @f
    inc  edi
   loop @b
   jmp	.end
  @@:
   cmp	cx,256
   je	.end
   xor	ebx,ebx
   mov	bl,[gs:edi-1]
   mov	edi,WINDOW_BASE
   shl	bx,5
   add	edi,ebx

   ;push es
   ;  xor  bx,bx
   ;  mov  es,bx
   ;  mov  dl,[es:0417]
   ;pop  es

   ;test dl,00000001b
   ;jz   .shifted
   ;test dl,00000010b
   ;jz   .shifted
   mov	si,scancode_en
   ;jmp  .load_ascii
  ;.shifted:
   ; mov  si,scancode_en_shifted
   ;.load_ascii:
    add  si,ax
    mov  ah,al
    mov  al,[cs:si]
    ;jmp  .send

   .send:
    ;Отправим сообщение окну о нажатии клавиши
    cmp  [gs:edi+WINDOW.msg_pres],0
    jne  @f
      mov  [gs:edi+WINDOW.msg_pres],1
      mov  [gs:edi+WINDOW.lparam],GUI_WIN_KEY_PRESS
      mov  [gs:edi+WINDOW.rparam],ax
    @@:
  jmp .my_end

 .end:
  popa32
  push [cs:oldseg09]
  push [cs:oldofs09]
  retf
 .my_end:
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