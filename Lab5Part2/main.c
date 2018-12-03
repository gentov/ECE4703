#define CHIP_6713 1

#include <stdio.h>
#include <math.h>
#include <c6x.h>
#include <csl.h>
#include <csl_mcbsp.h>
#include <csl_irq.h>
#include "dsk6713_led.h"
#include "dsk6713.h"
#include "dsk6713_aic23.h"
#include "fdacoefs_10.h"

#define N 32
#define RADIX 2
#define PI 3.14159265358979


DSK6713_AIC23_CodecHandle hCodec;                           // Codec handle
DSK6713_AIC23_Config config = DSK6713_AIC23_DEFAULTCONFIG;  // Codec configuration with default settings

// complex typedef
typedef struct
{
    float re, im;
} COMPLEX;

// align data (nothing works if you omit these pragma)
#pragma DATA_ALIGN(w,sizeof(COMPLEX))   //align w
#pragma DATA_ALIGN(x,sizeof(COMPLEX))   //align x

// function prototypes
void cfftr2_dit(COMPLEX*, COMPLEX*, short);
void bitrev(COMPLEX*, short*, int);
void digitrev_index(short*, int, int);

// global variables
short xxx = 0;
COMPLEX w[N / RADIX];        // array of complex twiddle factors
// Recall how decimation in time works. The algorithms separates
// N-point input x into even and odd sequences and performs N/2
// point FFT on even and odd parts so we need only pass in N/2
// twiddle factors.
// Also remember Eulers exp(-j*theta) = cos(theta) - j*sin(theta)

short yyy = 0;
COMPLEX h[N];     // array of complex FFT input/output samples
float DELTA = 2.0 * PI / N;   // makes calculating twiddles easier
short iw[N / 2], ix[N]; // indices for bit reversal (these arrays are unnecessarily large)
int i, n;

void main(void)
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
    DSK6713_AIC23_setFreq(hCodec, DSK6713_AIC23_FREQ_8KHZ);

// interrupt setup
    IRQ_globalDisable();            // Globally disables interrupts
    IRQ_nmiEnable();                // Enables the NMI interrupt
    IRQ_map(IRQ_EVT_RINT1, 15);      // Maps an event to a physical interrupt
    IRQ_enable(IRQ_EVT_RINT1);      // Enables the event
    IRQ_globalEnable();             // Globally enables interrupts

    // compute first N/2 twiddle factors
    for (i = 0; i < N / RADIX; i++)
    {
        w[i].re = cos(DELTA * i);
        w[i].im = sin(DELTA * i);  // negative imag component
    }

    for (i = 0; i < N / 2; i++)
        iw[i] = -1;

    for (i = 0; i < N; i++)
        ix[i] = -1;

    // initialize complex FFT input array with fixed, known values
    // these are the same values as in the Welch textbook
    // In Lab 5, x will be your full input buffer plus pre-pended
    // history points
    for (i = 0; i < N; i++)
    {
        //x[i].re = cos(PI/2*i);
        if (i < B)
            h[i].re = B[i]; // IS THIS RIGHT?!!!>!>!>!>!>!>>!!:?>!?!?!??!>!!L!>!>!1111>>!>?!>! MAYBE.
        else
            h[i].re = 0;
        h[i].im = 0;
    }

    digitrev_index(iw, N / RADIX, RADIX);   //produces index for bitrev() W
    bitrev(w, iw, N / RADIX);               //bit reverse W
    cfftr2_dit(h, w, N); //TI floating-pt complex FFT. Read comments at beginning of code.
    // Time series x is passed in order but w is bit reversed
    // Spectrum X is passed back in x array out but of order
    digitrev_index(ix, N, RADIX);       //produces index for bitrev() X
    bitrev(x, ix, N);             //freq scrambled->bit-reverse X
    // x array now hold spectrum X in correct frequency order

    // See comments in cfftr2_dit() for instructions of how to do inverse FFT

    while (1)                  // main loop - do nothing but wait for interrupts
    {
    }

}

interrupt void serialPortRcvISR()
{
    union {Uint32 combo; short channel[2];} temp;

    temp.combo = MCBSP_read(DSK6713_AIC23_DATAHANDLE);
    // Note that right channel is in temp.channel[0]
    // Note that left channel is in temp.channel[1]

    MCBSP_write(DSK6713_AIC23_DATAHANDLE, temp.combo); //ship it


}

