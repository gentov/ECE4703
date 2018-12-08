/*************************************************************************
 *  Basic stereo loop code for C6713 DSK and AIC23 codec
 *  D. Richard Brown on 22-Aug-2011
 *  Based on code from "Real-Time Digital Signal Processing Based on TMS320C6000"
 *  by N. Kehtarnavaz and N. Kim.
 *************************************************************************/

#define CHIP_6713 1

#define order 27

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
DSK6713_AIC23_Config config = DSK6713_AIC23_DEFAULTCONFIG; // Codec configuration with default settings

float filter[order] = { 0 };
//float b_adpt[order] = {0};
int index = 0;
float filterOut;
float error;
float BA[order];

float mu = .01;
interrupt void serialPortRcvISR(void);

void main()
{

    DSK6713_init(); // Initialize the board support library, must be called first
    hCodec = DSK6713_AIC23_openCodec(0, &config);   // open codec and get handle

    DSK6713_LED_init(); // to use LEDs

    // Configure buffered serial ports for 32 bit operation
    // This allows transfer of both right and left channels in one read/write
    MCBSP_FSETS(SPCR1, RINTM, FRM);
    MCBSP_FSETS(SPCR1, XINTM, FRM);
    MCBSP_FSETS(RCR1, RWDLEN1, 32BIT);
    MCBSP_FSETS(XCR1, XWDLEN1, 32BIT);

    // set codec sampling frequency, change to 16 for Nyquist rate of 11k
    DSK6713_AIC23_setFreq(hCodec, DSK6713_AIC23_FREQ_44KHZ);
    int k;
      for(k = 0; k<order; k++)
      {
          BA[k] = 0.0;
          filter[k] = 0.0;
          error = 0.0;
      }
    // interrupt setup
    IRQ_globalDisable();            // Globally disables interrupts
    IRQ_nmiEnable();                // Enables the NMI interrupt
    IRQ_map(IRQ_EVT_RINT1, 15);      // Maps an event to a physical interrupt
    IRQ_enable(IRQ_EVT_RINT1);      // Enables the event
    IRQ_globalEnable();             // Globally enables interrupts


    while (1)                  // main loop - do nothing but wait for interrupts
    {
    }
}

interrupt void serialPortRcvISR()
{
    union
    {
        Uint32 combo;
        short channel[2];
    } temp;

    temp.combo = MCBSP_read(DSK6713_AIC23_DATAHANDLE);
    // Note that right channel is in temp.channel[0]
    // Note that left channel is in temp.channel[1]
    float unKnownOut = (((float) (temp.channel[0])) / 32768); // Cast to a float, then divide by 32768 (16 bit datatype)
    float filterIn = (((float) (temp.channel[1])) / 32768);

    //circular indexing
    index++;
    index = index % order;
    filter[index] = filterIn;

    int i; //iterator for for loop
    for (i = 0; i < order; i++)
    {
        int arrayIndex = index - i;
        if (arrayIndex < 0)
            arrayIndex += order; //wrap around
        filterOut += filter[arrayIndex] * BA[i]; //multiply and compute output
    }

    error = unKnownOut - filterOut;
    for (i = 0; i < order; i++)
    {

        int arrayIndex = index - i;
        if (arrayIndex < 0)
            arrayIndex += order; //wrap around
        BA[i] = BA[i] + mu*error*filter[arrayIndex]; //modify coeffs

    }

    short yHat = ((short) (filterOut * 32768)); //multiply it by 32768 * 0.5 (stop clippin from dc offset)
    short errorOut =  ((short) (error * 32768));
    temp.channel[0] = errorOut; //set the right channel to the error
    temp.channel[1] = yHat; //set the left channel to adaptive output
    MCBSP_write(DSK6713_AIC23_DATAHANDLE, temp.combo); //ship it
    filterOut = 0;
    error = 0;

}

