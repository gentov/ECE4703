/*
 * Filter Coefficients (C Source) generated by the Filter Design and Analysis Tool
 * Generated by MATLAB(R) 9.4 and DSP System Toolbox 9.6.
 * Generated on: 29-Nov-2018 12:48:54
 */

/*
 * Discrete-Time IIR Filter (real)
 * -------------------------------
 * Filter Structure    : Direct-Form II
 * Numerator Length    : 11
 * Denominator Length  : 11
 * Stable              : Yes
 * Linear Phase        : No
 * Arithmetic          : single
 */

/* General type conversion for MATLAB generated C-code  */
//#include "tmwtypes.h"
/* 
 * Expected path to tmwtypes.h 
 * C:\Program Files\MATLAB\R2018a\extern\include\tmwtypes.h 
 */
const int NL2 = 11;
const float NUM2[11] = {
   0.003279216588,              0, -0.01639608294,              0,  0.03279216588,
                0, -0.03279216588,              0,  0.01639608294,              0,
  -0.003279216588
};
const int DL2 = 11;
const float DEN2[11] = {
                1,9.373040939e-17,    -2.474416256,1.375924819e-16,    -2.811006308,
  1.327415772e-16,    -1.703772306,7.219422613e-17,   -0.5444326997,1.636396682e-17,
    -0.07231567055
};