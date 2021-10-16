.model small
.stack 100h
.data

	a1 dw ?
	b1 dw ?
	c1 dw ?
	a3 dw ?
	t0 dw ?
	a4 dw ?
	b4 dw ?
	c4 dw ?
	t1 dw ?
	t2 dw ?
	t3 dw ?
	i5 dw ?
	j5 dw ?
	k5 dw ?
	l5 dw ?
	t4 dw ?
	t5 dw ?
	t6 dw ?
	t7 dw ?
	t8 dw ?
	t9 dw ?
	t10 dw ?
	t11 dw ?
	t12 dw ?

	address dw 0

.code

func_a proc
	pop address
	mov ax, 7
	mov a1, ax
	push address
	ret
func_a endp

foo proc
	pop address
	pop a3
	mov ax, a3
	add ax, 3
	mov t0, ax
	mov a3, ax
	push a3
	push address
	ret
foo endp

bar proc
	pop address
	pop b4
	pop a4
	mov ax, 4
	mov bx, a4
	imul bx
	mov t1, ax
	mov ax, 2
	mov bx, b4
	imul bx
	mov t2, ax
	mov ax, t1
	add ax, t2
	mov t3, ax
	mov c4, ax
	push c4
	push address
	ret
bar endp

main proc
	mov ax, @data
	mov ds ,ax

	mov ax, 5
	mov i5, ax
	mov ax, 6
	mov j5, ax
	push address
	call func_a
	pop address
	mov ax, a1
	call println
	push address
	push i5
	call foo
	pop t5
	pop address
	mov ax, t5
	mov k5, ax
	call println
	push address
	push i5
	push j5
	call bar
	pop t6
	pop address
	mov ax, t6
	mov l5, ax
	call println
	push address
	push i5
	push j5
	call bar
	pop t7
	pop address
	mov ax, 6
	mov bx, t7
	imul bx
	mov t8, ax
	add ax, 2
	mov t9, ax
	push address
	push i5
	call foo
	pop t10
	pop address
	mov ax, 3
	mov bx, t10
	imul bx
	mov t11, ax
	mov ax, t9
	sub ax, t11
	mov t12, ax
	mov j5, ax
	call println
	push 0


	mov ah, 4ch
	int 21h
main endp

println PROC 
 		 PUSH AX 
    		 PUSH BX 
                PUSH CX 
                PUSH DX  
                CMP AX,0 
                JGE BEGIN 
                PUSH AX 
                MOV DL,'-' 
                MOV AH,2 
                INT 21H 
                POP AX 
                NEG AX 
     		 BEGIN: 
    		XOR CX,CX 
    		MOV BX,10 
    		REPEAT: 
    		XOR DX,DX 
    		IDIV BX 
    		PUSH DX 
    		INC CX 
    		OR AX,AX 
    		JNE REPEAT 
    		MOV AH,2 
    		PRINT_LOOP: 
    		POP DX 
    		ADD DL,30H 
    		INT 21H 
    		LOOP PRINT_LOOP 
    		MOV AH,2
    		MOV DL,10
    		INT 21H
    		MOV DL,13
    		INT 21H
    		POP DX 
    		POP CX 
    		POP BX 
    		POP AX 
    		ret 
    		println ENDP 
end main
