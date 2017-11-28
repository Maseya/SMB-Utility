; For assembly by NASM only
bits 32
;
; m6502zp - V1.6 - Copyright 1998, Neil Bradley (neil@synthcom.com)
;
;

; Using stack calling conventions
; Zero page version (all zero page accesses are direct)


		section	.data

times ($$-$) & 3 nop	; pad with NOPs to 4-byte boundary

		global	_m6502zppc

		global	m6502zppbBankSwitch_, _m6502zppbBankSwitch

_m6502zpcontextBegin:

; DO NOT CHANGE THE ORDER OF THE FOLLOWING REGISTERS!

_m6502zpBase	dd	0	; Base address for 6502 stuff
_m6502zpMemRead	dd	0	; Offset of memory read structure array
_m6502zpMemWrite	dd	0	; Offset of memory write structure array
_m6502zpaf	dw	0	; A register and flags
_m6502zppc:
m6502zppc	dw	0	; 6502 Program counter
_m6502zpx		db	0	; X
_m6502zpy		db	0	; Y
_m6502zps		db	0	; s
_irqPending	db	0	; Non-zero if an IRQ is pending

_m6502zpcontextEnd:


times ($$-$) & 3 nop	; pad with NOPs to 4-byte boundary

_tempAddr	dd	0	; Temporary address storage
dwElapsedTicks	dd 0	; Elapsed ticks!
cyclesRemaining	dd	0	; # Of cycles remaining
_altFlags	db	0	; Storage for I, D, and B

times ($$-$) & 3 nop	; pad with NOPs to 4-byte boundary

bit6502tox86:
		db	00h, 01h, 40h, 41h, 02h, 03h, 42h, 43h, 00h, 01h, 40h, 41h, 02h, 03h, 42h, 43h
		db	00h, 01h, 40h, 41h, 02h, 03h, 42h, 43h, 00h, 01h, 40h, 41h, 02h, 03h, 42h, 43h
		db	00h, 01h, 40h, 41h, 02h, 03h, 42h, 43h, 00h, 01h, 40h, 41h, 02h, 03h, 42h, 43h
		db	00h, 01h, 40h, 41h, 02h, 03h, 42h, 43h, 00h, 01h, 40h, 41h, 02h, 03h, 42h, 43h
		db	10h, 11h, 50h, 51h, 12h, 13h, 52h, 53h, 10h, 11h, 50h, 51h, 12h, 13h, 52h, 53h
		db	10h, 11h, 50h, 51h, 12h, 13h, 52h, 53h, 10h, 11h, 50h, 51h, 12h, 13h, 52h, 53h
		db	10h, 11h, 50h, 51h, 12h, 13h, 52h, 53h, 10h, 11h, 50h, 51h, 12h, 13h, 52h, 53h
		db	10h, 11h, 50h, 51h, 12h, 13h, 52h, 53h, 10h, 11h, 50h, 51h, 12h, 13h, 52h, 53h
		db	80h, 81h, 0c0h, 0c1h, 82h, 83h, 0c2h, 0c3h, 80h, 81h, 0c0h, 0c1h, 82h, 83h, 0c2h, 0c3h
		db	80h, 81h, 0c0h, 0c1h, 82h, 83h, 0c2h, 0c3h, 80h, 81h, 0c0h, 0c1h, 82h, 83h, 0c2h, 0c3h
		db	80h, 81h, 0c0h, 0c1h, 82h, 83h, 0c2h, 0c3h, 80h, 81h, 0c0h, 0c1h, 82h, 83h, 0c2h, 0c3h
		db	80h, 81h, 0c0h, 0c1h, 82h, 83h, 0c2h, 0c3h, 80h, 81h, 0c0h, 0c1h, 82h, 83h, 0c2h, 0c3h
		db	90h, 91h, 0d0h, 0d1h, 92h, 93h, 0d2h, 0d3h, 90h, 91h, 0d0h, 0d1h, 92h, 93h, 0d2h, 0d3h
		db	90h, 91h, 0d0h, 0d1h, 92h, 93h, 0d2h, 0d3h, 90h, 91h, 0d0h, 0d1h, 92h, 93h, 0d2h, 0d3h
		db	90h, 91h, 0d0h, 0d1h, 92h, 93h, 0d2h, 0d3h, 90h, 91h, 0d0h, 0d1h, 92h, 93h, 0d2h, 0d3h
		db	90h, 91h, 0d0h, 0d1h, 92h, 93h, 0d2h, 0d3h, 90h, 91h, 0d0h, 0d1h, 92h, 93h, 0d2h, 0d3h

bitx86to6502:
		db	00h, 01h, 00h, 01h, 00h, 01h, 00h, 01h, 00h, 01h, 00h, 01h, 00h, 01h, 00h, 01h
		db	40h, 41h, 40h, 41h, 40h, 41h, 40h, 41h, 40h, 41h, 40h, 41h, 40h, 41h, 40h, 41h
		db	00h, 01h, 00h, 01h, 00h, 01h, 00h, 01h, 00h, 01h, 00h, 01h, 00h, 01h, 00h, 01h
		db	40h, 41h, 40h, 41h, 40h, 41h, 40h, 41h, 40h, 41h, 40h, 41h, 40h, 41h, 40h, 41h
		db	02h, 03h, 02h, 03h, 02h, 03h, 02h, 03h, 02h, 03h, 02h, 03h, 02h, 03h, 02h, 03h
		db	42h, 43h, 42h, 43h, 42h, 43h, 42h, 43h, 42h, 43h, 42h, 43h, 42h, 43h, 42h, 43h
		db	02h, 03h, 02h, 03h, 02h, 03h, 02h, 03h, 02h, 03h, 02h, 03h, 02h, 03h, 02h, 03h
		db	42h, 43h, 42h, 43h, 42h, 43h, 42h, 43h, 42h, 43h, 42h, 43h, 42h, 43h, 42h, 43h
		db	80h, 81h, 80h, 81h, 80h, 81h, 80h, 81h, 80h, 81h, 80h, 81h, 80h, 81h, 80h, 81h
		db	0c0h, 0c1h, 0c0h, 0c1h, 0c0h, 0c1h, 0c0h, 0c1h, 0c0h, 0c1h, 0c0h, 0c1h, 0c0h, 0c1h, 0c0h, 0c1h
		db	80h, 81h, 80h, 81h, 80h, 81h, 80h, 81h, 80h, 81h, 80h, 81h, 80h, 81h, 80h, 81h
		db	0c0h, 0c1h, 0c0h, 0c1h, 0c0h, 0c1h, 0c0h, 0c1h, 0c0h, 0c1h, 0c0h, 0c1h, 0c0h, 0c1h, 0c0h, 0c1h
		db	82h, 83h, 82h, 83h, 82h, 83h, 82h, 83h, 82h, 83h, 82h, 83h, 82h, 83h, 82h, 83h
		db	0c2h, 0c3h, 0c2h, 0c3h, 0c2h, 0c3h, 0c2h, 0c3h, 0c2h, 0c3h, 0c2h, 0c3h, 0c2h, 0c3h, 0c2h, 0c3h
		db	82h, 83h, 82h, 83h, 82h, 83h, 82h, 83h, 82h, 83h, 82h, 83h, 82h, 83h, 82h, 83h
		db	0c2h, 0c3h, 0c2h, 0c3h, 0c2h, 0c3h, 0c2h, 0c3h, 0c2h, 0c3h, 0c2h, 0c3h, 0c2h, 0c3h, 0c2h, 0c3h

m6502zpregular:
		dd	RegInst00
		dd	RegInst01
		dd	invalidInsByte
		dd	invalidInsByte
		dd	invalidInsByte
		dd	RegInst05
		dd	RegInst06
		dd	invalidInsByte
		dd	RegInst08
		dd	RegInst09
		dd	RegInst0a
		dd	invalidInsByte
		dd	invalidInsByte
		dd	RegInst0d
		dd	RegInst0e
		dd	invalidInsByte
		dd	RegInst10
		dd	RegInst11
		dd	invalidInsByte
		dd	invalidInsByte
		dd	invalidInsByte
		dd	RegInst15
		dd	RegInst16
		dd	invalidInsByte
		dd	RegInst18
		dd	RegInst19
		dd	invalidInsByte
		dd	invalidInsByte
		dd	invalidInsByte
		dd	RegInst1d
		dd	RegInst1e
		dd	invalidInsByte
		dd	RegInst20
		dd	RegInst21
		dd	invalidInsByte
		dd	invalidInsByte
		dd	RegInst24
		dd	RegInst25
		dd	RegInst26
		dd	invalidInsByte
		dd	RegInst28
		dd	RegInst29
		dd	RegInst2a
		dd	invalidInsByte
		dd	RegInst2c
		dd	RegInst2d
		dd	RegInst2e
		dd	invalidInsByte
		dd	RegInst30
		dd	RegInst31
		dd	invalidInsByte
		dd	invalidInsByte
		dd	invalidInsByte
		dd	RegInst35
		dd	RegInst36
		dd	invalidInsByte
		dd	RegInst38
		dd	RegInst39
		dd	invalidInsByte
		dd	invalidInsByte
		dd	invalidInsByte
		dd	RegInst3d
		dd	RegInst3e
		dd	invalidInsByte
		dd	RegInst40
		dd	RegInst41
		dd	invalidInsByte
		dd	invalidInsByte
		dd	invalidInsByte
		dd	RegInst45
		dd	RegInst46
		dd	invalidInsByte
		dd	RegInst48
		dd	RegInst49
		dd	RegInst4a
		dd	invalidInsByte
		dd	RegInst4c
		dd	RegInst4d
		dd	RegInst4e
		dd	invalidInsByte
		dd	RegInst50
		dd	RegInst51
		dd	invalidInsByte
		dd	invalidInsByte
		dd	invalidInsByte
		dd	RegInst55
		dd	RegInst56
		dd	invalidInsByte
		dd	RegInst58
		dd	RegInst59
		dd	invalidInsByte
		dd	invalidInsByte
		dd	invalidInsByte
		dd	RegInst5d
		dd	RegInst5e
		dd	invalidInsByte
		dd	RegInst60
		dd	RegInst61
		dd	invalidInsByte
		dd	invalidInsByte
		dd	invalidInsByte
		dd	RegInst65
		dd	RegInst66
		dd	invalidInsByte
		dd	RegInst68
		dd	RegInst69
		dd	RegInst6a
		dd	invalidInsByte
		dd	RegInst6c
		dd	RegInst6d
		dd	RegInst6e
		dd	invalidInsByte
		dd	RegInst70
		dd	RegInst71
		dd	invalidInsByte
		dd	invalidInsByte
		dd	invalidInsByte
		dd	RegInst75
		dd	RegInst76
		dd	invalidInsByte
		dd	RegInst78
		dd	RegInst79
		dd	invalidInsByte
		dd	invalidInsByte
		dd	invalidInsByte
		dd	RegInst7d
		dd	RegInst7e
		dd	invalidInsByte
		dd	invalidInsByte
		dd	RegInst81
		dd	invalidInsByte
		dd	invalidInsByte
		dd	RegInst84
		dd	RegInst85
		dd	RegInst86
		dd	invalidInsByte
		dd	RegInst88
		dd	invalidInsByte
		dd	RegInst8a
		dd	invalidInsByte
		dd	RegInst8c
		dd	RegInst8d
		dd	RegInst8e
		dd	invalidInsByte
		dd	RegInst90
		dd	RegInst91
		dd	invalidInsByte
		dd	invalidInsByte
		dd	RegInst94
		dd	RegInst95
		dd	RegInst96
		dd	invalidInsByte
		dd	RegInst98
		dd	RegInst99
		dd	RegInst9a
		dd	invalidInsByte
		dd	invalidInsByte
		dd	RegInst9d
		dd	invalidInsByte
		dd	invalidInsByte
		dd	RegInsta0
		dd	RegInsta1
		dd	RegInsta2
		dd	invalidInsByte
		dd	RegInsta4
		dd	RegInsta5
		dd	RegInsta6
		dd	invalidInsByte
		dd	RegInsta8
		dd	RegInsta9
		dd	RegInstaa
		dd	invalidInsByte
		dd	RegInstac
		dd	RegInstad
		dd	RegInstae
		dd	invalidInsByte
		dd	RegInstb0
		dd	RegInstb1
		dd	invalidInsByte
		dd	invalidInsByte
		dd	RegInstb4
		dd	RegInstb5
		dd	RegInstb6
		dd	invalidInsByte
		dd	RegInstb8
		dd	RegInstb9
		dd	RegInstba
		dd	invalidInsByte
		dd	RegInstbc
		dd	RegInstbd
		dd	RegInstbe
		dd	invalidInsByte
		dd	RegInstc0
		dd	RegInstc1
		dd	invalidInsByte
		dd	invalidInsByte
		dd	RegInstc4
		dd	RegInstc5
		dd	RegInstc6
		dd	invalidInsByte
		dd	RegInstc8
		dd	RegInstc9
		dd	RegInstca
		dd	invalidInsByte
		dd	RegInstcc
		dd	RegInstcd
		dd	RegInstce
		dd	invalidInsByte
		dd	RegInstd0
		dd	RegInstd1
		dd	invalidInsByte
		dd	invalidInsByte
		dd	invalidInsByte
		dd	RegInstd5
		dd	RegInstd6
		dd	invalidInsByte
		dd	RegInstd8
		dd	RegInstd9
		dd	invalidInsByte
		dd	invalidInsByte
		dd	invalidInsByte
		dd	RegInstdd
		dd	RegInstde
		dd	invalidInsByte
		dd	RegInste0
		dd	RegInste1
		dd	invalidInsByte
		dd	invalidInsByte
		dd	RegInste4
		dd	RegInste5
		dd	RegInste6
		dd	invalidInsByte
		dd	RegInste8
		dd	RegInste9
		dd	RegInstea
		dd	invalidInsByte
		dd	RegInstec
		dd	RegInsted
		dd	RegInstee
		dd	invalidInsByte
		dd	RegInstf0
		dd	RegInstf1
		dd	invalidInsByte
		dd	invalidInsByte
		dd	invalidInsByte
		dd	RegInstf5
		dd	RegInstf6
		dd	invalidInsByte
		dd	RegInstf8
		dd	RegInstf9
		dd	invalidInsByte
		dd	invalidInsByte
		dd	invalidInsByte
		dd	RegInstfd
		dd	RegInstfe
		dd	invalidInsByte

		section	.text

times ($$-$) & 3 nop	; pad with NOPs to 4-byte boundary

; This is a generic read memory byte handler when a foreign
; handler is to be called

ReadMemoryByte:
		mov	[_m6502zpx], bl	; Save X
		mov	[_m6502zpy], cl	; Save Y
		mov	[_m6502zpaf], ax	; Save Accumulator & flags
		sub	esi, ebp	; Our program counter
		mov	[_m6502zppc], si	; Save our program counter
		push	edi	; Save our structure address
		and	edx, 0ffffh	; Only the lower 16 bits
		push	edx	; And our desired address
		call	dword [edi + 8]	; Go call our handler
		add	esp, 8	; Get the junk off the stack
		xor	ebx, ebx	; Zero X
		xor	ecx, ecx	; Zero Y
		xor	esi, esi	; Zero it!
		mov	si, [_m6502zppc]	; Get our program counter back
		mov	ebp, [_m6502zpBase] ; Base pointer comes back
		add	esi, ebp	; Rebase it properly
		ret

;
; BRK
;

times ($$-$) & 3 nop	; pad with NOPs to 4-byte boundary

RegInst00:
		sub	dword [cyclesRemaining], byte 7
		jc	near noMoreExec	; Can't execute anymore!
		add	dword [dwElapsedTicks], byte 7
		sub	esi, ebp	 ; Get our real program counter
		inc	si	; Move to the next position
		or	[_altFlags], byte 14h	; Indicate a break happened
		xor	edx, edx
		mov	dl, ah
		mov	ah, [bitx86to6502+edx]
		or		ah, [_altFlags]
		mov	dl, [_m6502zps]	; Get our stack pointer
		add	dx, 0ffh		; Stack area is 100-1ffh
		mov	[edx+ebp], si	; Store our PC
		dec	dx		; Back up for flag storage
		mov	[edx+ebp], ah	; Store our flags
		dec	dx		; Back up for flag storage
		mov	[_m6502zps], dl	; Store our new stack area
		xor	esi, esi	; Zero our program counter
		mov	si, [ebp+0fffeh]	; Get our break handler
		add	esi, ebp	; Add our base pointer back in
		xor	edx, edx
		mov	dl, ah
		mov	[_altFlags], dl
		and	[_altFlags], byte 3ch;
		mov	ah, [bit6502tox86+edx]
		xor	edx, edx
		mov	dl, [esi]
		inc	esi
		jmp	dword [m6502zpregular+edx*4]

;
; Ora
;

times ($$-$) & 3 nop	; pad with NOPs to 4-byte boundary

RegInst01:
		sub	dword [cyclesRemaining], byte 6
		jc	near noMoreExec	; Can't execute anymore!
		add	dword [dwElapsedTicks], byte 6
		mov	dl, [esi]		; Get the next instruction
		inc	esi		; Advance PC!
		add	dl, bl	; Add in X
		mov	dx, [ebp+edx]	; Get our 16 bit address
		mov	edi, [_m6502zpMemRead]	; Point to the read array

checkLoop2:
		cmp	[edi], word 0ffffh ; End of the list?
		je		short memoryRead2
		cmp	dx, [edi]	; Are we smaller?
		jb		short nextAddr2		; Yes, go to the next address
		cmp	dx, [edi+4]	; Are we bigger?
		jbe	short callRoutine2

nextAddr2:
		add	edi, 10h		; Next structure!
		jmp	short checkLoop2

callRoutine2:
		call	ReadMemoryByte	; Standard read routine
		mov	bl, [_m6502zpx]	; Get X back
		mov	cl, [_m6502zpy]	; Get Y back
		mov	dl, al	; Get our value
		mov	ax, [_m6502zpaf]	; Get our flags & stuff back
		jmp	short readExit2

memoryRead2:
		mov	dl, [ebp + edx]	; Get our data

readExit2:
		mov	dh, ah	; Get the flags
		or	al, dl		; OR In our value
		lahf			; Get the flag settings
		and	ah, 0c0h	; Only sign and zero flag
		and	dh, 03fh	; Clear sign & zero flags
		or	ah, dh		; Merge the affected flags together
		xor	edx, edx
		mov	dl, [esi]
		inc	esi
		jmp	dword [m6502zpregular+edx*4]

;
; Ora
;

times ($$-$) & 3 nop	; pad with NOPs to 4-byte boundary

RegInst05:
		sub	dword [cyclesRemaining], byte 3
		jc	near noMoreExec	; Can't execute anymore!
		add	dword [dwElapsedTicks], byte 3
		mov	dl, [esi]		; Get the next instruction
		inc	esi		; Advance PC!
		mov	dl, [ebp+edx]	; Get our zero page data
		mov	dh, ah	; Get the flags
		or	al, dl		; OR In our value
		lahf			; Get the flag settings
		and	ah, 0c0h	; Only sign and zero flag
		and	dh, 03fh	; Clear sign & zero flags
		or	ah, dh		; Merge the affected flags together
		xor	edx, edx
		mov	dl, [esi]
		inc	esi
		jmp	dword [m6502zpregular+edx*4]

;
; Asl
;

times ($$-$) & 3 nop	; pad with NOPs to 4-byte boundary

RegInst06:
		sub	dword [cyclesRemaining], byte 5
		jc	near noMoreExec	; Can't execute anymore!
		add	dword [dwElapsedTicks], byte 5
		mov	dl, [esi]		; Get the next instruction
		inc	esi		; Advance PC!
		mov	bh, [ebp+edx]	; Get our zero page stuff
		sahf		; Restore flags
		mov	ch, ah	; Store our original flags
		shl	bh, 1	; Shift left by 1
		lahf	; Load the flags back in
		and	ch, 03eh	; No carry, zero, or sign
		and	ah, 0c1h	; Only carry, zero and sign
		or	ah, ch		; Or it into our flags
		xor	ch, ch	; Clear it!
		mov	[ebp+edx], bh	; Set our zero page byte
		xor	bh, bh	; Prevent us from being hosed later
		xor	edx, edx
		mov	dl, [esi]
		inc	esi
		jmp	dword [m6502zpregular+edx*4]

;
; PHP
;

times ($$-$) & 3 nop	; pad with NOPs to 4-byte boundary

RegInst08:
		sub	dword [cyclesRemaining], byte 3
		jc	near noMoreExec	; Can't execute anymore!
		add	dword [dwElapsedTicks], byte 3
		xor	edx, edx
		mov	dl, ah
		mov	ah, [bitx86to6502+edx]
		or		ah, [_altFlags]
		inc	dh		; Stack page
		mov	dl, [_m6502zps]	; Stack pointer
		mov	[ebp+edx], ah
		dec	byte [_m6502zps]	; Decrement our stack pointer
		xor	edx, edx
		mov	dl, ah
		mov	[_altFlags], dl
		and	[_altFlags], byte 3ch;
		mov	ah, [bit6502tox86+edx]
		xor	edx, edx
		mov	dl, [esi]
		inc	esi
		jmp	dword [m6502zpregular+edx*4]

;
; Ora
;

times ($$-$) & 3 nop	; pad with NOPs to 4-byte boundary

RegInst09:
		sub	dword [cyclesRemaining], byte 2
		jc	near noMoreExec	; Can't execute anymore!
		add	dword [dwElapsedTicks], byte 2
		mov	dl, [esi]	; Get our next byte
		inc	esi		; Increment to our next byte
		mov	dh, ah	; Get the flags
		or	al, dl		; OR In our value
		lahf			; Get the flag settings
		and	ah, 0c0h	; Only sign and zero flag
		and	dh, 03fh	; Clear sign & zero flags
		or	ah, dh		; Merge the affected flags together
		xor	edx, edx
		mov	dl, [esi]
		inc	esi
		jmp	dword [m6502zpregular+edx*4]

;
; Asl
;

times ($$-$) & 3 nop	; pad with NOPs to 4-byte boundary

RegInst0a:
		sub	dword [cyclesRemaining], byte 2
		jc	near noMoreExec	; Can't execute anymore!
		add	dword [dwElapsedTicks], byte 2
		sahf		; Restore flags
		mov	dl, ah	; Store our original flags
		shl	al, 1	; Shift left by 1
		lahf	; Load the flags back in
		and	dl, 03eh	; No carry, zero, or sign
		and	ah, 0c1h	; Only carry, zero and sign
		or	ah, dl		; Or it into our flags
		xor	edx, edx
		mov	dl, [esi]
		inc	esi
		jmp	dword [m6502zpregular+edx*4]

;
; Ora
;

times ($$-$) & 3 nop	; pad with NOPs to 4-byte boundary

RegInst0d:
		sub	dword [cyclesRemaining], byte 4
		jc	near noMoreExec	; Can't execute anymore!
		add	dword [dwElapsedTicks], byte 4
		mov	dx, [esi]	; Get our address
		add	esi, 2	; Increment past instruction
		mov	edi, [_m6502zpMemRead]	; Point to the read array

checkLoop3:
		cmp	[edi], word 0ffffh ; End of the list?
		je		short memoryRead3
		cmp	dx, [edi]	; Are we smaller?
		jb		short nextAddr3		; Yes, go to the next address
		cmp	dx, [edi+4]	; Are we bigger?
		jbe	short callRoutine3

nextAddr3:
		add	edi, 10h		; Next structure!
		jmp	short checkLoop3

callRoutine3:
		call	ReadMemoryByte	; Standard read routine
		mov	bl, [_m6502zpx]	; Get X back
		mov	cl, [_m6502zpy]	; Get Y back
		mov	dl, al	; Get our value
		mov	ax, [_m6502zpaf]	; Get our flags & stuff back
		jmp	short readExit3

memoryRead3:
		mov	dl, [ebp + edx]	; Get our data

readExit3:
		mov	dh, ah	; Get the flags
		or	al, dl		; OR In our value
		lahf			; Get the flag settings
		and	ah, 0c0h	; Only sign and zero flag
		and	dh, 03fh	; Clear sign & zero flags
		or	ah, dh		; Merge the affected flags together
		xor	edx, edx
		mov	dl, [esi]
		inc	esi
		jmp	dword [m6502zpregular+edx*4]

;
; Asl
;

times ($$-$) & 3 nop	; pad with NOPs to 4-byte boundary

RegInst0e:
		sub	dword [cyclesRemaining], byte 6
		jc	near noMoreExec	; Can't execute anymore!
		add	dword [dwElapsedTicks], byte 6
		mov	dx, [esi]	; Get our address
		add	esi, 2	; Increment past instruction
		push	edx	; Save our address
		mov	edi, [_m6502zpMemRead]	; Point to the read array

checkLoop4:
		cmp	[edi], word 0ffffh ; End of the list?
		je		short memoryRead4
		cmp	dx, [edi]	; Are we smaller?
		jb		short nextAddr4		; Yes, go to the next address
		cmp	dx, [edi+4]	; Are we bigger?
		jbe	short callRoutine4

nextAddr4:
		add	edi, 10h		; Next structure!
		jmp	short checkLoop4

callRoutine4:
		call	ReadMemoryByte	; Standard read routine
		mov	bl, [_m6502zpx]	; Get X back
		mov	cl, [_m6502zpy]	; Get Y back
		mov	bh, al	; Get our value
		mov	ax, [_m6502zpaf]	; Get our flags & stuff back
		jmp	short readExit4

memoryRead4:
		mov	bh, [ebp + edx]	; Get our data

readExit4:
		pop	edx	; Restore our address
		sahf		; Restore flags
		mov	ch, ah	; Store our original flags
		shl	bh, 1	; Shift left by 1
		lahf	; Load the flags back in
		and	ch, 03eh	; No carry, zero, or sign
		and	ah, 0c1h	; Only carry, zero and sign
		or	ah, ch		; Or it into our flags
		xor	ch, ch	; Clear it!
		mov	edi, [_m6502zpMemWrite]	; Point to the write array

checkLoop5:
		cmp	[edi], word 0ffffh ; End of our list?
		je	near memoryWrite5	; Yes - go write it!
		cmp	dx, [edi]	; Are we smaller?
		jb	nextAddr5	; Yes... go to the next addr
		cmp	dx, [edi+4]	; Are we bigger?
		jbe	callRoutine5	; If not, go call it!

nextAddr5:
		add	edi, 10h		; Next structure, please
		jmp	short checkLoop5

callRoutine5:
		mov	[_m6502zpx], bl	; Save X
		mov	[_m6502zpy], cl	; Save Y
		mov	[_m6502zpaf], ax	; Save Accumulator & flags
		sub	esi, ebp	; Our program counter
		mov	[_m6502zppc], si	; Save our program counter
		push	edi	; Pointer to MemoryWriteByte structure
		mov	bl, bh	; Put a copy here
		push	ebx	; The byte value
		and	edx, 0ffffh	; Only lower 16 bits
		push	edx	; The address
		call	dword [edi + 8] ; Go call our handler
		add	esp, 12	; Get rid of our stack
		xor	ebx, ebx	; Zero this
		xor	ecx, ecx	; This too!
		mov	bl, [_m6502zpx]	; Get X back
		mov	cl, [_m6502zpy]	; Get Y back
		xor	esi, esi	; Zero it!
		mov	si, [_m6502zppc]	; Get our program counter back
		mov	ebp, [_m6502zpBase] ; Base pointer comes back
		add	esi, ebp	; Rebase it properly
		mov	ax, [_m6502zpaf]	; Get our flags & stuff back
		jmp	short writeMacroExit5

memoryWrite5:
		mov	[ebp + edx], bh ; Store the byte

writeMacroExit5:
		xor	bh, bh	; Prevent us from being hosed later
		xor	edx, edx
		mov	dl, [esi]
		inc	esi
		jmp	dword [m6502zpregular+edx*4]

;
; Branch
;

times ($$-$) & 3 nop	; pad with NOPs to 4-byte boundary

RegInst10:
		sub	dword [cyclesRemaining], byte 3
		jc	near noMoreExec	; Can't execute anymore!
		add	dword [dwElapsedTicks], byte 3
		test	ah, 80h	; Are we plus?
		jz		short takeJump10 ; Do it!
		inc	esi	; Skip past the offset
		xor	edx, edx
		mov	dl, [esi]
		inc	esi
		jmp	dword [m6502zpregular+edx*4]

takeJump10:
		mov	dl, [esi]		; Get the next instruction
		inc	esi		; Advance PC!
		movsx	dx, dl
		sub	esi, ebp
		add	si, dx
		add	esi, ebp
		xor	edx, edx
		mov	dl, [esi]
		inc	esi
		jmp	dword [m6502zpregular+edx*4]

;
; Ora
;

times ($$-$) & 3 nop	; pad with NOPs to 4-byte boundary

RegInst11:
		sub	dword [cyclesRemaining], byte 5
		jc	near noMoreExec	; Can't execute anymore!
		add	dword [dwElapsedTicks], byte 5
		mov	dl, [esi]		; Get the next instruction
		inc	esi		; Advance PC!
		mov	dx, [ebp+edx]	; Get our 16 bit address
		add	dx, cx	; Add in Y
		mov	edi, [_m6502zpMemRead]	; Point to the read array

checkLoop7:
		cmp	[edi], word 0ffffh ; End of the list?
		je		short memoryRead7
		cmp	dx, [edi]	; Are we smaller?
		jb		short nextAddr7		; Yes, go to the next address
		cmp	dx, [edi+4]	; Are we bigger?
		jbe	short callRoutine7

nextAddr7:
		add	edi, 10h		; Next structure!
		jmp	short checkLoop7

callRoutine7:
		call	ReadMemoryByte	; Standard read routine
		mov	bl, [_m6502zpx]	; Get X back
		mov	cl, [_m6502zpy]	; Get Y back
		mov	dl, al	; Get our value
		mov	ax, [_m6502zpaf]	; Get our flags & stuff back
		jmp	short readExit7

memoryRead7:
		mov	dl, [ebp + edx]	; Get our data

readExit7:
		mov	dh, ah	; Get the flags
		or	al, dl		; OR In our value
		lahf			; Get the flag settings
		and	ah, 0c0h	; Only sign and zero flag
		and	dh, 03fh	; Clear sign & zero flags
		or	ah, dh		; Merge the affected flags together
		xor	edx, edx
		mov	dl, [esi]
		inc	esi
		jmp	dword [m6502zpregular+edx*4]

;
; Ora
;

times ($$-$) & 3 nop	; pad with NOPs to 4-byte boundary

RegInst15:
		sub	dword [cyclesRemaining], byte 4
		jc	near noMoreExec	; Can't execute anymore!
		add	dword [dwElapsedTicks], byte 4
		mov	dl, [esi]		; Get the next instruction
		inc	esi		; Advance PC!
		add	dl, bl	; Add X
		mov	dl, [ebp+edx]	; Get our zero page data
		mov	dh, ah	; Get the flags
		or	al, dl		; OR In our value
		lahf			; Get the flag settings
		and	ah, 0c0h	; Only sign and zero flag
		and	dh, 03fh	; Clear sign & zero flags
		or	ah, dh		; Merge the affected flags together
		xor	edx, edx
		mov	dl, [esi]
		inc	esi
		jmp	dword [m6502zpregular+edx*4]

;
; Asl
;

times ($$-$) & 3 nop	; pad with NOPs to 4-byte boundary

RegInst16:
		sub	dword [cyclesRemaining], byte 6
		jc	near noMoreExec	; Can't execute anymore!
		add	dword [dwElapsedTicks], byte 6
		mov	dl, [esi]		; Get the next instruction
		inc	esi		; Advance PC!
		add	dl, bl	; Add X
		mov	bh, [ebp+edx]	; Get our zero page stuff
		sahf		; Restore flags
		mov	ch, ah	; Store our original flags
		shl	bh, 1	; Shift left by 1
		lahf	; Load the flags back in
		and	ch, 03eh	; No carry, zero, or sign
		and	ah, 0c1h	; Only carry, zero and sign
		or	ah, ch		; Or it into our flags
		xor	ch, ch	; Clear it!
		mov	[ebp+edx], bh	; Set our zero page byte
		xor	bh, bh	; Prevent us from being hosed later
		xor	edx, edx
		mov	dl, [esi]
		inc	esi
		jmp	dword [m6502zpregular+edx*4]

;
; CLC
;

times ($$-$) & 3 nop	; pad with NOPs to 4-byte boundary

RegInst18:
		sub	dword [cyclesRemaining], byte 2
		jc	near noMoreExec	; Can't execute anymore!
		add	dword [dwElapsedTicks], byte 2
		and	ah, 0feh	; No carry
		xor	edx, edx
		mov	dl, [esi]
		inc	esi
		jmp	dword [m6502zpregular+edx*4]

;
; Ora
;

times ($$-$) & 3 nop	; pad with NOPs to 4-byte boundary

RegInst19:
		sub	dword [cyclesRemaining], byte 4
		jc	near noMoreExec	; Can't execute anymore!
		add	dword [dwElapsedTicks], byte 4
		mov	dx, [esi]	; Get our address
		add	esi, 2	; Increment past instruction
		add	dx, cx	 ; Add Y
		mov	edi, [_m6502zpMemRead]	; Point to the read array

checkLoop8:
		cmp	[edi], word 0ffffh ; End of the list?
		je		short memoryRead8
		cmp	dx, [edi]	; Are we smaller?
		jb		short nextAddr8		; Yes, go to the next address
		cmp	dx, [edi+4]	; Are we bigger?
		jbe	short callRoutine8

nextAddr8:
		add	edi, 10h		; Next structure!
		jmp	short checkLoop8

callRoutine8:
		call	ReadMemoryByte	; Standard read routine
		mov	bl, [_m6502zpx]	; Get X back
		mov	cl, [_m6502zpy]	; Get Y back
		mov	dl, al	; Get our value
		mov	ax, [_m6502zpaf]	; Get our flags & stuff back
		jmp	short readExit8

memoryRead8:
		mov	dl, [ebp + edx]	; Get our data

readExit8:
		mov	dh, ah	; Get the flags
		or	al, dl		; OR In our value
		lahf			; Get the flag settings
		and	ah, 0c0h	; Only sign and zero flag
		and	dh, 03fh	; Clear sign & zero flags
		or	ah, dh		; Merge the affected flags together
		xor	edx, edx
		mov	dl, [esi]
		inc	esi
		jmp	dword [m6502zpregular+edx*4]

;
; Ora
;

times ($$-$) & 3 nop	; pad with NOPs to 4-byte boundary

RegInst1d:
		sub	dword [cyclesRemaining], byte 4
		jc	near noMoreExec	; Can't execute anymore!
		add	dword [dwElapsedTicks], byte 4
		mov	dx, [esi]	; Get our address
		add	esi, 2	; Increment past instruction
		add	dx, bx	 ; Add X
		mov	edi, [_m6502zpMemRead]	; Point to the read array

checkLoop9:
		cmp	[edi], word 0ffffh ; End of the list?
		je		short memoryRead9
		cmp	dx, [edi]	; Are we smaller?
		jb		short nextAddr9		; Yes, go to the next address
		cmp	dx, [edi+4]	; Are we bigger?
		jbe	short callRoutine9

nextAddr9:
		add	edi, 10h		; Next structure!
		jmp	short checkLoop9

callRoutine9:
		call	ReadMemoryByte	; Standard read routine
		mov	bl, [_m6502zpx]	; Get X back
		mov	cl, [_m6502zpy]	; Get Y back
		mov	dl, al	; Get our value
		mov	ax, [_m6502zpaf]	; Get our flags & stuff back
		jmp	short readExit9

memoryRead9:
		mov	dl, [ebp + edx]	; Get our data

readExit9:
		mov	dh, ah	; Get the flags
		or	al, dl		; OR In our value
		lahf			; Get the flag settings
		and	ah, 0c0h	; Only sign and zero flag
		and	dh, 03fh	; Clear sign & zero flags
		or	ah, dh		; Merge the affected flags together
		xor	edx, edx
		mov	dl, [esi]
		inc	esi
		jmp	dword [m6502zpregular+edx*4]

;
; Asl
;

times ($$-$) & 3 nop	; pad with NOPs to 4-byte boundary

RegInst1e:
		sub	dword [cyclesRemaining], byte 7
		jc	near noMoreExec	; Can't execute anymore!
		add	dword [dwElapsedTicks], byte 7
		mov	dx, [esi]	; Get our address
		add	esi, 2	; Increment past instruction
		add	dx, bx	 ; Add X
		push	edx	; Save our address
		mov	edi, [_m6502zpMemRead]	; Point to the read array

checkLoop10:
		cmp	[edi], word 0ffffh ; End of the list?
		je		short memoryRead10
		cmp	dx, [edi]	; Are we smaller?
		jb		short nextAddr10		; Yes, go to the next address
		cmp	dx, [edi+4]	; Are we bigger?
		jbe	short callRoutine10

nextAddr10:
		add	edi, 10h		; Next structure!
		jmp	short checkLoop10

callRoutine10:
		call	ReadMemoryByte	; Standard read routine
		mov	bl, [_m6502zpx]	; Get X back
		mov	cl, [_m6502zpy]	; Get Y back
		mov	bh, al	; Get our value
		mov	ax, [_m6502zpaf]	; Get our flags & stuff back
		jmp	short readExit10

memoryRead10:
		mov	bh, [ebp + edx]	; Get our data

readExit10:
		pop	edx	; Restore our address
		sahf		; Restore flags
		mov	ch, ah	; Store our original flags
		shl	bh, 1	; Shift left by 1
		lahf	; Load the flags back in
		and	ch, 03eh	; No carry, zero, or sign
		and	ah, 0c1h	; Only carry, zero and sign
		or	ah, ch		; Or it into our flags
		xor	ch, ch	; Clear it!
		mov	edi, [_m6502zpMemWrite]	; Point to the write array

checkLoop11:
		cmp	[edi], word 0ffffh ; End of our list?
		je	near memoryWrite11	; Yes - go write it!
		cmp	dx, [edi]	; Are we smaller?
		jb	nextAddr11	; Yes... go to the next addr
		cmp	dx, [edi+4]	; Are we bigger?
		jbe	callRoutine11	; If not, go call it!

nextAddr11:
		add	edi, 10h		; Next structure, please
		jmp	short checkLoop11

callRoutine11:
		mov	[_m6502zpx], bl	; Save X
		mov	[_m6502zpy], cl	; Save Y
		mov	[_m6502zpaf], ax	; Save Accumulator & flags
		sub	esi, ebp	; Our program counter
		mov	[_m6502zppc], si	; Save our program counter
		push	edi	; Pointer to MemoryWriteByte structure
		mov	bl, bh	; Put a copy here
		push	ebx	; The byte value
		and	edx, 0ffffh	; Only lower 16 bits
		push	edx	; The address
		call	dword [edi + 8] ; Go call our handler
		add	esp, 12	; Get rid of our stack
		xor	ebx, ebx	; Zero this
		xor	ecx, ecx	; This too!
		mov	bl, [_m6502zpx]	; Get X back
		mov	cl, [_m6502zpy]	; Get Y back
		xor	esi, esi	; Zero it!
		mov	si, [_m6502zppc]	; Get our program counter back
		mov	ebp, [_m6502zpBase] ; Base pointer comes back
		add	esi, ebp	; Rebase it properly
		mov	ax, [_m6502zpaf]	; Get our flags & stuff back
		jmp	short writeMacroExit11

memoryWrite11:
		mov	[ebp + edx], bh ; Store the byte

writeMacroExit11:
		xor	bh, bh	; Prevent us from being hosed later
		xor	edx, edx
		mov	dl, [esi]
		inc	esi
		jmp	dword [m6502zpregular+edx*4]

;
; JSR
;

times ($$-$) & 3 nop	; pad with NOPs to 4-byte boundary

RegInst20:
		sub	dword [cyclesRemaining], byte 6
		jc	near noMoreExec	; Can't execute anymore!
		add	dword [dwElapsedTicks], byte 6
		mov	dx, [esi]	; Get our address
		add	esi, 2	; Increment past instruction
		sub	esi, ebp	 ; Get our real program counter
		dec	si		; Our offset to return to
		sub	[_m6502zps], byte 2	; Back up 2 byte for stack push
		mov	di, word [_m6502zps]	; Our stack area
		and	edi, 0ffh	; Only the lower byte matters
		add	edi, 101h	; Make sure it's on the stack page
		mov	[edi+ebp], si	; Store our return address
		add	edx, ebp	; Our new address
		mov	esi, edx	; Put it in here for the fetch
		xor	edx, edx
		mov	dl, [esi]
		inc	esi
		jmp	dword [m6502zpregular+edx*4]

;
; AND
;

times ($$-$) & 3 nop	; pad with NOPs to 4-byte boundary

RegInst21:
		sub	dword [cyclesRemaining], byte 6
		jc	near noMoreExec	; Can't execute anymore!
		add	dword [dwElapsedTicks], byte 6
		mov	dl, [esi]		; Get the next instruction
		inc	esi		; Advance PC!
		add	dl, bl	; Add in X
		mov	dx, [ebp+edx]	; Get our 16 bit address
		mov	edi, [_m6502zpMemRead]	; Point to the read array

checkLoop13:
		cmp	[edi], word 0ffffh ; End of the list?
		je		short memoryRead13
		cmp	dx, [edi]	; Are we smaller?
		jb		short nextAddr13		; Yes, go to the next address
		cmp	dx, [edi+4]	; Are we bigger?
		jbe	short callRoutine13

nextAddr13:
		add	edi, 10h		; Next structure!
		jmp	short checkLoop13

callRoutine13:
		call	ReadMemoryByte	; Standard read routine
		mov	bl, [_m6502zpx]	; Get X back
		mov	cl, [_m6502zpy]	; Get Y back
		mov	dl, al	; Get our value
		mov	ax, [_m6502zpaf]	; Get our flags & stuff back
		jmp	short readExit13

memoryRead13:
		mov	dl, [ebp + edx]	; Get our data

readExit13:
		and	al, dl	; And it
		mov	bh, ah	; Save flags for later
		lahf			; Get the flags
		and	ah, 0c0h	; Only sign and zero flag
		and	bh, 03fh	; Kill sign and zero flag
		or	ah, bh	; Get our original (other) flags back
		xor	bh, bh	; Kill it so we don't screw X up for later
		xor	edx, edx
		mov	dl, [esi]
		inc	esi
		jmp	dword [m6502zpregular+edx*4]

;
; BIT
;

times ($$-$) & 3 nop	; pad with NOPs to 4-byte boundary

RegInst24:
		sub	dword [cyclesRemaining], byte 3
		jc	near noMoreExec	; Can't execute anymore!
		add	dword [dwElapsedTicks], byte 3
		mov	dl, [esi]		; Get the next instruction
		inc	esi		; Advance PC!
		mov	dl, [ebp+edx]	; Get our zero page byte
		mov	dh, dl	; Save this, too
		and	ah, 2fh	; Kill sign, zero, and overflow
		mov	bh, ah	; Put it here for later
		and	dl, al	; And this value with the accumulator
		lahf			; Get our flags
		and	ah, 040h	; Only the zero matters
		or		ah, bh	; Get our other original flags back
		mov	dl, dh	; Get our value back
		and	dl, 80h	; Only the negative flag
		or		ah, dl	; OR It in with the original
		and	dh, 40h	; What we do with the 6th bit
		shr	dh, 2		; Save this for later
		or		ah, dh	; OR In our "overflow"
		xor	bh, bh	; Zero this!
		xor	edx, edx
		mov	dl, [esi]
		inc	esi
		jmp	dword [m6502zpregular+edx*4]

;
; AND
;

times ($$-$) & 3 nop	; pad with NOPs to 4-byte boundary

RegInst25:
		sub	dword [cyclesRemaining], byte 3
		jc	near noMoreExec	; Can't execute anymore!
		add	dword [dwElapsedTicks], byte 3
		mov	dl, [esi]		; Get the next instruction
		inc	esi		; Advance PC!
		and	al, [ebp+edx]	; And it!
		mov	bh, ah	; Save flags for later
		lahf			; Get the flags
		and	ah, 0c0h	; Only sign and zero flag
		and	bh, 03fh	; Kill sign and zero flag
		or	ah, bh	; Get our original (other) flags back
		xor	bh, bh	; Kill it so we don't screw X up for later
		xor	edx, edx
		mov	dl, [esi]
		inc	esi
		jmp	dword [m6502zpregular+edx*4]

;
; ROL
;

times ($$-$) & 3 nop	; pad with NOPs to 4-byte boundary

RegInst26:
		sub	dword [cyclesRemaining], byte 5
		jc	near noMoreExec	; Can't execute anymore!
		add	dword [dwElapsedTicks], byte 5
		mov	dl, [esi]		; Get the next instruction
		inc	esi		; Advance PC!
		push	edx		; Save our address
		mov	dl, [ebp+edx]	; Get our zero page byte
		mov	dh, ah	; Save off our original flags
		and	dh, 3eh	; No carry, zero, or sign
		sahf		; Restore flags
		rcl	dl, 1	; Through carry rotate
		lahf		; Get the current flags back
		mov	bh, ah ; Store it here for later
		and	bh, 1	; Only the carry matters
		or		dl, dl	; Set sign/zero
		lahf		; Get the flags
		and	ah, 0c0h	; Only sign and zero
		or		ah, bh	; Or In our carry
		or		ah, dh	; Or in our original flags
		mov	bh, dl	; Get our byte to write
		pop	edx		; Restore the address
		mov	[ebp+edx], bh	; Set our zero page byte
		xor	bh, bh	; Zero this so we don't screw up things
		xor	edx, edx
		mov	dl, [esi]
		inc	esi
		jmp	dword [m6502zpregular+edx*4]

;
; PLP
;

times ($$-$) & 3 nop	; pad with NOPs to 4-byte boundary

RegInst28:
		sub	dword [cyclesRemaining], byte 4
		jc	near noMoreExec	; Can't execute anymore!
		add	dword [dwElapsedTicks], byte 4
		inc	dh		; Stack page
		inc	byte [_m6502zps]	; Increment our stack pointer
		mov	dl, [_m6502zps]	; Stack pointer
		mov	ah, [ebp+edx]
		xor	edx, edx
		mov	dl, ah
		mov	[_altFlags], dl
		and	[_altFlags], byte 3ch;
		mov	ah, [bit6502tox86+edx]
		xor	edx, edx
		mov	dl, [esi]
		inc	esi
		jmp	dword [m6502zpregular+edx*4]

;
; AND
;

times ($$-$) & 3 nop	; pad with NOPs to 4-byte boundary

RegInst29:
		sub	dword [cyclesRemaining], byte 2
		jc	near noMoreExec	; Can't execute anymore!
		add	dword [dwElapsedTicks], byte 2
		mov	dl, [esi]	; Get our next byte
		inc	esi		; Increment to our next byte
		and	al, dl	; And it
		mov	bh, ah	; Save flags for later
		lahf			; Get the flags
		and	ah, 0c0h	; Only sign and zero flag
		and	bh, 03fh	; Kill sign and zero flag
		or	ah, bh	; Get our original (other) flags back
		xor	bh, bh	; Kill it so we don't screw X up for later
		xor	edx, edx
		mov	dl, [esi]
		inc	esi
		jmp	dword [m6502zpregular+edx*4]

;
; ROL
;

times ($$-$) & 3 nop	; pad with NOPs to 4-byte boundary

RegInst2a:
		sub	dword [cyclesRemaining], byte 2
		jc	near noMoreExec	; Can't execute anymore!
		add	dword [dwElapsedTicks], byte 2
		mov	dh, ah	; Save off our original flags
		and	dh, 3eh	; No carry, zero, or sign
		sahf		; Restore flags
		rcl	al, 1	; Through carry rotate
		lahf		; Get the current flags back
		mov	bh, ah ; Store it here for later
		and	bh, 1	; Only the carry matters
		or		al, al	; Set sign/zero
		lahf		; Get the flags
		and	ah, 0c0h	; Only sign and zero
		or		ah, bh	; Or In our carry
		or		ah, dh	; Or in our original flags
		xor	bh, bh	; Zero this so we don't screw up things
		xor	edx, edx
		mov	dl, [esi]
		inc	esi
		jmp	dword [m6502zpregular+edx*4]

;
; BIT
;

times ($$-$) & 3 nop	; pad with NOPs to 4-byte boundary

RegInst2c:
		sub	dword [cyclesRemaining], byte 4
		jc	near noMoreExec	; Can't execute anymore!
		add	dword [dwElapsedTicks], byte 4
		mov	dx, [esi]	; Get our address
		add	esi, 2	; Increment past instruction
		mov	edi, [_m6502zpMemRead]	; Point to the read array

checkLoop14:
		cmp	[edi], word 0ffffh ; End of the list?
		je		short memoryRead14
		cmp	dx, [edi]	; Are we smaller?
		jb		short nextAddr14		; Yes, go to the next address
		cmp	dx, [edi+4]	; Are we bigger?
		jbe	short callRoutine14

nextAddr14:
		add	edi, 10h		; Next structure!
		jmp	short checkLoop14

callRoutine14:
		call	ReadMemoryByte	; Standard read routine
		mov	bl, [_m6502zpx]	; Get X back
		mov	cl, [_m6502zpy]	; Get Y back
		mov	dl, al	; Get our value
		mov	ax, [_m6502zpaf]	; Get our flags & stuff back
		jmp	short readExit14

memoryRead14:
		mov	dl, [ebp + edx]	; Get our data

readExit14:
		mov	dh, dl	; Save this, too
		and	ah, 2fh	; Kill sign, zero, and overflow
		mov	bh, ah	; Put it here for later
		and	dl, al	; And this value with the accumulator
		lahf			; Get our flags
		and	ah, 040h	; Only the zero matters
		or		ah, bh	; Get our other original flags back
		mov	dl, dh	; Get our value back
		and	dl, 80h	; Only the negative flag
		or		ah, dl	; OR It in with the original
		and	dh, 40h	; What we do with the 6th bit
		shr	dh, 2		; Save this for later
		or		ah, dh	; OR In our "overflow"
		xor	bh, bh	; Zero this!
		xor	edx, edx
		mov	dl, [esi]
		inc	esi
		jmp	dword [m6502zpregular+edx*4]

;
; AND
;

times ($$-$) & 3 nop	; pad with NOPs to 4-byte boundary

RegInst2d:
		sub	dword [cyclesRemaining], byte 4
		jc	near noMoreExec	; Can't execute anymore!
		add	dword [dwElapsedTicks], byte 4
		mov	dx, [esi]	; Get our address
		add	esi, 2	; Increment past instruction
		mov	edi, [_m6502zpMemRead]	; Point to the read array

checkLoop15:
		cmp	[edi], word 0ffffh ; End of the list?
		je		short memoryRead15
		cmp	dx, [edi]	; Are we smaller?
		jb		short nextAddr15		; Yes, go to the next address
		cmp	dx, [edi+4]	; Are we bigger?
		jbe	short callRoutine15

nextAddr15:
		add	edi, 10h		; Next structure!
		jmp	short checkLoop15

callRoutine15:
		call	ReadMemoryByte	; Standard read routine
		mov	bl, [_m6502zpx]	; Get X back
		mov	cl, [_m6502zpy]	; Get Y back
		mov	dl, al	; Get our value
		mov	ax, [_m6502zpaf]	; Get our flags & stuff back
		jmp	short readExit15

memoryRead15:
		mov	dl, [ebp + edx]	; Get our data

readExit15:
		and	al, dl	; And it
		mov	bh, ah	; Save flags for later
		lahf			; Get the flags
		and	ah, 0c0h	; Only sign and zero flag
		and	bh, 03fh	; Kill sign and zero flag
		or	ah, bh	; Get our original (other) flags back
		xor	bh, bh	; Kill it so we don't screw X up for later
		xor	edx, edx
		mov	dl, [esi]
		inc	esi
		jmp	dword [m6502zpregular+edx*4]

;
; ROL
;

times ($$-$) & 3 nop	; pad with NOPs to 4-byte boundary

RegInst2e:
		sub	dword [cyclesRemaining], byte 6
		jc	near noMoreExec	; Can't execute anymore!
		add	dword [dwElapsedTicks], byte 6
		mov	dx, [esi]	; Get our address
		add	esi, 2	; Increment past instruction
		push	edx		; Save our address
		mov	edi, [_m6502zpMemRead]	; Point to the read array

checkLoop16:
		cmp	[edi], word 0ffffh ; End of the list?
		je		short memoryRead16
		cmp	dx, [edi]	; Are we smaller?
		jb		short nextAddr16		; Yes, go to the next address
		cmp	dx, [edi+4]	; Are we bigger?
		jbe	short callRoutine16

nextAddr16:
		add	edi, 10h		; Next structure!
		jmp	short checkLoop16

callRoutine16:
		call	ReadMemoryByte	; Standard read routine
		mov	bl, [_m6502zpx]	; Get X back
		mov	cl, [_m6502zpy]	; Get Y back
		mov	dl, al	; Get our value
		mov	ax, [_m6502zpaf]	; Get our flags & stuff back
		jmp	short readExit16

memoryRead16:
		mov	dl, [ebp + edx]	; Get our data

readExit16:
		mov	dh, ah	; Save off our original flags
		and	dh, 3eh	; No carry, zero, or sign
		sahf		; Restore flags
		rcl	dl, 1	; Through carry rotate
		lahf		; Get the current flags back
		mov	bh, ah ; Store it here for later
		and	bh, 1	; Only the carry matters
		or		dl, dl	; Set sign/zero
		lahf		; Get the flags
		and	ah, 0c0h	; Only sign and zero
		or		ah, bh	; Or In our carry
		or		ah, dh	; Or in our original flags
		mov	bh, dl	; Get our byte to write
		pop	edx		; Restore the address
		mov	edi, [_m6502zpMemWrite]	; Point to the write array

checkLoop17:
		cmp	[edi], word 0ffffh ; End of our list?
		je	near memoryWrite17	; Yes - go write it!
		cmp	dx, [edi]	; Are we smaller?
		jb	nextAddr17	; Yes... go to the next addr
		cmp	dx, [edi+4]	; Are we bigger?
		jbe	callRoutine17	; If not, go call it!

nextAddr17:
		add	edi, 10h		; Next structure, please
		jmp	short checkLoop17

callRoutine17:
		mov	[_m6502zpx], bl	; Save X
		mov	[_m6502zpy], cl	; Save Y
		mov	[_m6502zpaf], ax	; Save Accumulator & flags
		sub	esi, ebp	; Our program counter
		mov	[_m6502zppc], si	; Save our program counter
		push	edi	; Pointer to MemoryWriteByte structure
		mov	bl, bh	; Put a copy here
		push	ebx	; The byte value
		and	edx, 0ffffh	; Only lower 16 bits
		push	edx	; The address
		call	dword [edi + 8] ; Go call our handler
		add	esp, 12	; Get rid of our stack
		xor	ebx, ebx	; Zero this
		xor	ecx, ecx	; This too!
		mov	bl, [_m6502zpx]	; Get X back
		mov	cl, [_m6502zpy]	; Get Y back
		xor	esi, esi	; Zero it!
		mov	si, [_m6502zppc]	; Get our program counter back
		mov	ebp, [_m6502zpBase] ; Base pointer comes back
		add	esi, ebp	; Rebase it properly
		mov	ax, [_m6502zpaf]	; Get our flags & stuff back
		jmp	short writeMacroExit17

memoryWrite17:
		mov	[ebp + edx], bh ; Store the byte

writeMacroExit17:
		xor	bh, bh	; Zero this so we don't screw up things
		xor	edx, edx
		mov	dl, [esi]
		inc	esi
		jmp	dword [m6502zpregular+edx*4]

;
; Branch
;

times ($$-$) & 3 nop	; pad with NOPs to 4-byte boundary

RegInst30:
		sub	dword [cyclesRemaining], byte 3
		jc	near noMoreExec	; Can't execute anymore!
		add	dword [dwElapsedTicks], byte 3
		test	ah, 80h	; Are we minus?
		jnz	short takeJump30	; Do it!
		inc	esi	; Skip past the offset
		xor	edx, edx
		mov	dl, [esi]
		inc	esi
		jmp	dword [m6502zpregular+edx*4]

takeJump30:
		mov	dl, [esi]		; Get the next instruction
		inc	esi		; Advance PC!
		movsx	dx, dl
		sub	esi, ebp
		add	si, dx
		add	esi, ebp
		xor	edx, edx
		mov	dl, [esi]
		inc	esi
		jmp	dword [m6502zpregular+edx*4]

;
; AND
;

times ($$-$) & 3 nop	; pad with NOPs to 4-byte boundary

RegInst31:
		sub	dword [cyclesRemaining], byte 5
		jc	near noMoreExec	; Can't execute anymore!
		add	dword [dwElapsedTicks], byte 5
		mov	dl, [esi]		; Get the next instruction
		inc	esi		; Advance PC!
		mov	dx, [ebp+edx]	; Get our 16 bit address
		add	dx, cx	; Add in Y
		mov	edi, [_m6502zpMemRead]	; Point to the read array

checkLoop19:
		cmp	[edi], word 0ffffh ; End of the list?
		je		short memoryRead19
		cmp	dx, [edi]	; Are we smaller?
		jb		short nextAddr19		; Yes, go to the next address
		cmp	dx, [edi+4]	; Are we bigger?
		jbe	short callRoutine19

nextAddr19:
		add	edi, 10h		; Next structure!
		jmp	short checkLoop19

callRoutine19:
		call	ReadMemoryByte	; Standard read routine
		mov	bl, [_m6502zpx]	; Get X back
		mov	cl, [_m6502zpy]	; Get Y back
		mov	dl, al	; Get our value
		mov	ax, [_m6502zpaf]	; Get our flags & stuff back
		jmp	short readExit19

memoryRead19:
		mov	dl, [ebp + edx]	; Get our data

readExit19:
		and	al, dl	; And it
		mov	bh, ah	; Save flags for later
		lahf			; Get the flags
		and	ah, 0c0h	; Only sign and zero flag
		and	bh, 03fh	; Kill sign and zero flag
		or	ah, bh	; Get our original (other) flags back
		xor	bh, bh	; Kill it so we don't screw X up for later
		xor	edx, edx
		mov	dl, [esi]
		inc	esi
		jmp	dword [m6502zpregular+edx*4]

;
; AND
;

times ($$-$) & 3 nop	; pad with NOPs to 4-byte boundary

RegInst35:
		sub	dword [cyclesRemaining], byte 4
		jc	near noMoreExec	; Can't execute anymore!
		add	dword [dwElapsedTicks], byte 4
		mov	dl, [esi]		; Get the next instruction
		inc	esi		; Advance PC!
		add	dl, bl	; Add X
		and	al, [ebp+edx]	; And it!
		mov	bh, ah	; Save flags for later
		lahf			; Get the flags
		and	ah, 0c0h	; Only sign and zero flag
		and	bh, 03fh	; Kill sign and zero flag
		or	ah, bh	; Get our original (other) flags back
		xor	bh, bh	; Kill it so we don't screw X up for later
		xor	edx, edx
		mov	dl, [esi]
		inc	esi
		jmp	dword [m6502zpregular+edx*4]

;
; ROL
;

times ($$-$) & 3 nop	; pad with NOPs to 4-byte boundary

RegInst36:
		sub	dword [cyclesRemaining], byte 6
		jc	near noMoreExec	; Can't execute anymore!
		add	dword [dwElapsedTicks], byte 6
		mov	dl, [esi]		; Get the next instruction
		inc	esi		; Advance PC!
		add	dl, bl	; Add X
		push	edx		; Save our address
		mov	dl, [ebp+edx]	; Get our zero page byte
		mov	dh, ah	; Save off our original flags
		and	dh, 3eh	; No carry, zero, or sign
		sahf		; Restore flags
		rcl	dl, 1	; Through carry rotate
		lahf		; Get the current flags back
		mov	bh, ah ; Store it here for later
		and	bh, 1	; Only the carry matters
		or		dl, dl	; Set sign/zero
		lahf		; Get the flags
		and	ah, 0c0h	; Only sign and zero
		or		ah, bh	; Or In our carry
		or		ah, dh	; Or in our original flags
		mov	bh, dl	; Get our byte to write
		pop	edx		; Restore the address
		mov	[ebp+edx], bh	; Set our zero page byte
		xor	bh, bh	; Zero this so we don't screw up things
		xor	edx, edx
		mov	dl, [esi]
		inc	esi
		jmp	dword [m6502zpregular+edx*4]

;
; SEC
;

times ($$-$) & 3 nop	; pad with NOPs to 4-byte boundary

RegInst38:
		sub	dword [cyclesRemaining], byte 2
		jc	near noMoreExec	; Can't execute anymore!
		add	dword [dwElapsedTicks], byte 2
		or	ah, 01h	; Carry!
		xor	edx, edx
		mov	dl, [esi]
		inc	esi
		jmp	dword [m6502zpregular+edx*4]

;
; AND
;

times ($$-$) & 3 nop	; pad with NOPs to 4-byte boundary

RegInst39:
		sub	dword [cyclesRemaining], byte 4
		jc	near noMoreExec	; Can't execute anymore!
		add	dword [dwElapsedTicks], byte 4
		mov	dx, [esi]	; Get our address
		add	esi, 2	; Increment past instruction
		add	dx, cx	 ; Add Y
		mov	edi, [_m6502zpMemRead]	; Point to the read array

checkLoop20:
		cmp	[edi], word 0ffffh ; End of the list?
		je		short memoryRead20
		cmp	dx, [edi]	; Are we smaller?
		jb		short nextAddr20		; Yes, go to the next address
		cmp	dx, [edi+4]	; Are we bigger?
		jbe	short callRoutine20

nextAddr20:
		add	edi, 10h		; Next structure!
		jmp	short checkLoop20

callRoutine20:
		call	ReadMemoryByte	; Standard read routine
		mov	bl, [_m6502zpx]	; Get X back
		mov	cl, [_m6502zpy]	; Get Y back
		mov	dl, al	; Get our value
		mov	ax, [_m6502zpaf]	; Get our flags & stuff back
		jmp	short readExit20

memoryRead20:
		mov	dl, [ebp + edx]	; Get our data

readExit20:
		and	al, dl	; And it
		mov	bh, ah	; Save flags for later
		lahf			; Get the flags
		and	ah, 0c0h	; Only sign and zero flag
		and	bh, 03fh	; Kill sign and zero flag
		or	ah, bh	; Get our original (other) flags back
		xor	bh, bh	; Kill it so we don't screw X up for later
		xor	edx, edx
		mov	dl, [esi]
		inc	esi
		jmp	dword [m6502zpregular+edx*4]

;
; AND
;

times ($$-$) & 3 nop	; pad with NOPs to 4-byte boundary

RegInst3d:
		sub	dword [cyclesRemaining], byte 4
		jc	near noMoreExec	; Can't execute anymore!
		add	dword [dwElapsedTicks], byte 4
		mov	dx, [esi]	; Get our address
		add	esi, 2	; Increment past instruction
		add	dx, bx	 ; Add X
		mov	edi, [_m6502zpMemRead]	; Point to the read array

checkLoop21:
		cmp	[edi], word 0ffffh ; End of the list?
		je		short memoryRead21
		cmp	dx, [edi]	; Are we smaller?
		jb		short nextAddr21		; Yes, go to the next address
		cmp	dx, [edi+4]	; Are we bigger?
		jbe	short callRoutine21

nextAddr21:
		add	edi, 10h		; Next structure!
		jmp	short checkLoop21

callRoutine21:
		call	ReadMemoryByte	; Standard read routine
		mov	bl, [_m6502zpx]	; Get X back
		mov	cl, [_m6502zpy]	; Get Y back
		mov	dl, al	; Get our value
		mov	ax, [_m6502zpaf]	; Get our flags & stuff back
		jmp	short readExit21

memoryRead21:
		mov	dl, [ebp + edx]	; Get our data

readExit21:
		and	al, dl	; And it
		mov	bh, ah	; Save flags for later
		lahf			; Get the flags
		and	ah, 0c0h	; Only sign and zero flag
		and	bh, 03fh	; Kill sign and zero flag
		or	ah, bh	; Get our original (other) flags back
		xor	bh, bh	; Kill it so we don't screw X up for later
		xor	edx, edx
		mov	dl, [esi]
		inc	esi
		jmp	dword [m6502zpregular+edx*4]

;
; ROL
;

times ($$-$) & 3 nop	; pad with NOPs to 4-byte boundary

RegInst3e:
		sub	dword [cyclesRemaining], byte 7
		jc	near noMoreExec	; Can't execute anymore!
		add	dword [dwElapsedTicks], byte 7
		mov	dx, [esi]	; Get our address
		add	esi, 2	; Increment past instruction
		add	dx, bx	 ; Add X
		push	edx		; Save our address
		mov	edi, [_m6502zpMemRead]	; Point to the read array

checkLoop22:
		cmp	[edi], word 0ffffh ; End of the list?
		je		short memoryRead22
		cmp	dx, [edi]	; Are we smaller?
		jb		short nextAddr22		; Yes, go to the next address
		cmp	dx, [edi+4]	; Are we bigger?
		jbe	short callRoutine22

nextAddr22:
		add	edi, 10h		; Next structure!
		jmp	short checkLoop22

callRoutine22:
		call	ReadMemoryByte	; Standard read routine
		mov	bl, [_m6502zpx]	; Get X back
		mov	cl, [_m6502zpy]	; Get Y back
		mov	dl, al	; Get our value
		mov	ax, [_m6502zpaf]	; Get our flags & stuff back
		jmp	short readExit22

memoryRead22:
		mov	dl, [ebp + edx]	; Get our data

readExit22:
		mov	dh, ah	; Save off our original flags
		and	dh, 3eh	; No carry, zero, or sign
		sahf		; Restore flags
		rcl	dl, 1	; Through carry rotate
		lahf		; Get the current flags back
		mov	bh, ah ; Store it here for later
		and	bh, 1	; Only the carry matters
		or		dl, dl	; Set sign/zero
		lahf		; Get the flags
		and	ah, 0c0h	; Only sign and zero
		or		ah, bh	; Or In our carry
		or		ah, dh	; Or in our original flags
		mov	bh, dl	; Get our byte to write
		pop	edx		; Restore the address
		mov	edi, [_m6502zpMemWrite]	; Point to the write array

checkLoop23:
		cmp	[edi], word 0ffffh ; End of our list?
		je	near memoryWrite23	; Yes - go write it!
		cmp	dx, [edi]	; Are we smaller?
		jb	nextAddr23	; Yes... go to the next addr
		cmp	dx, [edi+4]	; Are we bigger?
		jbe	callRoutine23	; If not, go call it!

nextAddr23:
		add	edi, 10h		; Next structure, please
		jmp	short checkLoop23

callRoutine23:
		mov	[_m6502zpx], bl	; Save X
		mov	[_m6502zpy], cl	; Save Y
		mov	[_m6502zpaf], ax	; Save Accumulator & flags
		sub	esi, ebp	; Our program counter
		mov	[_m6502zppc], si	; Save our program counter
		push	edi	; Pointer to MemoryWriteByte structure
		mov	bl, bh	; Put a copy here
		push	ebx	; The byte value
		and	edx, 0ffffh	; Only lower 16 bits
		push	edx	; The address
		call	dword [edi + 8] ; Go call our handler
		add	esp, 12	; Get rid of our stack
		xor	ebx, ebx	; Zero this
		xor	ecx, ecx	; This too!
		mov	bl, [_m6502zpx]	; Get X back
		mov	cl, [_m6502zpy]	; Get Y back
		xor	esi, esi	; Zero it!
		mov	si, [_m6502zppc]	; Get our program counter back
		mov	ebp, [_m6502zpBase] ; Base pointer comes back
		add	esi, ebp	; Rebase it properly
		mov	ax, [_m6502zpaf]	; Get our flags & stuff back
		jmp	short writeMacroExit23

memoryWrite23:
		mov	[ebp + edx], bh ; Store the byte

writeMacroExit23:
		xor	bh, bh	; Zero this so we don't screw up things
		xor	edx, edx
		mov	dl, [esi]
		inc	esi
		jmp	dword [m6502zpregular+edx*4]

;
; RTI
;

times ($$-$) & 3 nop	; pad with NOPs to 4-byte boundary

RegInst40:
		sub	dword [cyclesRemaining], byte 6
		jc	near noMoreExec	; Can't execute anymore!
		add	dword [dwElapsedTicks], byte 6
		mov	dl, [_m6502zps]	; Get our stack pointer
		add	[_m6502zps], byte 3	; Restore our stack
		inc	dh		; Increment to the stack page
		inc	dl		; And our pointer to the flags
		mov	ah, [ebp+edx]	; Get our flags
		inc	dl		; Next address
		mov	si, [ebp+edx]	; Get our new address
		mov	[_m6502zppc], si	; Store our program counter
		or	ah, 20h	; This bit is always 1
		mov	[_m6502zpaf + 1], ah	; Store our flags
		test	ah, 04h	; Interrupts reenabled?
		jnz	notEnabled
		cmp	[_irqPending], byte 0 ; IRQ pending?
		je		notEnabled
		push	eax		; Save this - we need it
		call	_m6502zpint	; Go do an interrupt
		pop	eax		; Restore this - we need it!
notEnabled:
		xor	esi, esi
		mov	si, [_m6502zppc]	; Get our program counter
		add	esi, ebp ; So it properly points to the code
		xor	edx, edx
		mov	dl, ah
		mov	[_altFlags], dl
		and	[_altFlags], byte 3ch;
		mov	ah, [bit6502tox86+edx]
		xor	edx, edx
		mov	dl, [esi]
		inc	esi
		jmp	dword [m6502zpregular+edx*4]

;
; Eor
;

times ($$-$) & 3 nop	; pad with NOPs to 4-byte boundary

RegInst41:
		sub	dword [cyclesRemaining], byte 6
		jc	near noMoreExec	; Can't execute anymore!
		add	dword [dwElapsedTicks], byte 6
		mov	dl, [esi]		; Get the next instruction
		inc	esi		; Advance PC!
		add	dl, bl	; Add in X
		mov	dx, [ebp+edx]	; Get our 16 bit address
		mov	edi, [_m6502zpMemRead]	; Point to the read array

checkLoop25:
		cmp	[edi], word 0ffffh ; End of the list?
		je		short memoryRead25
		cmp	dx, [edi]	; Are we smaller?
		jb		short nextAddr25		; Yes, go to the next address
		cmp	dx, [edi+4]	; Are we bigger?
		jbe	short callRoutine25

nextAddr25:
		add	edi, 10h		; Next structure!
		jmp	short checkLoop25

callRoutine25:
		call	ReadMemoryByte	; Standard read routine
		mov	bl, [_m6502zpx]	; Get X back
		mov	cl, [_m6502zpy]	; Get Y back
		mov	dl, al	; Get our value
		mov	ax, [_m6502zpaf]	; Get our flags & stuff back
		jmp	short readExit25

memoryRead25:
		mov	dl, [ebp + edx]	; Get our data

readExit25:
		mov	dh, ah	; Get the flags
		xor	al, dl		; XOR In our value
		lahf			; Get the flag settings
		and	ah, 0c0h	; Only sign and zero flag
		and	dh, 03fh	; Clear sign & zero flags
		or	ah, dh		; Merge the affected flags together
		xor	edx, edx
		mov	dl, [esi]
		inc	esi
		jmp	dword [m6502zpregular+edx*4]

;
; Eor
;

times ($$-$) & 3 nop	; pad with NOPs to 4-byte boundary

RegInst45:
		sub	dword [cyclesRemaining], byte 3
		jc	near noMoreExec	; Can't execute anymore!
		add	dword [dwElapsedTicks], byte 3
		mov	dl, [esi]		; Get the next instruction
		inc	esi		; Advance PC!
		mov	dl, [ebp+edx]	; Get our zero page data
		mov	dh, ah	; Get the flags
		xor	al, dl		; XOR In our value
		lahf			; Get the flag settings
		and	ah, 0c0h	; Only sign and zero flag
		and	dh, 03fh	; Clear sign & zero flags
		or	ah, dh		; Merge the affected flags together
		xor	edx, edx
		mov	dl, [esi]
		inc	esi
		jmp	dword [m6502zpregular+edx*4]

;
; Lsr
;

times ($$-$) & 3 nop	; pad with NOPs to 4-byte boundary

RegInst46:
		sub	dword [cyclesRemaining], byte 5
		jc	near noMoreExec	; Can't execute anymore!
		add	dword [dwElapsedTicks], byte 5
		mov	dl, [esi]		; Get the next instruction
		inc	esi		; Advance PC!
		push	edx		; Save our address away
		mov	bh, [ebp+edx]	; Get our zero page stuff
		sahf		; Restore flags
		mov	ch, ah	; Store our original flags
		shr	bh, 1	; Shift right by 1
		lahf	; Load the flags back in
		and	ch, 03eh	; No carry, zero, or sign
		and	ah, 0c1h	; Only carry, zero and sign
		or	ah, ch		; Or it into our flags
		xor	ch, ch	; Clear it!
		pop	edx	; Restore our address
		mov	[ebp+edx], bh	; Set our zero page byte
		xor	bh, bh	; Zero the upper part so we don't host X!
		xor	edx, edx
		mov	dl, [esi]
		inc	esi
		jmp	dword [m6502zpregular+edx*4]

;
; PHA
;

times ($$-$) & 3 nop	; pad with NOPs to 4-byte boundary

RegInst48:
		sub	dword [cyclesRemaining], byte 3
		jc	near noMoreExec	; Can't execute anymore!
		add	dword [dwElapsedTicks], byte 3
		inc	dh		; Stack page
		mov	dl, [_m6502zps]	; Stack pointer
		mov	[ebp+edx], al
		dec	byte [_m6502zps]	; Decrement our stack pointer
		xor	edx, edx
		mov	dl, [esi]
		inc	esi
		jmp	dword [m6502zpregular+edx*4]

;
; Eor
;

times ($$-$) & 3 nop	; pad with NOPs to 4-byte boundary

RegInst49:
		sub	dword [cyclesRemaining], byte 2
		jc	near noMoreExec	; Can't execute anymore!
		add	dword [dwElapsedTicks], byte 2
		mov	dl, [esi]	; Get our next byte
		inc	esi		; Increment to our next byte
		mov	dh, ah	; Get the flags
		xor	al, dl		; XOR In our value
		lahf			; Get the flag settings
		and	ah, 0c0h	; Only sign and zero flag
		and	dh, 03fh	; Clear sign & zero flags
		or	ah, dh		; Merge the affected flags together
		xor	edx, edx
		mov	dl, [esi]
		inc	esi
		jmp	dword [m6502zpregular+edx*4]

;
; Lsr
;

times ($$-$) & 3 nop	; pad with NOPs to 4-byte boundary

RegInst4a:
		sub	dword [cyclesRemaining], byte 2
		jc	near noMoreExec	; Can't execute anymore!
		add	dword [dwElapsedTicks], byte 2
		sahf		; Restore flags
		mov	dl, ah	; Store our original flags
		shr	al, 1	; Shift right by 1
		lahf	; Load the flags back in
		and	dl, 03eh	; No carry, zero, or sign
		and	ah, 0c1h	; Only carry, zero and sign
		or	ah, dl		; Or it into our flags
		xor	edx, edx
		mov	dl, [esi]
		inc	esi
		jmp	dword [m6502zpregular+edx*4]

;
; JMP
;

times ($$-$) & 3 nop	; pad with NOPs to 4-byte boundary

RegInst4c:
		sub	dword [cyclesRemaining], byte 3
		jc	near noMoreExec	; Can't execute anymore!
		add	dword [dwElapsedTicks], byte 3
		mov	dx, [esi]	; Get our address
		add	esi, 2	; Increment past instruction
		add	edx, ebp	; Add in our base
		mov	esi, edx	; Put it here for execution
		xor	edx, edx
		mov	dl, [esi]
		inc	esi
		jmp	dword [m6502zpregular+edx*4]

;
; Eor
;

times ($$-$) & 3 nop	; pad with NOPs to 4-byte boundary

RegInst4d:
		sub	dword [cyclesRemaining], byte 4
		jc	near noMoreExec	; Can't execute anymore!
		add	dword [dwElapsedTicks], byte 4
		mov	dx, [esi]	; Get our address
		add	esi, 2	; Increment past instruction
		mov	edi, [_m6502zpMemRead]	; Point to the read array

checkLoop27:
		cmp	[edi], word 0ffffh ; End of the list?
		je		short memoryRead27
		cmp	dx, [edi]	; Are we smaller?
		jb		short nextAddr27		; Yes, go to the next address
		cmp	dx, [edi+4]	; Are we bigger?
		jbe	short callRoutine27

nextAddr27:
		add	edi, 10h		; Next structure!
		jmp	short checkLoop27

callRoutine27:
		call	ReadMemoryByte	; Standard read routine
		mov	bl, [_m6502zpx]	; Get X back
		mov	cl, [_m6502zpy]	; Get Y back
		mov	dl, al	; Get our value
		mov	ax, [_m6502zpaf]	; Get our flags & stuff back
		jmp	short readExit27

memoryRead27:
		mov	dl, [ebp + edx]	; Get our data

readExit27:
		mov	dh, ah	; Get the flags
		xor	al, dl		; XOR In our value
		lahf			; Get the flag settings
		and	ah, 0c0h	; Only sign and zero flag
		and	dh, 03fh	; Clear sign & zero flags
		or	ah, dh		; Merge the affected flags together
		xor	edx, edx
		mov	dl, [esi]
		inc	esi
		jmp	dword [m6502zpregular+edx*4]

;
; Lsr
;

times ($$-$) & 3 nop	; pad with NOPs to 4-byte boundary

RegInst4e:
		sub	dword [cyclesRemaining], byte 6
		jc	near noMoreExec	; Can't execute anymore!
		add	dword [dwElapsedTicks], byte 6
		mov	dx, [esi]	; Get our address
		add	esi, 2	; Increment past instruction
		push	edx		; Save our address away
		mov	edi, [_m6502zpMemRead]	; Point to the read array

checkLoop28:
		cmp	[edi], word 0ffffh ; End of the list?
		je		short memoryRead28
		cmp	dx, [edi]	; Are we smaller?
		jb		short nextAddr28		; Yes, go to the next address
		cmp	dx, [edi+4]	; Are we bigger?
		jbe	short callRoutine28

nextAddr28:
		add	edi, 10h		; Next structure!
		jmp	short checkLoop28

callRoutine28:
		call	ReadMemoryByte	; Standard read routine
		mov	bl, [_m6502zpx]	; Get X back
		mov	cl, [_m6502zpy]	; Get Y back
		mov	bh, al	; Get our value
		mov	ax, [_m6502zpaf]	; Get our flags & stuff back
		jmp	short readExit28

memoryRead28:
		mov	bh, [ebp + edx]	; Get our data

readExit28:
		sahf		; Restore flags
		mov	ch, ah	; Store our original flags
		shr	bh, 1	; Shift right by 1
		lahf	; Load the flags back in
		and	ch, 03eh	; No carry, zero, or sign
		and	ah, 0c1h	; Only carry, zero and sign
		or	ah, ch		; Or it into our flags
		xor	ch, ch	; Clear it!
		pop	edx	; Restore our address
		mov	edi, [_m6502zpMemWrite]	; Point to the write array

checkLoop29:
		cmp	[edi], word 0ffffh ; End of our list?
		je	near memoryWrite29	; Yes - go write it!
		cmp	dx, [edi]	; Are we smaller?
		jb	nextAddr29	; Yes... go to the next addr
		cmp	dx, [edi+4]	; Are we bigger?
		jbe	callRoutine29	; If not, go call it!

nextAddr29:
		add	edi, 10h		; Next structure, please
		jmp	short checkLoop29

callRoutine29:
		mov	[_m6502zpx], bl	; Save X
		mov	[_m6502zpy], cl	; Save Y
		mov	[_m6502zpaf], ax	; Save Accumulator & flags
		sub	esi, ebp	; Our program counter
		mov	[_m6502zppc], si	; Save our program counter
		push	edi	; Pointer to MemoryWriteByte structure
		mov	bl, bh	; Put a copy here
		push	ebx	; The byte value
		and	edx, 0ffffh	; Only lower 16 bits
		push	edx	; The address
		call	dword [edi + 8] ; Go call our handler
		add	esp, 12	; Get rid of our stack
		xor	ebx, ebx	; Zero this
		xor	ecx, ecx	; This too!
		mov	bl, [_m6502zpx]	; Get X back
		mov	cl, [_m6502zpy]	; Get Y back
		xor	esi, esi	; Zero it!
		mov	si, [_m6502zppc]	; Get our program counter back
		mov	ebp, [_m6502zpBase] ; Base pointer comes back
		add	esi, ebp	; Rebase it properly
		mov	ax, [_m6502zpaf]	; Get our flags & stuff back
		jmp	short writeMacroExit29

memoryWrite29:
		mov	[ebp + edx], bh ; Store the byte

writeMacroExit29:
		xor	bh, bh	; Zero the upper part so we don't host X!
		xor	edx, edx
		mov	dl, [esi]
		inc	esi
		jmp	dword [m6502zpregular+edx*4]

;
; Branch
;

times ($$-$) & 3 nop	; pad with NOPs to 4-byte boundary

RegInst50:
		sub	dword [cyclesRemaining], byte 3
		jc	near noMoreExec	; Can't execute anymore!
		add	dword [dwElapsedTicks], byte 3
		test	ah, 10h	; Overflow not set?
		jz	short takeJump50 ; Do it!
		inc	esi	; Skip past the offset
		xor	edx, edx
		mov	dl, [esi]
		inc	esi
		jmp	dword [m6502zpregular+edx*4]

takeJump50:
		mov	dl, [esi]		; Get the next instruction
		inc	esi		; Advance PC!
		movsx	dx, dl
		sub	esi, ebp
		add	si, dx
		add	esi, ebp
		xor	edx, edx
		mov	dl, [esi]
		inc	esi
		jmp	dword [m6502zpregular+edx*4]

;
; Eor
;

times ($$-$) & 3 nop	; pad with NOPs to 4-byte boundary

RegInst51:
		sub	dword [cyclesRemaining], byte 5
		jc	near noMoreExec	; Can't execute anymore!
		add	dword [dwElapsedTicks], byte 5
		mov	dl, [esi]		; Get the next instruction
		inc	esi		; Advance PC!
		mov	dx, [ebp+edx]	; Get our 16 bit address
		add	dx, cx	; Add in Y
		mov	edi, [_m6502zpMemRead]	; Point to the read array

checkLoop31:
		cmp	[edi], word 0ffffh ; End of the list?
		je		short memoryRead31
		cmp	dx, [edi]	; Are we smaller?
		jb		short nextAddr31		; Yes, go to the next address
		cmp	dx, [edi+4]	; Are we bigger?
		jbe	short callRoutine31

nextAddr31:
		add	edi, 10h		; Next structure!
		jmp	short checkLoop31

callRoutine31:
		call	ReadMemoryByte	; Standard read routine
		mov	bl, [_m6502zpx]	; Get X back
		mov	cl, [_m6502zpy]	; Get Y back
		mov	dl, al	; Get our value
		mov	ax, [_m6502zpaf]	; Get our flags & stuff back
		jmp	short readExit31

memoryRead31:
		mov	dl, [ebp + edx]	; Get our data

readExit31:
		mov	dh, ah	; Get the flags
		xor	al, dl		; XOR In our value
		lahf			; Get the flag settings
		and	ah, 0c0h	; Only sign and zero flag
		and	dh, 03fh	; Clear sign & zero flags
		or	ah, dh		; Merge the affected flags together
		xor	edx, edx
		mov	dl, [esi]
		inc	esi
		jmp	dword [m6502zpregular+edx*4]

;
; Eor
;

times ($$-$) & 3 nop	; pad with NOPs to 4-byte boundary

RegInst55:
		sub	dword [cyclesRemaining], byte 4
		jc	near noMoreExec	; Can't execute anymore!
		add	dword [dwElapsedTicks], byte 4
		mov	dl, [esi]		; Get the next instruction
		inc	esi		; Advance PC!
		add	dl, bl	; Add X
		mov	dl, [ebp+edx]	; Get our zero page data
		mov	dh, ah	; Get the flags
		xor	al, dl		; XOR In our value
		lahf			; Get the flag settings
		and	ah, 0c0h	; Only sign and zero flag
		and	dh, 03fh	; Clear sign & zero flags
		or	ah, dh		; Merge the affected flags together
		xor	edx, edx
		mov	dl, [esi]
		inc	esi
		jmp	dword [m6502zpregular+edx*4]

;
; Lsr
;

times ($$-$) & 3 nop	; pad with NOPs to 4-byte boundary

RegInst56:
		sub	dword [cyclesRemaining], byte 6
		jc	near noMoreExec	; Can't execute anymore!
		add	dword [dwElapsedTicks], byte 6
		mov	dl, [esi]		; Get the next instruction
		inc	esi		; Advance PC!
		add	dl, bl	; Add X
		push	edx		; Save our address away
		mov	bh, [ebp+edx]	; Get our zero page stuff
		sahf		; Restore flags
		mov	ch, ah	; Store our original flags
		shr	bh, 1	; Shift right by 1
		lahf	; Load the flags back in
		and	ch, 03eh	; No carry, zero, or sign
		and	ah, 0c1h	; Only carry, zero and sign
		or	ah, ch		; Or it into our flags
		xor	ch, ch	; Clear it!
		pop	edx	; Restore our address
		mov	[ebp+edx], bh	; Set our zero page byte
		xor	bh, bh	; Zero the upper part so we don't host X!
		xor	edx, edx
		mov	dl, [esi]
		inc	esi
		jmp	dword [m6502zpregular+edx*4]

;
; CLI
;

times ($$-$) & 3 nop	; pad with NOPs to 4-byte boundary

RegInst58:
		sub	dword [cyclesRemaining], byte 2
		jc	near noMoreExec	; Can't execute anymore!
		add	dword [dwElapsedTicks], byte 2
		and	[_altFlags], byte 0fbh	; No interrupts!
		cmp	[_irqPending], byte 0 ; IRQ pending?
		je		near notEnabledCli
		sub	esi, ebp	 ; Get our real program counter
		mov	[_m6502zppc], si
		xor	edx, edx
		mov	dl, ah
		mov	ah, [bitx86to6502+edx]
		or		ah, [_altFlags]
		mov	[_m6502zpaf], ax	; Save this
		push	eax		; Save this - we need it
		call	_m6502zpint	; Go do an interrupt
		pop	eax		; Restore this - we need it!
		mov	si, [_m6502zppc]	; Get our program counter
		and	esi, 0ffffh	; Only the lower 16 bits
		add	esi, ebp ; So it properly points to the code
		mov	ax, [_m6502zpaf] ; Restore this
		xor	edx, edx
		mov	dl, ah
		mov	[_altFlags], dl
		and	[_altFlags], byte 3ch;
		mov	ah, [bit6502tox86+edx]
notEnabledCli:
		xor	edx, edx
		mov	dl, [esi]
		inc	esi
		jmp	dword [m6502zpregular+edx*4]

;
; Eor
;

times ($$-$) & 3 nop	; pad with NOPs to 4-byte boundary

RegInst59:
		sub	dword [cyclesRemaining], byte 4
		jc	near noMoreExec	; Can't execute anymore!
		add	dword [dwElapsedTicks], byte 4
		mov	dx, [esi]	; Get our address
		add	esi, 2	; Increment past instruction
		add	dx, cx	 ; Add Y
		mov	edi, [_m6502zpMemRead]	; Point to the read array

checkLoop32:
		cmp	[edi], word 0ffffh ; End of the list?
		je		short memoryRead32
		cmp	dx, [edi]	; Are we smaller?
		jb		short nextAddr32		; Yes, go to the next address
		cmp	dx, [edi+4]	; Are we bigger?
		jbe	short callRoutine32

nextAddr32:
		add	edi, 10h		; Next structure!
		jmp	short checkLoop32

callRoutine32:
		call	ReadMemoryByte	; Standard read routine
		mov	bl, [_m6502zpx]	; Get X back
		mov	cl, [_m6502zpy]	; Get Y back
		mov	dl, al	; Get our value
		mov	ax, [_m6502zpaf]	; Get our flags & stuff back
		jmp	short readExit32

memoryRead32:
		mov	dl, [ebp + edx]	; Get our data

readExit32:
		mov	dh, ah	; Get the flags
		xor	al, dl		; XOR In our value
		lahf			; Get the flag settings
		and	ah, 0c0h	; Only sign and zero flag
		and	dh, 03fh	; Clear sign & zero flags
		or	ah, dh		; Merge the affected flags together
		xor	edx, edx
		mov	dl, [esi]
		inc	esi
		jmp	dword [m6502zpregular+edx*4]

;
; Eor
;

times ($$-$) & 3 nop	; pad with NOPs to 4-byte boundary

RegInst5d:
		sub	dword [cyclesRemaining], byte 4
		jc	near noMoreExec	; Can't execute anymore!
		add	dword [dwElapsedTicks], byte 4
		mov	dx, [esi]	; Get our address
		add	esi, 2	; Increment past instruction
		add	dx, bx	 ; Add X
		mov	edi, [_m6502zpMemRead]	; Point to the read array

checkLoop33:
		cmp	[edi], word 0ffffh ; End of the list?
		je		short memoryRead33
		cmp	dx, [edi]	; Are we smaller?
		jb		short nextAddr33		; Yes, go to the next address
		cmp	dx, [edi+4]	; Are we bigger?
		jbe	short callRoutine33

nextAddr33:
		add	edi, 10h		; Next structure!
		jmp	short checkLoop33

callRoutine33:
		call	ReadMemoryByte	; Standard read routine
		mov	bl, [_m6502zpx]	; Get X back
		mov	cl, [_m6502zpy]	; Get Y back
		mov	dl, al	; Get our value
		mov	ax, [_m6502zpaf]	; Get our flags & stuff back
		jmp	short readExit33

memoryRead33:
		mov	dl, [ebp + edx]	; Get our data

readExit33:
		mov	dh, ah	; Get the flags
		xor	al, dl		; XOR In our value
		lahf			; Get the flag settings
		and	ah, 0c0h	; Only sign and zero flag
		and	dh, 03fh	; Clear sign & zero flags
		or	ah, dh		; Merge the affected flags together
		xor	edx, edx
		mov	dl, [esi]
		inc	esi
		jmp	dword [m6502zpregular+edx*4]

;
; Lsr
;

times ($$-$) & 3 nop	; pad with NOPs to 4-byte boundary

RegInst5e:
		sub	dword [cyclesRemaining], byte 7
		jc	near noMoreExec	; Can't execute anymore!
		add	dword [dwElapsedTicks], byte 7
		mov	dx, [esi]	; Get our address
		add	esi, 2	; Increment past instruction
		add	dx, bx	 ; Add X
		push	edx		; Save our address away
		mov	edi, [_m6502zpMemRead]	; Point to the read array

checkLoop34:
		cmp	[edi], word 0ffffh ; End of the list?
		je		short memoryRead34
		cmp	dx, [edi]	; Are we smaller?
		jb		short nextAddr34		; Yes, go to the next address
		cmp	dx, [edi+4]	; Are we bigger?
		jbe	short callRoutine34

nextAddr34:
		add	edi, 10h		; Next structure!
		jmp	short checkLoop34

callRoutine34:
		call	ReadMemoryByte	; Standard read routine
		mov	bl, [_m6502zpx]	; Get X back
		mov	cl, [_m6502zpy]	; Get Y back
		mov	bh, al	; Get our value
		mov	ax, [_m6502zpaf]	; Get our flags & stuff back
		jmp	short readExit34

memoryRead34:
		mov	bh, [ebp + edx]	; Get our data

readExit34:
		sahf		; Restore flags
		mov	ch, ah	; Store our original flags
		shr	bh, 1	; Shift right by 1
		lahf	; Load the flags back in
		and	ch, 03eh	; No carry, zero, or sign
		and	ah, 0c1h	; Only carry, zero and sign
		or	ah, ch		; Or it into our flags
		xor	ch, ch	; Clear it!
		pop	edx	; Restore our address
		mov	edi, [_m6502zpMemWrite]	; Point to the write array

checkLoop35:
		cmp	[edi], word 0ffffh ; End of our list?
		je	near memoryWrite35	; Yes - go write it!
		cmp	dx, [edi]	; Are we smaller?
		jb	nextAddr35	; Yes... go to the next addr
		cmp	dx, [edi+4]	; Are we bigger?
		jbe	callRoutine35	; If not, go call it!

nextAddr35:
		add	edi, 10h		; Next structure, please
		jmp	short checkLoop35

callRoutine35:
		mov	[_m6502zpx], bl	; Save X
		mov	[_m6502zpy], cl	; Save Y
		mov	[_m6502zpaf], ax	; Save Accumulator & flags
		sub	esi, ebp	; Our program counter
		mov	[_m6502zppc], si	; Save our program counter
		push	edi	; Pointer to MemoryWriteByte structure
		mov	bl, bh	; Put a copy here
		push	ebx	; The byte value
		and	edx, 0ffffh	; Only lower 16 bits
		push	edx	; The address
		call	dword [edi + 8] ; Go call our handler
		add	esp, 12	; Get rid of our stack
		xor	ebx, ebx	; Zero this
		xor	ecx, ecx	; This too!
		mov	bl, [_m6502zpx]	; Get X back
		mov	cl, [_m6502zpy]	; Get Y back
		xor	esi, esi	; Zero it!
		mov	si, [_m6502zppc]	; Get our program counter back
		mov	ebp, [_m6502zpBase] ; Base pointer comes back
		add	esi, ebp	; Rebase it properly
		mov	ax, [_m6502zpaf]	; Get our flags & stuff back
		jmp	short writeMacroExit35

memoryWrite35:
		mov	[ebp + edx], bh ; Store the byte

writeMacroExit35:
		xor	bh, bh	; Zero the upper part so we don't host X!
		xor	edx, edx
		mov	dl, [esi]
		inc	esi
		jmp	dword [m6502zpregular+edx*4]

;
; RTS
;

times ($$-$) & 3 nop	; pad with NOPs to 4-byte boundary

RegInst60:
		sub	dword [cyclesRemaining], byte 6
		jc	near noMoreExec	; Can't execute anymore!
		add	dword [dwElapsedTicks], byte 6
		xor	esi, esi	; Zero ESI for later
		mov	dl, [_m6502zps]	; Get our stack
		add	[_m6502zps], byte 2	; Pop off a word
		inc	dl	; Increment our stack page
		inc	dh	; Our stack page
		mov	si, [ebp+edx]	; Get our stack area
		inc	si	; Increment!
		add	esi, ebp	; Add in our base address
		xor	edx, edx
		mov	dl, [esi]
		inc	esi
		jmp	dword [m6502zpregular+edx*4]

;
; ADC
;

times ($$-$) & 3 nop	; pad with NOPs to 4-byte boundary

RegInst61:
		sub	dword [cyclesRemaining], byte 6
		jc	near noMoreExec	; Can't execute anymore!
		add	dword [dwElapsedTicks], byte 6
		mov	dl, [esi]		; Get the next instruction
		inc	esi		; Advance PC!
		add	dl, bl	; Add in X
		mov	dx, [ebp+edx]	; Get our 16 bit address
		mov	edi, [_m6502zpMemRead]	; Point to the read array

checkLoop37:
		cmp	[edi], word 0ffffh ; End of the list?
		je		short memoryRead37
		cmp	dx, [edi]	; Are we smaller?
		jb		short nextAddr37		; Yes, go to the next address
		cmp	dx, [edi+4]	; Are we bigger?
		jbe	short callRoutine37

nextAddr37:
		add	edi, 10h		; Next structure!
		jmp	short checkLoop37

callRoutine37:
		call	ReadMemoryByte	; Standard read routine
		mov	bl, [_m6502zpx]	; Get X back
		mov	cl, [_m6502zpy]	; Get Y back
		mov	dl, al	; Get our value
		mov	ax, [_m6502zpaf]	; Get our flags & stuff back
		jmp	short readExit37

memoryRead37:
		mov	dl, [ebp + edx]	; Get our data

readExit37:
		sahf		; Restore our flags for the adc
		adc	al, dl	; Add in our value
		o16 pushf	; Push our flags (and overflow)
		and	ah, 02eh	; No carry, overflow, zero or sign
		pop	dx	; Restore our flags into DX
		shl	dh, 1	; Shift overflow into position
		and	dh, 10h	; Only the overflow
		and	dl, 0c1h	; Only carry, sign, and zero
		or		ah, dl	; OR In our new flags
		or		ah, dh	; OR In overflow
		xor	edx, edx
		mov	dl, [esi]
		inc	esi
		jmp	dword [m6502zpregular+edx*4]

;
; ADC
;

times ($$-$) & 3 nop	; pad with NOPs to 4-byte boundary

RegInst65:
		sub	dword [cyclesRemaining], byte 3
		jc	near noMoreExec	; Can't execute anymore!
		add	dword [dwElapsedTicks], byte 3
		mov	dl, [esi]		; Get the next instruction
		inc	esi		; Advance PC!
		mov	dl, [ebp+edx]	; Get our zero page data
		sahf		; Restore our flags for the adc
		adc	al, dl	; Add in our value
		o16 pushf	; Push our flags (and overflow)
		and	ah, 02eh	; No carry, overflow, zero or sign
		pop	dx	; Restore our flags into DX
		shl	dh, 1	; Shift overflow into position
		and	dh, 10h	; Only the overflow
		and	dl, 0c1h	; Only carry, sign, and zero
		or		ah, dl	; OR In our new flags
		or		ah, dh	; OR In overflow
		xor	edx, edx
		mov	dl, [esi]
		inc	esi
		jmp	dword [m6502zpregular+edx*4]

;
; ROR
;

times ($$-$) & 3 nop	; pad with NOPs to 4-byte boundary

RegInst66:
		sub	dword [cyclesRemaining], byte 5
		jc	near noMoreExec	; Can't execute anymore!
		add	dword [dwElapsedTicks], byte 5
		mov	dl, [esi]		; Get the next instruction
		inc	esi		; Advance PC!
		push	edx		; Save our address
		mov	dl, [ebp+edx]	; Get our zero page byte
		mov	dh, ah	; Save off our original flags
		and	dh, 3eh	; No carry, zero, or sign
		sahf		; Restore flags
		rcr	dl, 1	; Through carry rotate
		lahf		; Get the current flags back
		mov	bh, ah ; Store it here for later
		and	bh, 1	; Only the carry matters
		or		dl, dl	; Set sign/zero
		lahf		; Get the flags
		and	ah, 0c0h	; Only sign and zero
		or		ah, bh	; Or In our carry
		or		ah, dh	; Or in our original flags
		mov	bh, dl	; Get our byte to write
		pop	edx		; Restore the address
		mov	[ebp+edx], bh	; Set our zero page byte
		xor	bh, bh	; Zero this so we don't screw up things
		xor	edx, edx
		mov	dl, [esi]
		inc	esi
		jmp	dword [m6502zpregular+edx*4]

;
; PLA
;

times ($$-$) & 3 nop	; pad with NOPs to 4-byte boundary

RegInst68:
		sub	dword [cyclesRemaining], byte 4
		jc	near noMoreExec	; Can't execute anymore!
		add	dword [dwElapsedTicks], byte 4
		inc	dh		; Stack page
		inc	byte [_m6502zps]	; Increment our stack pointer
		mov	dl, [_m6502zps]	; Stack pointer
		mov	al, [ebp+edx]
		mov	dl, ah
		and	dl, 03fh
		or		al, al
		lahf
		and	ah, 0c0h
		or		ah, dl
		xor	edx, edx
		mov	dl, [esi]
		inc	esi
		jmp	dword [m6502zpregular+edx*4]

;
; ADC
;

times ($$-$) & 3 nop	; pad with NOPs to 4-byte boundary

RegInst69:
		sub	dword [cyclesRemaining], byte 2
		jc	near noMoreExec	; Can't execute anymore!
		add	dword [dwElapsedTicks], byte 2
		mov	dl, [esi]	; Get our next byte
		inc	esi		; Increment to our next byte
		sahf		; Restore our flags for the adc
		adc	al, dl	; Add in our value
		o16 pushf	; Push our flags (and overflow)
		and	ah, 02eh	; No carry, overflow, zero or sign
		pop	dx	; Restore our flags into DX
		shl	dh, 1	; Shift overflow into position
		and	dh, 10h	; Only the overflow
		and	dl, 0c1h	; Only carry, sign, and zero
		or		ah, dl	; OR In our new flags
		or		ah, dh	; OR In overflow
		xor	edx, edx
		mov	dl, [esi]
		inc	esi
		jmp	dword [m6502zpregular+edx*4]

;
; ROR
;

times ($$-$) & 3 nop	; pad with NOPs to 4-byte boundary

RegInst6a:
		sub	dword [cyclesRemaining], byte 2
		jc	near noMoreExec	; Can't execute anymore!
		add	dword [dwElapsedTicks], byte 2
		mov	dh, ah	; Save off our original flags
		and	dh, 3eh	; No carry, zero, or sign
		sahf		; Restore flags
		rcr	al, 1	; Through carry rotate
		lahf		; Get the current flags back
		mov	bh, ah ; Store it here for later
		and	bh, 1	; Only the carry matters
		or		al, al	; Set sign/zero
		lahf		; Get the flags
		and	ah, 0c0h	; Only sign and zero
		or		ah, bh	; Or In our carry
		or		ah, dh	; Or in our original flags
		xor	bh, bh	; Zero this so we don't screw up things
		xor	edx, edx
		mov	dl, [esi]
		inc	esi
		jmp	dword [m6502zpregular+edx*4]

;
; JMP
;

times ($$-$) & 3 nop	; pad with NOPs to 4-byte boundary

RegInst6c:
		sub	dword [cyclesRemaining], byte 5
		jc	near noMoreExec	; Can't execute anymore!
		add	dword [dwElapsedTicks], byte 5
		mov	dx, [esi]	; Get our address
		add	esi, 2	; Increment past instruction
		push	edx	; Save address
		mov	[_m6502zpaf], ax	; Store AF
		mov	edi, [_m6502zpMemRead]	; Point to the read array

checkLoop38:
		cmp	[edi], word 0ffffh ; End of the list?
		je		short memoryRead38
		cmp	dx, [edi]	; Are we smaller?
		jb		short nextAddr38		; Yes, go to the next address
		cmp	dx, [edi+4]	; Are we bigger?
		jbe	short callRoutine38

nextAddr38:
		add	edi, 10h		; Next structure!
		jmp	short checkLoop38

callRoutine38:
		call	ReadMemoryByte	; Standard read routine
		mov	bl, [_m6502zpx]	; Get X back
		mov	cl, [_m6502zpy]	; Get Y back
		mov	ah, [_m6502zpaf + 1] ; Get our flags back
		jmp	short readExit38

memoryRead38:
		mov	al, [ebp + edx]	; Get our data

readExit38:
		pop	edx	; Restore address
		inc	dx	; Next address
		push	eax	; Save it for later
		mov	ax, [_m6502zpaf]	; Restore AF because it gets used later
		mov	edi, [_m6502zpMemRead]	; Point to the read array

checkLoop39:
		cmp	[edi], word 0ffffh ; End of the list?
		je		short memoryRead39
		cmp	dx, [edi]	; Are we smaller?
		jb		short nextAddr39		; Yes, go to the next address
		cmp	dx, [edi+4]	; Are we bigger?
		jbe	short callRoutine39

nextAddr39:
		add	edi, 10h		; Next structure!
		jmp	short checkLoop39

callRoutine39:
		call	ReadMemoryByte	; Standard read routine
		mov	bl, [_m6502zpx]	; Get X back
		mov	cl, [_m6502zpy]	; Get Y back
		mov	dh, al	; Get our value
		mov	ax, [_m6502zpaf]	; Get our flags & stuff back
		jmp	short readExit39

memoryRead39:
		mov	dh, [ebp + edx]	; Get our data

readExit39:
		pop	eax	; Restore it!
		mov	dl, al	; Restore our word into DX
		mov	ax, [_m6502zpaf]	; Restore AF
		add	edx, ebp	; Add in our base
		mov	esi, edx	; Put it here for execution
		xor	edx, edx
		mov	dl, [esi]
		inc	esi
		jmp	dword [m6502zpregular+edx*4]

;
; ADC
;

times ($$-$) & 3 nop	; pad with NOPs to 4-byte boundary

RegInst6d:
		sub	dword [cyclesRemaining], byte 4
		jc	near noMoreExec	; Can't execute anymore!
		add	dword [dwElapsedTicks], byte 4
		mov	dx, [esi]	; Get our address
		add	esi, 2	; Increment past instruction
		mov	edi, [_m6502zpMemRead]	; Point to the read array

checkLoop41:
		cmp	[edi], word 0ffffh ; End of the list?
		je		short memoryRead41
		cmp	dx, [edi]	; Are we smaller?
		jb		short nextAddr41		; Yes, go to the next address
		cmp	dx, [edi+4]	; Are we bigger?
		jbe	short callRoutine41

nextAddr41:
		add	edi, 10h		; Next structure!
		jmp	short checkLoop41

callRoutine41:
		call	ReadMemoryByte	; Standard read routine
		mov	bl, [_m6502zpx]	; Get X back
		mov	cl, [_m6502zpy]	; Get Y back
		mov	dl, al	; Get our value
		mov	ax, [_m6502zpaf]	; Get our flags & stuff back
		jmp	short readExit41

memoryRead41:
		mov	dl, [ebp + edx]	; Get our data

readExit41:
		sahf		; Restore our flags for the adc
		adc	al, dl	; Add in our value
		o16 pushf	; Push our flags (and overflow)
		and	ah, 02eh	; No carry, overflow, zero or sign
		pop	dx	; Restore our flags into DX
		shl	dh, 1	; Shift overflow into position
		and	dh, 10h	; Only the overflow
		and	dl, 0c1h	; Only carry, sign, and zero
		or		ah, dl	; OR In our new flags
		or		ah, dh	; OR In overflow
		xor	edx, edx
		mov	dl, [esi]
		inc	esi
		jmp	dword [m6502zpregular+edx*4]

;
; ROR
;

times ($$-$) & 3 nop	; pad with NOPs to 4-byte boundary

RegInst6e:
		sub	dword [cyclesRemaining], byte 6
		jc	near noMoreExec	; Can't execute anymore!
		add	dword [dwElapsedTicks], byte 6
		mov	dx, [esi]	; Get our address
		add	esi, 2	; Increment past instruction
		push	edx		; Save our address
		mov	edi, [_m6502zpMemRead]	; Point to the read array

checkLoop42:
		cmp	[edi], word 0ffffh ; End of the list?
		je		short memoryRead42
		cmp	dx, [edi]	; Are we smaller?
		jb		short nextAddr42		; Yes, go to the next address
		cmp	dx, [edi+4]	; Are we bigger?
		jbe	short callRoutine42

nextAddr42:
		add	edi, 10h		; Next structure!
		jmp	short checkLoop42

callRoutine42:
		call	ReadMemoryByte	; Standard read routine
		mov	bl, [_m6502zpx]	; Get X back
		mov	cl, [_m6502zpy]	; Get Y back
		mov	dl, al	; Get our value
		mov	ax, [_m6502zpaf]	; Get our flags & stuff back
		jmp	short readExit42

memoryRead42:
		mov	dl, [ebp + edx]	; Get our data

readExit42:
		mov	dh, ah	; Save off our original flags
		and	dh, 3eh	; No carry, zero, or sign
		sahf		; Restore flags
		rcr	dl, 1	; Through carry rotate
		lahf		; Get the current flags back
		mov	bh, ah ; Store it here for later
		and	bh, 1	; Only the carry matters
		or		dl, dl	; Set sign/zero
		lahf		; Get the flags
		and	ah, 0c0h	; Only sign and zero
		or		ah, bh	; Or In our carry
		or		ah, dh	; Or in our original flags
		mov	bh, dl	; Get our byte to write
		pop	edx		; Restore the address
		mov	edi, [_m6502zpMemWrite]	; Point to the write array

checkLoop43:
		cmp	[edi], word 0ffffh ; End of our list?
		je	near memoryWrite43	; Yes - go write it!
		cmp	dx, [edi]	; Are we smaller?
		jb	nextAddr43	; Yes... go to the next addr
		cmp	dx, [edi+4]	; Are we bigger?
		jbe	callRoutine43	; If not, go call it!

nextAddr43:
		add	edi, 10h		; Next structure, please
		jmp	short checkLoop43

callRoutine43:
		mov	[_m6502zpx], bl	; Save X
		mov	[_m6502zpy], cl	; Save Y
		mov	[_m6502zpaf], ax	; Save Accumulator & flags
		sub	esi, ebp	; Our program counter
		mov	[_m6502zppc], si	; Save our program counter
		push	edi	; Pointer to MemoryWriteByte structure
		mov	bl, bh	; Put a copy here
		push	ebx	; The byte value
		and	edx, 0ffffh	; Only lower 16 bits
		push	edx	; The address
		call	dword [edi + 8] ; Go call our handler
		add	esp, 12	; Get rid of our stack
		xor	ebx, ebx	; Zero this
		xor	ecx, ecx	; This too!
		mov	bl, [_m6502zpx]	; Get X back
		mov	cl, [_m6502zpy]	; Get Y back
		xor	esi, esi	; Zero it!
		mov	si, [_m6502zppc]	; Get our program counter back
		mov	ebp, [_m6502zpBase] ; Base pointer comes back
		add	esi, ebp	; Rebase it properly
		mov	ax, [_m6502zpaf]	; Get our flags & stuff back
		jmp	short writeMacroExit43

memoryWrite43:
		mov	[ebp + edx], bh ; Store the byte

writeMacroExit43:
		xor	bh, bh	; Zero this so we don't screw up things
		xor	edx, edx
		mov	dl, [esi]
		inc	esi
		jmp	dword [m6502zpregular+edx*4]

;
; Branch
;

times ($$-$) & 3 nop	; pad with NOPs to 4-byte boundary

RegInst70:
		sub	dword [cyclesRemaining], byte 3
		jc	near noMoreExec	; Can't execute anymore!
		add	dword [dwElapsedTicks], byte 3
		test	ah, 10h	; Overflow set?
		jnz	short takeJump70 ; Do it!
		inc	esi	; Skip past the offset
		xor	edx, edx
		mov	dl, [esi]
		inc	esi
		jmp	dword [m6502zpregular+edx*4]

takeJump70:
		mov	dl, [esi]		; Get the next instruction
		inc	esi		; Advance PC!
		movsx	dx, dl
		sub	esi, ebp
		add	si, dx
		add	esi, ebp
		xor	edx, edx
		mov	dl, [esi]
		inc	esi
		jmp	dword [m6502zpregular+edx*4]

;
; ADC
;

times ($$-$) & 3 nop	; pad with NOPs to 4-byte boundary

RegInst71:
		sub	dword [cyclesRemaining], byte 5
		jc	near noMoreExec	; Can't execute anymore!
		add	dword [dwElapsedTicks], byte 5
		mov	dl, [esi]		; Get the next instruction
		inc	esi		; Advance PC!
		mov	dx, [ebp+edx]	; Get our 16 bit address
		add	dx, cx	; Add in Y
		mov	edi, [_m6502zpMemRead]	; Point to the read array

checkLoop45:
		cmp	[edi], word 0ffffh ; End of the list?
		je		short memoryRead45
		cmp	dx, [edi]	; Are we smaller?
		jb		short nextAddr45		; Yes, go to the next address
		cmp	dx, [edi+4]	; Are we bigger?
		jbe	short callRoutine45

nextAddr45:
		add	edi, 10h		; Next structure!
		jmp	short checkLoop45

callRoutine45:
		call	ReadMemoryByte	; Standard read routine
		mov	bl, [_m6502zpx]	; Get X back
		mov	cl, [_m6502zpy]	; Get Y back
		mov	dl, al	; Get our value
		mov	ax, [_m6502zpaf]	; Get our flags & stuff back
		jmp	short readExit45

memoryRead45:
		mov	dl, [ebp + edx]	; Get our data

readExit45:
		sahf		; Restore our flags for the adc
		adc	al, dl	; Add in our value
		o16 pushf	; Push our flags (and overflow)
		and	ah, 02eh	; No carry, overflow, zero or sign
		pop	dx	; Restore our flags into DX
		shl	dh, 1	; Shift overflow into position
		and	dh, 10h	; Only the overflow
		and	dl, 0c1h	; Only carry, sign, and zero
		or		ah, dl	; OR In our new flags
		or		ah, dh	; OR In overflow
		xor	edx, edx
		mov	dl, [esi]
		inc	esi
		jmp	dword [m6502zpregular+edx*4]

;
; ADC
;

times ($$-$) & 3 nop	; pad with NOPs to 4-byte boundary

RegInst75:
		sub	dword [cyclesRemaining], byte 4
		jc	near noMoreExec	; Can't execute anymore!
		add	dword [dwElapsedTicks], byte 4
		mov	dl, [esi]		; Get the next instruction
		inc	esi		; Advance PC!
		add	dl, bl	; Add X
		mov	dl, [ebp+edx]	; Get our zero page data
		sahf		; Restore our flags for the adc
		adc	al, dl	; Add in our value
		o16 pushf	; Push our flags (and overflow)
		and	ah, 02eh	; No carry, overflow, zero or sign
		pop	dx	; Restore our flags into DX
		shl	dh, 1	; Shift overflow into position
		and	dh, 10h	; Only the overflow
		and	dl, 0c1h	; Only carry, sign, and zero
		or		ah, dl	; OR In our new flags
		or		ah, dh	; OR In overflow
		xor	edx, edx
		mov	dl, [esi]
		inc	esi
		jmp	dword [m6502zpregular+edx*4]

;
; ROR
;

times ($$-$) & 3 nop	; pad with NOPs to 4-byte boundary

RegInst76:
		sub	dword [cyclesRemaining], byte 6
		jc	near noMoreExec	; Can't execute anymore!
		add	dword [dwElapsedTicks], byte 6
		mov	dl, [esi]		; Get the next instruction
		inc	esi		; Advance PC!
		add	dl, bl	; Add X
		push	edx		; Save our address
		mov	dl, [ebp+edx]	; Get our zero page byte
		mov	dh, ah	; Save off our original flags
		and	dh, 3eh	; No carry, zero, or sign
		sahf		; Restore flags
		rcr	dl, 1	; Through carry rotate
		lahf		; Get the current flags back
		mov	bh, ah ; Store it here for later
		and	bh, 1	; Only the carry matters
		or		dl, dl	; Set sign/zero
		lahf		; Get the flags
		and	ah, 0c0h	; Only sign and zero
		or		ah, bh	; Or In our carry
		or		ah, dh	; Or in our original flags
		mov	bh, dl	; Get our byte to write
		pop	edx		; Restore the address
		mov	[ebp+edx], bh	; Set our zero page byte
		xor	bh, bh	; Zero this so we don't screw up things
		xor	edx, edx
		mov	dl, [esi]
		inc	esi
		jmp	dword [m6502zpregular+edx*4]

;
; SEI
;

times ($$-$) & 3 nop	; pad with NOPs to 4-byte boundary

RegInst78:
		sub	dword [cyclesRemaining], byte 2
		jc	near noMoreExec	; Can't execute anymore!
		add	dword [dwElapsedTicks], byte 2
		or		[_altFlags], byte 04h	; Interrupts!
		xor	edx, edx
		mov	dl, [esi]
		inc	esi
		jmp	dword [m6502zpregular+edx*4]

;
; ADC
;

times ($$-$) & 3 nop	; pad with NOPs to 4-byte boundary

RegInst79:
		sub	dword [cyclesRemaining], byte 4
		jc	near noMoreExec	; Can't execute anymore!
		add	dword [dwElapsedTicks], byte 4
		mov	dx, [esi]	; Get our address
		add	esi, 2	; Increment past instruction
		add	dx, cx	 ; Add Y
		mov	edi, [_m6502zpMemRead]	; Point to the read array

checkLoop46:
		cmp	[edi], word 0ffffh ; End of the list?
		je		short memoryRead46
		cmp	dx, [edi]	; Are we smaller?
		jb		short nextAddr46		; Yes, go to the next address
		cmp	dx, [edi+4]	; Are we bigger?
		jbe	short callRoutine46

nextAddr46:
		add	edi, 10h		; Next structure!
		jmp	short checkLoop46

callRoutine46:
		call	ReadMemoryByte	; Standard read routine
		mov	bl, [_m6502zpx]	; Get X back
		mov	cl, [_m6502zpy]	; Get Y back
		mov	dl, al	; Get our value
		mov	ax, [_m6502zpaf]	; Get our flags & stuff back
		jmp	short readExit46

memoryRead46:
		mov	dl, [ebp + edx]	; Get our data

readExit46:
		sahf		; Restore our flags for the adc
		adc	al, dl	; Add in our value
		o16 pushf	; Push our flags (and overflow)
		and	ah, 02eh	; No carry, overflow, zero or sign
		pop	dx	; Restore our flags into DX
		shl	dh, 1	; Shift overflow into position
		and	dh, 10h	; Only the overflow
		and	dl, 0c1h	; Only carry, sign, and zero
		or		ah, dl	; OR In our new flags
		or		ah, dh	; OR In overflow
		xor	edx, edx
		mov	dl, [esi]
		inc	esi
		jmp	dword [m6502zpregular+edx*4]

;
; ADC
;

times ($$-$) & 3 nop	; pad with NOPs to 4-byte boundary

RegInst7d:
		sub	dword [cyclesRemaining], byte 4
		jc	near noMoreExec	; Can't execute anymore!
		add	dword [dwElapsedTicks], byte 4
		mov	dx, [esi]	; Get our address
		add	esi, 2	; Increment past instruction
		add	dx, bx	 ; Add X
		mov	edi, [_m6502zpMemRead]	; Point to the read array

checkLoop47:
		cmp	[edi], word 0ffffh ; End of the list?
		je		short memoryRead47
		cmp	dx, [edi]	; Are we smaller?
		jb		short nextAddr47		; Yes, go to the next address
		cmp	dx, [edi+4]	; Are we bigger?
		jbe	short callRoutine47

nextAddr47:
		add	edi, 10h		; Next structure!
		jmp	short checkLoop47

callRoutine47:
		call	ReadMemoryByte	; Standard read routine
		mov	bl, [_m6502zpx]	; Get X back
		mov	cl, [_m6502zpy]	; Get Y back
		mov	dl, al	; Get our value
		mov	ax, [_m6502zpaf]	; Get our flags & stuff back
		jmp	short readExit47

memoryRead47:
		mov	dl, [ebp + edx]	; Get our data

readExit47:
		sahf		; Restore our flags for the adc
		adc	al, dl	; Add in our value
		o16 pushf	; Push our flags (and overflow)
		and	ah, 02eh	; No carry, overflow, zero or sign
		pop	dx	; Restore our flags into DX
		shl	dh, 1	; Shift overflow into position
		and	dh, 10h	; Only the overflow
		and	dl, 0c1h	; Only carry, sign, and zero
		or		ah, dl	; OR In our new flags
		or		ah, dh	; OR In overflow
		xor	edx, edx
		mov	dl, [esi]
		inc	esi
		jmp	dword [m6502zpregular+edx*4]

;
; ROR
;

times ($$-$) & 3 nop	; pad with NOPs to 4-byte boundary

RegInst7e:
		sub	dword [cyclesRemaining], byte 7
		jc	near noMoreExec	; Can't execute anymore!
		add	dword [dwElapsedTicks], byte 7
		mov	dx, [esi]	; Get our address
		add	esi, 2	; Increment past instruction
		add	dx, bx	 ; Add X
		push	edx		; Save our address
		mov	edi, [_m6502zpMemRead]	; Point to the read array

checkLoop48:
		cmp	[edi], word 0ffffh ; End of the list?
		je		short memoryRead48
		cmp	dx, [edi]	; Are we smaller?
		jb		short nextAddr48		; Yes, go to the next address
		cmp	dx, [edi+4]	; Are we bigger?
		jbe	short callRoutine48

nextAddr48:
		add	edi, 10h		; Next structure!
		jmp	short checkLoop48

callRoutine48:
		call	ReadMemoryByte	; Standard read routine
		mov	bl, [_m6502zpx]	; Get X back
		mov	cl, [_m6502zpy]	; Get Y back
		mov	dl, al	; Get our value
		mov	ax, [_m6502zpaf]	; Get our flags & stuff back
		jmp	short readExit48

memoryRead48:
		mov	dl, [ebp + edx]	; Get our data

readExit48:
		mov	dh, ah	; Save off our original flags
		and	dh, 3eh	; No carry, zero, or sign
		sahf		; Restore flags
		rcr	dl, 1	; Through carry rotate
		lahf		; Get the current flags back
		mov	bh, ah ; Store it here for later
		and	bh, 1	; Only the carry matters
		or		dl, dl	; Set sign/zero
		lahf		; Get the flags
		and	ah, 0c0h	; Only sign and zero
		or		ah, bh	; Or In our carry
		or		ah, dh	; Or in our original flags
		mov	bh, dl	; Get our byte to write
		pop	edx		; Restore the address
		mov	edi, [_m6502zpMemWrite]	; Point to the write array

checkLoop49:
		cmp	[edi], word 0ffffh ; End of our list?
		je	near memoryWrite49	; Yes - go write it!
		cmp	dx, [edi]	; Are we smaller?
		jb	nextAddr49	; Yes... go to the next addr
		cmp	dx, [edi+4]	; Are we bigger?
		jbe	callRoutine49	; If not, go call it!

nextAddr49:
		add	edi, 10h		; Next structure, please
		jmp	short checkLoop49

callRoutine49:
		mov	[_m6502zpx], bl	; Save X
		mov	[_m6502zpy], cl	; Save Y
		mov	[_m6502zpaf], ax	; Save Accumulator & flags
		sub	esi, ebp	; Our program counter
		mov	[_m6502zppc], si	; Save our program counter
		push	edi	; Pointer to MemoryWriteByte structure
		mov	bl, bh	; Put a copy here
		push	ebx	; The byte value
		and	edx, 0ffffh	; Only lower 16 bits
		push	edx	; The address
		call	dword [edi + 8] ; Go call our handler
		add	esp, 12	; Get rid of our stack
		xor	ebx, ebx	; Zero this
		xor	ecx, ecx	; This too!
		mov	bl, [_m6502zpx]	; Get X back
		mov	cl, [_m6502zpy]	; Get Y back
		xor	esi, esi	; Zero it!
		mov	si, [_m6502zppc]	; Get our program counter back
		mov	ebp, [_m6502zpBase] ; Base pointer comes back
		add	esi, ebp	; Rebase it properly
		mov	ax, [_m6502zpaf]	; Get our flags & stuff back
		jmp	short writeMacroExit49

memoryWrite49:
		mov	[ebp + edx], bh ; Store the byte

writeMacroExit49:
		xor	bh, bh	; Zero this so we don't screw up things
		xor	edx, edx
		mov	dl, [esi]
		inc	esi
		jmp	dword [m6502zpregular+edx*4]

;
; STA
;

times ($$-$) & 3 nop	; pad with NOPs to 4-byte boundary

RegInst81:
		sub	dword [cyclesRemaining], byte 6
		jc	near noMoreExec	; Can't execute anymore!
		add	dword [dwElapsedTicks], byte 6
		mov	dl, [esi]		; Get the next instruction
		inc	esi		; Advance PC!
		add	dl, bl	; Add in X
		mov	dx, [ebp+edx]	; Get our 16 bit address
		mov	edi, [_m6502zpMemWrite]	; Point to the write array

checkLoop50:
		cmp	[edi], word 0ffffh ; End of our list?
		je	near memoryWrite50	; Yes - go write it!
		cmp	dx, [edi]	; Are we smaller?
		jb	nextAddr50	; Yes... go to the next addr
		cmp	dx, [edi+4]	; Are we bigger?
		jbe	callRoutine50	; If not, go call it!

nextAddr50:
		add	edi, 10h		; Next structure, please
		jmp	short checkLoop50

callRoutine50:
		mov	[_m6502zpx], bl	; Save X
		mov	[_m6502zpy], cl	; Save Y
		mov	[_m6502zpaf], ax	; Save Accumulator & flags
		sub	esi, ebp	; Our program counter
		mov	[_m6502zppc], si	; Save our program counter
		push	edi	; Pointer to MemoryWriteByte structure
		push	eax	; The byte value
		and	edx, 0ffffh	; Only lower 16 bits
		push	edx	; The address
		call	dword [edi + 8] ; Go call our handler
		add	esp, 12	; Get rid of our stack
		xor	ebx, ebx	; Zero this
		xor	ecx, ecx	; This too!
		mov	bl, [_m6502zpx]	; Get X back
		mov	cl, [_m6502zpy]	; Get Y back
		xor	esi, esi	; Zero it!
		mov	si, [_m6502zppc]	; Get our program counter back
		mov	ebp, [_m6502zpBase] ; Base pointer comes back
		add	esi, ebp	; Rebase it properly
		mov	ax, [_m6502zpaf]	; Get our flags & stuff back
		jmp	short writeMacroExit50

memoryWrite50:
		mov	[ebp + edx], al ; Store the byte

writeMacroExit50:
		xor	edx, edx
		mov	dl, [esi]
		inc	esi
		jmp	dword [m6502zpregular+edx*4]

;
; STY
;

times ($$-$) & 3 nop	; pad with NOPs to 4-byte boundary

RegInst84:
		sub	dword [cyclesRemaining], byte 3
		jc	near noMoreExec	; Can't execute anymore!
		add	dword [dwElapsedTicks], byte 3
		mov	dl, [esi]		; Get the next instruction
		inc	esi		; Advance PC!
		mov	edi, [_m6502zpMemWrite]	; Point to the write array

checkLoop51:
		cmp	[edi], word 0ffffh ; End of our list?
		je	near memoryWrite51	; Yes - go write it!
		cmp	dx, [edi]	; Are we smaller?
		jb	nextAddr51	; Yes... go to the next addr
		cmp	dx, [edi+4]	; Are we bigger?
		jbe	callRoutine51	; If not, go call it!

nextAddr51:
		add	edi, 10h		; Next structure, please
		jmp	short checkLoop51

callRoutine51:
		mov	[_m6502zpx], bl	; Save X
		mov	[_m6502zpy], cl	; Save Y
		mov	[_m6502zpaf], ax	; Save Accumulator & flags
		sub	esi, ebp	; Our program counter
		mov	[_m6502zppc], si	; Save our program counter
		push	edi	; Pointer to MemoryWriteByte structure
		push	ecx	; The byte value
		and	edx, 0ffffh	; Only lower 16 bits
		push	edx	; The address
		call	dword [edi + 8] ; Go call our handler
		add	esp, 12	; Get rid of our stack
		xor	ebx, ebx	; Zero this
		xor	ecx, ecx	; This too!
		mov	bl, [_m6502zpx]	; Get X back
		mov	cl, [_m6502zpy]	; Get Y back
		xor	esi, esi	; Zero it!
		mov	si, [_m6502zppc]	; Get our program counter back
		mov	ebp, [_m6502zpBase] ; Base pointer comes back
		add	esi, ebp	; Rebase it properly
		mov	ax, [_m6502zpaf]	; Get our flags & stuff back
		jmp	short writeMacroExit51

memoryWrite51:
		mov	[ebp + edx], cl ; Store the byte

writeMacroExit51:
		xor	edx, edx
		mov	dl, [esi]
		inc	esi
		jmp	dword [m6502zpregular+edx*4]

;
; STA
;

times ($$-$) & 3 nop	; pad with NOPs to 4-byte boundary

RegInst85:
		sub	dword [cyclesRemaining], byte 3
		jc	near noMoreExec	; Can't execute anymore!
		add	dword [dwElapsedTicks], byte 3
		mov	dl, [esi]		; Get the next instruction
		inc	esi		; Advance PC!
		mov	[ebp+edx], al	; Store A
		xor	edx, edx
		mov	dl, [esi]
		inc	esi
		jmp	dword [m6502zpregular+edx*4]

;
; STX
;

times ($$-$) & 3 nop	; pad with NOPs to 4-byte boundary

RegInst86:
		sub	dword [cyclesRemaining], byte 3
		jc	near noMoreExec	; Can't execute anymore!
		add	dword [dwElapsedTicks], byte 3
		mov	dl, [esi]		; Get the next instruction
		inc	esi		; Advance PC!
		mov	[ebp+edx], bl	; Store X
		xor	edx, edx
		mov	dl, [esi]
		inc	esi
		jmp	dword [m6502zpregular+edx*4]

;
; DEY
;

times ($$-$) & 3 nop	; pad with NOPs to 4-byte boundary

RegInst88:
		sub	dword [cyclesRemaining], byte 2
		jc	near noMoreExec	; Can't execute anymore!
		add	dword [dwElapsedTicks], byte 2
		mov	dl, ah	; Save flags
		dec	cl	; Decrement
		lahf		; Restore flags
		and	dl, 03fh	; Original value
		and	ah, 0c0h	; Only zero and sign
		or	ah, dl		; New flags with the old!
		xor	edx, edx
		mov	dl, [esi]
		inc	esi
		jmp	dword [m6502zpregular+edx*4]

;
; TXA
;

times ($$-$) & 3 nop	; pad with NOPs to 4-byte boundary

RegInst8a:
		sub	dword [cyclesRemaining], byte 2
		jc	near noMoreExec	; Can't execute anymore!
		add	dword [dwElapsedTicks], byte 2
		mov	al, bl	; A = X
		mov	dl, ah	; Save flags
		or	al, al	; OR Our new value
		lahf		; Restore flags
		and	dl, 03fh	; Original value
		and	ah, 0c0h	; Only zero and sign
		or	ah, dl		; New flags with the old!
		xor	edx, edx
		mov	dl, [esi]
		inc	esi
		jmp	dword [m6502zpregular+edx*4]

;
; STY
;

times ($$-$) & 3 nop	; pad with NOPs to 4-byte boundary

RegInst8c:
		sub	dword [cyclesRemaining], byte 4
		jc	near noMoreExec	; Can't execute anymore!
		add	dword [dwElapsedTicks], byte 4
		mov	dx, [esi]	; Get our address
		add	esi, 2	; Increment past instruction
		mov	edi, [_m6502zpMemWrite]	; Point to the write array

checkLoop52:
		cmp	[edi], word 0ffffh ; End of our list?
		je	near memoryWrite52	; Yes - go write it!
		cmp	dx, [edi]	; Are we smaller?
		jb	nextAddr52	; Yes... go to the next addr
		cmp	dx, [edi+4]	; Are we bigger?
		jbe	callRoutine52	; If not, go call it!

nextAddr52:
		add	edi, 10h		; Next structure, please
		jmp	short checkLoop52

callRoutine52:
		mov	[_m6502zpx], bl	; Save X
		mov	[_m6502zpy], cl	; Save Y
		mov	[_m6502zpaf], ax	; Save Accumulator & flags
		sub	esi, ebp	; Our program counter
		mov	[_m6502zppc], si	; Save our program counter
		push	edi	; Pointer to MemoryWriteByte structure
		push	ecx	; The byte value
		and	edx, 0ffffh	; Only lower 16 bits
		push	edx	; The address
		call	dword [edi + 8] ; Go call our handler
		add	esp, 12	; Get rid of our stack
		xor	ebx, ebx	; Zero this
		xor	ecx, ecx	; This too!
		mov	bl, [_m6502zpx]	; Get X back
		mov	cl, [_m6502zpy]	; Get Y back
		xor	esi, esi	; Zero it!
		mov	si, [_m6502zppc]	; Get our program counter back
		mov	ebp, [_m6502zpBase] ; Base pointer comes back
		add	esi, ebp	; Rebase it properly
		mov	ax, [_m6502zpaf]	; Get our flags & stuff back
		jmp	short writeMacroExit52

memoryWrite52:
		mov	[ebp + edx], cl ; Store the byte

writeMacroExit52:
		xor	edx, edx
		mov	dl, [esi]
		inc	esi
		jmp	dword [m6502zpregular+edx*4]

;
; STA
;

times ($$-$) & 3 nop	; pad with NOPs to 4-byte boundary

RegInst8d:
		sub	dword [cyclesRemaining], byte 4
		jc	near noMoreExec	; Can't execute anymore!
		add	dword [dwElapsedTicks], byte 4
		mov	dx, [esi]	; Get our address
		add	esi, 2	; Increment past instruction
		mov	edi, [_m6502zpMemWrite]	; Point to the write array

checkLoop53:
		cmp	[edi], word 0ffffh ; End of our list?
		je	near memoryWrite53	; Yes - go write it!
		cmp	dx, [edi]	; Are we smaller?
		jb	nextAddr53	; Yes... go to the next addr
		cmp	dx, [edi+4]	; Are we bigger?
		jbe	callRoutine53	; If not, go call it!

nextAddr53:
		add	edi, 10h		; Next structure, please
		jmp	short checkLoop53

callRoutine53:
		mov	[_m6502zpx], bl	; Save X
		mov	[_m6502zpy], cl	; Save Y
		mov	[_m6502zpaf], ax	; Save Accumulator & flags
		sub	esi, ebp	; Our program counter
		mov	[_m6502zppc], si	; Save our program counter
		push	edi	; Pointer to MemoryWriteByte structure
		push	eax	; The byte value
		and	edx, 0ffffh	; Only lower 16 bits
		push	edx	; The address
		call	dword [edi + 8] ; Go call our handler
		add	esp, 12	; Get rid of our stack
		xor	ebx, ebx	; Zero this
		xor	ecx, ecx	; This too!
		mov	bl, [_m6502zpx]	; Get X back
		mov	cl, [_m6502zpy]	; Get Y back
		xor	esi, esi	; Zero it!
		mov	si, [_m6502zppc]	; Get our program counter back
		mov	ebp, [_m6502zpBase] ; Base pointer comes back
		add	esi, ebp	; Rebase it properly
		mov	ax, [_m6502zpaf]	; Get our flags & stuff back
		jmp	short writeMacroExit53

memoryWrite53:
		mov	[ebp + edx], al ; Store the byte

writeMacroExit53:
		xor	edx, edx
		mov	dl, [esi]
		inc	esi
		jmp	dword [m6502zpregular+edx*4]

;
; STX
;

times ($$-$) & 3 nop	; pad with NOPs to 4-byte boundary

RegInst8e:
		sub	dword [cyclesRemaining], byte 4
		jc	near noMoreExec	; Can't execute anymore!
		add	dword [dwElapsedTicks], byte 4
		mov	dx, [esi]	; Get our address
		add	esi, 2	; Increment past instruction
		mov	edi, [_m6502zpMemWrite]	; Point to the write array

checkLoop54:
		cmp	[edi], word 0ffffh ; End of our list?
		je	near memoryWrite54	; Yes - go write it!
		cmp	dx, [edi]	; Are we smaller?
		jb	nextAddr54	; Yes... go to the next addr
		cmp	dx, [edi+4]	; Are we bigger?
		jbe	callRoutine54	; If not, go call it!

nextAddr54:
		add	edi, 10h		; Next structure, please
		jmp	short checkLoop54

callRoutine54:
		mov	[_m6502zpx], bl	; Save X
		mov	[_m6502zpy], cl	; Save Y
		mov	[_m6502zpaf], ax	; Save Accumulator & flags
		sub	esi, ebp	; Our program counter
		mov	[_m6502zppc], si	; Save our program counter
		push	edi	; Pointer to MemoryWriteByte structure
		push	ebx	; The byte value
		and	edx, 0ffffh	; Only lower 16 bits
		push	edx	; The address
		call	dword [edi + 8] ; Go call our handler
		add	esp, 12	; Get rid of our stack
		xor	ebx, ebx	; Zero this
		xor	ecx, ecx	; This too!
		mov	bl, [_m6502zpx]	; Get X back
		mov	cl, [_m6502zpy]	; Get Y back
		xor	esi, esi	; Zero it!
		mov	si, [_m6502zppc]	; Get our program counter back
		mov	ebp, [_m6502zpBase] ; Base pointer comes back
		add	esi, ebp	; Rebase it properly
		mov	ax, [_m6502zpaf]	; Get our flags & stuff back
		jmp	short writeMacroExit54

memoryWrite54:
		mov	[ebp + edx], bl ; Store the byte

writeMacroExit54:
		xor	edx, edx
		mov	dl, [esi]
		inc	esi
		jmp	dword [m6502zpregular+edx*4]

;
; Branch
;

times ($$-$) & 3 nop	; pad with NOPs to 4-byte boundary

RegInst90:
		sub	dword [cyclesRemaining], byte 3
		jc	near noMoreExec	; Can't execute anymore!
		add	dword [dwElapsedTicks], byte 3
		test	ah, 1h	; Carry not set?
		jz		short takeJump90 ; Do it!
		inc	esi	; Skip past the offset
		xor	edx, edx
		mov	dl, [esi]
		inc	esi
		jmp	dword [m6502zpregular+edx*4]

takeJump90:
		mov	dl, [esi]		; Get the next instruction
		inc	esi		; Advance PC!
		movsx	dx, dl
		sub	esi, ebp
		add	si, dx
		add	esi, ebp
		xor	edx, edx
		mov	dl, [esi]
		inc	esi
		jmp	dword [m6502zpregular+edx*4]

;
; STA
;

times ($$-$) & 3 nop	; pad with NOPs to 4-byte boundary

RegInst91:
		sub	dword [cyclesRemaining], byte 6
		jc	near noMoreExec	; Can't execute anymore!
		add	dword [dwElapsedTicks], byte 6
		mov	dl, [esi]		; Get the next instruction
		inc	esi		; Advance PC!
		mov	dx, [ebp+edx]	; Get our 16 bit address
		add	dx, cx	; Add in Y
		mov	edi, [_m6502zpMemWrite]	; Point to the write array

checkLoop56:
		cmp	[edi], word 0ffffh ; End of our list?
		je	near memoryWrite56	; Yes - go write it!
		cmp	dx, [edi]	; Are we smaller?
		jb	nextAddr56	; Yes... go to the next addr
		cmp	dx, [edi+4]	; Are we bigger?
		jbe	callRoutine56	; If not, go call it!

nextAddr56:
		add	edi, 10h		; Next structure, please
		jmp	short checkLoop56

callRoutine56:
		mov	[_m6502zpx], bl	; Save X
		mov	[_m6502zpy], cl	; Save Y
		mov	[_m6502zpaf], ax	; Save Accumulator & flags
		sub	esi, ebp	; Our program counter
		mov	[_m6502zppc], si	; Save our program counter
		push	edi	; Pointer to MemoryWriteByte structure
		push	eax	; The byte value
		and	edx, 0ffffh	; Only lower 16 bits
		push	edx	; The address
		call	dword [edi + 8] ; Go call our handler
		add	esp, 12	; Get rid of our stack
		xor	ebx, ebx	; Zero this
		xor	ecx, ecx	; This too!
		mov	bl, [_m6502zpx]	; Get X back
		mov	cl, [_m6502zpy]	; Get Y back
		xor	esi, esi	; Zero it!
		mov	si, [_m6502zppc]	; Get our program counter back
		mov	ebp, [_m6502zpBase] ; Base pointer comes back
		add	esi, ebp	; Rebase it properly
		mov	ax, [_m6502zpaf]	; Get our flags & stuff back
		jmp	short writeMacroExit56

memoryWrite56:
		mov	[ebp + edx], al ; Store the byte

writeMacroExit56:
		xor	edx, edx
		mov	dl, [esi]
		inc	esi
		jmp	dword [m6502zpregular+edx*4]

;
; STY
;

times ($$-$) & 3 nop	; pad with NOPs to 4-byte boundary

RegInst94:
		sub	dword [cyclesRemaining], byte 4
		jc	near noMoreExec	; Can't execute anymore!
		add	dword [dwElapsedTicks], byte 4
		mov	dl, [esi]		; Get the next instruction
		inc	esi		; Advance PC!
		add	dl, bl	; Add X
		mov	edi, [_m6502zpMemWrite]	; Point to the write array

checkLoop57:
		cmp	[edi], word 0ffffh ; End of our list?
		je	near memoryWrite57	; Yes - go write it!
		cmp	dx, [edi]	; Are we smaller?
		jb	nextAddr57	; Yes... go to the next addr
		cmp	dx, [edi+4]	; Are we bigger?
		jbe	callRoutine57	; If not, go call it!

nextAddr57:
		add	edi, 10h		; Next structure, please
		jmp	short checkLoop57

callRoutine57:
		mov	[_m6502zpx], bl	; Save X
		mov	[_m6502zpy], cl	; Save Y
		mov	[_m6502zpaf], ax	; Save Accumulator & flags
		sub	esi, ebp	; Our program counter
		mov	[_m6502zppc], si	; Save our program counter
		push	edi	; Pointer to MemoryWriteByte structure
		push	ecx	; The byte value
		and	edx, 0ffffh	; Only lower 16 bits
		push	edx	; The address
		call	dword [edi + 8] ; Go call our handler
		add	esp, 12	; Get rid of our stack
		xor	ebx, ebx	; Zero this
		xor	ecx, ecx	; This too!
		mov	bl, [_m6502zpx]	; Get X back
		mov	cl, [_m6502zpy]	; Get Y back
		xor	esi, esi	; Zero it!
		mov	si, [_m6502zppc]	; Get our program counter back
		mov	ebp, [_m6502zpBase] ; Base pointer comes back
		add	esi, ebp	; Rebase it properly
		mov	ax, [_m6502zpaf]	; Get our flags & stuff back
		jmp	short writeMacroExit57

memoryWrite57:
		mov	[ebp + edx], cl ; Store the byte

writeMacroExit57:
		xor	edx, edx
		mov	dl, [esi]
		inc	esi
		jmp	dword [m6502zpregular+edx*4]

;
; STA
;

times ($$-$) & 3 nop	; pad with NOPs to 4-byte boundary

RegInst95:
		sub	dword [cyclesRemaining], byte 4
		jc	near noMoreExec	; Can't execute anymore!
		add	dword [dwElapsedTicks], byte 4
		mov	dl, [esi]		; Get the next instruction
		inc	esi		; Advance PC!
		add	dl, bl	; Add X
		mov	[ebp+edx], al	; Store A
		xor	edx, edx
		mov	dl, [esi]
		inc	esi
		jmp	dword [m6502zpregular+edx*4]

;
; STX
;

times ($$-$) & 3 nop	; pad with NOPs to 4-byte boundary

RegInst96:
		sub	dword [cyclesRemaining], byte 4
		jc	near noMoreExec	; Can't execute anymore!
		add	dword [dwElapsedTicks], byte 4
		mov	dl, [esi]		; Get the next instruction
		inc	esi		; Advance PC!
		add	dl, cl	; Add Y
		mov	[ebp+edx], bl	; Store X
		xor	edx, edx
		mov	dl, [esi]
		inc	esi
		jmp	dword [m6502zpregular+edx*4]

;
; TYA
;

times ($$-$) & 3 nop	; pad with NOPs to 4-byte boundary

RegInst98:
		sub	dword [cyclesRemaining], byte 2
		jc	near noMoreExec	; Can't execute anymore!
		add	dword [dwElapsedTicks], byte 2
		mov	al, cl	; A = Y
		mov	dl, ah	; Save flags
		or	al, al	; OR Our new value
		lahf		; Restore flags
		and	dl, 03fh	; Original value
		and	ah, 0c0h	; Only zero and sign
		or	ah, dl		; New flags with the old!
		xor	edx, edx
		mov	dl, [esi]
		inc	esi
		jmp	dword [m6502zpregular+edx*4]

;
; STA
;

times ($$-$) & 3 nop	; pad with NOPs to 4-byte boundary

RegInst99:
		sub	dword [cyclesRemaining], byte 5
		jc	near noMoreExec	; Can't execute anymore!
		add	dword [dwElapsedTicks], byte 5
		mov	dx, [esi]	; Get our address
		add	esi, 2	; Increment past instruction
		add	dx, cx	 ; Add Y
		mov	edi, [_m6502zpMemWrite]	; Point to the write array

checkLoop58:
		cmp	[edi], word 0ffffh ; End of our list?
		je	near memoryWrite58	; Yes - go write it!
		cmp	dx, [edi]	; Are we smaller?
		jb	nextAddr58	; Yes... go to the next addr
		cmp	dx, [edi+4]	; Are we bigger?
		jbe	callRoutine58	; If not, go call it!

nextAddr58:
		add	edi, 10h		; Next structure, please
		jmp	short checkLoop58

callRoutine58:
		mov	[_m6502zpx], bl	; Save X
		mov	[_m6502zpy], cl	; Save Y
		mov	[_m6502zpaf], ax	; Save Accumulator & flags
		sub	esi, ebp	; Our program counter
		mov	[_m6502zppc], si	; Save our program counter
		push	edi	; Pointer to MemoryWriteByte structure
		push	eax	; The byte value
		and	edx, 0ffffh	; Only lower 16 bits
		push	edx	; The address
		call	dword [edi + 8] ; Go call our handler
		add	esp, 12	; Get rid of our stack
		xor	ebx, ebx	; Zero this
		xor	ecx, ecx	; This too!
		mov	bl, [_m6502zpx]	; Get X back
		mov	cl, [_m6502zpy]	; Get Y back
		xor	esi, esi	; Zero it!
		mov	si, [_m6502zppc]	; Get our program counter back
		mov	ebp, [_m6502zpBase] ; Base pointer comes back
		add	esi, ebp	; Rebase it properly
		mov	ax, [_m6502zpaf]	; Get our flags & stuff back
		jmp	short writeMacroExit58

memoryWrite58:
		mov	[ebp + edx], al ; Store the byte

writeMacroExit58:
		xor	edx, edx
		mov	dl, [esi]
		inc	esi
		jmp	dword [m6502zpregular+edx*4]

;
; TXS
;

times ($$-$) & 3 nop	; pad with NOPs to 4-byte boundary

RegInst9a:
		sub	dword [cyclesRemaining], byte 2
		jc	near noMoreExec	; Can't execute anymore!
		add	dword [dwElapsedTicks], byte 2
		mov	[_m6502zps], bl	; X -> S
		xor	edx, edx
		mov	dl, [esi]
		inc	esi
		jmp	dword [m6502zpregular+edx*4]

;
; STA
;

times ($$-$) & 3 nop	; pad with NOPs to 4-byte boundary

RegInst9d:
		sub	dword [cyclesRemaining], byte 5
		jc	near noMoreExec	; Can't execute anymore!
		add	dword [dwElapsedTicks], byte 5
		mov	dx, [esi]	; Get our address
		add	esi, 2	; Increment past instruction
		add	dx, bx	 ; Add X
		mov	edi, [_m6502zpMemWrite]	; Point to the write array

checkLoop59:
		cmp	[edi], word 0ffffh ; End of our list?
		je	near memoryWrite59	; Yes - go write it!
		cmp	dx, [edi]	; Are we smaller?
		jb	nextAddr59	; Yes... go to the next addr
		cmp	dx, [edi+4]	; Are we bigger?
		jbe	callRoutine59	; If not, go call it!

nextAddr59:
		add	edi, 10h		; Next structure, please
		jmp	short checkLoop59

callRoutine59:
		mov	[_m6502zpx], bl	; Save X
		mov	[_m6502zpy], cl	; Save Y
		mov	[_m6502zpaf], ax	; Save Accumulator & flags
		sub	esi, ebp	; Our program counter
		mov	[_m6502zppc], si	; Save our program counter
		push	edi	; Pointer to MemoryWriteByte structure
		push	eax	; The byte value
		and	edx, 0ffffh	; Only lower 16 bits
		push	edx	; The address
		call	dword [edi + 8] ; Go call our handler
		add	esp, 12	; Get rid of our stack
		xor	ebx, ebx	; Zero this
		xor	ecx, ecx	; This too!
		mov	bl, [_m6502zpx]	; Get X back
		mov	cl, [_m6502zpy]	; Get Y back
		xor	esi, esi	; Zero it!
		mov	si, [_m6502zppc]	; Get our program counter back
		mov	ebp, [_m6502zpBase] ; Base pointer comes back
		add	esi, ebp	; Rebase it properly
		mov	ax, [_m6502zpaf]	; Get our flags & stuff back
		jmp	short writeMacroExit59

memoryWrite59:
		mov	[ebp + edx], al ; Store the byte

writeMacroExit59:
		xor	edx, edx
		mov	dl, [esi]
		inc	esi
		jmp	dword [m6502zpregular+edx*4]

;
; LDY
;

times ($$-$) & 3 nop	; pad with NOPs to 4-byte boundary

RegInsta0:
		sub	dword [cyclesRemaining], byte 2
		jc	near noMoreExec	; Can't execute anymore!
		add	dword [dwElapsedTicks], byte 2
		mov	cl, [esi]	; Get our next byte
		inc	esi		; Increment to our next byte
		mov	dl, ah	; Save flags
		or	cl, cl	; OR Our new value
		lahf		; Restore flags
		and	dl, 03fh	; Original value
		and	ah, 0c0h	; Only zero and sign
		or	ah, dl		; New flags with the old!
		xor	edx, edx
		mov	dl, [esi]
		inc	esi
		jmp	dword [m6502zpregular+edx*4]

;
; LDA
;

times ($$-$) & 3 nop	; pad with NOPs to 4-byte boundary

RegInsta1:
		sub	dword [cyclesRemaining], byte 6
		jc	near noMoreExec	; Can't execute anymore!
		add	dword [dwElapsedTicks], byte 6
		mov	dl, [esi]		; Get the next instruction
		inc	esi		; Advance PC!
		add	dl, bl	; Add in X
		mov	dx, [ebp+edx]	; Get our 16 bit address
		mov	edi, [_m6502zpMemRead]	; Point to the read array

checkLoop60:
		cmp	[edi], word 0ffffh ; End of the list?
		je		short memoryRead60
		cmp	dx, [edi]	; Are we smaller?
		jb		short nextAddr60		; Yes, go to the next address
		cmp	dx, [edi+4]	; Are we bigger?
		jbe	short callRoutine60

nextAddr60:
		add	edi, 10h		; Next structure!
		jmp	short checkLoop60

callRoutine60:
		call	ReadMemoryByte	; Standard read routine
		mov	bl, [_m6502zpx]	; Get X back
		mov	cl, [_m6502zpy]	; Get Y back
		mov	ah, [_m6502zpaf + 1] ; Get our flags back
		jmp	short readExit60

memoryRead60:
		mov	al, [ebp + edx]	; Get our data

readExit60:
		mov	dl, ah	; Save flags
		or	al, al	; OR Our new value
		lahf		; Restore flags
		and	dl, 03fh	; Original value
		and	ah, 0c0h	; Only zero and sign
		or	ah, dl		; New flags with the old!
		xor	edx, edx
		mov	dl, [esi]
		inc	esi
		jmp	dword [m6502zpregular+edx*4]

;
; LDX
;

times ($$-$) & 3 nop	; pad with NOPs to 4-byte boundary

RegInsta2:
		sub	dword [cyclesRemaining], byte 2
		jc	near noMoreExec	; Can't execute anymore!
		add	dword [dwElapsedTicks], byte 2
		mov	bl, [esi]	; Get our next byte
		inc	esi		; Increment to our next byte
		mov	dl, ah	; Save flags
		or	bl, bl	; OR Our new value
		lahf		; Restore flags
		and	dl, 03fh	; Original value
		and	ah, 0c0h	; Only zero and sign
		or	ah, dl		; New flags with the old!
		xor	edx, edx
		mov	dl, [esi]
		inc	esi
		jmp	dword [m6502zpregular+edx*4]

;
; LDY
;

times ($$-$) & 3 nop	; pad with NOPs to 4-byte boundary

RegInsta4:
		sub	dword [cyclesRemaining], byte 3
		jc	near noMoreExec	; Can't execute anymore!
		add	dword [dwElapsedTicks], byte 3
		mov	dl, [esi]		; Get the next instruction
		inc	esi		; Advance PC!
		mov	cl, [ebp + edx]	; Zero page!
		mov	dl, ah	; Save flags
		or	cl, cl	; OR Our new value
		lahf		; Restore flags
		and	dl, 03fh	; Original value
		and	ah, 0c0h	; Only zero and sign
		or	ah, dl		; New flags with the old!
		xor	edx, edx
		mov	dl, [esi]
		inc	esi
		jmp	dword [m6502zpregular+edx*4]

;
; LDA
;

times ($$-$) & 3 nop	; pad with NOPs to 4-byte boundary

RegInsta5:
		sub	dword [cyclesRemaining], byte 3
		jc	near noMoreExec	; Can't execute anymore!
		add	dword [dwElapsedTicks], byte 3
		mov	dl, [esi]		; Get the next instruction
		inc	esi		; Advance PC!
		mov	al, [ebp+edx]	; Get our zero page byte
		mov	dl, ah	; Save flags
		or	al, al	; OR Our new value
		lahf		; Restore flags
		and	dl, 03fh	; Original value
		and	ah, 0c0h	; Only zero and sign
		or	ah, dl		; New flags with the old!
		xor	edx, edx
		mov	dl, [esi]
		inc	esi
		jmp	dword [m6502zpregular+edx*4]

;
; LDX
;

times ($$-$) & 3 nop	; pad with NOPs to 4-byte boundary

RegInsta6:
		sub	dword [cyclesRemaining], byte 3
		jc	near noMoreExec	; Can't execute anymore!
		add	dword [dwElapsedTicks], byte 3
		mov	dl, [esi]		; Get the next instruction
		inc	esi		; Advance PC!
		mov	bl, [ebp + edx]	; Zero page!
		mov	dl, ah	; Save flags
		or	bl, bl	; OR Our new value
		lahf		; Restore flags
		and	dl, 03fh	; Original value
		and	ah, 0c0h	; Only zero and sign
		or	ah, dl		; New flags with the old!
		xor	edx, edx
		mov	dl, [esi]
		inc	esi
		jmp	dword [m6502zpregular+edx*4]

;
; TAY
;

times ($$-$) & 3 nop	; pad with NOPs to 4-byte boundary

RegInsta8:
		sub	dword [cyclesRemaining], byte 2
		jc	near noMoreExec	; Can't execute anymore!
		add	dword [dwElapsedTicks], byte 2
		mov	cl, al	; Y = A
		mov	dl, ah	; Save flags
		or	cl, cl	; OR Our new value
		lahf		; Restore flags
		and	dl, 03fh	; Original value
		and	ah, 0c0h	; Only zero and sign
		or	ah, dl		; New flags with the old!
		xor	edx, edx
		mov	dl, [esi]
		inc	esi
		jmp	dword [m6502zpregular+edx*4]

;
; LDA
;

times ($$-$) & 3 nop	; pad with NOPs to 4-byte boundary

RegInsta9:
		sub	dword [cyclesRemaining], byte 2
		jc	near noMoreExec	; Can't execute anymore!
		add	dword [dwElapsedTicks], byte 2
		mov	al, [esi]	; Get our next byte
		inc	esi		; Increment to our next byte
		mov	dl, ah	; Save flags
		or	al, al	; OR Our new value
		lahf		; Restore flags
		and	dl, 03fh	; Original value
		and	ah, 0c0h	; Only zero and sign
		or	ah, dl		; New flags with the old!
		xor	edx, edx
		mov	dl, [esi]
		inc	esi
		jmp	dword [m6502zpregular+edx*4]

;
; TAX
;

times ($$-$) & 3 nop	; pad with NOPs to 4-byte boundary

RegInstaa:
		sub	dword [cyclesRemaining], byte 2
		jc	near noMoreExec	; Can't execute anymore!
		add	dword [dwElapsedTicks], byte 2
		mov	bl, al	; X = A
		mov	dl, ah	; Save flags
		or	bl, bl	; OR Our new value
		lahf		; Restore flags
		and	dl, 03fh	; Original value
		and	ah, 0c0h	; Only zero and sign
		or	ah, dl		; New flags with the old!
		xor	edx, edx
		mov	dl, [esi]
		inc	esi
		jmp	dword [m6502zpregular+edx*4]

;
; LDY
;

times ($$-$) & 3 nop	; pad with NOPs to 4-byte boundary

RegInstac:
		sub	dword [cyclesRemaining], byte 4
		jc	near noMoreExec	; Can't execute anymore!
		add	dword [dwElapsedTicks], byte 4
		mov	dx, [esi]	; Get our address
		add	esi, 2	; Increment past instruction
		mov	edi, [_m6502zpMemRead]	; Point to the read array

checkLoop61:
		cmp	[edi], word 0ffffh ; End of the list?
		je		short memoryRead61
		cmp	dx, [edi]	; Are we smaller?
		jb		short nextAddr61		; Yes, go to the next address
		cmp	dx, [edi+4]	; Are we bigger?
		jbe	short callRoutine61

nextAddr61:
		add	edi, 10h		; Next structure!
		jmp	short checkLoop61

callRoutine61:
		call	ReadMemoryByte	; Standard read routine
		mov	bl, [_m6502zpx]	; Get X back
		mov	cl, al	; Get our value
		mov	ax, [_m6502zpaf]	; Get our flags & stuff back
		jmp	short readExit61

memoryRead61:
		mov	cl, [ebp + edx]	; Get our data

readExit61:
		mov	dl, ah	; Save flags
		or	cl, cl	; OR Our new value
		lahf		; Restore flags
		and	dl, 03fh	; Original value
		and	ah, 0c0h	; Only zero and sign
		or	ah, dl		; New flags with the old!
		xor	edx, edx
		mov	dl, [esi]
		inc	esi
		jmp	dword [m6502zpregular+edx*4]

;
; LDA
;

times ($$-$) & 3 nop	; pad with NOPs to 4-byte boundary

RegInstad:
		sub	dword [cyclesRemaining], byte 4
		jc	near noMoreExec	; Can't execute anymore!
		add	dword [dwElapsedTicks], byte 4
		mov	dx, [esi]	; Get our address
		add	esi, 2	; Increment past instruction
		mov	edi, [_m6502zpMemRead]	; Point to the read array

checkLoop62:
		cmp	[edi], word 0ffffh ; End of the list?
		je		short memoryRead62
		cmp	dx, [edi]	; Are we smaller?
		jb		short nextAddr62		; Yes, go to the next address
		cmp	dx, [edi+4]	; Are we bigger?
		jbe	short callRoutine62

nextAddr62:
		add	edi, 10h		; Next structure!
		jmp	short checkLoop62

callRoutine62:
		call	ReadMemoryByte	; Standard read routine
		mov	bl, [_m6502zpx]	; Get X back
		mov	cl, [_m6502zpy]	; Get Y back
		mov	ah, [_m6502zpaf + 1] ; Get our flags back
		jmp	short readExit62

memoryRead62:
		mov	al, [ebp + edx]	; Get our data

readExit62:
		mov	dl, ah	; Save flags
		or	al, al	; OR Our new value
		lahf		; Restore flags
		and	dl, 03fh	; Original value
		and	ah, 0c0h	; Only zero and sign
		or	ah, dl		; New flags with the old!
		xor	edx, edx
		mov	dl, [esi]
		inc	esi
		jmp	dword [m6502zpregular+edx*4]

;
; LDX
;

times ($$-$) & 3 nop	; pad with NOPs to 4-byte boundary

RegInstae:
		sub	dword [cyclesRemaining], byte 4
		jc	near noMoreExec	; Can't execute anymore!
		add	dword [dwElapsedTicks], byte 4
		mov	dx, [esi]	; Get our address
		add	esi, 2	; Increment past instruction
		mov	edi, [_m6502zpMemRead]	; Point to the read array

checkLoop63:
		cmp	[edi], word 0ffffh ; End of the list?
		je		short memoryRead63
		cmp	dx, [edi]	; Are we smaller?
		jb		short nextAddr63		; Yes, go to the next address
		cmp	dx, [edi+4]	; Are we bigger?
		jbe	short callRoutine63

nextAddr63:
		add	edi, 10h		; Next structure!
		jmp	short checkLoop63

callRoutine63:
		call	ReadMemoryByte	; Standard read routine
		mov	cl, [_m6502zpy]	; Get Y back
		mov	bl, al	; Get our value
		mov	ax, [_m6502zpaf]	; Get our flags & stuff back
		jmp	short readExit63

memoryRead63:
		mov	bl, [ebp + edx]	; Get our data

readExit63:
		mov	dl, ah	; Save flags
		or	bl, bl	; OR Our new value
		lahf		; Restore flags
		and	dl, 03fh	; Original value
		and	ah, 0c0h	; Only zero and sign
		or	ah, dl		; New flags with the old!
		xor	edx, edx
		mov	dl, [esi]
		inc	esi
		jmp	dword [m6502zpregular+edx*4]

;
; Branch
;

times ($$-$) & 3 nop	; pad with NOPs to 4-byte boundary

RegInstb0:
		sub	dword [cyclesRemaining], byte 3
		jc	near noMoreExec	; Can't execute anymore!
		add	dword [dwElapsedTicks], byte 3
		test	ah, 1h	; Is carry set?
		jnz	short takeJumpb0	; Do it!
		inc	esi	; Skip past the offset
		xor	edx, edx
		mov	dl, [esi]
		inc	esi
		jmp	dword [m6502zpregular+edx*4]

takeJumpb0:
		mov	dl, [esi]		; Get the next instruction
		inc	esi		; Advance PC!
		movsx	dx, dl
		sub	esi, ebp
		add	si, dx
		add	esi, ebp
		xor	edx, edx
		mov	dl, [esi]
		inc	esi
		jmp	dword [m6502zpregular+edx*4]

;
; LDA
;

times ($$-$) & 3 nop	; pad with NOPs to 4-byte boundary

RegInstb1:
		sub	dword [cyclesRemaining], byte 5
		jc	near noMoreExec	; Can't execute anymore!
		add	dword [dwElapsedTicks], byte 5
		mov	dl, [esi]		; Get the next instruction
		inc	esi		; Advance PC!
		mov	dx, [ebp+edx]	; Get our 16 bit address
		add	dx, cx	; Add in Y
		mov	edi, [_m6502zpMemRead]	; Point to the read array

checkLoop65:
		cmp	[edi], word 0ffffh ; End of the list?
		je		short memoryRead65
		cmp	dx, [edi]	; Are we smaller?
		jb		short nextAddr65		; Yes, go to the next address
		cmp	dx, [edi+4]	; Are we bigger?
		jbe	short callRoutine65

nextAddr65:
		add	edi, 10h		; Next structure!
		jmp	short checkLoop65

callRoutine65:
		call	ReadMemoryByte	; Standard read routine
		mov	bl, [_m6502zpx]	; Get X back
		mov	cl, [_m6502zpy]	; Get Y back
		mov	ah, [_m6502zpaf + 1] ; Get our flags back
		jmp	short readExit65

memoryRead65:
		mov	al, [ebp + edx]	; Get our data

readExit65:
		mov	dl, ah	; Save flags
		or	al, al	; OR Our new value
		lahf		; Restore flags
		and	dl, 03fh	; Original value
		and	ah, 0c0h	; Only zero and sign
		or	ah, dl		; New flags with the old!
		xor	edx, edx
		mov	dl, [esi]
		inc	esi
		jmp	dword [m6502zpregular+edx*4]

;
; LDY
;

times ($$-$) & 3 nop	; pad with NOPs to 4-byte boundary

RegInstb4:
		sub	dword [cyclesRemaining], byte 4
		jc	near noMoreExec	; Can't execute anymore!
		add	dword [dwElapsedTicks], byte 4
		mov	dl, [esi]		; Get the next instruction
		inc	esi		; Advance PC!
		add	dl, bl	; Add X
		mov	cl, [ebp + edx]	; Zero page!
		mov	dl, ah	; Save flags
		or	cl, cl	; OR Our new value
		lahf		; Restore flags
		and	dl, 03fh	; Original value
		and	ah, 0c0h	; Only zero and sign
		or	ah, dl		; New flags with the old!
		xor	edx, edx
		mov	dl, [esi]
		inc	esi
		jmp	dword [m6502zpregular+edx*4]

;
; LDA
;

times ($$-$) & 3 nop	; pad with NOPs to 4-byte boundary

RegInstb5:
		sub	dword [cyclesRemaining], byte 4
		jc	near noMoreExec	; Can't execute anymore!
		add	dword [dwElapsedTicks], byte 4
		mov	dl, [esi]		; Get the next instruction
		inc	esi		; Advance PC!
		add	dl, bl	; Add X
		mov	al, [ebp+edx]	; Get our zero page byte
		mov	dl, ah	; Save flags
		or	al, al	; OR Our new value
		lahf		; Restore flags
		and	dl, 03fh	; Original value
		and	ah, 0c0h	; Only zero and sign
		or	ah, dl		; New flags with the old!
		xor	edx, edx
		mov	dl, [esi]
		inc	esi
		jmp	dword [m6502zpregular+edx*4]

;
; LDX
;

times ($$-$) & 3 nop	; pad with NOPs to 4-byte boundary

RegInstb6:
		sub	dword [cyclesRemaining], byte 4
		jc	near noMoreExec	; Can't execute anymore!
		add	dword [dwElapsedTicks], byte 4
		mov	dl, [esi]		; Get the next instruction
		inc	esi		; Advance PC!
		add	dl, cl	; Add Y
		mov	bl, [ebp + edx]	; Zero page!
		mov	dl, ah	; Save flags
		or	bl, bl	; OR Our new value
		lahf		; Restore flags
		and	dl, 03fh	; Original value
		and	ah, 0c0h	; Only zero and sign
		or	ah, dl		; New flags with the old!
		xor	edx, edx
		mov	dl, [esi]
		inc	esi
		jmp	dword [m6502zpregular+edx*4]

;
; CLV
;

times ($$-$) & 3 nop	; pad with NOPs to 4-byte boundary

RegInstb8:
		sub	dword [cyclesRemaining], byte 2
		jc	near noMoreExec	; Can't execute anymore!
		add	dword [dwElapsedTicks], byte 2
		and	ah, 0efh	; Clear out overflow
		xor	edx, edx
		mov	dl, [esi]
		inc	esi
		jmp	dword [m6502zpregular+edx*4]

;
; LDA
;

times ($$-$) & 3 nop	; pad with NOPs to 4-byte boundary

RegInstb9:
		sub	dword [cyclesRemaining], byte 4
		jc	near noMoreExec	; Can't execute anymore!
		add	dword [dwElapsedTicks], byte 4
		mov	dx, [esi]	; Get our address
		add	esi, 2	; Increment past instruction
		add	dx, cx	 ; Add Y
		mov	edi, [_m6502zpMemRead]	; Point to the read array

checkLoop66:
		cmp	[edi], word 0ffffh ; End of the list?
		je		short memoryRead66
		cmp	dx, [edi]	; Are we smaller?
		jb		short nextAddr66		; Yes, go to the next address
		cmp	dx, [edi+4]	; Are we bigger?
		jbe	short callRoutine66

nextAddr66:
		add	edi, 10h		; Next structure!
		jmp	short checkLoop66

callRoutine66:
		call	ReadMemoryByte	; Standard read routine
		mov	bl, [_m6502zpx]	; Get X back
		mov	cl, [_m6502zpy]	; Get Y back
		mov	ah, [_m6502zpaf + 1] ; Get our flags back
		jmp	short readExit66

memoryRead66:
		mov	al, [ebp + edx]	; Get our data

readExit66:
		mov	dl, ah	; Save flags
		or	al, al	; OR Our new value
		lahf		; Restore flags
		and	dl, 03fh	; Original value
		and	ah, 0c0h	; Only zero and sign
		or	ah, dl		; New flags with the old!
		xor	edx, edx
		mov	dl, [esi]
		inc	esi
		jmp	dword [m6502zpregular+edx*4]

;
; TSX
;

times ($$-$) & 3 nop	; pad with NOPs to 4-byte boundary

RegInstba:
		sub	dword [cyclesRemaining], byte 2
		jc	near noMoreExec	; Can't execute anymore!
		add	dword [dwElapsedTicks], byte 2
		mov	bl, [_m6502zps]	; S -> X
		mov	dl, ah	; Save flags
		or	bl, bl	; OR Our new value
		lahf		; Restore flags
		and	dl, 03fh	; Original value
		and	ah, 0c0h	; Only zero and sign
		or	ah, dl		; New flags with the old!
		xor	edx, edx
		mov	dl, [esi]
		inc	esi
		jmp	dword [m6502zpregular+edx*4]

;
; LDY
;

times ($$-$) & 3 nop	; pad with NOPs to 4-byte boundary

RegInstbc:
		sub	dword [cyclesRemaining], byte 4
		jc	near noMoreExec	; Can't execute anymore!
		add	dword [dwElapsedTicks], byte 4
		mov	dx, [esi]	; Get our address
		add	esi, 2	; Increment past instruction
		add	dx, bx	 ; Add X
		mov	edi, [_m6502zpMemRead]	; Point to the read array

checkLoop67:
		cmp	[edi], word 0ffffh ; End of the list?
		je		short memoryRead67
		cmp	dx, [edi]	; Are we smaller?
		jb		short nextAddr67		; Yes, go to the next address
		cmp	dx, [edi+4]	; Are we bigger?
		jbe	short callRoutine67

nextAddr67:
		add	edi, 10h		; Next structure!
		jmp	short checkLoop67

callRoutine67:
		call	ReadMemoryByte	; Standard read routine
		mov	bl, [_m6502zpx]	; Get X back
		mov	cl, al	; Get our value
		mov	ax, [_m6502zpaf]	; Get our flags & stuff back
		jmp	short readExit67

memoryRead67:
		mov	cl, [ebp + edx]	; Get our data

readExit67:
		mov	dl, ah	; Save flags
		or	cl, cl	; OR Our new value
		lahf		; Restore flags
		and	dl, 03fh	; Original value
		and	ah, 0c0h	; Only zero and sign
		or	ah, dl		; New flags with the old!
		xor	edx, edx
		mov	dl, [esi]
		inc	esi
		jmp	dword [m6502zpregular+edx*4]

;
; LDA
;

times ($$-$) & 3 nop	; pad with NOPs to 4-byte boundary

RegInstbd:
		sub	dword [cyclesRemaining], byte 4
		jc	near noMoreExec	; Can't execute anymore!
		add	dword [dwElapsedTicks], byte 4
		mov	dx, [esi]	; Get our address
		add	esi, 2	; Increment past instruction
		add	dx, bx	 ; Add X
		mov	edi, [_m6502zpMemRead]	; Point to the read array

checkLoop68:
		cmp	[edi], word 0ffffh ; End of the list?
		je		short memoryRead68
		cmp	dx, [edi]	; Are we smaller?
		jb		short nextAddr68		; Yes, go to the next address
		cmp	dx, [edi+4]	; Are we bigger?
		jbe	short callRoutine68

nextAddr68:
		add	edi, 10h		; Next structure!
		jmp	short checkLoop68

callRoutine68:
		call	ReadMemoryByte	; Standard read routine
		mov	bl, [_m6502zpx]	; Get X back
		mov	cl, [_m6502zpy]	; Get Y back
		mov	ah, [_m6502zpaf + 1] ; Get our flags back
		jmp	short readExit68

memoryRead68:
		mov	al, [ebp + edx]	; Get our data

readExit68:
		mov	dl, ah	; Save flags
		or	al, al	; OR Our new value
		lahf		; Restore flags
		and	dl, 03fh	; Original value
		and	ah, 0c0h	; Only zero and sign
		or	ah, dl		; New flags with the old!
		xor	edx, edx
		mov	dl, [esi]
		inc	esi
		jmp	dword [m6502zpregular+edx*4]

;
; LDX
;

times ($$-$) & 3 nop	; pad with NOPs to 4-byte boundary

RegInstbe:
		sub	dword [cyclesRemaining], byte 4
		jc	near noMoreExec	; Can't execute anymore!
		add	dword [dwElapsedTicks], byte 4
		mov	dx, [esi]	; Get our address
		add	esi, 2	; Increment past instruction
		add	dx, cx	 ; Add Y
		mov	edi, [_m6502zpMemRead]	; Point to the read array

checkLoop69:
		cmp	[edi], word 0ffffh ; End of the list?
		je		short memoryRead69
		cmp	dx, [edi]	; Are we smaller?
		jb		short nextAddr69		; Yes, go to the next address
		cmp	dx, [edi+4]	; Are we bigger?
		jbe	short callRoutine69

nextAddr69:
		add	edi, 10h		; Next structure!
		jmp	short checkLoop69

callRoutine69:
		call	ReadMemoryByte	; Standard read routine
		mov	cl, [_m6502zpy]	; Get Y back
		mov	bl, al	; Get our value
		mov	ax, [_m6502zpaf]	; Get our flags & stuff back
		jmp	short readExit69

memoryRead69:
		mov	bl, [ebp + edx]	; Get our data

readExit69:
		mov	dl, ah	; Save flags
		or	bl, bl	; OR Our new value
		lahf		; Restore flags
		and	dl, 03fh	; Original value
		and	ah, 0c0h	; Only zero and sign
		or	ah, dl		; New flags with the old!
		xor	edx, edx
		mov	dl, [esi]
		inc	esi
		jmp	dword [m6502zpregular+edx*4]

;
; CPY
;

times ($$-$) & 3 nop	; pad with NOPs to 4-byte boundary

RegInstc0:
		sub	dword [cyclesRemaining], byte 2
		jc	near noMoreExec	; Can't execute anymore!
		add	dword [dwElapsedTicks], byte 2
		mov	dl, [esi]	; Get our next byte
		inc	esi		; Increment to our next byte
		mov	dh, ah	; Get our flags
		cmp	cl, dl	; Compare with Y!
		cmc			; Compliment carry flag
		lahf
		and	ah, 0c1h	; Sign, zero, and carry
		and	dh, 03eh	; Everything but sign, zero and carry
		or		ah, dh	; OR In our new flags
		xor	edx, edx
		mov	dl, [esi]
		inc	esi
		jmp	dword [m6502zpregular+edx*4]

;
; CMP
;

times ($$-$) & 3 nop	; pad with NOPs to 4-byte boundary

RegInstc1:
		sub	dword [cyclesRemaining], byte 6
		jc	near noMoreExec	; Can't execute anymore!
		add	dword [dwElapsedTicks], byte 6
		mov	dl, [esi]		; Get the next instruction
		inc	esi		; Advance PC!
		add	dl, bl	; Add in X
		mov	dx, [ebp+edx]	; Get our 16 bit address
		mov	edi, [_m6502zpMemRead]	; Point to the read array

checkLoop70:
		cmp	[edi], word 0ffffh ; End of the list?
		je		short memoryRead70
		cmp	dx, [edi]	; Are we smaller?
		jb		short nextAddr70		; Yes, go to the next address
		cmp	dx, [edi+4]	; Are we bigger?
		jbe	short callRoutine70

nextAddr70:
		add	edi, 10h		; Next structure!
		jmp	short checkLoop70

callRoutine70:
		call	ReadMemoryByte	; Standard read routine
		mov	bl, [_m6502zpx]	; Get X back
		mov	cl, [_m6502zpy]	; Get Y back
		mov	dl, al	; Get our value
		mov	ax, [_m6502zpaf]	; Get our flags & stuff back
		jmp	short readExit70

memoryRead70:
		mov	dl, [ebp + edx]	; Get our data

readExit70:
		mov	dh, ah	; Get our flags
		cmp	al, dl	; Compare!
		cmc			; Compliment carry flag
		lahf
		and	ah, 0c1h	; Sign, zero, and carry
		and	dh, 03eh	; Everything but sign, zero and carry
		or		ah, dh	; OR In our new flags
		xor	edx, edx
		mov	dl, [esi]
		inc	esi
		jmp	dword [m6502zpregular+edx*4]

;
; CPY
;

times ($$-$) & 3 nop	; pad with NOPs to 4-byte boundary

RegInstc4:
		sub	dword [cyclesRemaining], byte 3
		jc	near noMoreExec	; Can't execute anymore!
		add	dword [dwElapsedTicks], byte 3
		mov	dl, [esi]		; Get the next instruction
		inc	esi		; Advance PC!
		mov	dl, [ebp+edx]	; Get our zero page data
		mov	dh, ah	; Get our flags
		cmp	cl, dl	; Compare with Y!
		cmc			; Compliment carry flag
		lahf
		and	ah, 0c1h	; Sign, zero, and carry
		and	dh, 03eh	; Everything but sign, zero and carry
		or		ah, dh	; OR In our new flags
		xor	edx, edx
		mov	dl, [esi]
		inc	esi
		jmp	dword [m6502zpregular+edx*4]

;
; CMP
;

times ($$-$) & 3 nop	; pad with NOPs to 4-byte boundary

RegInstc5:
		sub	dword [cyclesRemaining], byte 3
		jc	near noMoreExec	; Can't execute anymore!
		add	dword [dwElapsedTicks], byte 3
		mov	dl, [esi]		; Get the next instruction
		inc	esi		; Advance PC!
		mov	dl, [ebp+edx]	; Get our zero page data
		mov	dh, ah	; Get our flags
		cmp	al, dl	; Compare!
		cmc			; Compliment carry flag
		lahf
		and	ah, 0c1h	; Sign, zero, and carry
		and	dh, 03eh	; Everything but sign, zero and carry
		or		ah, dh	; OR In our new flags
		xor	edx, edx
		mov	dl, [esi]
		inc	esi
		jmp	dword [m6502zpregular+edx*4]

;
; DEC
;

times ($$-$) & 3 nop	; pad with NOPs to 4-byte boundary

RegInstc6:
		sub	dword [cyclesRemaining], byte 5
		jc	near noMoreExec	; Can't execute anymore!
		add	dword [dwElapsedTicks], byte 5
		mov	dl, [esi]		; Get the next instruction
		inc	esi		; Advance PC!
		mov	ch, ah	; Save flags
		and	ch, 03fh	; No sign or zero flags
		dec	byte [edx+ebp]	; Increment our zero page stuff
		lahf
		and	ah, 0c0h	; Only sign &  zero flags
		or	ah, ch	; Merge the two flags together
		xor	bh, bh	; Zero this so we don't totally screw things up
		xor	ch, ch	; Zero this as well
		xor	edx, edx
		mov	dl, [esi]
		inc	esi
		jmp	dword [m6502zpregular+edx*4]

;
; INY
;

times ($$-$) & 3 nop	; pad with NOPs to 4-byte boundary

RegInstc8:
		sub	dword [cyclesRemaining], byte 2
		jc	near noMoreExec	; Can't execute anymore!
		add	dword [dwElapsedTicks], byte 2
		mov	dl, ah	; Save flags
		inc	cl	; Increment
		lahf		; Restore flags
		and	dl, 03fh	; Original value
		and	ah, 0c0h	; Only zero and sign
		or	ah, dl		; New flags with the old!
		xor	edx, edx
		mov	dl, [esi]
		inc	esi
		jmp	dword [m6502zpregular+edx*4]

;
; CMP
;

times ($$-$) & 3 nop	; pad with NOPs to 4-byte boundary

RegInstc9:
		sub	dword [cyclesRemaining], byte 2
		jc	near noMoreExec	; Can't execute anymore!
		add	dword [dwElapsedTicks], byte 2
		mov	dl, [esi]	; Get our next byte
		inc	esi		; Increment to our next byte
		mov	dh, ah	; Get our flags
		cmp	al, dl	; Compare!
		cmc			; Compliment carry flag
		lahf
		and	ah, 0c1h	; Sign, zero, and carry
		and	dh, 03eh	; Everything but sign, zero and carry
		or		ah, dh	; OR In our new flags
		xor	edx, edx
		mov	dl, [esi]
		inc	esi
		jmp	dword [m6502zpregular+edx*4]

;
; DEX
;

times ($$-$) & 3 nop	; pad with NOPs to 4-byte boundary

RegInstca:
		sub	dword [cyclesRemaining], byte 2
		jc	near noMoreExec	; Can't execute anymore!
		add	dword [dwElapsedTicks], byte 2
		mov	dl, ah	; Save flags
		dec	bl	; Decrement
		lahf		; Restore flags
		and	dl, 03fh	; Original value
		and	ah, 0c0h	; Only zero and sign
		or	ah, dl		; New flags with the old!
		xor	edx, edx
		mov	dl, [esi]
		inc	esi
		jmp	dword [m6502zpregular+edx*4]

;
; CPY
;

times ($$-$) & 3 nop	; pad with NOPs to 4-byte boundary

RegInstcc:
		sub	dword [cyclesRemaining], byte 4
		jc	near noMoreExec	; Can't execute anymore!
		add	dword [dwElapsedTicks], byte 4
		mov	dx, [esi]	; Get our address
		add	esi, 2	; Increment past instruction
		mov	edi, [_m6502zpMemRead]	; Point to the read array

checkLoop71:
		cmp	[edi], word 0ffffh ; End of the list?
		je		short memoryRead71
		cmp	dx, [edi]	; Are we smaller?
		jb		short nextAddr71		; Yes, go to the next address
		cmp	dx, [edi+4]	; Are we bigger?
		jbe	short callRoutine71

nextAddr71:
		add	edi, 10h		; Next structure!
		jmp	short checkLoop71

callRoutine71:
		call	ReadMemoryByte	; Standard read routine
		mov	bl, [_m6502zpx]	; Get X back
		mov	cl, [_m6502zpy]	; Get Y back
		mov	dl, al	; Get our value
		mov	ax, [_m6502zpaf]	; Get our flags & stuff back
		jmp	short readExit71

memoryRead71:
		mov	dl, [ebp + edx]	; Get our data

readExit71:
		mov	dh, ah	; Get our flags
		cmp	cl, dl	; Compare with Y!
		cmc			; Compliment carry flag
		lahf
		and	ah, 0c1h	; Sign, zero, and carry
		and	dh, 03eh	; Everything but sign, zero and carry
		or		ah, dh	; OR In our new flags
		xor	edx, edx
		mov	dl, [esi]
		inc	esi
		jmp	dword [m6502zpregular+edx*4]

;
; CMP
;

times ($$-$) & 3 nop	; pad with NOPs to 4-byte boundary

RegInstcd:
		sub	dword [cyclesRemaining], byte 4
		jc	near noMoreExec	; Can't execute anymore!
		add	dword [dwElapsedTicks], byte 4
		mov	dx, [esi]	; Get our address
		add	esi, 2	; Increment past instruction
		mov	edi, [_m6502zpMemRead]	; Point to the read array

checkLoop72:
		cmp	[edi], word 0ffffh ; End of the list?
		je		short memoryRead72
		cmp	dx, [edi]	; Are we smaller?
		jb		short nextAddr72		; Yes, go to the next address
		cmp	dx, [edi+4]	; Are we bigger?
		jbe	short callRoutine72

nextAddr72:
		add	edi, 10h		; Next structure!
		jmp	short checkLoop72

callRoutine72:
		call	ReadMemoryByte	; Standard read routine
		mov	bl, [_m6502zpx]	; Get X back
		mov	cl, [_m6502zpy]	; Get Y back
		mov	dl, al	; Get our value
		mov	ax, [_m6502zpaf]	; Get our flags & stuff back
		jmp	short readExit72

memoryRead72:
		mov	dl, [ebp + edx]	; Get our data

readExit72:
		mov	dh, ah	; Get our flags
		cmp	al, dl	; Compare!
		cmc			; Compliment carry flag
		lahf
		and	ah, 0c1h	; Sign, zero, and carry
		and	dh, 03eh	; Everything but sign, zero and carry
		or		ah, dh	; OR In our new flags
		xor	edx, edx
		mov	dl, [esi]
		inc	esi
		jmp	dword [m6502zpregular+edx*4]

;
; DEC
;

times ($$-$) & 3 nop	; pad with NOPs to 4-byte boundary

RegInstce:
		sub	dword [cyclesRemaining], byte 6
		jc	near noMoreExec	; Can't execute anymore!
		add	dword [dwElapsedTicks], byte 6
		mov	dx, [esi]	; Get our address
		add	esi, 2	; Increment past instruction
		push	edx	; Save this for later
		mov	edi, [_m6502zpMemRead]	; Point to the read array

checkLoop73:
		cmp	[edi], word 0ffffh ; End of the list?
		je		short memoryRead73
		cmp	dx, [edi]	; Are we smaller?
		jb		short nextAddr73		; Yes, go to the next address
		cmp	dx, [edi+4]	; Are we bigger?
		jbe	short callRoutine73

nextAddr73:
		add	edi, 10h		; Next structure!
		jmp	short checkLoop73

callRoutine73:
		call	ReadMemoryByte	; Standard read routine
		mov	bl, [_m6502zpx]	; Get X back
		mov	cl, [_m6502zpy]	; Get Y back
		mov	dl, al	; Get our value
		mov	ax, [_m6502zpaf]	; Get our flags & stuff back
		jmp	short readExit73

memoryRead73:
		mov	dl, [ebp + edx]	; Get our data

readExit73:
		mov	bh, dl	; Save the data we just got
		mov	ch, ah	; Save flags
		and	ch, 03fh	; No sign or zero flags
		dec	bh		; Decrement!
		lahf
		and	ah, 0c0h	; Only sign &  zero flags
		or	ah, ch	; Merge the two flags together
		pop	edx	; Restore our address
		mov	edi, [_m6502zpMemWrite]	; Point to the write array

checkLoop74:
		cmp	[edi], word 0ffffh ; End of our list?
		je	near memoryWrite74	; Yes - go write it!
		cmp	dx, [edi]	; Are we smaller?
		jb	nextAddr74	; Yes... go to the next addr
		cmp	dx, [edi+4]	; Are we bigger?
		jbe	callRoutine74	; If not, go call it!

nextAddr74:
		add	edi, 10h		; Next structure, please
		jmp	short checkLoop74

callRoutine74:
		mov	[_m6502zpx], bl	; Save X
		mov	[_m6502zpy], cl	; Save Y
		mov	[_m6502zpaf], ax	; Save Accumulator & flags
		sub	esi, ebp	; Our program counter
		mov	[_m6502zppc], si	; Save our program counter
		push	edi	; Pointer to MemoryWriteByte structure
		mov	bl, bh	; Put a copy here
		push	ebx	; The byte value
		and	edx, 0ffffh	; Only lower 16 bits
		push	edx	; The address
		call	dword [edi + 8] ; Go call our handler
		add	esp, 12	; Get rid of our stack
		xor	ebx, ebx	; Zero this
		xor	ecx, ecx	; This too!
		mov	bl, [_m6502zpx]	; Get X back
		mov	cl, [_m6502zpy]	; Get Y back
		xor	esi, esi	; Zero it!
		mov	si, [_m6502zppc]	; Get our program counter back
		mov	ebp, [_m6502zpBase] ; Base pointer comes back
		add	esi, ebp	; Rebase it properly
		mov	ax, [_m6502zpaf]	; Get our flags & stuff back
		jmp	short writeMacroExit74

memoryWrite74:
		mov	[ebp + edx], bh ; Store the byte

writeMacroExit74:
		xor	bh, bh	; Zero this so we don't totally screw things up
		xor	ch, ch	; Zero this as well
		xor	edx, edx
		mov	dl, [esi]
		inc	esi
		jmp	dword [m6502zpregular+edx*4]

;
; Branch
;

times ($$-$) & 3 nop	; pad with NOPs to 4-byte boundary

RegInstd0:
		sub	dword [cyclesRemaining], byte 3
		jc	near noMoreExec	; Can't execute anymore!
		add	dword [dwElapsedTicks], byte 3
		test	ah, 40h	; Are we non-zero?
		jz	short takeJumpd0	; Do it!
		inc	esi	; Skip past the offset
		xor	edx, edx
		mov	dl, [esi]
		inc	esi
		jmp	dword [m6502zpregular+edx*4]

takeJumpd0:
		mov	dl, [esi]		; Get the next instruction
		inc	esi		; Advance PC!
		movsx	dx, dl
		sub	esi, ebp
		add	si, dx
		add	esi, ebp
		xor	edx, edx
		mov	dl, [esi]
		inc	esi
		jmp	dword [m6502zpregular+edx*4]

;
; CMP
;

times ($$-$) & 3 nop	; pad with NOPs to 4-byte boundary

RegInstd1:
		sub	dword [cyclesRemaining], byte 5
		jc	near noMoreExec	; Can't execute anymore!
		add	dword [dwElapsedTicks], byte 5
		mov	dl, [esi]		; Get the next instruction
		inc	esi		; Advance PC!
		mov	dx, [ebp+edx]	; Get our 16 bit address
		add	dx, cx	; Add in Y
		mov	edi, [_m6502zpMemRead]	; Point to the read array

checkLoop76:
		cmp	[edi], word 0ffffh ; End of the list?
		je		short memoryRead76
		cmp	dx, [edi]	; Are we smaller?
		jb		short nextAddr76		; Yes, go to the next address
		cmp	dx, [edi+4]	; Are we bigger?
		jbe	short callRoutine76

nextAddr76:
		add	edi, 10h		; Next structure!
		jmp	short checkLoop76

callRoutine76:
		call	ReadMemoryByte	; Standard read routine
		mov	bl, [_m6502zpx]	; Get X back
		mov	cl, [_m6502zpy]	; Get Y back
		mov	dl, al	; Get our value
		mov	ax, [_m6502zpaf]	; Get our flags & stuff back
		jmp	short readExit76

memoryRead76:
		mov	dl, [ebp + edx]	; Get our data

readExit76:
		mov	dh, ah	; Get our flags
		cmp	al, dl	; Compare!
		cmc			; Compliment carry flag
		lahf
		and	ah, 0c1h	; Sign, zero, and carry
		and	dh, 03eh	; Everything but sign, zero and carry
		or		ah, dh	; OR In our new flags
		xor	edx, edx
		mov	dl, [esi]
		inc	esi
		jmp	dword [m6502zpregular+edx*4]

;
; CMP
;

times ($$-$) & 3 nop	; pad with NOPs to 4-byte boundary

RegInstd5:
		sub	dword [cyclesRemaining], byte 4
		jc	near noMoreExec	; Can't execute anymore!
		add	dword [dwElapsedTicks], byte 4
		mov	dl, [esi]		; Get the next instruction
		inc	esi		; Advance PC!
		add	dl, bl	; Add X
		mov	dl, [ebp+edx]	; Get our zero page data
		mov	dh, ah	; Get our flags
		cmp	al, dl	; Compare!
		cmc			; Compliment carry flag
		lahf
		and	ah, 0c1h	; Sign, zero, and carry
		and	dh, 03eh	; Everything but sign, zero and carry
		or		ah, dh	; OR In our new flags
		xor	edx, edx
		mov	dl, [esi]
		inc	esi
		jmp	dword [m6502zpregular+edx*4]

;
; DEC
;

times ($$-$) & 3 nop	; pad with NOPs to 4-byte boundary

RegInstd6:
		sub	dword [cyclesRemaining], byte 6
		jc	near noMoreExec	; Can't execute anymore!
		add	dword [dwElapsedTicks], byte 6
		mov	dl, [esi]		; Get the next instruction
		inc	esi		; Advance PC!
		add	dl, bl	; Add X
		mov	ch, ah	; Save flags
		and	ch, 03fh	; No sign or zero flags
		dec	byte [edx+ebp]	; Increment our zero page stuff
		lahf
		and	ah, 0c0h	; Only sign &  zero flags
		or	ah, ch	; Merge the two flags together
		xor	bh, bh	; Zero this so we don't totally screw things up
		xor	ch, ch	; Zero this as well
		xor	edx, edx
		mov	dl, [esi]
		inc	esi
		jmp	dword [m6502zpregular+edx*4]

;
; CLD
;

times ($$-$) & 3 nop	; pad with NOPs to 4-byte boundary

RegInstd8:
		sub	dword [cyclesRemaining], byte 2
		jc	near noMoreExec	; Can't execute anymore!
		add	dword [dwElapsedTicks], byte 2
		and	[_altFlags], byte 0f7h	; Binary mode
		xor	edx, edx
		mov	dl, [esi]
		inc	esi
		jmp	dword [m6502zpregular+edx*4]

;
; CMP
;

times ($$-$) & 3 nop	; pad with NOPs to 4-byte boundary

RegInstd9:
		sub	dword [cyclesRemaining], byte 4
		jc	near noMoreExec	; Can't execute anymore!
		add	dword [dwElapsedTicks], byte 4
		mov	dx, [esi]	; Get our address
		add	esi, 2	; Increment past instruction
		add	dx, cx	 ; Add Y
		mov	edi, [_m6502zpMemRead]	; Point to the read array

checkLoop77:
		cmp	[edi], word 0ffffh ; End of the list?
		je		short memoryRead77
		cmp	dx, [edi]	; Are we smaller?
		jb		short nextAddr77		; Yes, go to the next address
		cmp	dx, [edi+4]	; Are we bigger?
		jbe	short callRoutine77

nextAddr77:
		add	edi, 10h		; Next structure!
		jmp	short checkLoop77

callRoutine77:
		call	ReadMemoryByte	; Standard read routine
		mov	bl, [_m6502zpx]	; Get X back
		mov	cl, [_m6502zpy]	; Get Y back
		mov	dl, al	; Get our value
		mov	ax, [_m6502zpaf]	; Get our flags & stuff back
		jmp	short readExit77

memoryRead77:
		mov	dl, [ebp + edx]	; Get our data

readExit77:
		mov	dh, ah	; Get our flags
		cmp	al, dl	; Compare!
		cmc			; Compliment carry flag
		lahf
		and	ah, 0c1h	; Sign, zero, and carry
		and	dh, 03eh	; Everything but sign, zero and carry
		or		ah, dh	; OR In our new flags
		xor	edx, edx
		mov	dl, [esi]
		inc	esi
		jmp	dword [m6502zpregular+edx*4]

;
; CMP
;

times ($$-$) & 3 nop	; pad with NOPs to 4-byte boundary

RegInstdd:
		sub	dword [cyclesRemaining], byte 4
		jc	near noMoreExec	; Can't execute anymore!
		add	dword [dwElapsedTicks], byte 4
		mov	dx, [esi]	; Get our address
		add	esi, 2	; Increment past instruction
		add	dx, bx	 ; Add X
		mov	edi, [_m6502zpMemRead]	; Point to the read array

checkLoop78:
		cmp	[edi], word 0ffffh ; End of the list?
		je		short memoryRead78
		cmp	dx, [edi]	; Are we smaller?
		jb		short nextAddr78		; Yes, go to the next address
		cmp	dx, [edi+4]	; Are we bigger?
		jbe	short callRoutine78

nextAddr78:
		add	edi, 10h		; Next structure!
		jmp	short checkLoop78

callRoutine78:
		call	ReadMemoryByte	; Standard read routine
		mov	bl, [_m6502zpx]	; Get X back
		mov	cl, [_m6502zpy]	; Get Y back
		mov	dl, al	; Get our value
		mov	ax, [_m6502zpaf]	; Get our flags & stuff back
		jmp	short readExit78

memoryRead78:
		mov	dl, [ebp + edx]	; Get our data

readExit78:
		mov	dh, ah	; Get our flags
		cmp	al, dl	; Compare!
		cmc			; Compliment carry flag
		lahf
		and	ah, 0c1h	; Sign, zero, and carry
		and	dh, 03eh	; Everything but sign, zero and carry
		or		ah, dh	; OR In our new flags
		xor	edx, edx
		mov	dl, [esi]
		inc	esi
		jmp	dword [m6502zpregular+edx*4]

;
; DEC
;

times ($$-$) & 3 nop	; pad with NOPs to 4-byte boundary

RegInstde:
		sub	dword [cyclesRemaining], byte 7
		jc	near noMoreExec	; Can't execute anymore!
		add	dword [dwElapsedTicks], byte 7
		mov	dx, [esi]	; Get our address
		add	esi, 2	; Increment past instruction
		add	dx, bx	 ; Add X
		push	edx	; Save this for later
		mov	edi, [_m6502zpMemRead]	; Point to the read array

checkLoop79:
		cmp	[edi], word 0ffffh ; End of the list?
		je		short memoryRead79
		cmp	dx, [edi]	; Are we smaller?
		jb		short nextAddr79		; Yes, go to the next address
		cmp	dx, [edi+4]	; Are we bigger?
		jbe	short callRoutine79

nextAddr79:
		add	edi, 10h		; Next structure!
		jmp	short checkLoop79

callRoutine79:
		call	ReadMemoryByte	; Standard read routine
		mov	bl, [_m6502zpx]	; Get X back
		mov	cl, [_m6502zpy]	; Get Y back
		mov	dl, al	; Get our value
		mov	ax, [_m6502zpaf]	; Get our flags & stuff back
		jmp	short readExit79

memoryRead79:
		mov	dl, [ebp + edx]	; Get our data

readExit79:
		mov	bh, dl	; Save the data we just got
		mov	ch, ah	; Save flags
		and	ch, 03fh	; No sign or zero flags
		dec	bh		; Decrement!
		lahf
		and	ah, 0c0h	; Only sign &  zero flags
		or	ah, ch	; Merge the two flags together
		pop	edx	; Restore our address
		mov	edi, [_m6502zpMemWrite]	; Point to the write array

checkLoop80:
		cmp	[edi], word 0ffffh ; End of our list?
		je	near memoryWrite80	; Yes - go write it!
		cmp	dx, [edi]	; Are we smaller?
		jb	nextAddr80	; Yes... go to the next addr
		cmp	dx, [edi+4]	; Are we bigger?
		jbe	callRoutine80	; If not, go call it!

nextAddr80:
		add	edi, 10h		; Next structure, please
		jmp	short checkLoop80

callRoutine80:
		mov	[_m6502zpx], bl	; Save X
		mov	[_m6502zpy], cl	; Save Y
		mov	[_m6502zpaf], ax	; Save Accumulator & flags
		sub	esi, ebp	; Our program counter
		mov	[_m6502zppc], si	; Save our program counter
		push	edi	; Pointer to MemoryWriteByte structure
		mov	bl, bh	; Put a copy here
		push	ebx	; The byte value
		and	edx, 0ffffh	; Only lower 16 bits
		push	edx	; The address
		call	dword [edi + 8] ; Go call our handler
		add	esp, 12	; Get rid of our stack
		xor	ebx, ebx	; Zero this
		xor	ecx, ecx	; This too!
		mov	bl, [_m6502zpx]	; Get X back
		mov	cl, [_m6502zpy]	; Get Y back
		xor	esi, esi	; Zero it!
		mov	si, [_m6502zppc]	; Get our program counter back
		mov	ebp, [_m6502zpBase] ; Base pointer comes back
		add	esi, ebp	; Rebase it properly
		mov	ax, [_m6502zpaf]	; Get our flags & stuff back
		jmp	short writeMacroExit80

memoryWrite80:
		mov	[ebp + edx], bh ; Store the byte

writeMacroExit80:
		xor	bh, bh	; Zero this so we don't totally screw things up
		xor	ch, ch	; Zero this as well
		xor	edx, edx
		mov	dl, [esi]
		inc	esi
		jmp	dword [m6502zpregular+edx*4]

;
; CPX
;

times ($$-$) & 3 nop	; pad with NOPs to 4-byte boundary

RegInste0:
		sub	dword [cyclesRemaining], byte 2
		jc	near noMoreExec	; Can't execute anymore!
		add	dword [dwElapsedTicks], byte 2
		mov	dl, [esi]	; Get our next byte
		inc	esi		; Increment to our next byte
		mov	dh, ah	; Get our flags
		cmp	bl, dl	; Compare with X!
		cmc			; Compliment carry flag
		lahf
		and	ah, 0c1h	; Sign, zero, and carry
		and	dh, 03eh	; Everything but sign, zero and carry
		or		ah, dh	; OR In our new flags
		xor	edx, edx
		mov	dl, [esi]
		inc	esi
		jmp	dword [m6502zpregular+edx*4]

;
; SBC
;

times ($$-$) & 3 nop	; pad with NOPs to 4-byte boundary

RegInste1:
		sub	dword [cyclesRemaining], byte 6
		jc	near noMoreExec	; Can't execute anymore!
		add	dword [dwElapsedTicks], byte 6
		mov	dl, [esi]		; Get the next instruction
		inc	esi		; Advance PC!
		add	dl, bl	; Add in X
		mov	dx, [ebp+edx]	; Get our 16 bit address
		mov	edi, [_m6502zpMemRead]	; Point to the read array

checkLoop81:
		cmp	[edi], word 0ffffh ; End of the list?
		je		short memoryRead81
		cmp	dx, [edi]	; Are we smaller?
		jb		short nextAddr81		; Yes, go to the next address
		cmp	dx, [edi+4]	; Are we bigger?
		jbe	short callRoutine81

nextAddr81:
		add	edi, 10h		; Next structure!
		jmp	short checkLoop81

callRoutine81:
		call	ReadMemoryByte	; Standard read routine
		mov	bl, [_m6502zpx]	; Get X back
		mov	cl, [_m6502zpy]	; Get Y back
		mov	dl, al	; Get our value
		mov	ax, [_m6502zpaf]	; Get our flags & stuff back
		jmp	short readExit81

memoryRead81:
		mov	dl, [ebp + edx]	; Get our data

readExit81:
		sahf		; Restore our flags for the adc
		cmc
		sbb	al, dl	; Subtract our value
		cmc
		o16 pushf	; Push our flags (and overflow)
		and	ah, 02eh	; No carry, overflow, zero or sign
		pop	dx	; Restore our flags into DX
		shl	dh, 1	; Shift overflow into position
		and	dh, 10h	; Only the overflow
		and	dl, 0c1h	; Only carry, sign, and zero
		or		ah, dl	; OR In our new flags
		or		ah, dh	; OR In overflow
		xor	edx, edx
		mov	dl, [esi]
		inc	esi
		jmp	dword [m6502zpregular+edx*4]

;
; CPX
;

times ($$-$) & 3 nop	; pad with NOPs to 4-byte boundary

RegInste4:
		sub	dword [cyclesRemaining], byte 3
		jc	near noMoreExec	; Can't execute anymore!
		add	dword [dwElapsedTicks], byte 3
		mov	dl, [esi]		; Get the next instruction
		inc	esi		; Advance PC!
		mov	dl, [ebp+edx]	; Get our zero page data
		mov	dh, ah	; Get our flags
		cmp	bl, dl	; Compare with X!
		cmc			; Compliment carry flag
		lahf
		and	ah, 0c1h	; Sign, zero, and carry
		and	dh, 03eh	; Everything but sign, zero and carry
		or		ah, dh	; OR In our new flags
		xor	edx, edx
		mov	dl, [esi]
		inc	esi
		jmp	dword [m6502zpregular+edx*4]

;
; SBC
;

times ($$-$) & 3 nop	; pad with NOPs to 4-byte boundary

RegInste5:
		sub	dword [cyclesRemaining], byte 3
		jc	near noMoreExec	; Can't execute anymore!
		add	dword [dwElapsedTicks], byte 3
		mov	dl, [esi]		; Get the next instruction
		inc	esi		; Advance PC!
		mov	dl, [ebp+edx]	; Get our zero page data
		sahf		; Restore our flags for the adc
		cmc
		sbb	al, dl	; Subtract our value
		cmc
		o16 pushf	; Push our flags (and overflow)
		and	ah, 02eh	; No carry, overflow, zero or sign
		pop	dx	; Restore our flags into DX
		shl	dh, 1	; Shift overflow into position
		and	dh, 10h	; Only the overflow
		and	dl, 0c1h	; Only carry, sign, and zero
		or		ah, dl	; OR In our new flags
		or		ah, dh	; OR In overflow
		xor	edx, edx
		mov	dl, [esi]
		inc	esi
		jmp	dword [m6502zpregular+edx*4]

;
; INC
;

times ($$-$) & 3 nop	; pad with NOPs to 4-byte boundary

RegInste6:
		sub	dword [cyclesRemaining], byte 5
		jc	near noMoreExec	; Can't execute anymore!
		add	dword [dwElapsedTicks], byte 5
		mov	dl, [esi]		; Get the next instruction
		inc	esi		; Advance PC!
		mov	ch, ah	; Save flags
		and	ch, 03fh	; No sign or zero flags
		inc	byte [edx+ebp]	; Increment our zero page stuff
		lahf
		and	ah, 0c0h	; Only sign &  zero flags
		or	ah, ch	; Merge the two flags together
		xor	bh, bh	; Zero this so we don't totally screw things up
		xor	ch, ch	; Zero this as well
		xor	edx, edx
		mov	dl, [esi]
		inc	esi
		jmp	dword [m6502zpregular+edx*4]

;
; INX
;

times ($$-$) & 3 nop	; pad with NOPs to 4-byte boundary

RegInste8:
		sub	dword [cyclesRemaining], byte 2
		jc	near noMoreExec	; Can't execute anymore!
		add	dword [dwElapsedTicks], byte 2
		mov	dl, ah	; Save flags
		inc	bl	; Increment
		lahf		; Restore flags
		and	dl, 03fh	; Original value
		and	ah, 0c0h	; Only zero and sign
		or	ah, dl		; New flags with the old!
		xor	edx, edx
		mov	dl, [esi]
		inc	esi
		jmp	dword [m6502zpregular+edx*4]

;
; SBC
;

times ($$-$) & 3 nop	; pad with NOPs to 4-byte boundary

RegInste9:
		sub	dword [cyclesRemaining], byte 2
		jc	near noMoreExec	; Can't execute anymore!
		add	dword [dwElapsedTicks], byte 2
		mov	dl, [esi]	; Get our next byte
		inc	esi		; Increment to our next byte
		sahf		; Restore our flags for the adc
		cmc
		sbb	al, dl	; Subtract our value
		cmc
		o16 pushf	; Push our flags (and overflow)
		and	ah, 02eh	; No carry, overflow, zero or sign
		pop	dx	; Restore our flags into DX
		shl	dh, 1	; Shift overflow into position
		and	dh, 10h	; Only the overflow
		and	dl, 0c1h	; Only carry, sign, and zero
		or		ah, dl	; OR In our new flags
		or		ah, dh	; OR In overflow
		xor	edx, edx
		mov	dl, [esi]
		inc	esi
		jmp	dword [m6502zpregular+edx*4]

;
; NOP
;

times ($$-$) & 3 nop	; pad with NOPs to 4-byte boundary

RegInstea:
		sub	dword [cyclesRemaining], byte 2
		jc	near noMoreExec	; Can't execute anymore!
		add	dword [dwElapsedTicks], byte 2
		xor	edx, edx
		mov	dl, [esi]
		inc	esi
		jmp	dword [m6502zpregular+edx*4]

;
; CPX
;

times ($$-$) & 3 nop	; pad with NOPs to 4-byte boundary

RegInstec:
		sub	dword [cyclesRemaining], byte 4
		jc	near noMoreExec	; Can't execute anymore!
		add	dword [dwElapsedTicks], byte 4
		mov	dx, [esi]	; Get our address
		add	esi, 2	; Increment past instruction
		mov	edi, [_m6502zpMemRead]	; Point to the read array

checkLoop82:
		cmp	[edi], word 0ffffh ; End of the list?
		je		short memoryRead82
		cmp	dx, [edi]	; Are we smaller?
		jb		short nextAddr82		; Yes, go to the next address
		cmp	dx, [edi+4]	; Are we bigger?
		jbe	short callRoutine82

nextAddr82:
		add	edi, 10h		; Next structure!
		jmp	short checkLoop82

callRoutine82:
		call	ReadMemoryByte	; Standard read routine
		mov	bl, [_m6502zpx]	; Get X back
		mov	cl, [_m6502zpy]	; Get Y back
		mov	dl, al	; Get our value
		mov	ax, [_m6502zpaf]	; Get our flags & stuff back
		jmp	short readExit82

memoryRead82:
		mov	dl, [ebp + edx]	; Get our data

readExit82:
		mov	dh, ah	; Get our flags
		cmp	bl, dl	; Compare with X!
		cmc			; Compliment carry flag
		lahf
		and	ah, 0c1h	; Sign, zero, and carry
		and	dh, 03eh	; Everything but sign, zero and carry
		or		ah, dh	; OR In our new flags
		xor	edx, edx
		mov	dl, [esi]
		inc	esi
		jmp	dword [m6502zpregular+edx*4]

;
; SBC
;

times ($$-$) & 3 nop	; pad with NOPs to 4-byte boundary

RegInsted:
		sub	dword [cyclesRemaining], byte 4
		jc	near noMoreExec	; Can't execute anymore!
		add	dword [dwElapsedTicks], byte 4
		mov	dx, [esi]	; Get our address
		add	esi, 2	; Increment past instruction
		mov	edi, [_m6502zpMemRead]	; Point to the read array

checkLoop83:
		cmp	[edi], word 0ffffh ; End of the list?
		je		short memoryRead83
		cmp	dx, [edi]	; Are we smaller?
		jb		short nextAddr83		; Yes, go to the next address
		cmp	dx, [edi+4]	; Are we bigger?
		jbe	short callRoutine83

nextAddr83:
		add	edi, 10h		; Next structure!
		jmp	short checkLoop83

callRoutine83:
		call	ReadMemoryByte	; Standard read routine
		mov	bl, [_m6502zpx]	; Get X back
		mov	cl, [_m6502zpy]	; Get Y back
		mov	dl, al	; Get our value
		mov	ax, [_m6502zpaf]	; Get our flags & stuff back
		jmp	short readExit83

memoryRead83:
		mov	dl, [ebp + edx]	; Get our data

readExit83:
		sahf		; Restore our flags for the adc
		cmc
		sbb	al, dl	; Subtract our value
		cmc
		o16 pushf	; Push our flags (and overflow)
		and	ah, 02eh	; No carry, overflow, zero or sign
		pop	dx	; Restore our flags into DX
		shl	dh, 1	; Shift overflow into position
		and	dh, 10h	; Only the overflow
		and	dl, 0c1h	; Only carry, sign, and zero
		or		ah, dl	; OR In our new flags
		or		ah, dh	; OR In overflow
		xor	edx, edx
		mov	dl, [esi]
		inc	esi
		jmp	dword [m6502zpregular+edx*4]

;
; INC
;

times ($$-$) & 3 nop	; pad with NOPs to 4-byte boundary

RegInstee:
		sub	dword [cyclesRemaining], byte 6
		jc	near noMoreExec	; Can't execute anymore!
		add	dword [dwElapsedTicks], byte 6
		mov	dx, [esi]	; Get our address
		add	esi, 2	; Increment past instruction
		push	edx	; Save this for later
		mov	edi, [_m6502zpMemRead]	; Point to the read array

checkLoop84:
		cmp	[edi], word 0ffffh ; End of the list?
		je		short memoryRead84
		cmp	dx, [edi]	; Are we smaller?
		jb		short nextAddr84		; Yes, go to the next address
		cmp	dx, [edi+4]	; Are we bigger?
		jbe	short callRoutine84

nextAddr84:
		add	edi, 10h		; Next structure!
		jmp	short checkLoop84

callRoutine84:
		call	ReadMemoryByte	; Standard read routine
		mov	bl, [_m6502zpx]	; Get X back
		mov	cl, [_m6502zpy]	; Get Y back
		mov	dl, al	; Get our value
		mov	ax, [_m6502zpaf]	; Get our flags & stuff back
		jmp	short readExit84

memoryRead84:
		mov	dl, [ebp + edx]	; Get our data

readExit84:
		mov	bh, dl	; Save the data we just got
		mov	ch, ah	; Save flags
		and	ch, 03fh	; No sign or zero flags
		inc	bh		; Increment!
		lahf
		and	ah, 0c0h	; Only sign &  zero flags
		or	ah, ch	; Merge the two flags together
		pop	edx	; Restore our address
		mov	edi, [_m6502zpMemWrite]	; Point to the write array

checkLoop85:
		cmp	[edi], word 0ffffh ; End of our list?
		je	near memoryWrite85	; Yes - go write it!
		cmp	dx, [edi]	; Are we smaller?
		jb	nextAddr85	; Yes... go to the next addr
		cmp	dx, [edi+4]	; Are we bigger?
		jbe	callRoutine85	; If not, go call it!

nextAddr85:
		add	edi, 10h		; Next structure, please
		jmp	short checkLoop85

callRoutine85:
		mov	[_m6502zpx], bl	; Save X
		mov	[_m6502zpy], cl	; Save Y
		mov	[_m6502zpaf], ax	; Save Accumulator & flags
		sub	esi, ebp	; Our program counter
		mov	[_m6502zppc], si	; Save our program counter
		push	edi	; Pointer to MemoryWriteByte structure
		mov	bl, bh	; Put a copy here
		push	ebx	; The byte value
		and	edx, 0ffffh	; Only lower 16 bits
		push	edx	; The address
		call	dword [edi + 8] ; Go call our handler
		add	esp, 12	; Get rid of our stack
		xor	ebx, ebx	; Zero this
		xor	ecx, ecx	; This too!
		mov	bl, [_m6502zpx]	; Get X back
		mov	cl, [_m6502zpy]	; Get Y back
		xor	esi, esi	; Zero it!
		mov	si, [_m6502zppc]	; Get our program counter back
		mov	ebp, [_m6502zpBase] ; Base pointer comes back
		add	esi, ebp	; Rebase it properly
		mov	ax, [_m6502zpaf]	; Get our flags & stuff back
		jmp	short writeMacroExit85

memoryWrite85:
		mov	[ebp + edx], bh ; Store the byte

writeMacroExit85:
		xor	bh, bh	; Zero this so we don't totally screw things up
		xor	ch, ch	; Zero this as well
		xor	edx, edx
		mov	dl, [esi]
		inc	esi
		jmp	dword [m6502zpregular+edx*4]

;
; Branch
;

times ($$-$) & 3 nop	; pad with NOPs to 4-byte boundary

RegInstf0:
		sub	dword [cyclesRemaining], byte 3
		jc	near noMoreExec	; Can't execute anymore!
		add	dword [dwElapsedTicks], byte 3
		test	ah, 40h	; Non-zero set?
		jnz	short takeJumpf0 ; Do it!
		inc	esi	; Skip past the offset
		xor	edx, edx
		mov	dl, [esi]
		inc	esi
		jmp	dword [m6502zpregular+edx*4]

takeJumpf0:
		mov	dl, [esi]		; Get the next instruction
		inc	esi		; Advance PC!
		movsx	dx, dl
		sub	esi, ebp
		add	si, dx
		add	esi, ebp
		xor	edx, edx
		mov	dl, [esi]
		inc	esi
		jmp	dword [m6502zpregular+edx*4]

;
; SBC
;

times ($$-$) & 3 nop	; pad with NOPs to 4-byte boundary

RegInstf1:
		sub	dword [cyclesRemaining], byte 5
		jc	near noMoreExec	; Can't execute anymore!
		add	dword [dwElapsedTicks], byte 5
		mov	dl, [esi]		; Get the next instruction
		inc	esi		; Advance PC!
		mov	dx, [ebp+edx]	; Get our 16 bit address
		add	dx, cx	; Add in Y
		mov	edi, [_m6502zpMemRead]	; Point to the read array

checkLoop87:
		cmp	[edi], word 0ffffh ; End of the list?
		je		short memoryRead87
		cmp	dx, [edi]	; Are we smaller?
		jb		short nextAddr87		; Yes, go to the next address
		cmp	dx, [edi+4]	; Are we bigger?
		jbe	short callRoutine87

nextAddr87:
		add	edi, 10h		; Next structure!
		jmp	short checkLoop87

callRoutine87:
		call	ReadMemoryByte	; Standard read routine
		mov	bl, [_m6502zpx]	; Get X back
		mov	cl, [_m6502zpy]	; Get Y back
		mov	dl, al	; Get our value
		mov	ax, [_m6502zpaf]	; Get our flags & stuff back
		jmp	short readExit87

memoryRead87:
		mov	dl, [ebp + edx]	; Get our data

readExit87:
		sahf		; Restore our flags for the adc
		cmc
		sbb	al, dl	; Subtract our value
		cmc
		o16 pushf	; Push our flags (and overflow)
		and	ah, 02eh	; No carry, overflow, zero or sign
		pop	dx	; Restore our flags into DX
		shl	dh, 1	; Shift overflow into position
		and	dh, 10h	; Only the overflow
		and	dl, 0c1h	; Only carry, sign, and zero
		or		ah, dl	; OR In our new flags
		or		ah, dh	; OR In overflow
		xor	edx, edx
		mov	dl, [esi]
		inc	esi
		jmp	dword [m6502zpregular+edx*4]

;
; SBC
;

times ($$-$) & 3 nop	; pad with NOPs to 4-byte boundary

RegInstf5:
		sub	dword [cyclesRemaining], byte 4
		jc	near noMoreExec	; Can't execute anymore!
		add	dword [dwElapsedTicks], byte 4
		mov	dl, [esi]		; Get the next instruction
		inc	esi		; Advance PC!
		add	dl, bl	; Add X
		mov	dl, [ebp+edx]	; Get our zero page data
		sahf		; Restore our flags for the adc
		cmc
		sbb	al, dl	; Subtract our value
		cmc
		o16 pushf	; Push our flags (and overflow)
		and	ah, 02eh	; No carry, overflow, zero or sign
		pop	dx	; Restore our flags into DX
		shl	dh, 1	; Shift overflow into position
		and	dh, 10h	; Only the overflow
		and	dl, 0c1h	; Only carry, sign, and zero
		or		ah, dl	; OR In our new flags
		or		ah, dh	; OR In overflow
		xor	edx, edx
		mov	dl, [esi]
		inc	esi
		jmp	dword [m6502zpregular+edx*4]

;
; INC
;

times ($$-$) & 3 nop	; pad with NOPs to 4-byte boundary

RegInstf6:
		sub	dword [cyclesRemaining], byte 6
		jc	near noMoreExec	; Can't execute anymore!
		add	dword [dwElapsedTicks], byte 6
		mov	dl, [esi]		; Get the next instruction
		inc	esi		; Advance PC!
		add	dl, bl	; Add X
		mov	ch, ah	; Save flags
		and	ch, 03fh	; No sign or zero flags
		inc	byte [edx+ebp]	; Increment our zero page stuff
		lahf
		and	ah, 0c0h	; Only sign &  zero flags
		or	ah, ch	; Merge the two flags together
		xor	bh, bh	; Zero this so we don't totally screw things up
		xor	ch, ch	; Zero this as well
		xor	edx, edx
		mov	dl, [esi]
		inc	esi
		jmp	dword [m6502zpregular+edx*4]

;
; SED
;

times ($$-$) & 3 nop	; pad with NOPs to 4-byte boundary

RegInstf8:
		sub	dword [cyclesRemaining], byte 2
		jc	near noMoreExec	; Can't execute anymore!
		add	dword [dwElapsedTicks], byte 2
		or		[_altFlags], byte 08h	; Decimal mode
		xor	edx, edx
		mov	dl, [esi]
		inc	esi
		jmp	dword [m6502zpregular+edx*4]

;
; SBC
;

times ($$-$) & 3 nop	; pad with NOPs to 4-byte boundary

RegInstf9:
		sub	dword [cyclesRemaining], byte 4
		jc	near noMoreExec	; Can't execute anymore!
		add	dword [dwElapsedTicks], byte 4
		mov	dx, [esi]	; Get our address
		add	esi, 2	; Increment past instruction
		add	dx, cx	 ; Add Y
		mov	edi, [_m6502zpMemRead]	; Point to the read array

checkLoop88:
		cmp	[edi], word 0ffffh ; End of the list?
		je		short memoryRead88
		cmp	dx, [edi]	; Are we smaller?
		jb		short nextAddr88		; Yes, go to the next address
		cmp	dx, [edi+4]	; Are we bigger?
		jbe	short callRoutine88

nextAddr88:
		add	edi, 10h		; Next structure!
		jmp	short checkLoop88

callRoutine88:
		call	ReadMemoryByte	; Standard read routine
		mov	bl, [_m6502zpx]	; Get X back
		mov	cl, [_m6502zpy]	; Get Y back
		mov	dl, al	; Get our value
		mov	ax, [_m6502zpaf]	; Get our flags & stuff back
		jmp	short readExit88

memoryRead88:
		mov	dl, [ebp + edx]	; Get our data

readExit88:
		sahf		; Restore our flags for the adc
		cmc
		sbb	al, dl	; Subtract our value
		cmc
		o16 pushf	; Push our flags (and overflow)
		and	ah, 02eh	; No carry, overflow, zero or sign
		pop	dx	; Restore our flags into DX
		shl	dh, 1	; Shift overflow into position
		and	dh, 10h	; Only the overflow
		and	dl, 0c1h	; Only carry, sign, and zero
		or		ah, dl	; OR In our new flags
		or		ah, dh	; OR In overflow
		xor	edx, edx
		mov	dl, [esi]
		inc	esi
		jmp	dword [m6502zpregular+edx*4]

;
; SBC
;

times ($$-$) & 3 nop	; pad with NOPs to 4-byte boundary

RegInstfd:
		sub	dword [cyclesRemaining], byte 4
		jc	near noMoreExec	; Can't execute anymore!
		add	dword [dwElapsedTicks], byte 4
		mov	dx, [esi]	; Get our address
		add	esi, 2	; Increment past instruction
		add	dx, bx	 ; Add X
		mov	edi, [_m6502zpMemRead]	; Point to the read array

checkLoop89:
		cmp	[edi], word 0ffffh ; End of the list?
		je		short memoryRead89
		cmp	dx, [edi]	; Are we smaller?
		jb		short nextAddr89		; Yes, go to the next address
		cmp	dx, [edi+4]	; Are we bigger?
		jbe	short callRoutine89

nextAddr89:
		add	edi, 10h		; Next structure!
		jmp	short checkLoop89

callRoutine89:
		call	ReadMemoryByte	; Standard read routine
		mov	bl, [_m6502zpx]	; Get X back
		mov	cl, [_m6502zpy]	; Get Y back
		mov	dl, al	; Get our value
		mov	ax, [_m6502zpaf]	; Get our flags & stuff back
		jmp	short readExit89

memoryRead89:
		mov	dl, [ebp + edx]	; Get our data

readExit89:
		sahf		; Restore our flags for the adc
		cmc
		sbb	al, dl	; Subtract our value
		cmc
		o16 pushf	; Push our flags (and overflow)
		and	ah, 02eh	; No carry, overflow, zero or sign
		pop	dx	; Restore our flags into DX
		shl	dh, 1	; Shift overflow into position
		and	dh, 10h	; Only the overflow
		and	dl, 0c1h	; Only carry, sign, and zero
		or		ah, dl	; OR In our new flags
		or		ah, dh	; OR In overflow
		xor	edx, edx
		mov	dl, [esi]
		inc	esi
		jmp	dword [m6502zpregular+edx*4]

;
; INC
;

times ($$-$) & 3 nop	; pad with NOPs to 4-byte boundary

RegInstfe:
		sub	dword [cyclesRemaining], byte 7
		jc	near noMoreExec	; Can't execute anymore!
		add	dword [dwElapsedTicks], byte 7
		mov	dx, [esi]	; Get our address
		add	esi, 2	; Increment past instruction
		add	dx, bx	 ; Add X
		push	edx	; Save this for later
		mov	edi, [_m6502zpMemRead]	; Point to the read array

checkLoop90:
		cmp	[edi], word 0ffffh ; End of the list?
		je		short memoryRead90
		cmp	dx, [edi]	; Are we smaller?
		jb		short nextAddr90		; Yes, go to the next address
		cmp	dx, [edi+4]	; Are we bigger?
		jbe	short callRoutine90

nextAddr90:
		add	edi, 10h		; Next structure!
		jmp	short checkLoop90

callRoutine90:
		call	ReadMemoryByte	; Standard read routine
		mov	bl, [_m6502zpx]	; Get X back
		mov	cl, [_m6502zpy]	; Get Y back
		mov	dl, al	; Get our value
		mov	ax, [_m6502zpaf]	; Get our flags & stuff back
		jmp	short readExit90

memoryRead90:
		mov	dl, [ebp + edx]	; Get our data

readExit90:
		mov	bh, dl	; Save the data we just got
		mov	ch, ah	; Save flags
		and	ch, 03fh	; No sign or zero flags
		inc	bh		; Increment!
		lahf
		and	ah, 0c0h	; Only sign &  zero flags
		or	ah, ch	; Merge the two flags together
		pop	edx	; Restore our address
		mov	edi, [_m6502zpMemWrite]	; Point to the write array

checkLoop91:
		cmp	[edi], word 0ffffh ; End of our list?
		je	near memoryWrite91	; Yes - go write it!
		cmp	dx, [edi]	; Are we smaller?
		jb	nextAddr91	; Yes... go to the next addr
		cmp	dx, [edi+4]	; Are we bigger?
		jbe	callRoutine91	; If not, go call it!

nextAddr91:
		add	edi, 10h		; Next structure, please
		jmp	short checkLoop91

callRoutine91:
		mov	[_m6502zpx], bl	; Save X
		mov	[_m6502zpy], cl	; Save Y
		mov	[_m6502zpaf], ax	; Save Accumulator & flags
		sub	esi, ebp	; Our program counter
		mov	[_m6502zppc], si	; Save our program counter
		push	edi	; Pointer to MemoryWriteByte structure
		mov	bl, bh	; Put a copy here
		push	ebx	; The byte value
		and	edx, 0ffffh	; Only lower 16 bits
		push	edx	; The address
		call	dword [edi + 8] ; Go call our handler
		add	esp, 12	; Get rid of our stack
		xor	ebx, ebx	; Zero this
		xor	ecx, ecx	; This too!
		mov	bl, [_m6502zpx]	; Get X back
		mov	cl, [_m6502zpy]	; Get Y back
		xor	esi, esi	; Zero it!
		mov	si, [_m6502zppc]	; Get our program counter back
		mov	ebp, [_m6502zpBase] ; Base pointer comes back
		add	esi, ebp	; Rebase it properly
		mov	ax, [_m6502zpaf]	; Get our flags & stuff back
		jmp	short writeMacroExit91

memoryWrite91:
		mov	[ebp + edx], bh ; Store the byte

writeMacroExit91:
		xor	bh, bh	; Zero this so we don't totally screw things up
		xor	ch, ch	; Zero this as well
		xor	edx, edx
		mov	dl, [esi]
		inc	esi
		jmp	dword [m6502zpregular+edx*4]

		global	_m6502zpGetContext
		global	m6502zpGetContext_
		global	m6502zpGetContext

times ($$-$) & 3 nop	; pad with NOPs to 4-byte boundary

m6502zpGetContext_:
_m6502zpGetContext:
		mov	eax, [esp+4]	; Get our context address
		push	esi		; Save registers we use
		push	edi
		push	ecx
		mov     ecx, _m6502zpcontextEnd - _m6502zpcontextBegin
		mov	esi, _m6502zpcontextBegin
		mov	edi, eax	; Source address in ESI
		rep	movsb		; Move it as fast as we can!
		pop	ecx
		pop	edi
		pop	esi
		ret			; No return code
		global	_m6502zpSetContext
		global	m6502zpSetContext_
		global	m6502zpSetContext

times ($$-$) & 3 nop	; pad with NOPs to 4-byte boundary

m6502zpSetContext_:
_m6502zpSetContext:
		mov	eax, [esp+4]	; Get our context address
		push	esi		; Save registers we use
		push	edi
		push	ecx
		mov     ecx, _m6502zpcontextEnd - _m6502zpcontextBegin
		mov	edi, _m6502zpcontextBegin
		mov	esi, eax	; Source address in ESI
		rep	movsb		; Move it as fast as we can!
		pop	ecx
		pop	edi
		pop	esi
		ret			; No return code
		global	_m6502zpGetContextSize
		global	m6502zpGetContextSize_
		global	m6502zpGetContextSize

times ($$-$) & 3 nop	; pad with NOPs to 4-byte boundary

m6502zpGetContextSize_:
_m6502zpGetContextSize:
		mov     eax, _m6502zpcontextEnd - _m6502zpcontextBegin
		ret
		global	_m6502zpinit
		global	m6502zpinit_

times ($$-$) & 3 nop	; pad with NOPs to 4-byte boundary

m6502zpinit_:
_m6502zpinit:
		ret
		global	_m6502zpGetElapsedTicks
		global	m6502zpGetElapsedTicks_

times ($$-$) & 3 nop	; pad with NOPs to 4-byte boundary

m6502zpGetElapsedTicks_:
_m6502zpGetElapsedTicks:
		mov	eax, [esp+4]	; Get our context address
		or	eax, eax	; Should we clear it?
		jz	getTicks
		xor	eax, eax
		xchg	eax, [dwElapsedTicks]
		ret
getTicks:
		mov	eax, [dwElapsedTicks]
		ret
		global	_m6502zpReleaseTimeslice
		global	m6502zpReleaseTimeslice_

times ($$-$) & 3 nop	; pad with NOPs to 4-byte boundary

m6502zpReleaseTimeslice_:
_m6502zpReleaseTimeslice:
		mov	[cyclesRemaining], dword 1
		ret
		global	_m6502zpreset
		global	m6502zpreset_
		global	m6502zpreset

times ($$-$) & 3 nop	; pad with NOPs to 4-byte boundary

m6502zpreset_:
_m6502zpreset:
		push	ebp	; Save our important register
		xor	eax, eax
		mov	ebp, [_m6502zpBase]
		mov	[_m6502zpx], al
		mov	[_m6502zpy], al
		mov	[_irqPending], al
		mov	[_m6502zps], byte 0ffh
		mov	[_m6502zpaf], word 2200h
		mov	ax, [ebp + 0fffch] ; Get reset address
		mov	[_m6502zppc], ax
		pop	ebp
		ret

		global	_m6502zpint
		global	m6502zpint_

times ($$-$) & 3 nop	; pad with NOPs to 4-byte boundary

m6502zpint_:
_m6502zpint:
		test	[_m6502zpaf + 1], byte 04h	; Are interrupts disabled?
		jnz	intNotTaken	; Nope! Make it pending!
		mov	[_irqPending], byte 00h	; No longer pending
		push    edi
		push    ebx
		push    ax
		mov     ebx, 0100h              ; Point to stack page
		mov	  edi, [_m6502zpBase]	; Get pointer to game image
		mov     bl, [_m6502zps]           ; Get our S reg pointer
		mov     ax, [_m6502zppc]          ; Get our PC
		mov     [edi + ebx], ah         ; Store it!
		dec     bl
		mov     [edi + ebx], al         ; Store it!
		dec     bl
		mov     al, byte [_m6502zpaf+1] ; Get our flags
		mov     [edi + ebx], al         ; Store flags
		dec     bl
		mov     ax, [edi+0fffeh]        ; Get our start vector!
		mov     [_m6502zps], bl             ; Store S reg pointer
		and	  [_m6502zpaf + 1], byte 0efh		; Knock out source of interrupt bit
		or		  [_m6502zpaf + 1], byte 24h		; Turn on something
		mov     [_m6502zppc], ax            ; Store our new PC
		pop     ax                      ; Restore used registers
		pop     ebx
		pop     edi
		xor	eax, eax		; Indicate we've taken the interrupt
		mov	[_irqPending], al	; No more IRQ pending!
		ret
intNotTaken:
		mov	eax, 1		; Indicate we didn't take it
		mov	[_irqPending], al ; Indicate we have a pending IRQ
		ret
		global	_m6502zpnmi
		global	m6502zpnmi_

times ($$-$) & 3 nop	; pad with NOPs to 4-byte boundary

m6502zpnmi_:
_m6502zpnmi:
		push    edi
		push    ebx

		mov     ebx, 0100h              ; Point to stack page

		mov     edi, [_m6502zpBase]       ; Get pointer to game image
		mov     bl, [_m6502zps]           ; Get our S reg pointer
		mov     ax, [_m6502zppc]          ; Get our PC

		mov     [edi + ebx], ah         ; Store it!
		dec     bl
		mov     [edi + ebx], al         ; Store it!
		dec     bl

		mov     al, byte [_m6502zpaf+1] ; Get our flags


		mov     [edi + ebx], al         ; Store flags
		dec     bl
		mov     ax, [edi+0fffah]        ; Get our start vector!

		mov     [_m6502zps], bl             ; Store S reg pointer
		and     [_m6502zpaf + 1], byte 0efh      ; Knock out source of interrupt bit
		or      [_m6502zpaf + 1], byte 24h       ; Turn on something
		mov     [_m6502zppc], ax            ; Store our new PC

		pop     ebx
		pop     edi

		xor	eax, eax		; Indicate that we took the NMI
		ret
		global	_m6502zpexec
		global	m6502zpexec_

times ($$-$) & 3 nop	; pad with NOPs to 4-byte boundary

m6502zpexec_:
_m6502zpexec:
		mov	eax, [esp+4]	; Get our execution cycle count
		push	ebx			; Save all registers we use
		push	ecx
		push	edx
		push	ebp
		push	esi
		push	edi

		mov	dword [cyclesRemaining], eax	; Store # of instructions to
		cld				; Go forward!

		xor	eax, eax		; Zero EAX 'cause we use it!
		xor	ebx, ebx		; Zero EBX, too
		xor	ecx, ecx		; Zero ECX
		xor	esi, esi		; Zero our source address

		mov	bl, [_m6502zpx]	; Get X
		mov	cl, [_m6502zpy]	; Get Y
		mov	ax, [_m6502zpaf]	; Get our flags and accumulator
		mov	si, [_m6502zppc]	; Get our program counter
		mov	ebp, [_m6502zpBase]	; Get our base address register
		add	esi, ebp		; Add in our base address
		xor	edx, edx		; And EDX
		xor	edi, edi		; Zero EDI as well
		xor	edx, edx
		mov	dl, ah
		mov	[_altFlags], dl
		and	[_altFlags], byte 3ch;
		mov	ah, [bit6502tox86+edx]
		xor	edx, edx
		mov	dl, [esi]
		inc	esi
		jmp	dword [m6502zpregular+edx*4]

invalidInsWord:
		dec	esi

; We get to invalidInsByte if it's a single byte invalid opcode

invalidInsByte:
		dec	esi			; Back up one instruction...
		mov	edx, esi		; Get our address in EAX
		sub	edx, ebp		; And subtract our base for
						; an invalid instruction
		jmp	short emulateEnd

noMoreExec:
		dec	esi
		mov	edx, 80000000h		; Indicate successful exec
emulateEnd:
		push	edx		; Save this for the return
		xor	edx, edx
		mov	dl, ah
		mov	ah, [bitx86to6502+edx]
		or		ah, [_altFlags]
		mov	[_m6502zpx], bl	; Store X
		mov	[_m6502zpy], cl	; Store Y
		mov	[_m6502zpaf], ax	; Store A & flags
		sub	esi, ebp	; Get our PC back
		mov	[_m6502zppc], si	; Store PC
		pop	edx		; Restore EDX for later

popReg:
		mov	eax, edx	; Get our result code
		pop	edi			; Restore registers
		pop	esi
		pop	ebp
		pop	edx
		pop	ecx
		pop	ebx

		ret

		end
