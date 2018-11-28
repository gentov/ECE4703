; nonparallel assembly code for N element floating point dot product
; A4 and B4 contain pointers to arrays of floats
; A6 contains an integer with N 
			.def 	_iirDFII_asm
			.ref  	_index
			.ref	_DEN2
			.ref    _NUM2
			;.ref 	_tempOmega

_iirDFII_asm:
			MVKL	.S1 _index, A5
			MVKH 	.S1 _index, A5
			LDW		.D1 *A5, A5
			NOP 6

			MVKL	.S1 _DEN2, A1
			MVKH 	.S1 _DEN2, A1

			MVKL	.S1 _NUM2, A7
			MVKH 	.S1 _NUM2, A7

			;MVKL	.S1 _tempOmega, A1
			;MVKH 	.S1 _tempOmega, A1

			;MV		.S1	B4, A1		; copy count to register A1 (input)
			ZERO	.L1	A9			; zero accumulator (index = 0)
			ZERO 	.L1	A3			; This will hold the value to put into tempOmega[index]
			ZERO	.L1	A11			; zero arrayIndex
			ADD	 	.S1 A9, 1, A9   ; increment i
			NOP 	 2
			;;INTSP 	.L1 A1, A1 ; a1 == input2 just so we're clear. Now input2 is float input
			;

LOOP1:

			SUB A5, A9, A11; arrayIndex = index - i
			CMPLT .L1 A11, 0, A2
		    [!A2] B .S2  DENMULT
		    NOP 5
			ADD .L1 A11, B4, A11; ; ARRAYINDEX SHUOLD NEVER BE MORE THAN 10 THOUGH



			;CMPGT 	.L1	A9, A5, A2 	;
			; If a9 > a5, then would underflow. So if A9 > A5...
			;[!A2] 	B 		.S2 	SKIPADD
			;ZERO    .L1 A11 ; reset ArrayIndex
			;ADD     .S1 A11, B4, A11 ; add order to ArrayIndex
			;SUB     .S1 A11, A9, A11 ; sub A9 from A11
			;ADD     .S1 A11, A5, A11 ; add a5 from a11
			;NOP		5





DENMULT:
		;      MPYSP	.M1  *+A6[A9], *+A6[A11], *+A6[A5] ;
		       LDW		.D1	*A1[A9], A13	; get element from second array. A13 holds DEN2[i]
		       NOP	5
		       LDW		.D1	*A6[A11], A14 ;A14 holds tempOmega[arrayIndex]
		       NOP	5
		       LDW      .D1 *A6[A5], A3; load into A3 tempOmega[index]
		       NOP	5
		       MPYSP	.M1  A14, A13, A15 ; A15 holds the product
		       ;ZERO .L1	B1
		       NOP 	 8
		       ADDSP	.L1	A3, A15, A3 ; A3 holds (tempOmega[arrayIndex] * DEN2[i])
		       NOP 	 4
		       MV	.L2	 A3, B1 ;  Multiply A3 by -1
		       ZERO .L1  A3
		       SUBSP	.L1 A3, B1, A3 ;SUBSP???????
		       NOP 	 5
		       STW		.D1	A3, *A6[A5] ;Store A3 into tempOmega[index]
		       NOP	5
		       ADD 	.L1	A9, 1, A9 ; increment i
		       NOP 	 4
		       CMPGT 	.L1	A9, B4, A2 	; see if A9 (i) is less than order ;; NEED GREATER THAN OR EQUAL TOO
		       [!A2] 	B 		.S2 	LOOP1 ;if i is less than order, we go on and repeat loop
			   NOP 6


		       ;BEFORE LOOP TWO WE MUST ADD INPUT2 TO TEMPOMEGA[INDEX]
		       LDW		.D1	*A6[A5], A10
		       NOP 	 5
		       ADDSP	.L1	A10, A4, A10 ; tempOmega[index] += input2
		       NOP	4
		       STW		.D1	A10, *A6[A5] ;Store A3 into tempOmega[index]
		       NOP 	 8
			   ZERO		.L1	A9
			   NOP	 2
			   ZERO		.L1	A11 ; reset arrayIndex
			   NOP	 2
LOOP2:

			SUB A5, A9, A11; arrayIndex = index - 1
			CMPLT .L1 A11, 0, A2
		    [!A2] B .S2  NUMMULT
		    NOP	6
			ADD A11, B4, A11;


		;CMPGT 	.L1	A9, A5, A2 	;
			;[!A2] 	B 		.S2 	SKIPADD1
			;ZERO    .L1 A11 ; reset ArrayIndex
			;ADD     .S1 A11, B4, A11 ; add order to ArrayIndex
			;SUB     .S1 A11, A9, A11 ; sub A9 from A11
			;ADD     .S1 A11, A5, A11 ; sub a5 from a11
			;NOP		5

;SKIPADD1:	SUB 	.S1 A9, A5, A11 ; int arrayindex = index - i


NUMMULT: ;Can we reuse the same variables from above instead of allocating new memory?
		       ;
			   LDW		.D1	*A7[A9], A13	; get element from second array. A13 holds NUM2[i]
			   NOP	5
		       LDW		.D1	*A6[A11], A14 ;A14 holds tempOmega[arrayIndex]
		       NOP	5
		       MPYSP	.M1	A14, A13, A15 ; A15 holds the product
		       NOP	8
		       ADDSP	.L1	A8, A15, A8 ; A8 (filterOut) holds (tempOmega[arrayIndex] * NUM2[i])
		       NOP	5
		       ADD 	.L1	A9, 1, A9 ; increment "j"
		       CMPGT 	.L1	A9, B4, A2 	; see if A9 (i) is less than order ;; NEED GREATER THAN OR EQUAL TOO
		       [!A2] 	B 		.S2 	LOOP2 ;if i is less than order, we go on and repeat loop
			   NOP 6

			MV	.S1	A8,A4
			B	B3
			NOP	6

			.end
