#define CHIP_6713 1

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <c6x.h>
#include <csl.h>
#include <csl_mcbsp.h>
#include <csl_irq.h>
#include "dsk6713_led.h"
#include "dsk6713.h"
#include "dsk6713_aic23.h"
#include "fdacoefs_8_n.h"

#define N 32
#define RADIX 2
#define PI 3.14159265358979
#define K 25

DSK6713_AIC23_CodecHandle hCodec;                           // Codec handle
DSK6713_AIC23_Config config = DSK6713_AIC23_DEFAULTCONFIG; // Codec configuration with default settings

// complex typedef
        typedef struct
        {
            float re, im;
        }COMPLEX;

// align data (nothing works if you omit these pragma)
#pragma DATA_ALIGN(w,sizeof(COMPLEX))   
#pragma DATA_ALIGN(w_conj,sizeof(COMPLEX))  
#pragma DATA_ALIGN(h,sizeof(COMPLEX))   
#pragma DATA_ALIGN(prodRes,sizeof(COMPLEX))   
#pragma DATA_ALIGN(app,sizeof(COMPLEX))   
#pragma DATA_ALIGN(pongInZ,sizeof(COMPLEX))  
#pragma DATA_ALIGN(pingInZ,sizeof(COMPLEX))   
#pragma DATA_ALIGN(pongIn,sizeof(COMPLEX))  
#pragma DATA_ALIGN(pingIn,sizeof(COMPLEX))  
#pragma DATA_ALIGN(pongOut,sizeof(COMPLEX))  
#pragma DATA_ALIGN(pingOut,sizeof(COMPLEX))   

// function prototypes
        int globalIndex = 0;
        void cfftr2_dit(COMPLEX*, COMPLEX*, short);
        void bitrev(COMPLEX*, short*, int);
        void digitrev_index(short*, int, int);
        int switchBuffer = 0;
        int pingActive = 1;// Zero is Ping, One is Pong. Active means its filling
// global variables
        COMPLEX w[N / RADIX];// array of complex twiddle factors
        COMPLEX w_conj[N / RADIX];// array of complex conj of the complex twiddle factors,
// Recall how decimation in time works. The algorithms separates
// N-point input x into even and odd sequences and performs N/2
// point FFT on even and odd parts so we need only pass in N/2
// twiddle factors.
// Also remember Eulers exp(-j*theta) = cos(theta) - j*sin(theta)

        short yyy = 0;
        COMPLEX h[N];// array of complex coefficients
        float DELTA = 2.0 * PI / N;// makes calculating twiddles easier
        short iw[N / 2],
ih[N], iapp[N], iprod[N], ix[N]; // indices for bit reversal (these arrays are unnecessarily large)
int i, n;

//k = N - (10)

//COMPLEX arrays initialized to zero
COMPLEX pingIn[K] = {0,0};
COMPLEX pongIn[K] = {0,0};
COMPLEX pingInZ[7] = {0,0}; //BL-1
COMPLEX pongInZ[7] = {0,0};
COMPLEX pingOut[K] = {0,0};
COMPLEX pongOut[K] = {0,0};
COMPLEX app[N] = {0,0};
COMPLEX prodRes[N] = {0,0};

//function to perform complex multiplication
inline COMPLEX complexMult(COMPLEX a, COMPLEX b)
{
    float re = ((a.re * b.re) - (a.im * b.im));
    float im = ((a.im * b.re) + (a.re * b.im));

    COMPLEX res;
    res.re = re;
    res.im = im;

    return res;
}

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

    // compute first N/2 twiddle factors, as well as complex conj of it
    for (i = 0; i < N / RADIX; i++)
    {
        w[i].re = cos(DELTA * i);
        w[i].im = sin(DELTA * i);  
        w_conj[i].re = w[i].re;
        w_conj[i].im = -1 * (w[i].im); // negative imag component
    }

    for (i = 0; i < N / 2; i++)
        iw[i] = -1;

    for (i = 0; i < N; i++)
    {
        ih[i] = -1;
        iapp[i] = -1;
        ix[i] = -1;
        //iprod[i] = -1;
    }

     // fill array h with the filter coefficients, pad the rest with zeros
    for (i = 0; i < N; i++)
    {
        //h[i].re = cos(PI/2*i);
        if (i < BL)
            h[i].re = B[i]; // fill with B[i], pad the rest with zeros to match length N
        else
            h[i].re = 0;
        h[i].im = 0;
    }
    
    digitrev_index(iw, N / RADIX, RADIX);   //produces index for bitrev() W
    bitrev(w, iw, N / RADIX);               //bit reverse W
    bitrev(w_conj, iw, N / RADIX);               //bit reverse W
    
    //compute the FFT of the filter coefficients
    cfftr2_dit(h, w, N); //h is out of order
    digitrev_index(ih, N, RADIX);       //produces index for bitrev() X
    bitrev(h, ih, N);             //freq scrambled->bit-reverse X

    while (1)                  // main loop
    {
        //if one of the buffers is full
        if(switchBuffer == 1)
        {
            switchBuffer = 0;    // clear flag
            //if ping is filling
            if (pingActive == 1)
            { // Pong is processing
                    
              // Step one: Prepend pongInZ to pongIn
                memcpy(app, pongInZ, (BL - 1) * sizeof(COMPLEX));
                memcpy(&app[BL - 1], pongIn, K * sizeof(COMPLEX));

                // Step 2: Take FFT of app =  [ponginZ, pongIn]
                cfftr2_dit(app, w, N); 
                // Do we need digitrev?
                digitrev_index(ix, N, RADIX);     //produces index for bitrev() X
                //digitrev_index(iapp, N, RADIX);  //produces index for bitrev() X
                bitrev(app, ix, N);            //freq scrambled->bit-reverse X

                //Step 3: compute the N-pt product of the app and h, and put it into prodRes
                int k;
                for (k = 0; k < N; k++)
                {
                    prodRes[k] = complexMult(app[k], h[k]);
                }

                //Step 4: Compute N-pt IFFT
                // The invFFT will be calculated by taking the FFT with the complex conj. of the w coeffs, 
                // then dividing outputs by N
                cfftr2_dit(prodRes, w_conj, N); 
                //do we need digitrev?
                digitrev_index(ix, N, RADIX);     //produces index for bitrev() X
                //digitrev_index(iprod, N, RADIX); //produces index for bitrev() X
                bitrev(prodRes, ix, N);       //freq scrambled->bit-reverse X
                
                // divide the product by N
                //After this loop, im values of prodRres becomes very small: on the order of e-10
                int l;
                for (l = 0; l < N; l++)
                {
                    prodRes[l].im /= N;
                    prodRes[l].re /= N;
                }
                
                  
                //Step 5: write last K samples of prodRes to output buffer
                memcpy(pongOut, &prodRes[N - K], K * sizeof(COMPLEX));

                //Step 6: put last M - 1 values of input into PingInZ
                memcpy(pongInZ, &pongIn[K - (BL -1)], (BL - 1) * sizeof(COMPLEX));

            }

            else

            {    // Ping is processing
                 // Pong is filling
                    
                // Step one: Prepend pingInZ to pingIn
                memcpy(app, pingInZ, (BL - 1) * sizeof(COMPLEX));
                memcpy(&app[BL - 1], pingIn, (K) * sizeof(COMPLEX));

                // Step 2: Take FFT of app =  [pinginZ, pingIn]
                cfftr2_dit(app, w, N); //TI floating-pt complex FFT. Read comments at beginning of code.
                //do we need digitRev?
                digitrev_index(ix, N, RADIX);     //produces index for bitrev() X
                //digitrev_index(iapp, N, RADIX);  //produces index for bitrev() X //replaced iapp with ih
                bitrev(app, ih, N);            //freq scrambled->bit-reverse X

                //Step 3: compute the N-pt product of the app and h, and put it into prodRes
                int k;
                for (k = 0; k < N; k++)
                {
                    prodRes[k] = complexMult(app[k], h[k]);
                }
                
                //Step 4: Compute N-pt IFFT 
                // The invFFT will be calculated by taking the FFT with the complex conj. of the w coeffs, 
                // then dividing outputs by N
                cfftr2_dit(prodRes, w_conj, N); //TI floating-pt complex FFT. Read comments at beginning of code.
                //do we need digitRev
                digitrev_index(ix, N, RADIX);  //digitrev_index(iprod, N, RADIX); //produces index for bitrev() X
                bitrev(prodRes, ih, N);       //freq scrambled->bit-reverse X
                    
                // divide the product by N
                //After this loop, im values of prodRres becomes very small: on the order of e-10
                int l;
                for (l = 0; l < N; l++)
                {
                    prodRes[l].im = (prodRes[l].im / N);
                    prodRes[l].re = (prodRes[l].re / N);
                }

                //Step 5: write last K samples of prodRes to output buffer
                memcpy(pingOut, &prodRes[N - K], K * sizeof(COMPLEX));

                //Step 6: put last M - 1 values of input into PingInZ
                memcpy(pingInZ, &pingIn[K - (BL - 1)], (BL - 1) * sizeof(COMPLEX));

            }

            // Check if flag is still == 0. If not, it means system is not running in real-time (error)

            if (switchBuffer == 1)
            {
                printf("Game Over. You got dead.");

            }
        }
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
    float rChann = (float) ((temp.channel[0]) / (32768.0));


    if (pingActive == 0)
       { // Pong is filling, ping is outputting 
           pongIn[globalIndex].re = rChann;
           pongIn[globalIndex].im = 0.0; // Set the imag part equal to zero
           temp.channel[0] = (short)((pingOut[globalIndex].re) * 32768);
       }
       else
       {    // Ping is filling,pong is outputting
           pingIn[globalIndex].re = rChann;
           pingIn[globalIndex].im = 0.0; // Set the imag part equal to zero
           temp.channel[0] = (short)((pongOut[globalIndex].re) * 32768);
       }

    globalIndex++;

    if (globalIndex >= K)            //if the buffer is full
    {
        //reset the index
        globalIndex = 0;
        //it's time to switch buffers
        switchBuffer = 1;
        
        if(pingActive == 1)
        pingActive = 0;

        else if(pingActive == 0)
        pingActive = 1;
    }

    // Note that right channel is in temp.channel[0]
     // Note that left channel is in temp.channel[1]
     temp.channel[1] = 0;
     MCBSP_write(DSK6713_AIC23_DATAHANDLE, temp.combo); //ship it


}

