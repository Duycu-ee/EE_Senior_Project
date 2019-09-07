//////////////////////////////////////////////////////////////////////////////////////
////
////						~~~~ SENIOR PROJECT ~~~~
////				* ADC for flex sensors (12-bit)
////					- reads flex every 5 mili-sec interval
////					- total of 2.5 sec (0.0005 * 500)
////				* Digital read for accelerometer (12-bit)
////				* Store data into memory bank
////				* Process data to match expected result
////				* DAC for audio output
////
//////////////////////////////////////////////////////////////////////////////////////

// Author:
// C0-Author:
// Date:



#include <asf.h>
#include <stdio.h>
#include "adc_config.h"
#include "Tc6_config.h"
#include "USART_config.h"
#include "stored_data.h"

extern struct usart_module usart_instance;


// default system declarations
void wait(int t);
void Simple_Clk_Init(void);

void print(void);
void sort(void);

int result = false;
int finger_pointer = 0;
int finger_array_pointer = 0;			// placeholder for the array data
unsigned int finger1_data[200];			// finger 1 data
unsigned int finger2_data[200];			// finger 2 data
unsigned int finger3_data[200];			// finger 3 data
unsigned int finger4_data[200];			// finger 4 data
unsigned int finger5_data[200];			// finger 5 data


int dummy = 0;
int displayed = false;
int compare_finger[5] = {0, 0, 0, 0, 0};

// [0] -> "No"
// [1] -> "Hard-of-hearing"
// [2] -> "Yes"
// [3] -> "Hello"
// [4] -> "TY" 
int total_handgesture_data = 5;
unsigned int compare[5] = {0, 0, 0, 0, 0};				// used for comparing "No"
int compare_result[5] = {0, 1, 2, 3, 4};


// Hand gesture: "No" Data for comparison
extern unsigned int data_finger1_no[200];
extern unsigned int data_finger2_no[200];
extern unsigned int data_finger3_no[200];
extern unsigned int data_finger4_no[200];
extern unsigned int data_finger5_no[200];
	
// Hand gesture: "Hard-of-hearing" Data for comparison
extern unsigned int data_finger1_hoh[200];
extern unsigned int data_finger2_hoh[200];
extern unsigned int data_finger3_hoh[200];
extern unsigned int data_finger4_hoh[200];
extern unsigned int data_finger5_hoh[200];

// Hand gesture: "Yes" Data for comparison
extern unsigned int data_finger1_yes[200];
extern unsigned int data_finger2_yes[200];
extern unsigned int data_finger3_yes[200];
extern unsigned int data_finger4_yes[200];
extern unsigned int data_finger5_yes[200];

// Hand gesture: "Hello" Data for comparison
extern unsigned int data_finger1_hello[200];
extern unsigned int data_finger2_hello[200];
extern unsigned int data_finger3_hello[200];
extern unsigned int data_finger4_hello[200];
extern unsigned int data_finger5_hello[200];

// Hand gesture: "TY" Data for comparison
extern unsigned int data_finger1_ty[200];
extern unsigned int data_finger2_ty[200];
extern unsigned int data_finger3_ty[200];
extern unsigned int data_finger4_ty[200];
extern unsigned int data_finger5_ty[200];



// print a message onto the terminal
void print(void)
{
	switch (compare_result[0])
	{
		case 0:
		{
			uint8_t string_display[] = "No\r\n";
			usart_write_buffer_wait(&usart_instance, string_display, sizeof(string_display));
		}break;
		case 1:
		{
			uint8_t string_display[] = "Hard-of-hearing\r\n";
			usart_write_buffer_wait(&usart_instance, string_display, sizeof(string_display));
		}break;
		case 2:
		{
			uint8_t string_display[] = "Yes\r\n";
			usart_write_buffer_wait(&usart_instance, string_display, sizeof(string_display));
		}break;
		case 3:
		{
			uint8_t string_display[] = "Hello\r\n";
			usart_write_buffer_wait(&usart_instance, string_display, sizeof(string_display));
		}break;
		case 4:
		{
			uint8_t string_display[] = "Thank you!\r\n";
			usart_write_buffer_wait(&usart_instance, string_display, sizeof(string_display));
		}break;
	}
	displayed = true;
}



// sort compare array and gives the least error values at compare_result[0]
void sort(void)
{
	for (int i = 0; i < (total_handgesture_data - 1); i++)
	{
		unsigned int temp;
		unsigned int smallest = compare[i];
		unsigned int smallest_index = i;
		
		for (int j = i + 1; j < total_handgesture_data; j++)
		{
			if (compare[j] < smallest)
			{
				smallest = compare[j];
				smallest_index = j;
			}
			
		}
		
		temp = compare[i];
		compare[i] = compare[smallest_index];
		compare[smallest_index] = temp;
		
		temp = compare_result[i];
		compare_result[i] = compare_result[smallest_index];
		compare_result[smallest_index] = temp;
	}
}



/*//////////////////////////////////////////////////////////////
/////////////////     MAIN FUNCTION     ////////////////////////
*///////////////////////////////////////////////////////////////
int main(void)
{
	//system_init();
	
	Simple_Clk_Init();
	enable_adc_clock();
	enable_tc_clock();
	configure_usart();
	
	init_adc();
	init_tc6();
	configure_usart_callbacks();
	system_interrupt_enable_global();


	Port *por = PORT_INSTS;
	PortGroup *porA = &(por->Group[0]);
	//porA->DIRSET.reg = PORT_PA07;
	porA->DIRSET.reg = PORT_PA14;
	porA->OUTSET.reg = PORT_PA14;
	porA->DIRSET.reg = PORT_PA15;
	porA->OUTSET.reg = PORT_PA15;
	porA->PINCFG[15].reg = PORT_PINCFG_INEN;
	


//! [main_send_string]
	uint8_t string[] = "Hello SJSU!\r\n";
	usart_write_buffer_wait(&usart_instance, string, sizeof(string));
//! [main_recieve_string]	
	//usart_read_buffer_job(&usart_instance,
	//(uint8_t *)rx_buffer, MAX_RX_BUFFER_LENGTH);
	
	int test = 1 << 15;
	int start = false;

//! [main_loop]
	while (1) 
	{	
		// detect once data array captures 500 array
		if (finger_array_pointer == 200)
		{
			// blink LED to notify user it finish capture data array
			porA->OUTTGL.reg = PORT_PA14;
			wait(200);
			
			
			
			// compare data array and identify which hand gesture it is
			if (result == false)
			{
				for (int compare_index = 0; compare_index < total_handgesture_data; compare_index++)
				{
					// reset all values and ready for next compare & accumulate
					for (int i = 0; i < 5; i++)
					{
						compare_finger[i] = 0;
					}
					for (int finger_index = 0; finger_index < 5; finger_index++)
					{
						for (int array_index = 0; array_index < 200; array_index++)
						{
							
							// comparing input data to "No" data
							if (compare_index == 0)
							{
								switch (finger_index)
								{
									case 0:
									{
										compare_finger[0] += abs(finger1_data[array_index] - data_finger1_no[array_index])*1.25;
									}break;
									case 1:
									{
										compare_finger[1] += abs(finger2_data[array_index] - data_finger2_no[array_index])*1.64;
									}break;
									case 2:
									{
										compare_finger[2] += abs(finger3_data[array_index] - data_finger3_no[array_index])*1.35;
									}break;
									case 3:
									{
										compare_finger[3] += abs(finger4_data[array_index] - data_finger4_no[array_index]);
									}break;
									case 4:
									{
										compare_finger[4] += abs(finger5_data[array_index] - data_finger5_no[array_index])*1.2;
									}break;
								}
								
								// cumulate error values into one
								if ((finger_index == 4) && (array_index == 199))
								{
									
									for (int i = 0; i < 5; i++)
									{
										compare[0] += compare_finger[i];
									}
								}
							}
							
							
							// comparing input data to "Hard-of-hearing" data
							if (compare_index == 1)
							{
								switch (finger_index)
								{
									case 0:
									{
										compare_finger[0] += abs(finger1_data[array_index] - data_finger1_hoh[array_index])*1.25;
									}break;
									case 1:
									{
										compare_finger[1] += abs(finger2_data[array_index] - data_finger2_hoh[array_index])*1.64;
									}break;
									case 2:
									{
										compare_finger[2] += abs(finger3_data[array_index] - data_finger3_hoh[array_index])*1.35;
									}break;
									case 3:
									{
										compare_finger[3] += abs(finger4_data[array_index] - data_finger4_hoh[array_index]);
									}break;
									case 4:
									{
										compare_finger[4] += abs(finger5_data[array_index] - data_finger5_hoh[array_index])*1.2;
									}break;
								}
								
								// cumulate error values into one
								if ((finger_index == 4) && (array_index == 199))
								{
									
									for (int i = 0; i < 5; i++)
									{
										compare[1] += compare_finger[i];
									}
								}
							}
							
							
							// comparing input data to "Yes" data
							if (compare_index == 2)
							{
								switch (finger_index)
								{
									case 0:
									{
										compare_finger[0] += abs(finger1_data[array_index] - data_finger1_yes[array_index])*1.25;
									}break;
									case 1:
									{
										compare_finger[1] += abs(finger2_data[array_index] - data_finger2_yes[array_index])*1.64;
									}break;
									case 2:
									{
										compare_finger[2] += abs(finger3_data[array_index] - data_finger3_yes[array_index])*1.35;
									}break;
									case 3:
									{
										compare_finger[3] += abs(finger4_data[array_index] - data_finger4_yes[array_index]);
									}break;
									case 4:
									{
										compare_finger[4] += abs(finger5_data[array_index] - data_finger5_yes[array_index])*1.2;
									}break;
								}
								
								// cumulate error values into one
								if ((finger_index == 4) && (array_index == 199))
								{
									
									for (int i = 0; i < 5; i++)
									{
										compare[2] += compare_finger[i];
									}
								}
							}
							
							// comparing input data to "Hello" data
							if (compare_index == 3)
							{
								switch (finger_index)
								{
									case 0:
									{
										compare_finger[0] += abs(finger1_data[array_index] - data_finger1_hello[array_index])*1.25;
									}break;
									case 1:
									{
										compare_finger[1] += abs(finger2_data[array_index] - data_finger2_hello[array_index])*1.64;
									}break;
									case 2:
									{
										compare_finger[2] += abs(finger3_data[array_index] - data_finger3_hello[array_index])*1.35;
									}break;
									case 3:
									{
										compare_finger[3] += abs(finger4_data[array_index] - data_finger4_hello[array_index]);
									}break;
									case 4:
									{
										compare_finger[4] += abs(finger5_data[array_index] - data_finger5_hello[array_index])*1.2;
									}break;
								}
								
								// cumulate error values into one
								if ((finger_index == 4) && (array_index == 199))
								{
									
									for (int i = 0; i < 5; i++)
									{
										compare[3] += compare_finger[i];
									}
								}
							}
							
							// comparing input data to "TY" data
							if (compare_index == 4)
							{
								switch (finger_index)
								{
									case 0:
									{
										compare_finger[0] += abs(finger1_data[array_index] - data_finger1_ty[array_index])*1.25;
									}break;
									case 1:
									{
										compare_finger[1] += abs(finger2_data[array_index] - data_finger2_ty[array_index])*1.64;
									}break;
									case 2:
									{
										compare_finger[2] += abs(finger3_data[array_index] - data_finger3_ty[array_index])*1.35;
									}break;
									case 3:
									{
										compare_finger[3] += abs(finger4_data[array_index] - data_finger4_ty[array_index]);
									}break;
									case 4:
									{
										compare_finger[4] += abs(finger5_data[array_index] - data_finger5_ty[array_index])*1.2;
									}break;
								}
								
								// cumulate error values into one
								if ((finger_index == 4) && (array_index == 199))
								{
									
									for (int i = 0; i < 5; i++)
									{
										compare[4] += compare_finger[i];
									}
								}
							}
						}
					}
				}
				sort();						
				result = true;
				
				// display the best result once
				if ((result == true) && (displayed == false) && (start == true))
				{
					print();
				}
			}		
		}
		
		// display "processing..." while collecting data
		else
		{
			uint8_t string_display[] = "Processing...\r\n";
			usart_write_buffer_wait(&usart_instance, string_display, sizeof(string_display));
			wait(850);
		}
		
		
		
		// setup SW0 to reset data array
		if (!(porA->IN.reg & test))
		{
			start = true;
			displayed = false;
			result = false;
			porA->OUTSET.reg = PORT_PA14;
			for (int i = 0; i < total_handgesture_data; i++)
			{
				// reset all compare values
				compare[i] = 0;				
				compare_result[i] = i;
			}
			finger_array_pointer = 0;
		}
		
	}
}



// wait function to delay clock (mili-second)
void wait(int t)
{
	volatile int count = 0;
	while (count < t*1000)
	{
		count++;
	}
}


	
//Simple Clock Initialization
void Simple_Clk_Init(void)
{
	/* Various bits in the INTFLAG register can be set to one at startup.
	   This will ensure that these bits are cleared */
	
	SYSCTRL->INTFLAG.reg = SYSCTRL_INTFLAG_BOD33RDY | SYSCTRL_INTFLAG_BOD33DET |
			SYSCTRL_INTFLAG_DFLLRDY;
			
	system_flash_set_waitstates(0);  		//Clock_flash wait state = 0
	
	SYSCTRL_OSC8M_Type temp = SYSCTRL->OSC8M;      	/* for OSC8M initialization  */

	temp.bit.PRESC    = 0;    			// no divide, i.e., set clock=8Mhz  (see page 170)
	temp.bit.ONDEMAND = 1;    			//  On-demand is true
	temp.bit.RUNSTDBY = 0;    			//  Standby is false
	
	SYSCTRL->OSC8M = temp;

	SYSCTRL->OSC8M.reg |= 0x1u << 1;  		// SYSCTRL_OSC8M_ENABLE bit = bit-1 (page 170)
	
	PM->CPUSEL.reg = (uint32_t)0;    		// CPU and BUS clocks Divide by 1  (see page 110)
	PM->APBASEL.reg = (uint32_t)0;     		// APBA clock 0= Divide by 1  (see page 110)
	PM->APBBSEL.reg = (uint32_t)0;     		// APBB clock 0= Divide by 1  (see page 110)
	PM->APBCSEL.reg = (uint32_t)0;     		// APBC clock 0= Divide by 1  (see page 110)

	PM->APBAMASK.reg |= 01u<<3;   			// Enable Generic clock controller clock (page 127)
	

	/* Software reset Generic clock to ensure it is re-initialized correctly */
	GCLK->CTRL.reg = 0x1u << 0;   		// Reset gen. clock (see page 94)
	while (GCLK->CTRL.reg & 0x1u ) {  /* Wait for reset to complete */ }
	
	// Initialization and enable generic clock #0
	*((uint8_t*)&GCLK->GENDIV.reg) = 0;  	// Select GCLK0 (page 104, Table 14-10)

	GCLK->GENDIV.reg  = 0x0100;   		// Divide by 1 for GCLK #0 (page 104)
	GCLK->GENCTRL.reg = 0x030600;  		// GCLK#0 enable, Source=6(OSC8M), IDC=1 (page 101)
}