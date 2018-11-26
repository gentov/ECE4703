; nonparallel assembly code for N element floating point dot product
; A4 and B4 contain pointers to arrays of floats
; A6 contains an integer with N 
			.def 	_iirDFII_asm
			.ref  	_index
			.ref	_DEN2
			.ref     _NUM2
			.ref 	_filterOut
			.ref _tempOmega

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

			MVKL	.S1 _input2, A10
			MVKH 	.S1 _input2, A10

			MVKL	.S1 _arrayIndex, A11
			MVKH 	.S1 _arrayIndex, A11

			MVKL	.S1 _tempOmega, A12
			MVKH 	.S1 _tempOmega, A12

			MV		.S1	A4, A1		; copy count to register A1 (input)
			ZERO	.L1	A9			; zero accumulator (index = 0)
			ZERO 	.L1	A3			; This will hold the value to put into tempOmega[index]
			ADD	 	.S1 A9, 1, A9
			NOP 	 2
			INTSP 	.L1 A1, A0 ; a1 == input2 just so we're clear. Now input2 is float input

LOOP1:
			SUB		.S1 A5, A9, A11  	; A11 is the arrayIndex (index - i)
			NOP 	 2
			CMPGT 	.L1	A11, -1, A2 	; see if A11 (arrayIndex) greater than -1, set the result to register A2
			[A2] 	B 		.S2 	DENMULT
			NOP		5
			ADD 	.S1 A11, B4, A11
			;we still have to go to denmult here, right?
DENMULT:		;I'm probably missing som NOPs
;ARRAYS?!!
			   LDW		.D1	*+A6[A9], A13	; get element from second array. A13 holds DEN2[i]
		       LDW		.D1	*+A12[A11], A14 ;A14 holds tempOmega[arrayIndex]
		       MPYSP	.M1  A14, A13, A15 ; A15 holds the product
		       NOP 	 3
		       ADDSP	.L1	A3, A15, A3 ; A3 holds (tempOmega[arrayIndex] * DEN2[i])
		       NOP 	 3
		       MV	.L2	 A3, B1 ;  Multiply A3 by -1
		       ZERO .L1  A3
		       SUB		.S1 A3, B1, A3  	; A11 is the arrayIndex (index - i)
		       NOP 	 5
		       STW		.D1	A3, *+A12[A5] ;Store A3 into tempOmega[index]
		       ADD 	.L1	A9, 1, A9
		       NOP 	 3
		       CMPLT 	.L1	A9, B4, A2 	; see if A9 (i) is less than order
		       [A2] 	B 		.S2 	LOOP1 ;if i is less than order, we go on and repeat loop
LOOP2:
			ZERO	.L1	A9
			SUB		.S1 A5, A9, A11  	; get element from first array A11 is the arrayIndex
			NOP 	 2
			CMPGT 	.L1	A11, -1, A2 	; see if A11 (arrayIndex) greater than -1, set the result to register A2
			[A2] 	B 		.S2 	DENMULT
			NOP		5
			ADD 	.S1 A11, B4, A11
			NOP 	 2
NUMMULT: ;Can we reuse the same variables from above instead of allocating new memory?
		       ;
			   LDW		.D1	*+A7[A9], A13	; get element from second array. A13 holds NUM2[i]
		       LDW		.D1	*+A12[A11], A14 ;A14 holds tempOmega[arrayIndex]
		       MPYSP	.M1	A14, A13, A15 ; A15 holds the product
		       NOP	3
		       ADDSP	.L1	A8, A15, A8 ; A8 (filterOut) holds (tempOmega[arrayIndex] * DEN2[i])
		       NOP	3
		       ADD 	.L1	A9, 1, A9
		       NOP	3
		       CMPLT 	.L1	A9, B4, A2 	; see if A9 (i) is less than order
		       [A2] 	B 		.S2 	LOOP2 ;if i is less than order, we go on and repeat loop

			;BITSHIFT HERE, WE STILL NEED TO DO!
			MV	.S1	A8,A4
			B	B3
			NOP	5

			.end
