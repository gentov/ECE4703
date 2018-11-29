/*************************************************************************
 *  Basic stereo loop code for C6713 DSK and AIC23 codec
 *  D. Richard Brown on 22-Aug-2011
 *  Based on code from "Real-Time Digital Signal Processing Based on TMS320C6000"
 *  by N. Kehtarnavaz and N. Kim.
 *************************************************************************/

#define CHIP_6713 1

#include <stdio.h>
#include <c6x.h>
#include <csl.h>
#include <csl_mcbsp.h>
#include <csl_irq.h>
#include "dsk6713_led.h"
#include "dsk6713.h"
#include "dsk6713_aic23.h"
#include "fdacoefs.h"
#include "fdacoefs2.h"
DSK6713_AIC23_CodecHandle hCodec;                           // Codec handle
DSK6713_AIC23_Config config = DSK6713_AIC23_DEFAULTCONFIG;  // Codec configuration with default settings

float tempOmega[11];
int index = 0;
float filterOut = 0;
float rChann [11] = {-.1, .01, -.025, .5 , -.1, .45, -.6, .5, -.1, .1, -.3};
interrupt void serialPortRcvISR(void);
float iirDFII_asm(float input, short order, float* tempOmega);

int circIndex (int a, int b, int order)
{
    int arrayIndex = a - b;
    if (arrayIndex < 0)
        arrayIndex += order;
    return arrayIndex;
}

void main()
{

    DSK6713_init();     // Initialize the board support library, must be called first
    hCodec = DSK6713_AIC23_openCodec(0, &config);   // open codec and get handle

    DSK6713_LED_init(); // to use LEDs

    // Configure buffered serial ports for 32 bit operation
    // This allows transfer of both right and left channels in one read/write
    MCBSP_FSETS(SPCR1, RINTM, FRM);
    MCBSP_FSETS(SPCR1, XINTM, FRM);
    MCBSP_FSETS(RCR1, RWDLEN1, 32BIT);
    MCBSP_FSETS(XCR1, XWDLEN1, 32BIT);

    // set codec sampling frequency, change to 16 for Nyquist rate of 11k
    DSK6713_AIC23_setFreq(hCodec, DSK6713_AIC23_FREQ_16KHZ);

    // interrupt setup
    IRQ_globalDisable();            // Globally disables interrupts
    IRQ_nmiEnable();                // Enables the NMI interrupt
    IRQ_map(IRQ_EVT_RINT1,15);      // Maps an event to a physical interrupt
    IRQ_enable(IRQ_EVT_RINT1);      // Enables the event
    IRQ_globalEnable();             // Globally enables interrupts
    int k;
    for(k = 0; k< NL2; k++)
        tempOmega[k] = 0.0;
    while(1)                        // main loop - do nothing but wait for interrupts
    {
    }
}

interrupt void serialPortRcvISR()
{
    union {Uint32 combo; short channel[2];} temp;

    float tempOmegaOrg[11];
    int y;

    index++;
    index = index%NL2;
    tempOmega[index] = 0.0;

    for(y = 0; y < 11; y++){ // shuffle the entire array up one.
        tempOmegaOrg[y] = tempOmega[circIndex(index,y,NL2)];
    }


    temp.combo = MCBSP_read(DSK6713_AIC23_DATAHANDLE);
    // Note that right channel is in temp.channel[0]
    // Note that left channel is in temp.channel[1]

    float chanTheMan = (float)(temp.channel[0]); // Cast to a float, then divide by 32768 (16 bit datatype)
    float CTM = chanTheMan/32768.0;

    float s = iirDFII_asm(CTM, NL2, tempOmegaOrg); //multiply it by 32768 * 0.5 (stop clippin from dc offset)

    temp.channel[0] = (short)(s * 32768); //set the right channel to the new value
    temp.channel[1] = (short)(s * 32768);  //set the right channel to the new value
    MCBSP_write(DSK6713_AIC23_DATAHANDLE, temp.combo); //ship it
    filterOut = 0;
}

//short iirDFII(short input, short order)
//{
//    int i;
//    float input2 = ((float)(input)/32768);
//// compute filter output
//    for (i = 1; i < order; i++)
//    {
//        int arrayIndex = index - i;
//        if (arrayIndex < 0)
//            arrayIndex += order;
//        // denmult
//        tempOmega[index] += ((tempOmega[arrayIndex] * DEN2[i]));
//        tempOmega[index] *= -1;
//    }
//
//    tempOmega[index] += input2;
//
//// this for loop is essentially the same thing, using the b coeffs
//    int j;
//    for (j = 0; j < order; j++)
//    {
//        int arrayIndex2 = index - j;
//        if (arrayIndex2 < 0)
//            arrayIndex2 += order;
//        filterOut += (NUM2[j] * tempOmega[arrayIndex2]); //filter output
//    }
//
//    return (short) (filterOut * 32768);
//}

//inline short iirDFII(short input, short order)
//{
//    float input2 = ((float)(input)/32768);
//// compute filter output
//
//        int arrayIndex = index - 1;
//        if (arrayIndex < 0)
//            arrayIndex += order;
//        // multiply by acoeffs and by negative 1.
//        tempOmega[index] += ((tempOmega[arrayIndex] * DEN2[1]));
//        tempOmega[index] *= -1;
//
//        arrayIndex = index - 2;
//        if (arrayIndex < 0)
//            arrayIndex += order;
//        // multiply by acoeffs and by negative 1.
//        tempOmega[index] += ((tempOmega[arrayIndex] * DEN2[2]));
//        tempOmega[index] *= -1;
//
//        arrayIndex = index - 3;
//        if (arrayIndex < 0)
//            arrayIndex += order;
//        // multiply by acoeffs and by negative 1.
//        tempOmega[index] += ((tempOmega[arrayIndex] * DEN2[3]));
//        tempOmega[index] *= -1;
//
//        arrayIndex = index - 4;
//        if (arrayIndex < 0)
//            arrayIndex += order;
//        // multiply by acoeffs and by negative 1.
//        tempOmega[index] += ((tempOmega[arrayIndex] * DEN2[4]));
//        tempOmega[index] *= -1;
//
//        arrayIndex = index - 5;
//        if (arrayIndex < 0)
//            arrayIndex += order;
//        // multiply by acoeffs and by negative 1.
//        tempOmega[index] += ((tempOmega[arrayIndex] * DEN2[5]));
//        tempOmega[index] *= -1;
//
//        arrayIndex = index - 6;
//        if (arrayIndex < 0)
//            arrayIndex += order;
//        // multiply by acoeffs and by negative 1.
//        tempOmega[index] += ((tempOmega[arrayIndex] * DEN2[6]));
//        tempOmega[index] *= -1;
//
//        arrayIndex = index - 7;
//        if (arrayIndex < 0)
//            arrayIndex += order;
//        // multiply by acoeffs and by negative 1.
//        tempOmega[index] += ((tempOmega[arrayIndex] * DEN2[7]));
//        tempOmega[index] *= -1;
//
//        arrayIndex = index - 8;
//        if (arrayIndex < 0)
//            arrayIndex += order;
//        // multiply by acoeffs and by negative 1.
//        tempOmega[index] += ((tempOmega[arrayIndex] * DEN2[8]));
//        tempOmega[index] *= -1;
//
//        arrayIndex = index - 9;
//        if (arrayIndex < 0)
//            arrayIndex += order;
//        // multiply by acoeffs and by negative 1.
//        tempOmega[index] += ((tempOmega[arrayIndex] * DEN2[9]));
//        tempOmega[index] *= -1;
//
//        arrayIndex = index - 10;
//        if (arrayIndex < 0)
//            arrayIndex += order;
//        // multiply by acoeffs and by negative 1.
//        tempOmega[index] += ((tempOmega[arrayIndex] * DEN2[10]));
//        tempOmega[index] *= -1;
//
//    tempOmega[index] += input2;
//
//// this for loop is essentially the same thing, using the b coeffs
//    int arrayIndex2 = index - 0;
//    if (arrayIndex2 < 0)
//    arrayIndex2 += order;
//    filterOut += (NUM2[0] * tempOmega[arrayIndex2]); //filter output
//
//    arrayIndex2 = index - 1;
//    if (arrayIndex2 < 0)
//        arrayIndex2 += order;
//    filterOut += (NUM2[1] * tempOmega[arrayIndex2]); //filter output
//
//    arrayIndex2 = index - 2;
//    if (arrayIndex2 < 0)
//        arrayIndex2 += order;
//    filterOut += (NUM2[2] * tempOmega[arrayIndex2]); //filter output
//
//    arrayIndex2 = index - 3;
//    if (arrayIndex2 < 0)
//        arrayIndex2 += order;
//    filterOut += (NUM2[3] * tempOmega[arrayIndex2]); //filter output
//
//    arrayIndex2 = index - 4;
//    if (arrayIndex2 < 0)
//        arrayIndex2 += order;
//    filterOut += (NUM2[4] * tempOmega[arrayIndex2]); //filter output
//
//    arrayIndex2 = index - 5;
//    if (arrayIndex2 < 0)
//        arrayIndex2 += order;
//    filterOut += (NUM2[5] * tempOmega[arrayIndex2]); //filter output
//
//    arrayIndex2 = index - 6;
//    if (arrayIndex2 < 0)
//        arrayIndex2 += order;
//    filterOut += (NUM2[6] * tempOmega[arrayIndex2]); //filter output
//
//    arrayIndex2 = index - 7;
//    if (arrayIndex2 < 0)
//        arrayIndex2 += order;
//    filterOut += (NUM2[7] * tempOmega[arrayIndex2]); //filter output
//
//    arrayIndex2 = index - 8;
//    if (arrayIndex2 < 0)
//        arrayIndex2 += order;
//    filterOut += (NUM2[8] * tempOmega[arrayIndex2]); //filter output
//
//    arrayIndex2 = index - 9;
//    if (arrayIndex2 < 0)
//        arrayIndex2 += order;
//    filterOut += (NUM2[9] * tempOmega[arrayIndex2]); //filter output
//
//    arrayIndex2 = index - 10;
//    if (arrayIndex2 < 0)
//    arrayIndex2 += order;
//    filterOut += (NUM2[10] * tempOmega[arrayIndex2]); //filter output
//
//
//
//    return (short) (filterOut * 32768);
//}
