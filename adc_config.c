/*
 * adc_config.c
 *
 * Created: 11/18/2018 9:38:13 PM
 *  Author: duycu
 */ 
#include <asf.h>

Adc *adcPtr = (Adc *) ADC;						// define a pointer for the ADC


void enable_adc_clock(void);
void init_adc(void);
unsigned int read_adc(void);
void config_adc_port(int port);

// set up generic clock for ADC
void enable_adc_clock(void)
{
	PM->APBCMASK.bit.ADC_ = 0x1;			// enable ADC clock
	
	// allow generic clock 0 to be used by ADC (page 114)
	uint32_t temp1 = 0x17;					// enabling the ADC clock
	temp1 |= 0 << 8;						// select GCLK 0
	GCLK->CLKCTRL.reg = temp1;
	GCLK->CLKCTRL.bit.CLKEN = 0x1;			// enable ADC clock
}



// initialize ADC peripheral
void init_adc(void)
{
	// setup port structure
	Port *ports = PORT_INSTS;
	PortGroup *porA = &(ports->Group[0]);	// setup port A
	PortGroup *porB = &(ports->Group[1]);	// setup port B
	
	// set PB00-PB04 as input for ADC
	porB->DIRCLR.reg = PORT_PB00 | PORT_PB01 | PORT_PB02 | PORT_PB03 | PORT_PB04;

	
	// set PA04 as output for reference voltage
	porA->DIRSET.reg = PORT_PA04;
	porA->OUTSET.reg = PORT_PA04;
	
	// setup ADC peripheral
	adcPtr->CTRLA.reg &= ~(0x2);			// disable ADC peripheral
	
	adcPtr->REFCTRL.bit.REFSEL = 0x4;		// select external reference
	adcPtr->AVGCTRL.reg &= 0x8;				// 1 samples collected
	adcPtr->SAMPCTRL.reg &= 0x0;			// sampling time = CLK/2
	adcPtr->CTRLB.bit.RESSEL = 0;			// 12-bit ADC
	adcPtr->CTRLB.bit.PRESCALER = 0x3;		// ADC CLK = CLK/32
	adcPtr->INPUTCTRL.bit.GAIN = 0xF;		// gain = 1/2 (page 535)
	adcPtr->INPUTCTRL.bit.MUXNEG = 0x18;	// negative input internal GND
	adcPtr->INPUTCTRL.bit.MUXPOS = 0x08;	// positive input PB00 (or AIN8)
	
	adcPtr->CTRLA.reg |= 0x2;				// enable ADC
	
	// configure PA03 to be reference voltage
	porA->PMUX[2].bit.PMUXE = PORT_PMUX_PMUXE_B_Val;
	porA->PINCFG[4].reg = PORT_PINCFG_PMUXEN | PORT_PINCFG_INEN;
	
	// config PB00 to be owned by ADC Peripheral
	porB->PMUX[0].bit.PMUXE = PORT_PMUX_PMUXE_B_Val;		// [2*n] where n = 5
	porB->PINCFG[0].reg = PORT_PINCFG_PMUXEN | PORT_PINCFG_INEN;
	
	porB->PMUX[0].bit.PMUXO = PORT_PMUX_PMUXO_B_Val;		// [2*n] where n = 5
	porB->PINCFG[1].reg = PORT_PINCFG_PMUXEN | PORT_PINCFG_INEN;
	
	porB->PMUX[1].bit.PMUXE = PORT_PMUX_PMUXE_B_Val;		// [2*n] where n = 5
	porB->PINCFG[2].reg = PORT_PINCFG_PMUXEN | PORT_PINCFG_INEN;
	
	porB->PMUX[1].bit.PMUXO = PORT_PMUX_PMUXO_B_Val;		// [2*n] where n = 5
	porB->PINCFG[3].reg = PORT_PINCFG_PMUXEN | PORT_PINCFG_INEN;
	
	porB->PMUX[2].reg = PORT_PMUX_PMUXE_B_Val;		// [2*n] where n = 5
	porB->PINCFG[4].reg = PORT_PINCFG_PMUXEN | PORT_PINCFG_INEN;
}



// allow ADC peripheral to read analog value and output the result (12-bit)
unsigned int read_adc(void)
{
	// start the conversion, see 0x0C in the table in Section 28.7 (pg 534)
	adcPtr->SWTRIG.bit.START = 1;
	
	// wait for conversion to be available
	while(!(adcPtr->INTFLAG.bit.RESRDY));
	
	// insert register where ADC store value
	return(adcPtr->RESULT.reg);
}



// configure GPIO port for each finger
void config_adc_port(int port)
{
	switch (port)
	{
		// configure finger 1 (thumb)
		case 0:
		{
			adcPtr->CTRLA.reg &= ~(0x2);
			while (adcPtr->STATUS.bit.SYNCBUSY & ADC_STATUS_SYNCBUSY) { /* Wait for setup to complete */ }
			
			adcPtr->INPUTCTRL.bit.MUXPOS = 0x08;		// positive input PB01 (or AIN9)
			
			while (adcPtr->STATUS.bit.SYNCBUSY & ADC_STATUS_SYNCBUSY) { /* Wait for setup to complete */ }
			adcPtr->CTRLA.reg |= 0x2;
		}break;
		
		// configure finger 2 (index)
		case 1:
		{
			adcPtr->CTRLA.reg &= ~(0x2);
			while (adcPtr->STATUS.bit.SYNCBUSY & ADC_STATUS_SYNCBUSY) { /* Wait for setup to complete */ }
			
			adcPtr->INPUTCTRL.bit.MUXPOS = 0x09;		// positive input PB01 (or AIN9)
			
			while (adcPtr->STATUS.bit.SYNCBUSY & ADC_STATUS_SYNCBUSY) { /* Wait for setup to complete */ }
			adcPtr->CTRLA.reg |= 0x2;
		}break;
		
		// configure finger 3 (middle)
		case 2:
		{
			adcPtr->CTRLA.reg &= ~(0x2);
			while (adcPtr->STATUS.bit.SYNCBUSY & ADC_STATUS_SYNCBUSY) { /* Wait for setup to complete */ }
			
			adcPtr->INPUTCTRL.bit.MUXPOS = 0x0A;		// positive input PB01 (or AIN9)
			
			while (adcPtr->STATUS.bit.SYNCBUSY & ADC_STATUS_SYNCBUSY) { /* Wait for setup to complete */ }
			adcPtr->CTRLA.reg |= 0x2;
		}break;
		
		// configure finger 4 (ring)
		case 3:
		{
			adcPtr->CTRLA.reg &= ~(0x2);
			while (adcPtr->STATUS.bit.SYNCBUSY & ADC_STATUS_SYNCBUSY) { /* Wait for setup to complete */ }
			
			adcPtr->INPUTCTRL.bit.MUXPOS = 0x0B;		// positive input PB01 (or AIN9)
			
			while (adcPtr->STATUS.bit.SYNCBUSY & ADC_STATUS_SYNCBUSY) { /* Wait for setup to complete */ }
			adcPtr->CTRLA.reg |= 0x2;
		}break;
		
		// configure finger 5 (pinky)
		case 4:
		{
			adcPtr->CTRLA.reg &= ~(0x2);
			while (adcPtr->STATUS.bit.SYNCBUSY & ADC_STATUS_SYNCBUSY) { /* Wait for setup to complete */ }
			
			adcPtr->INPUTCTRL.bit.MUXPOS = 0x0C;		// positive input PB01 (or AIN9)
			
			while (adcPtr->STATUS.bit.SYNCBUSY & ADC_STATUS_SYNCBUSY) { /* Wait for setup to complete */ }
			adcPtr->CTRLA.reg |= 0x2;
		}break;
		
	}
	while (adcPtr->STATUS.bit.SYNCBUSY & ADC_STATUS_SYNCBUSY) { /* Wait for setup to complete */ }
}


