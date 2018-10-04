  
PAGE  59,132
  
;ЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫ
;ЫЫ								         ЫЫ
;ЫЫ			        NOMOUSE				         ЫЫ
;ЫЫ								         ЫЫ
;ЫЫ      Created:   16-Apr-91					         ЫЫ
;ЫЫ      Version:						         ЫЫ
;ЫЫ      Passes:    5	       Analysis Options on: none	         ЫЫ
;ЫЫ								         ЫЫ
;ЫЫ								         ЫЫ
;ЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫ
  
data_1e		equ	103h			; (0000:0103=2)
data_2e		equ	107h			; (0000:0107=2)
data_3e		equ	115h			; (0000:0115=0E8h)
data_4e		equ	417h			; (0000:0417=0)
data_5e		equ	41Ah			; (0000:041A=24h)
data_6e		equ	41Ch			; (0000:041C=24h)
data_7e		equ	114h			; (6FCE:0114=0)
data_8e		equ	2Ch			; (6FCF:002C=0)
data_31e	equ	0A2A9h			; (6FCF:A2A9=0)
data_32e	equ	0AEE2h			; (6FCF:AEE2=0)
  
seg_a		segment	byte public
		assume	cs:seg_a, ds:seg_a
  
  
		org	100h
  
nomouse		proc	far
  
start:
		jmp	loc_28			; (031E)
data_9		db	0, 0
data_10		dw	6FCFh
data_11		dd	6FCF0000h
data_13		db	0
data_14		dw	0
data_15		dw	0
data_16		db	0
data_17		dw	0
data_18		db	0
data_19		db	0Fh
  
nomouse		endp
  
;ЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫ
;
;			External Entry Point
;
;ЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫ
  
int_09h_entry	proc	far
		sti				; Enable interrupts
		pushf				; Push flags
		push	ax
		xor	ax,ax			; Zero register
		mov	cs:data_13,al		; (6FCF:010B=0)
		mov	cs:data_15,ax		; (6FCF:010E=0)
		mov	cs:data_14,ax		; (6FCF:010C=0)
		mov	cs:data_18,1		; (6FCF:0113=0)
		in	al,60h			; port 60h, keybd scan or sw1
		test	al,80h
		jz	loc_2			; Jump if zero
		mov	cs:data_18,0		; (6FCF:0113=0)
		and	al,7Fh
		jmp	short loc_11		; (01BA)
		db	90h
loc_2:
		cmp	al,39h			; '9'
		jne	loc_3			; Jump if not equal
		inc	cs:data_13		; (6FCF:010B=0)
		jmp	loc_14			; (020A)
loc_3:
		cmp	al,48h			; 'H'
		jne	loc_4			; Jump if not equal
		dec	cs:data_14		; (6FCF:010C=0)
		jmp	loc_14			; (020A)
loc_4:
		cmp	al,50h			; 'P'
		jne	loc_5			; Jump if not equal
		inc	cs:data_14		; (6FCF:010C=0)
		jmp	loc_14			; (020A)
loc_5:
		cmp	al,4Bh			; 'K'
		jne	loc_6			; Jump if not equal
		dec	cs:data_15		; (6FCF:010E=0)
		jmp	loc_14			; (020A)
loc_6:
		cmp	al,4Dh			; 'M'
		jne	loc_7			; Jump if not equal
		inc	cs:data_15		; (6FCF:010E=0)
		jmp	loc_14			; (020A)
loc_7:
		cmp	al,47h			; 'G'
		jne	loc_8			; Jump if not equal
		dec	cs:data_15		; (6FCF:010E=0)
		dec	cs:data_14		; (6FCF:010C=0)
		jmp	loc_14			; (020A)
loc_8:
		cmp	al,4Fh			; 'O'
		jne	loc_9			; Jump if not equal
		dec	cs:data_15		; (6FCF:010E=0)
		inc	cs:data_14		; (6FCF:010C=0)
		jmp	short loc_14		; (020A)
loc_9:
		cmp	al,49h			; 'I'
		jne	loc_10			; Jump if not equal
		inc	cs:data_15		; (6FCF:010E=0)
		dec	cs:data_14		; (6FCF:010C=0)
		jmp	short loc_14		; (020A)
loc_10:
		cmp	al,51h			; 'Q'
		jne	loc_11			; Jump if not equal
		inc	cs:data_15		; (6FCF:010E=0)
		inc	cs:data_14		; (6FCF:010C=0)
		jmp	short loc_14		; (020A)
loc_11:
		cmp	al,1Ch
		jne	loc_12			; Jump if not equal
		mov	al,cs:data_18		; (6FCF:0113=0)
		mov	ah,cs:data_16		; (6FCF:0110=0)
		and	ah,6
		or	ah,al
		mov	cs:data_16,ah		; (6FCF:0110=0)
		jmp	short loc_14		; (020A)
		db	90h
loc_12:
		cmp	al,1
		jne	loc_13			; Jump if not equal
		mov	al,cs:data_18		; (6FCF:0113=0)
		mov	ah,cs:data_16		; (6FCF:0110=0)
		and	ah,5
		shl	al,1			; Shift w/zeros fill
		or	ah,al
		mov	cs:data_16,ah		; (6FCF:0110=0)
		jmp	short loc_14		; (020A)
loc_13:
		cmp	al,53h			; 'S'
		jne	loc_14			; Jump if not equal
		mov	al,cs:data_18		; (6FCF:0113=0)
		mov	ah,cs:data_16		; (6FCF:0110=0)
		and	ah,3
		shl	al,1			; Shift w/zeros fill
		shl	al,1			; Shift w/zeros fill
		or	ah,al
		mov	cs:data_16,ah		; (6FCF:0110=0)
loc_14:
		pop	ax
		call	dword ptr cs:data_9	; (6FCF:0103=0)
		cli				; Disable interrupts
		push	es
		push	ax
		xor	ax,ax			; Zero register
		mov	es,ax
		mov	ax,es:data_4e		; (0000:0417=0)
		and	ax,3
		cmp	ax,3
		pop	ax
		pop	es
		jnz	loc_15			; Jump if not zero
		not	cs:data_19		; (6FCF:0114=0Fh)
		call	sub_1			; (02F7)
loc_15:
		cmp	cs:data_19,0		; (6FCF:0114=0Fh)
		je	loc_16			; Jump if equal
		push	ax
		push	es
		xor	ax,ax			; Zero register
		mov	es,ax
		mov	ax,es:data_5e		; (0000:041A=24h)
		mov	es:data_6e,ax		; (0000:041C=24h)
		pop	es
		pop	ax
		cmp	cs:data_13,0		; (6FCF:010B=0)
		jne	loc_17			; Jump if not equal
		cmp	cs:data_14,0		; (6FCF:010C=0)
		jne	loc_18			; Jump if not equal
		cmp	cs:data_15,0		; (6FCF:010E=0)
		jne	loc_18			; Jump if not equal
loc_16:
		jmp	short loc_23		; (02D4)
		db	90h
loc_17:
		push	ax
		push	bx
		mov	ah,0Fh
		int	10h			; Video display   ah=functn 0Fh
						;  get state, al=mode, bh=page
		mov	bl,bh
		xor	bh,bh			; Zero register
		mov	ax,1Dh
		int	33h			; Mouse driver  ax=function 1Dh
						;  define display page number
		pop	bx
		pop	ax
		jmp	short loc_23		; (02D4)
		db	90h
loc_18:
		push	ax
		push	bx
		push	cx
		push	dx
		push	es
		push	ds
		mov	ax,3
		int	33h			; Mouse driver  ax=function 03h
						;  get position, cx=horz,dx=vert
		xor	ax,ax			; Zero register
		mov	es,ax
		test	byte ptr es:data_4e,4	; (0000:0417=0)
		jnz	loc_22			; Jump if not zero
		cmp	cs:data_14,0		; (6FCF:010C=0)
		je	loc_20			; Jump if equal
		jg	loc_19			; Jump if >
		mov	cs:data_14,0FFF6h	; (6FCF:010C=0)
		jmp	short loc_20		; (02A5)
loc_19:
		mov	cs:data_14,0Ah		; (6FCF:010C=0)
loc_20:
		cmp	cs:data_15,0		; (6FCF:010E=0)
		je	loc_22			; Jump if equal
		jg	loc_21			; Jump if >
		mov	cs:data_15,0FFF6h	; (6FCF:010E=0)
		jmp	short loc_22		; (02BF)
loc_21:
		mov	cs:data_15,0Ah		; (6FCF:010E=0)
loc_22:
		add	dx,cs:data_14		; (6FCF:010C=0)
		add	cx,cs:data_15		; (6FCF:010E=0)
		mov	ax,4
		int	33h			; Mouse driver  ax=function 04h
						;  set position, cx=horz,dx=vert
		pop	ds
		pop	es
		pop	dx
		pop	cx
		pop	bx
		pop	ax
loc_23:
		sti				; Enable interrupts
		iret				; Interrupt return
int_09h_entry	endp
  
  
;ЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫ
;
;			External Entry Point
;
;ЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫЫ
  
int_33h_entry	proc	far
		sti				; Enable interrupts
		pushf				; Push flags
		mov	cs:data_17,ax		; (6FCF:0111=0)
		call	cs:data_11		; (6FCF:0107=0)
		cmp	cs:data_19,0		; (6FCF:0114=0Fh)
		je	loc_ret_24		; Jump if equal
		cmp	cs:data_17,3		; (6FCF:0111=0)
		jne	loc_ret_24		; Jump if not equal
		or	bl,cs:data_16		; (6FCF:0110=0)
  
loc_ret_24:
		iret				; Interrupt return
int_33h_entry	endp
  
  
;ЯЯЯЯЯЯЯЯЯЯЯЯЯЯЯЯЯЯЯЯЯЯЯЯЯЯЯЯЯЯЯЯЯЯЯЯЯЯЯЯЯЯЯЯЯЯЯЯЯЯЯЯЯЯЯЯЯЯЯЯЯЯЯЯЯЯЯЯЯЯЯЯЯЯ
;			       SUBROUTINE
;ЬЬЬЬЬЬЬЬЬЬЬЬЬЬЬЬЬЬЬЬЬЬЬЬЬЬЬЬЬЬЬЬЬЬЬЬЬЬЬЬЬЬЬЬЬЬЬЬЬЬЬЬЬЬЬЬЬЬЬЬЬЬЬЬЬЬЬЬЬЬЬЬЬЬ
  
sub_1		proc	near
		push	ax
		push	bx
		push	cx
		cli				; Disable interrupts
		mov	bx,3E8h
		in	al,61h			; port 61h, 8255 port B, read
		push	ax
loc_25:
		and	al,0FCh
		out	61h,al			; port 61h, 8255 B - spkr, etc
						;  al = 0, disable parity
		mov	cx,96h
  
locloop_26:
		loop	locloop_26		; Loop if cx > 0
  
		or	al,2
		out	61h,al			; port 61h, 8255 B - spkr, etc
		mov	cx,96h
  
locloop_27:
		loop	locloop_27		; Loop if cx > 0
  
		dec	bx
		jnz	loc_25			; Jump if not zero
		pop	ax
		out	61h,al			; port 61h, 8255 B - spkr, etc
						;  al = 0, disable parity
		sti				; Enable interrupts
		pop	cx
		pop	bx
		pop	ax
		retn
sub_1		endp
  
loc_28:
		mov	dx,115h
		mov	ax,cs
		mov	es,ax
loc_29:
		dec	ax
		mov	ds,ax
		mov	si,dx
		mov	di,dx
		mov	cx,0Ah
		cld				; Clear direction
		repe	cmpsw			; Rep zf=1+cx >0 Cmp [si] to es:[di]
		jnz	loc_30			; Jump if not zero
		cmp	byte ptr ds:data_7e,0Fh	; (6FCE:0114=0)
		jne	loc_33			; Jump if not equal
loc_30:
		cmp	ax,1
		jne	loc_29			; Jump if not equal
		push	cs
		pop	ds
		mov	data_19,0		; (6FCF:0114=0Fh)
		mov	ax,3533h
		int	21h			; DOS Services  ah=function 35h
						;  get intrpt vector al in es:bx
		mov	word ptr cs:data_11,bx	; (6FCF:0107=0)
		mov	word ptr cs:data_11+2,es	; (6FCF:0109=6FCFh)
		cmp	bx,0
		jne	loc_32			; Jump if not equal
		mov	ax,es
		cmp	ax,0
		jne	loc_32			; Jump if not equal
loc_31:
		jmp	loc_37			; (0400)
loc_32:
		cmp	byte ptr es:[bx],0CFh
		je	loc_31			; Jump if equal
		xor	ax,ax			; Zero register
		int	33h			; Mouse driver  ax=function 00h
						;  installed ? (ax=0 if not)
		mov	dx,offset int_33h_entry
		mov	ax,2533h
		int	21h			; DOS Services  ah=function 25h
						;  set intrpt vector al to ds:dx
		mov	ax,3509h
		int	21h			; DOS Services  ah=function 35h
						;  get intrpt vector al in es:bx
		mov	word ptr cs:data_9,bx	; (6FCF:0103=0)
		mov	cs:data_10,es		; (6FCF:0105=6FCFh)
		mov	dx,offset int_09h_entry
		mov	ax,2509h
		int	21h			; DOS Services  ah=function 25h
						;  set intrpt vector al to ds:dx
		push	cs
		pop	ds
		mov	es,ds:data_8e		; (6FCF:002C=0)
		mov	ah,49h			; 'I'
		int	21h			; DOS Services  ah=function 49h
						;  release memory block, es=seg
		mov	ah,9
		mov	dx,offset data_20	; (6FCF:0405=0C9h)
		int	21h			; DOS Services  ah=function 09h
						;  display char string at ds:dx
		call	sub_1			; (02F7)
		mov	dx,31Eh
		int	27h			; Terminate & stay resident
loc_33:
		mov	cx,ds
		push	ds
		mov	ax,3509h
		int	21h			; DOS Services  ah=function 35h
						;  get intrpt vector al in es:bx
		mov	ax,es
		cmp	cx,ax
		jne	loc_34			; Jump if not equal
		cmp	bx,115h
		jne	loc_34			; Jump if not equal
		mov	ax,3533h
		int	21h			; DOS Services  ah=function 35h
						;  get intrpt vector al in es:bx
		mov	ax,es
		cmp	cx,ax
		jne	loc_34			; Jump if not equal
		cmp	bx,2D6h
		je	locloop_35		; Jump if equal
loc_34:
		pop	ds
;*		mov	dx,offset loc_1		;*
		db	0BAh, 9Bh, 05h
		jmp	short loc_36		; (03F8)
		db	90h
  
locloop_35:
		pop	ds
		push	ds
		lds	dx,dword ptr ds:data_1e	; (0000:0103=0FF02h) Load 32 bit ptr
		mov	ax,2509h
		int	21h			; DOS Services  ah=function 25h
						;  set intrpt vector al to ds:dx
		pop	ds
		push	ds
		lds	dx,dword ptr ds:data_2e	; (0000:0107=7602h) Load 32 bit ptr
		mov	ax,2533h
		int	21h			; DOS Services  ah=function 25h
						;  set intrpt vector al to ds:dx
		pop	es
		mov	word ptr es:data_3e,0FFFFh	; (0000:0115=0E8h)
		mov	ah,49h			; 'I'
		int	21h			; DOS Services  ah=function 49h
						;  release memory block, es=seg
		mov	dx,offset data_24	; (6FCF:0525=0C9h)
loc_36:
		push	cs
		pop	ds
		mov	ah,9
		int	21h			; DOS Services  ah=function 09h
						;  display char string at ds:dx
		int	20h			; Program Terminate
loc_37:
		mov	dx,60Eh
		jmp	short loc_36		; (03F8)
data_20		db	'ЙННННННННННННННННННННННННННННННН'
		db	'НННННН»', 0Ah, 0Dh, 'є „а ©ўҐа-Ё'
		db	'¬¬Ёв в®а ¬лиЁ version 1.00 є', 0Ah
		db	0Dh, 'є 15.04.1991 14:32 •€ђќ ЏЊ '
		db	'(int 33h.3)є', 0Ah, 0Dh, 'є ЁбЇ®'
		db	'«м§®ў ­ЁҐ :                     '
		db	'є', 0Ah, 0Dh, 'є ўЄ«/ўлЄ«  - lef'
		db	'tshift+rightshift    є', 0Ah, 0Dh
		db	'є гЇа ў«Ґ­ЁҐ- бваҐ«ЄЁ,esc,enter,'
		db	'insertє', 0Ah, 0Dh, 'ИНННННННННН'
		db	'НННННННННННННННННННННННННННј', 0Ah
		db	0Dh, '$'
data_24		db	'ЙННННННННННННННННННННННННННННННН'
		db	'НННН»', 0Ah, 0Dh, 'є €¬¬Ёв в®а ¬'
		db	'лиЁ ўлЈаг¦Ґ­ Ё§ Ї ¬пвЁ є', 0Ah, 0Dh
		db	'ИННННННННННННННННННННННННННННННН'
		db	'ННННј', 0Ah, 0Dh, '$'
		db	0C9h
		db	34 dup (0CDh)
		db	0BBh, 0Ah, 0Dh,0BAh, 20h, 88h
		db	0A7h,0A2h,0A8h,0ADh,0A8h,0E2h
		db	0A5h, 2Ch, 20h,0ADh,0AEh, 20h
		db	0A2h,0EBh,0A3h,0E0h,0E3h,0A7h
		db	0AAh,0A0h, 20h,0ADh,0A5h,0A2h
		db	0AEh,0A7h,0ACh,0AEh,0A6h,0ADh
		db	0A0h, 20h,0BAh, 0Ah, 0Dh,0C8h
		db	34 dup (0CDh)
		db	0BCh, 0Ah, 0Dh, 24h,0C9h
		db	39 dup (0CDh)
		db	0BBh, 0Ah, 0Dh,0BAh, 20h
		db	 88h,0A7h,0A2h,0A8h,0ADh,0A8h
		db	0E2h,0A5h, 2Ch, 20h,0ADh,0AEh
		db	 20h,0A4h,0E0h,0A0h,0A9h,0A2h
		db	0A5h,0E0h, 20h,0ACh,0EBh,0E8h
		db	0A8h, 20h,0ADh,0A5h, 20h,0A7h
		db	0A0h,0A3h,0E0h,0E3h,0A6h,0A5h
		db	0ADh, 20h,0BAh, 0Ah, 0Dh,0C8h
		db	39 dup (0CDh)
		db	0BCh, 0Ah, 0Dh, 24h
  
seg_a		ends
  
  
  
		end	start
