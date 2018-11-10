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

DSK6713_AIC23_CodecHandle hCodec;                           // Codec handle
DSK6713_AIC23_Config config = DSK6713_AIC23_DEFAULTCONFIG;  // Codec configuration with default settings

long long tempOmega[11];
int index = 0;
long long filterOut = 0;
interrupt void serialPortRcvISR(void);

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
    for(k = 0; k< NL; k++)
        tempOmega[k] = 0;
    while(1)                        // main loop - do nothing but wait for interrupts
    {
    }
}

interrupt void serialPortRcvISR()
{
    union {Uint32 combo; short channel[2];} temp;

    temp.combo = MCBSP_read(DSK6713_AIC23_DATAHANDLE);
    // Note that right channel is in temp.channel[0]
    // Note that left channel is in temp.channel[1]

    long long rChann = ((long long) (temp.channel[0])); //

    //circular indexing
     index++;
     index = index%NL;
     tempOmega[index] = 0;

    int i;
    // compute filter output
    for(i = 1; i < NL; i++)
    {
        int arrayIndex = index -  i;
        if (arrayIndex < 0) arrayIndex += NL;
        // multiply by acoeffs and by negative 1.
        tempOmega[index] += ((tempOmega[arrayIndex] * DEN[i]));
    }

    tempOmega[index] *= -1;
    tempOmega[index] += rChann;

    // this for loop is essentially the same thing, using the b coeffs
    int j;
    for(j = 0; j < NL; j++)
    {
        int arrayIndex2 = index -  j;
        if (arrayIndex2 < 0) arrayIndex2 += NL;
        filterOut += (NUM[j]*tempOmega[arrayIndex2]); //filter output
    }

    rChann = filterOut >> 20;
//    short s = (short) (rChann); //cast to a float
    temp.channel[0] = rChann; //set the right channel to the new value
    MCBSP_write(DSK6713_AIC23_DATAHANDLE, temp.combo); //ship it
    filterOut = 0;

}

