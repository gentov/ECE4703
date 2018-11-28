; nonparallel assembly code for N element floating point dot product
; A4 and B4 contain pointers to arrays of floats
; A6 contains an integer with N 
			.def 	_iirDFII_asm
			.ref  	_index
			.ref	_DEN2
			.ref    _NUM2
			.ref 	_tempOmega

_iirDFII_asm:

			MVKL	.S1 _index, A5
			MVKH 	.S1 _index, A5
			LDW		.D1 *A5, A5
			NOP 6

			MVKL	.S1 _DEN2, A1
			MVKH 	.S1 _DEN2, A1

			MVKL	.S1 _NUM2, A7
			MVKH 	.S1 _NUM2, A7

			MVKL	.S1 _tempOmega, A10
			MVKH 	.S1 _tempOmega, A10

			MV		.S2	A4, B7		; copy input to B7
			MV 		.S2	B4, B12     ; copy order to b12
			ZERO	.L1	A9			; zero  (index = 0)
			ZERO 	.L1	A3			; Accumulator!
			ZERO	.L1	A11			; zero arrayIndex
			;I think we need to zero filterOut.... since filter out isn't global here, we just keep it in a register and add
			; we never reset that register.
			ZERO	.L1	A8 ; accumulator of filterout
			ADD	 	.S1 A9, 1, A9   ; increment i, start at 1.
			NOP 	 2


LOOP1:
			; A9: i of for loop. A13: den[i],
		       LDW		.D1	*A1[A9], A13	; get element from second array. A13 holds DEN2[i]
		       NOP	5
		       LDW		.D1	*A6[A9], A14 ;A14 holds tempOmega[i]
		       NOP	5
		       MPYSP	.M1  A14, A13, A15 ; A15 holds the product of A13 and A14.
		       NOP 	 8
		       ADDSP	.L1	A3, A15, A3 ; Add the sum to the accumulator
		       NOP 	 4

		       ADD 	.L1	A9, 1, A9 ; increment i
		       NOP 	 4
		       CMPGT 	.L1	A9, B12, A2
		       [!A2] 	B 		.S2 	LOOP1 ;if i is less than order, we go on and repeat loop
			   NOP 6

		       ;BEFORE LOOP TWO WE MUST ADD INPUT2 TO TEMPOMEGA[INDEX]

		       ADDSP	.L1	A3, B7, A3 ; tempOmega[index] += input2.
		       NOP	4
		       STW		.D1	A3, *A10[A5] ;Store A10 into tempOmega[index], located at the address sored in A6
		       NOP 	 8
			   ZERO		.L1	A9 ; Zero A9
			   NOP	 2
			   ZERO		.L1	A11 ; Zero Z11
			   NOP	 2
			   ZERO		.L1	A3 ; Zero A3
			   NOP	 2

LOOP2:
			   LDW		.D1	*A7[A9], A13	; get element from second array. A13 holds NUM2[i]
			   NOP	5
		       LDW		.D1	*A6[A9], A14   ;A14 holds tempOmega[arrayIndex]
		       NOP	5
		       MPYSP	.M1	A14, A13, A15 ; A15 holds the product
		       NOP	8
		       ADDSP	.L1	A8, A15, A8 ; A8 (filterOut) holds (tempOmega[arrayIndex] * NUM2[i])
		       NOP	5
		       ADD 	.L1	A9, 1, A9 ; increment "j"


		       CMPGT 	.L1	A9, B12, A2 	; see if A9 (i) is less than order ;; NEED GREATER THAN OR EQUAL TOO
		       [!A2] 	B 		.S2 	LOOP2 ;if i is less than order, we go on and repeat loop
			   NOP 6

			MV	.S1	A8,A4
			B	B3
			NOP	6

			.end
