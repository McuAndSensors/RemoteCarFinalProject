/*
 * UART.h
 *
 * Created: 06/11/2016 06:54:50
 *  Author: Azriel
 */ 


#ifndef UART_H_
#define UART_H_
/************************************************************************/
/*                              UART SETUP                              */
/************************************************************************/

#include <avr/io.h>
#include <util/delay.h>

#define BAUDRATE 9600
#define BAUD_PRESCALLER (((F_CPU / (BAUDRATE * 16UL))) - 1)

void USART_init(void);
unsigned char USART_receive_Data(void);
void USART_send( unsigned char data);
void USART_putstring(unsigned char *StringPtr);
void Verafy_num(unsigned char value);
void Send_num(unsigned char num);
void USART_init_3(void);
unsigned char USART_receive_Data_3(void);

void USART_init_3(void){

	UBRR3H = (uint8_t)(BAUD_PRESCALLER>>8);
	UBRR3L = (uint8_t)(BAUD_PRESCALLER);
	UCSR3B = (1<<RXEN3)|(1<<TXEN3)|(1<<RXCIE3);
	UCSR3C = (3<<UCSZ30);
	sei();
}
void USART_init(void){

	UBRR0H = (uint8_t)(BAUD_PRESCALLER>>8);
	UBRR0L = (uint8_t)(BAUD_PRESCALLER);
	UCSR0B = (1<<RXEN0)|(1<<TXEN0)|(1<<RXCIE0);
	UCSR0C = (3<<UCSZ00);
	sei();
}

unsigned char USART_receive_Data_3(void)
{
	while(!(UCSR3A & (1<<RXC3)));
	return UDR3;
}

unsigned char USART_receive_Data(void)
{
	while(!(UCSR0A & (1<<RXC0)));
	return UDR0;
}

void USART_send( unsigned char data){

	while(!(UCSR0A & (1<<UDRE0)));
	UDR0 = data;

}

void USART_putstring(unsigned char *StringPtr){

	unsigned char i = 0;
	while(i != 0)
	{
		USART_send(StringPtr[i]);
		i++;
	}

}
void Verafy_num(unsigned char value)
{
	switch (value)
	{
		case 0:USART_send('0'); break;
		case 1:USART_send('1'); break;
		case 2:USART_send('2'); break;
		case 3:USART_send('3'); break;
		case 4:USART_send('4'); break;
		case 5:USART_send('5'); break;
		case 6:USART_send('6'); break;
		case 7:USART_send('7'); break;
		case 8:USART_send('8'); break;
		case 9:USART_send('9'); break;
		case 10:USART_send('A'); break;
		case 11:USART_send('B'); break;
		case 12:USART_send('C'); break;
		case 13:USART_send('D'); break;
		case 14:USART_send('E'); break;
		case 15:USART_send('F'); break;
	}
}

void Send_int_decimal(unsigned int Num)
{
	Verafy_num(Num / 1000);
	Verafy_num(Num % 1000 / 100);
	Verafy_num(Num % 100 / 10);
	Verafy_num(Num % 10);
	USART_send(',');
	USART_send(' ');
}
void Send_num(unsigned char num)
{
	Verafy_num(num/16);
	Verafy_num(num%16);
	USART_send(',');
	USART_send(' ');
}


/************************************************************************/
/*                          END OF UART SETUP                           */
/************************************************************************/




#endif /* UART_H_ */