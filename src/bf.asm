section .text

global brainfuck

;; Purges e{a,b,d}x, returns 1 byte in eax.
read:
	push ecx
	mov eax, 3
	mov ebx, 0
	lea ecx, [esp-4]
	mov edx, 1
	int 80h
	mov eax, [esp-4]
	pop ecx
	ret



;; Takes eax, as argument, purges e{a,b,d}x.
write:
	push ecx
	mov [esp-4], eax
	mov eax, 4
	mov ebx, 1
	lea ecx, [esp-4]
	mov edx, 1
	int 80h
	pop ecx
	ret



brainfuck:
	; General initialization.
	; Saving e{s,d}i.
	; Size in esp+8/buf in esp+4/*ret in esp.
	push esi
	push edi
	; Size in esp+16/buf in esp+12/*ret in esp.
	; Reading/initializing arguments.
	mov eax, [esp+16]		; Pop size in SI.
	mov edi, [esp+12]		; Pop base(buf) in BX.
	xor ecx, ecx
	xor esi, esi
	; `enter 8, 0` ; allocating space for two local vars
	push ebp
	mov ebp, esp
	sub esp, 8
	mov [ebp-4], eax

; Table of contents:
	; [ebp+4]: size of buf
	; ecx: code base offset  (EIP)
	; edi: code base pointer (CS)
	;data: data base pointer (DS)
	; esi: data base offset
loop:
	; Check buf overflow.
	cmp ecx, [ebp-4]
	jnl done

	; Reading a byte from the buffer,
	xor eax, eax
	mov al, [edi+ecx]
	; switch (eax) {{{
	cmp eax, '+'
	jne ne1
	; CASE + Increment *(data+doffset)
	inc byte [data+esi]
	jmp loopdone	; Break!
ne1:cmp eax, '-'
	jne ne2
	; CASE - Decrement *(data+offset)
	dec byte [data+esi]
	jmp loopdone	; Break!
ne2:cmp eax, '<'
	jne ne3
	; CASE < If offset != 0
	test esi, esi
	jz loopdone
	; Decrement offset
	dec esi
	jmp loopdone	; Break!
ne3:cmp eax, '>'
	jne ne4
	; CASE > If offset < size
	cmp esi, [ebp-4]
	jnl loopdone
	inc esi
	jmp loopdone	; Break!
ne4:cmp eax, '.'
	jne ne5
	; CASE . write(eax)
	xor eax, eax
	mov al, byte [data+esi]
	call write
	jmp loopdone	; Break!
ne5:cmp eax, ',' ; Read
	jne ne6
	; CASE , [edi+esi] = read()
	call read
	mov byte [data+esi], al
	jmp loopdone	; Break!
ne6:cmp eax, '['
	jne ne7
	; CASE [ test if [edi+esi] = 0
	cmp byte [data+esi], 0
	jne bracketin
	;	Table of contents:
	;		o eax -- spinner
	;		o ebx -- nesting level counter
	mov eax, ecx
	xor ebx, ebx
	bracketloop:
		inc eax
		cmp eax, [ebp-4]
		jnl done				; Seeked further than the buffer. (unmatched bracket)
		cmp byte [edi+eax], ']'
		jne ne61
		test ebx, ebx			; Checking, if we have any nested brackets opened.
		jz bracketdone
		; ]    Nested closing bracket found.
		dec ebx
		jmp bracketloop
		ne61:cmp byte [edi+eax], '['
			; [
			jne bracketloop
			inc ebx
			jmp bracketloop
	bracketdone:
		; Set bf instruction pointer to the one we found.
		mov ecx, eax
		jmp loopdone
	bracketin:
		; Push brainfuck instruction pointer & continue.
		push ecx
		jmp loopdone	; Break!
ne7:cmp eax, ']'
	jne unknown
	cmp [ebp-8], esp
	mov eax, 0xff
	je done
	pop ecx
	jmp loop	; Break!
unknown:
	; To be implemented.
	; }}}
loopdone:
	inc ecx
	jmp loop
done:

	; `leave`
	mov esp, ebp
	pop ebp
	; Restoring e{s,d}i.
	pop edi
	pop esi
	ret



section .bss
	data:		resb 30000
	retval:		resd 1
