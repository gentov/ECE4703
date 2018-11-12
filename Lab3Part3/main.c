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

int tempOmega[5][3] = 0;
int gain[6];
int bcoeffs [5][3];
int acoeffs [5][3];
int index[5] = { 0, 0, 0, 0, 0};
float stageOut;
float filterOut = 0;
interrupt void serialPortRcvISR(void);

//chopping the HEADer file.... get it....
void guillotine1()
{
    int i, k; //i is for the column, and k is for the rows
    for(i = 0; i < MWSPT_NSEC; i++)
    {
        if(i%2 == 0)//if it's even
        {
            gain[i/2] = NUM[i][0]; //we always want only the first elmt
        }

        else //if it's odd
        {
            for(k = 0; k < 3; k++){
                bcoeffs[i/2][k] = NUM[i][k]; //fill the bcoeffs array
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
                acoeffs[i/2][k] = DEN[i][k]; //fill the acoeffs array
            }
        }
    }
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
   // int k;
    guillotine1();
    guillotine2();
//    for(k = 0; k< MWSPT_NSEC; k++)
//        tempOmega[z][k] = {0.0, 0.0, 0.0};
    while(1)                        // main loop - do nothing but wait for interrupts
    {
    }
}

//have to run this for as many stages as we have
float filterGenerator(float startingVal, int stageNumber)
{
    int output = 0;
    int tempVal = startingVal; //q15 number
    //have to multiply input by the gain
     tempVal *= gain[stageNumber]; //q15 * q6 = q21
    //in a for loop as large as the array of gains


     //circular indexing
      index[stageNumber]++;
      index[stageNumber] = index[stageNumber]%3;
      tempOmega[stageNumber][index[stageNumber]] = 0;

      //iterate to populate the omega array (in between output and input)
        int k;
        for(k = 1; k<3; k++)
        {
           // arrayIndex is the offset index, looking at
           // previous omega elements
            int arrayIndex = index[stageNumber] -  k;
            if (arrayIndex < 0) arrayIndex += 3; //wrap around arrayIndex if it goes below 0
            //populate current tempOmega value for the stageNumber in question
            tempOmega[stageNumber][index[stageNumber]] += ((tempOmega[stageNumber][arrayIndex] * acoeffs[stageNumber][k]));
        }
        // multiply by -1 and add the value on the rightChannel
        tempOmega[stageNumber][index[stageNumber]] *= -1;
        tempOmega[stageNumber][index[stageNumber]] += tempVal;
        tempOmega[stageNumber][index[stageNumber]] = tempOmega[stageNumber][index[stageNumber]] >> 6 ; // turns it back into a q-15 from a q-21


        //repeat the process with bcoeffs
        int j;
        for(j = 0; j < 3; j++)
        {
            int arrayIndex2 = index[stageNumber] -  j;
            if (arrayIndex2 < 0) arrayIndex2 += 3;
            output += (bcoeffs[stageNumber][j]*tempOmega[stageNumber][arrayIndex2]);
        }


        return (output >> 6); //function returns the output of this stage , q15

}

interrupt void serialPortRcvISR()
{
    union {Uint32 combo; short channel[2];} temp;

    temp.combo = MCBSP_read(DSK6713_AIC23_DATAHANDLE);
    // Note that right channel is in temp.channel[0]
    // Note that left channel is in temp.channel[1]

    int rChann = (((int) (temp.channel[0]))); //coeffs are q6, and input is q15, making q21
    rChann = rChann << 6; //shift q15 6 bits to make it q21
    int i;
    filterOut = rChann >> 6;
    for(i = 0; i<5; i++)
    {
         filterOut = filterGenerator(filterOut,i);

    }
    rChann = filterOut * gain[5]; //multiply it by 32768 * 0.5 (stop clippin from dc offset)
    short s = (short) (rChann >> 6); //cast to a float
    temp.channel[0] = s; //set the right channel to the new value
    MCBSP_write(DSK6713_AIC23_DATAHANDLE, temp.combo); //ship it
    filterOut = 0;

}

