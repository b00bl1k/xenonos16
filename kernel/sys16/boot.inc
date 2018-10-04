;***
; XenonOS - Boot code
; Copyright 2007-2008 Ryabov Alexey aka Sh@dy (alex_megasoft@mail.ru)
;***

vesa_buf_seg	 equ  4500h

check_more   db 0
mtrr_pres    db 0

sz_starting  db 13,10
	     db '╔══════════════════════════════════╗',13,10
	     db '║ XenonOS 16-bit v 0.04 Build 0041 ║',13,10
	     db '║      (C) Рябов Алексей aka Sh@dy ║',13,10
	     db '╚══════════════════════════════════╝',13,10
	     db 0

sz_init_ok   db '[ OK ]',13,10,0
sz_init_fail db '[FAIL]',13,10,0 

sz_mem_man   db  "Инициализация менеджера памяти:",0
sz_mousedrv  db  "Загрузка драйвера мыши: ",0
sz_mousenotf db  "Драйвер не найден!",0
sz_fonts     db  "Загрузка системного шрифта: ",0
sz_font_nf   db  "Шрифт не найден!",0
sz_mtrr      db  "Проверка MTRR: ",0
sz_mtrr_err  db  "Процессор не поддерживает MTRR",0
sz_mtrr_cpu  db  "Процессор не поддерживает CPUID",0
sz_memsize   db  "Определение размера RAM: ",0
sz_mem512    db  "512+ Mb",0
sz_mem256    db  "256 Mb",0
sz_mem128    db  "128 Mb",0
sz_mem64     db  "64 Mb",0
sz_mem32     db  "32 Mb",0
sz_mem16     db  "16 Mb",0
sz_vesa20    db  "Проверка Vesa 2.0: ",0
sz_vesa_mode db  "Выберите разрешение экрана:",13,10
	     db  "  <1>  640x480",13,10
	     db  "  <2>  800x600",13,10
	     db  "  <3> 1024x768",13,10
	     db  "? ",0
sz_checkmore db  13,10,"Проверить нестандартные видео режимы? (1-Да, 0-Нет)",0	     
sz_fatalmode db  "Ошибка: видео режим не поддерживается!",0

sz_font_path db  "/fd/xenon/fonts/system.fnt",0
sz_mouse_pth db  "/fd/xenon/mousedrv.com",0
;sz_mouse_pth db  "/fd/xenon/ptsmouse.com",0

sz_mouse_par db  0

set_pos_status:
  call get_cursor_pos
  mov  dl,70
  mov  ah,03
  call set_cursor_pos
  ret

print_fail:
  call set_pos_status
  mov  si,sz_init_fail
  xor  ax,ax
  int  80h
  ret

print_ok:
  call set_pos_status
  mov  si,sz_init_ok
  xor  ax,ax
  int  80h
  ret

k_panic:
  mov  ah,10h
  int  16h
  jmp  0xF000:0xFFF0


boot_start:
  ;╙ёЄрэютшь ъюфшЁютъє cp866
  mov  ah,06
  int  80h

  ;╧ЁштхЄёЄтшх
  xor  ax,ax
  mov  si,sz_starting
  int  80h

;
; ╬╧╨┼─┼╦┼═╚┼ ╩╬╦-┬└ ╬╧┼╨└╥╚┬═╬╔ ╧└╠▀╥╚
;
  xor  ax,ax
  mov  si,sz_memsize
  int  80h
  mov  ax,0E801h
  xor  cx,cx
  xor  dx,dx
  int  0x15
  movzx ebx,dx	  ;bx - first variant
  movzx eax,cx	  ;ax - first variant
  shl  ebx,6	  ; translation in kilobytes (x64)
  add  eax,ebx
  add  eax,1000h  ;440h - first variant
  cmp  eax,80000h ; 256?
  jge  mem_512_z
  cmp  eax,40000h ; 256?
  jge  mem_256_z
  cmp  eax,20000h ; 128?
  jge  mem_128_z
  cmp  eax,10000h ; 64?
  jge  mem_64_z
  cmp  eax,8000h  ; 32?
  jge  mem_32_z
  jmp  mem_16_z

mem_512_z:
  xor  ax,ax
  mov  si,sz_mem512
  int  80h
  mov  eax,6
  jmp  pre_mem
mem_256_z:
  xor  ax,ax
  mov  si,sz_mem256
  int  80h
  mov  eax,5
  jmp  pre_mem
mem_128_z:
  xor  ax,ax
  mov  si,sz_mem128
  int  80h
  mov  eax,4
  jmp  pre_mem
mem_64_z:
  xor  ax,ax
  mov  si,sz_mem64
  int  80h
  mov  eax,3
  jmp pre_mem
mem_32_z:
  xor  ax,ax
  mov  si,sz_mem32
  int  80h
  mov  eax,2
  jmp  pre_mem
mem_16_z:
  xor  ax,ax
  mov  si,sz_mem16
  int  80h
  mov  eax,1

pre_mem:
  mov  word [gs:RAM_SIZE],ax
  call print_ok

;
; MEMORY MANAGER
;

  xor  ax,ax
  mov  si,sz_mem_man
  int  80h
  call mem_man_init
  mov  bx,kernel_size
  call get_memory
  call print_ok
  
;
; ╟└╧╙╤╩ ─╨╔┬┼╨└ ╠█╪╚
;
  mov  si,sz_mousedrv
  int  80h
  mov  si,sz_mouse_pth
  mov  di,sz_mouse_par
  call create_process
  cmp  ax,0
  je   @f
  mov  si,sz_mousenotf
  xor  ax,ax
  int  80h
  call print_fail
  jmp  k_panic
;  ;╞фхь шэшЎшрышчрЎш■ фЁрщтхЁр
@@:
  mov  ax,0
  int  33h
  cmp  ax,0
  je   @b
  call print_ok

;
; ╟└├╨╙╟╩└ ╤╚╤╥┼╠═╬├╬ ╪╨╚╘╥└
;
  xor  ax,ax
  mov  si,sz_fonts
  int  80h
  push es
   ;┬√шўёышь ЁрчьхЁ Їрщыр
   mov	si,sz_font_path
   push cs
   pop	es
   mov	bx,f_info
   mov	ax,2
   mov	cx,0
   call file_system
   mov	bx,word [cs:f_info+1ch]
   call get_memory
   ;cmp  ax,SS_NO_MEMORY
   push bx
    mov  es,bx
    xor  bx,bx
    xor  ax,ax
    mov  cx,ax
    push cs
    pop  ds
    mov  si,sz_font_path
    call file_system
   pop	bx
   cmp	ax,CM_NO_ERRORS
   je	@f
   mov	si,sz_font_nf
   xor	ax,ax
   int	80h
   call print_fail
   jmp	k_panic
 @@:
   mov	word [gs:FONT_SEG],bx
  pop  es
  call print_ok

  ret
  
;
;╧╨╬┬┼╨╩└ ╧╨╬╓┼╤╤╬╨└ ═└ MTRR
;
  xor  ax,ax
  mov  si,sz_mtrr
  int  80h
  ;check cpu
  pushfd
  pop  eax
  mov  ecx,eax
  xor  eax,0x00200000
  push eax
  popfd
  pushfd
  pop  eax
  push ecx
  popfd
  and  eax,0x00200000
  and  ecx,0x00200000
  cmp  eax,ecx
  jnz  .ispentium
  ;cpu < 486
  xor  ax,ax
  mov  si,sz_mtrr_cpu
  int  80h
  call print_fail
  jmp  .mtrr_done
 .ispentium:
  mov  eax,1
  cpuid
  test edx,1000000000000b
  jnz  .mtrr_ok
  xor  ax,ax
  mov  si,sz_mtrr_err
  int  80h
  call print_fail
  jmp  .mtrr_done
  .mtrr_ok:
  call print_ok
  mov  [cs:mtrr_pres],1
  .mtrr_done:
  
;
;╧╨╬┬┼╨╩└ ┬╚─┼╬ ╨┼╞╚╠└
;
  mov  si,sz_vesa20
  xor  ax,ax
  int  80h
  push vesa_buf_seg
  pop  es
  ;╧юыєўрхь юс∙є■ SVGA шэЇюЁьрЎш■
  mov  ax,4f00h
  xor  di,di
  int  10h
  ;╧ЁютхЁшь тхЁёш■ VBE (─ы  LFB эєцэр тхЁёш  >= 2.0)
  mov  bx,word [es:0000+04h]
  cmp  bx,0200h
  jl   .vbe_error
  jmp  .no_vbe_error

.vbe_error:
  call print_fail
  jmp  k_panic

.no_vbe_error:
  call print_ok
  xor  ax,ax
  mov  si,sz_vesa_mode
  int  80h

.read_mode:
  mov  ah,10h
  int  16h
  cmp  al,31h
  je   .mode_1
  cmp  al,32h
  je   .mode_2
  cmp  al,33h
  je   .mode_3
  jmp  .read_mode

.mode_1:
  mov  word [gs:SCR_MODE],4112h ;640x480x24
  mov  word [gs:SCR_WIDTH],640
  mov  word [gs:SCR_HEIGHT],480
  jmp  .get_mode_info
.mode_2:
  mov  word [gs:SCR_MODE],4115h ;800x600x24
  mov  word [gs:SCR_WIDTH],800
  mov  word [gs:SCR_HEIGHT],600
  jmp  .get_mode_info
.mode_3:
  mov  word [gs:SCR_MODE],4118h ;1024x768x24
  mov  word [gs:SCR_WIDTH],1024
  mov  word [gs:SCR_HEIGHT],768
  jmp  .get_mode_info 
 
.get_mode_info:
  mov  ah,0eh
  int  10h
  
  mov  si,sz_checkmore
  xor  ax,ax
  int  80h
 @@:
  mov  ah,10h
  int  16h 
  cmp  al,30h
  je   @f
  cmp  al,31h
  je   @f
  jmp  @b
 @@:
  and  al,1
  mov  [cs:check_more],al

  mov  ax,0eh
  int  10h

  cmp  [cs:check_more],1
  je   .chk_more_modes

  push vesa_buf_seg
  pop  es
  mov  cx,[gs:SCR_MODE]
  mov  ax,4F01h
  xor  di,di
  int  10h

  ;╤юїЁрэшь LBF рфЁхё
  mov  esi,dword [es:0000+28h]
  mov  [gs:SCR_LFB],esi

  ;╤юїЁрэшь °шЁшэє ¤ъЁрэр
  mov  ax,word [es:0000+12h]
  mov  [gs:SCR_WIDTH],ax

  ;╤юїЁрэшь т√ёюЄє ¤ъЁрэр
  mov  ax,word [es:0000+14h]
  mov  [gs:SCR_HEIGHT],ax

  ;╤юїЁрэшь BPL
  mov  ax,word [es:0000+10h]
  mov  [gs:SCR_BPL],ax

  ;╤юїЁрэшь BPP
  movsx ax,byte [es:0000+19h]
  mov  word [gs:SCR_BPP],ax

  mov  ax,word [es:0000]
  and  ax,1
  cmp  ax,1
  jne  .fatalmode

  jmp  .mode_ok
.fatalmode:
  mov  ax,03
  int  10h
  mov  ah,06
  int  80h
  mov  si,sz_fatalmode
  xor  ax,ax
  int  80h
  jmp  k_panic
.chk_more_modes:
  mov  bx,0x100
.newprobe:
  inc  bx
  cmp  bx,0x17f
  jz   .fatalmode
.probemore:
  push cx bx
  mov  ax,0x4f01
  mov  cx,bx
  push vesa_buf_seg
  pop  es
  xor  di,di
  int  0x10
  pop  bx cx

  test byte [es:di],80h    ; lfb?
  je   .newprobe
  mov  cx,[gs:SCR_WIDTH]
  cmp  [es:di+12h],cx	 ; x size?
  jne  .newprobe
  mov  dx,[gs:SCR_HEIGHT]
  cmp  [es:di+14h],dx	 ; y size?
  jne  .newprobe
  cmp  byte [es:di+19h],32 ;24
  jb   .newprobe
  ;Save LBF
  mov  esi,dword [es:di+28h]
  mov  [gs:SCR_LFB],esi
  ;Save BPP
  movsx ax,byte [es:di+19h]
  mov  word [gs:SCR_BPP],ax
  ;Save BPL
  mov  ax,word [es:di+10h]
  mov  [gs:SCR_BPL],ax
  or   bh,40h
  mov  [gs:SCR_MODE],bx
  
.mode_ok:
  ;╧ЁюЎ эх яюффхЁцштрхЄ MTRR
  cmp  [cs:mtrr_pres],0
  jz   @f
  ;╧юяЁюсєхь тъы■ўшЄ№ MTRR
  mov  ecx,0x201-2
 .mtrr_find:
  add  ecx,2
  cmp  ecx,0x200+8*2
  jge  @f	    ;no free mtrr
  rdmsr
  test eax,0x0800
  jnz  .mtrr_find
  dec  ecx
  ;тъы■ўрхь...
  mov  edx,0x0
  mov  eax,[gs:SCR_LFB]
  or   eax,1
  wrmsr
  inc  ecx
  mov  edx,0xf
  mov  eax,0xff800800
  wrmsr
  mov  ecx,0x2ff	 ; enable mtrr's
  rdmsr
  or   eax,100000000000b ; set
  wrmsr
  ;done...:)
 @@:
  ;╙ёЄрэютшь Ёхцшь
  call set_video_mode

  mov  edx,004e98h
  call vga_fill
  ret

