/*************************************************************************
 *  Basic stereo loop code for C6713 DSK and AIC23 codec
 *  D. Richard Brown on 22-Aug-2011
 *  Based on code from "Real-Time Digital Signal Processing Based on TMS320C6000"
 *  by N. Kehtarnavaz and N. Kim.
 *************************************************************************/

#define CHIP_6713 1
#define coeffs 6001

#include <stdio.h>
#include <c6x.h>
#include <csl.h>
#include <csl_mcbsp.h>
#include <csl_irq.h>
#include "dsk6713_led.h"
#include "dsk6713.h"
#include "dsk6713_aic23.h"
#include "fdacoefs_6000.h"

DSK6713_AIC23_CodecHandle hCodec;                           // Codec handle
DSK6713_AIC23_Config config = DSK6713_AIC23_DEFAULTCONFIG;  // Codec configuration with default settings


float filter[coeffs] = 0;
int index = 0;
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
    DSK6713_AIC23_setFreq(hCodec, DSK6713_AIC23_FREQ_8KHZ);

    // interrupt setup
    IRQ_globalDisable();            // Globally disables interrupts
    IRQ_nmiEnable();                // Enables the NMI interrupt
    IRQ_map(IRQ_EVT_RINT1,15);      // Maps an event to a physical interrupt
    IRQ_enable(IRQ_EVT_RINT1);      // Enables the event
    IRQ_globalEnable();             // Globally enables interrupts

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

    float rChann = (((float) (temp.channel[0]))/32768); // Cast to a float, then divide by 32768 (16 bit datatype)

    //circular indexing
    index++;
    index = index%coeffs;
    filter[index] = rChann;

    int i; //iterator for for loop
    // compute filter output
    for(i = 0; i < coeffs; i++)
    {
        int arrayIndex = index -  i;
        if (arrayIndex < 0) arrayIndex += coeffs; //wrap around
        filterOut += filter[arrayIndex] * B[i]; //multiply and compute output
    }



    rChann = filterOut * 32768; //multiply it by 32768 * 0.5 (stop clippin from dc offset)
    short s = (short) (rChann); //cast to a float
    temp.channel[0] = s; //set the left channel to the new value
    temp.channel[1] = 0; //set the left channel to the new value
    MCBSP_write(DSK6713_AIC23_DATAHANDLE, temp.combo); //ship it
    filterOut = 0;

}

