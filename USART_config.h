/*
 * USART_config.h
 *
 * Created: 11/23/2018 12:31:58 PM
 *  Author: duycu
 */ 


#ifndef USART_CONFIG_H_
#define USART_CONFIG_H_


void usart_write_callback(struct usart_module *const usart_module);
void configure_usart(void);
void configure_usart_callbacks(void);
void system_interrupt_enable_global(void);


#endif /* USART_CONFIG_H_ */