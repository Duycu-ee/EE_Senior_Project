/*
 * TC6_config.c
 *
 * Created: 11/18/2018 9:45:40 PM
 *  Author: duycu
 */
#include <asf.h>

#include "adc_config.h"


// global variables for flex sensors
extern int finger_array_pointer;			// placeholder for the array data
extern unsigned int finger1_data[];			// finger 1 data
extern unsigned int finger2_data[];			// finger 2 data
extern unsigned int finger3_data[];			// finger 3 data
extern unsigned int finger4_data[];			// finger 4 data
extern unsigned int finger5_data[];			// finger 5 data



// enable TC4 and TC5 with generic clock 0
void enable_tc_clock(void)
{
	Tc *tc = TC6;
	TcCount16 *tcPtr6 = &(tc->COUNT16);
	
	// selecting TC6 clock and enable
	PM->APBCMASK.bit.TC6_ = 1;						// enable TC4 peripheral on PM
	uint32_t temp3 = 0x16;							// enabling the TC4 clock for GCLK (page 116)
	temp3 |= 0 << 8;								// select GCLK 0
	GCLK->CLKCTRL.reg = temp3;
	GCLK->CLKCTRL.bit.CLKEN = 0x1;					// enable generic clock clock
}



// initialize TC6 16-bit mode peripheral (used for system timer)
void init_tc6(void)
{
	Tc *tc = TC6;
	TcCount16 *tcPtr6 = &(tc->COUNT16);
	
	tcPtr6->CTRLA.reg &= ~(0x2);				// disable TC6 peripheral

	tcPtr6->CTRLA.bit.RUNSTDBY = 1;
	tcPtr6->READREQ.bit.RCONT = 1;				// enable continuous sync.
	tcPtr6->CTRLA.bit.MODE = 0x0;				// 16-bit mode
	tcPtr6->CTRLA.bit.WAVEGEN = 0x3;			// match PWM (page 490)
	tcPtr6->CTRLA.bit.PRESCALER = 0x5;			// TC4 clock = 8MHz/64 = 125kHz
	tcPtr6->CTRLA.bit.PRESCSYNC = 0x1;			// counter run on prescale clock
	tcPtr6->CTRLBCLR.bit.DIR = 1;				// count increment
	
	tcPtr6->CC[0].reg = 1234;					// 125 kHz / 200 Hz = 1250 (time = 0.01)
	tcPtr6->INTENSET.bit.MC0 = 1;				// enable interrupt when match for CC[0]
	
	NVIC->ISER[0] |= 1u << 19;
	NVIC->IP[4] |= 0xC0000000;						// priority level 1

	tcPtr6->CTRLA.reg |= 0x2;					// enable TC4 peripheral
}



void TC6_Handler()
{
	TcCount16 *tcPtr6 = &(TC6->COUNT16);			// define a pointer for TC6
	//Port *por = PORT_INSTS;
	//PortGroup *porA = &(por->Group[0]);
	//porA->OUTTGL.reg = PORT_PA07;
	
	if (finger_array_pointer < 200)
	{
		for (int i = 0; i < 5; i++)
		{
			config_adc_port(i);
			
			switch (i)
			{
				case (0):
				{
					finger1_data[finger_array_pointer] = read_adc();
				}break;
				
				case (1):
				{
					finger2_data[finger_array_pointer] = read_adc();
				}break;
				
				case (2):
				{
					finger3_data[finger_array_pointer] = read_adc();
				}break;
				
				case (3):
				{
					finger4_data[finger_array_pointer] = read_adc();
				}break;
				
				case (4):
				{
					finger5_data[finger_array_pointer] = read_adc();
				}break;
			}
		}
		
		finger_array_pointer++;
	}
	else
	{
		
	}
	tcPtr6->COUNT.reg = 0;
	tcPtr6->INTFLAG.bit.MC0 = 1;				// reset interrupt flag
}

