#include "ch32v003_touch.h"

#define ADD_N_NOPS( n ) asm volatile( ".rept " #n "\nc.nop\n.endr" );

void InitTouchADC( )
{
	// ADCCLK = 24 MHz => RCC_ADCPRE = 0: divide sys clock by 2
	RCC->CFGR0 &= ~(0x1F<<11);

	// Set up single conversion on chl 2
	ADC1->RSQR1 = 0;
	ADC1->RSQR2 = 0;

	// turn on ADC and set rule group to sw trig
	ADC1->CTLR2 |= ADC_ADON | ADC_EXTSEL;
	
	// Reset calibration
	ADC1->CTLR2 |= ADC_RSTCAL;
	while(ADC1->CTLR2 & ADC_RSTCAL);
	
	// Calibrate
	ADC1->CTLR2 |= ADC_CAL;
	while(ADC1->CTLR2 & ADC_CAL);
}

void INNER_LOOP(GPIO_TypeDef * io, int portpin, uint8_t n, uint32_t *ret)
{	
    uint32_t CFGBASE = io->CFGLR & (~(0xf<<(4*portpin)));
	uint32_t CFGFLOAT = ((GPIO_CFGLR_IN_PUPD)<<(4*portpin)) | CFGBASE;
	uint32_t CFGDRIVE = (GPIO_CFGLR_OUT_2Mhz_PP)<<(4*portpin) | CFGBASE;
    __disable_irq();
    FORCEALIGNADC
    /* Tricky - we start the ADC BEFORE we transition the pin.  By doing
        this We are catching it onthe slope much more effectively.  */
    ADC1->CTLR2 = ADC_SWSTART | ADC_ADON | ADC_EXTSEL;
//    ADD_N_NOPS(n)
#if TOUCH_FLAT == 1
    io->BSHR = 1<<(portpin+16*TOUCH_SLOPE); 
    io->CFGLR = CFGFLOAT;
#else
    io->CFGLR = CFGFLOAT; 
    io->BSHR = 1<<(portpin+16*TOUCH_SLOPE);
#endif
    /* Sampling actually starts here, somewhere, so we can let other
        interrupts run */
    __enable_irq();
    while(!(ADC1->STATR & ADC_EOC));
    io->CFGLR = CFGDRIVE;
    io->BSHR = 1<<(portpin+(16*(1-TOUCH_SLOPE)));
    *ret += ADC1->RDATAR;
}

// Run from RAM to get even more stable timing.
// This function call takes about 8.1uS to execute.
uint32_t ReadTouchPin( GPIO_TypeDef * io, int portpin, int adcno, int iterations )
{
	uint32_t ret = 0;

	__disable_irq();
	FORCEALIGNADC
	ADC1->RSQR3 = adcno;
	ADC1->SAMPTR2 = TOUCH_ADC_SAMPLE_TIME<<(3*adcno);
	__enable_irq();


	int i;
	for( i = 0; i < iterations; i++ )
	{
        INNER_LOOP(io, portpin, 0, &ret);
        INNER_LOOP(io, portpin, 2, &ret);
        INNER_LOOP(io, portpin, 4, &ret);
	}

	return ret;
}

