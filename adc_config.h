/*
 * adc_config.h
 *
 * Created: 11/18/2018 9:38:28 PM
 *  Author: duycu
 */ 


#ifndef ADC_CONFIG_H_
#define ADC_CONFIG_H_

void enable_adc_clock(void);
void init_adc(void);
unsigned int read_adc(void);
void config_adc_port(int port);



#endif /* ADC_CONFIG_H_ */