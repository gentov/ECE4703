; nonparallel assembly code for N element floating point dot product
; A4 and B4 contain pointers to arrays of floats
; A6 contains an integer with N 
			.def 	_iirDFII_asm
			.ref  	_index
			.ref	_DEN2
			.ref     _NUM2
			.ref 	_filterOut
			.ref  	_i
			.ref  	_input2
			.ref _arrayIndex
			.ref _tempOmega
			;.ref 	_j 
_iirDFII_asm:
			MVKL	.S1 _index, A5
			MVKH 	.S1 _index, A5

			MVKL	.S1 _DEN2, A6
			MVKH 	.S1 _DEN2, A6

			MVKL	.S1 _NUM2, A7
			MVKH 	.S1 _NUM2, A7

			MVKL	.S1 _filterOut, A8
			MVKH 	.S1 _filterOut, A8

			MVKL	.S1 _i, A9
			MVKH 	.S1 _i, A9
			
			;MVKL	.S1 _j, A13
			;MVKH 	.S1 _j, A13

			MVKL	.S1 _input2, A10
			MVKH 	.S1 _input2, A10

			MVKL	.S1 _arrayIndex, A11
			MVKH 	.S1 _arrayIndex, A11

			MVKL	.S1 _tempOmega, A12
			MVKH 	.S1 _tempOmega, A12

			MV		.S1	A4, A1		; copy count to register A1 (input)
			ZERO	.L1	A9			; zero accumulator (index = 0)
			ZERO 	.L1	A16			; This will hold the value to put into tempOmega[index]
			ADD	 	.S1 A9, 1, A9
			INTSP 	.L1 A1, A0 ; a1 == input2 just so we're clear. Now input2 is float input

LOOP1:
			SUB		.S1 A5, A9, A11  	; get element from first array A11 is the arrayIndex
			CMPGT 	.L1	A11, -1, A2 	; see if A11 (arrayIndex) greater than -1, set the result to register A2
			[A2] 	B 		.S2 	DENMULT
			NOP		5
			ADD 	.S1 A11, B4, A11
			;we still have to go to denmult here, right?
DENMULT:		;I'm probably missing som NOPs
;ARRAYS?!!	       ;LDW		.D1	*+A6[A9], A13	; get element from second array. A13 holds DEN2[i]
		       ;LDW		.D1	*+A12[A11], A14 ;A14 holds tempOmega[arrayIndex]
		       ;MPYSP		.M2	A14, A13, A15 ; A15 holds the product
		       ;ADDSP		.L2	A16, A15, A16 ; A16 holds (tempOmega[arrayIndex] * DEN2[i])
		       ;MPYSP		.M2	A16, -1, A16 ;  Multiply A16 by -1
		       ;STW		.D1	A16, *+A12[A5] ;Store A16 into tempOmega[index]
		       ;ADDSP 		.L2	A9, 1, A9	
		       ;CMPLT 	.L1	A9, B4, A2 	; see if A9 (i) is less than order
		       ;[A2] 	B 		.S2 	LOOP1 ;if i is less than order, we go on and repeat loop
;LOOP2:	
;NUMMULT: ;Can we reuse the same variables from above instead of allocating new memory?
		       ;
			LDW		.D1	*A12++, A13	; get element from second array
			NOP		4				; wait for data
			MPYSP A7	; multiply
			NOP	    3				; wait for result
			ADDSP	.L1	A7, A8, A8	; accumulate
			NOP		3
			SUB		.S1	A1, 1, A1	; decrement counter
	[A1]	B	.S2	LOOP1			; conditional branch
			NOP 	5
			; conditional branch occurs here
			MV		.S1	A8, A4		; put result in proper register
			B		B3				; branch back to calling function
			NOP		5

			.end
