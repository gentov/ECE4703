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

float tempOmega[5][3] = 0;
float gain[6];
float bcoeffs [5][3];
float acoeffs [5][3];
int index[5] = { 0, 0, 0, 0, 0};
float stageOut;
float filterOut;
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
//    for(k = 0; k< MWSPT_NSEC; k++)
//        tempOmega[z][k] = {0.0, 0.0, 0.0};
    while(1)                        // main loop - do nothing but wait for interrupts
    {
    }
}
//chopping the HEADer file.... get it....kill me please
void guillotine1()
{
    int i, k;
    for(i = 0; i < MWSPT_NSEC; i++)
    {
        if(i%2 == 0)//if it's even
        {
            gain[i/2] = NUM[i][0];
        }

        else //if it's odd
        {
            for(k = 0; k < 3; k++){
                bcoeffs[i/2][k] = NUM[i][k];
            }
        }
    }
}

void guillotine2()
{
    int i, k;
    for(i = 0; i < MWSPT_NSEC; i++)
    {
        if (i % 2 == 1) //if it's odd
        {
            for(k = 0; k < 3; k++){
                acoeffs[i/2][k] = DEN[i][k];
            }
        }
    }
}

//have to run this for as many stages as we have
float filterGenerator(short startingVal, int stageNumber)
{
    float output = 0;
    int k = 0;
    short tempVal = startingVal;
    //have to multiply input by the gain
     tempVal *= gain[stageNumber];
    //in a for loop as large as the array of gains


     //circular indexing
      index[stageNumber]++;
      index[stageNumber] = index[stageNumber]%3;
      tempOmega[stageNumber][index[stageNumber]] = 0;


        for(k; k<3; k++)
        {
            int arrayIndex = index[stageNumber] -  k;
            if (arrayIndex < 0) arrayIndex += 3;
            tempOmega[stageNumber][index[stageNumber]] += ((tempOmega[stageNumber][arrayIndex] * acoeffs[stageNumber][arrayIndex]));
            tempOmega[stageNumber][index[stageNumber]] *= -1;
        }

        tempOmega[stageNumber][index[stageNumber]] += startingVal; // change



        int j;
        for(j = 0; j < 3; j++)
        {
            int arrayIndex2 = index[stageNumber] -  j;
            if (arrayIndex2 < 0) arrayIndex2 += 3;
            output += (bcoeffs[stageNumber][arrayIndex2]*tempOmega[stageNumber][index[stageNumber]]);
        }


        return output;

}

interrupt void serialPortRcvISR()
{
    union {Uint32 combo; short channel[2];} temp;

    temp.combo = MCBSP_read(DSK6713_AIC23_DATAHANDLE);
    // Note that right channel is in temp.channel[0]
    // Note that left channel is in temp.channel[1]

    float rChann = (((float) (temp.channel[0]))/32768); // Cast to a float, then divide by 32768 (16 bit datatype)
    float lChann = (((float) (temp.channel[1]))/32768); // Cast to a float, then divide by 32768 (16 bit datatype)
    int i = 0;
    filterOut = rChann;
    for(i; i<5; i++)
    {
         filterOut = filterGenerator(filterOut,i);

    }
    rChann = filterOut * 32768 * gain[10]; //multiply it by 32768 * 0.5 (stop clippin from dc offset)
    short s = (short) (rChann); //cast to a float
    temp.channel[0] = s; //set the right channel to the new value
    MCBSP_write(DSK6713_AIC23_DATAHANDLE, temp.combo); //ship it
    filterOut = 0;

}

