#define F_CPU 16000000UL //12MHz connected to 328P MCU(Micro Controller)

#define  Left_and_Right			4   //this is analog pin number 4
#define  Forward_and_backward	5	//this is analog pin number 5
#define  Forward				3   //This will approve forward
#define  Backward				2	//This will approve backward
#define  Left					1	//This will approve Left
#define  Right					0	//This will approve Right

//Define port direction for motor controller of PWM, each pin worth direction
//when Forward = '1', backward = '0' motor go forward
//when Forward = '0', backward = '1' motor go forward
//when Forward = '0', backward = '0' motor break
//Forward = '1', backward = '1' not allowed

#define  Port_Direction_Right		DDRC
#define  Port_Direction_Forward		DDRC
#define  Port_Direction_Left		DDRC
#define  Port_Direction_Back		DDRC

//Define port value for motor controller of PWM, each pin worth direction

#define  Port_Right					PORTC
#define  Port_Forward				PORTC
#define  Port_Left					PORTC
#define  Port_Back					PORTC

/************************************************************************/
/*                              UART SETUP                              */
/************************************************************************/
/*
#include <avr/io.h>
#include <util/delay.h>

#define BAUDRATE 38400
#define BAUD_PRESCALLER (((F_CPU / (BAUDRATE * 16UL))) - 1)



void USART_init(void){

	UBRR0H = (uint8_t)(BAUD_PRESCALLER>>8);
	UBRR0L = (uint8_t)(BAUD_PRESCALLER);
	UCSR0B = (1<<RXEN0)|(1<<TXEN0);
	UCSR0C = (3<<UCSZ00);
}

unsigned char USART_receive(void){

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

*/
/************************************************************************/
/*                          END OF UART SETUP                           */
/************************************************************************/
unsigned int Get_AD_Value(unsigned char ch);
void A_to_D_setup();
unsigned char Get_speed_num_Forword_Backward();
unsigned char Get_speed_num_Left_Right();
void PWM_Setup();
void Read_Speed_and_send_to_Motor_Left_Right(unsigned char speed_value_Left_Right);
void Read_Speed_and_send_to_Motor_Forword_Backward(unsigned char speed_value_Forword_Backward);

/*
int main(void)
{
	unsigned int value_AD;
	USART_init();        //Call the USART initialization code
	A_to_D_setup();
	
	while(1)
	{
		value_AD = Get_speed_num_Left_Right();
		//if(ADC > 500)
		Send_int_decimal(value_AD);
		_delay_ms(50);
	}
	
	return 0;
}
*/

/*******************************A_to_D_setup*****************************/
/*This function configuring the the convention of the analog voltage to */
/*the digital value, because we must use frequency that is less then    */
/*200KHz so i divide it by 128. in the control status register i active */
/*the converter then and set the frequency in the 3 LSB of the register */
/************************************************************************/
void A_to_D_setup()
{
	
	//set the PWM mode
	void PWM_Setup();
	
		//define all control pins as output
	
	Port_Direction_Back		|= 1 << Backward;
	Port_Direction_Forward	|= 1 << Forward;
	Port_Direction_Left		|= 1 << Left;
	Port_Direction_Right	|= 1 << Right;
	
	// AREF = AVcc
	ADMUX = (1<<REFS0);
	
	// ADC Enable and prescaler of 128
	// 16000000/128 = 125000
	ADCSRA = (1<<ADEN)|(1<<ADPS2)|(1<<ADPS1)|(1<<ADPS0);
	
}


/********************************Get_AD_Value****************************/
/*in this function i choose what pin i like to convert the voltage from */
/*with variable "ch" and then start the convention by Bit ADSC Analog   */
/*Digital Start Convention and then i wait for the convention to be     */
/*completed, in the end i return the value of both registers.           */
/************************************************************************/
unsigned int Get_AD_Value(unsigned char ch)
{
	
	// select the corresponding channel 0~7
	// ANDing with ’7? will always keep the value
	// of ‘ch’ between 0 and 7
	ch &= 0b00000111;  // AND operation with 7
	ADMUX = (ADMUX & 0xF8)|ch; // clears the bottom 3 bits before ORing
	
	// start single conversion
	// write ’1? to ADSC
	ADCSRA |= (1<<ADSC);
	
	// wait for conversion to complete
	// ADSC becomes ’0? again
	// till then, run loop continuously
	while(ADCSRA & (1<<ADSC));
	
	return (ADC);
}


/**********************Get_speed_num_Forword_Backward********************/
/*This Function check the value of the AD conversion and convert it to  */
/*convert it to a lower number between 0-6 while 0 is no drive and      */
/*numbers 1-6 worth the speed of the motor while 1 is the slowest and   */
/*6 is the highest. number over 10 that mean that the motor will drive  */
/*backward, num 1-6 stay the same. 6 = max. 1 = min.                    */
/************************************************************************/

unsigned char Get_speed_num_Forword_Backward()
{
	//variable to get the value which is bigger then 255
	unsigned int Speed_value;
	
	//call function of converting A to D on F/B pin
	Speed_value = Get_AD_Value(Forward_and_backward);
	
	if(Speed_value >= 490 && Speed_value <= 520)
	{
		//Put in break condition
		Port_Forward	&= ~(1 << Forward); //Clear Bit
		Port_Back		&= ~(1 << Backward); //Clear Bit
		
		return 0; //don't drive
	}
	else if(Speed_value >= 520 && Speed_value <= 600)
	{
		//Put in Forward condition
		Port_Forward	|= 1 << Forward;
		Port_Back		&= ~(1 << Backward);  //Clear Bit
		
		return 1; //first level forward
	}
	else if(Speed_value >= 600 && Speed_value <= 700)
	{
		//Put in Forward condition
		Port_Forward	|= 1 << Forward;
		Port_Back		&= ~(1 << Backward);  //Clear Bit
		
		return 2; //second level forward
	}
	else if(Speed_value >= 700 && Speed_value <= 800)
	{
		//Put in Forward condition
		Port_Forward	|= 1 << Forward;
		Port_Back		&= ~(1 << Backward);  //Clear Bit
		
		return 3; //third level forward
	}
	else if(Speed_value >= 800 && Speed_value <= 900)
	{
		//Put in Forward condition
		Port_Forward	|= 1 << Forward;
		Port_Back		&= ~(1 << Backward);  //Clear Bit
		
		return 4; //fourth level forward
	}
	else if(Speed_value >= 900 && Speed_value <= 980)
	{
		//Put in Forward condition
		Port_Forward	|= 1 << Forward;
		Port_Back		&= ~(1 << Backward);  //Clear Bit
		
		return 5; //fifth level forward
	}
	else if(Speed_value >= 980 && Speed_value <= 1024)
	{
		//Put in Forward condition
		Port_Forward	|= 1 << Forward;
		Port_Back		&= ~(1 << Backward);  //Clear Bit
		
		return 6; //sixth level forward
	}
	//now backward will be checked
	else if(Speed_value >= 420 && Speed_value <= 520)
	{
		//Put in Backward condition
		Port_Forward	&= ~(1 << Forward);  //Clear Bit
		Port_Back		|= 1 << Backward;
		
		return 11; //first level backward
	}
	else if(Speed_value >= 320 && Speed_value <= 420)
	{
		//Put in Backward condition
		Port_Forward	&= ~(1 << Forward);  //Clear Bit
		Port_Back		|= 1 << Backward;
		
		return 12; //second level backward
	}
	else if(Speed_value >= 220 && Speed_value <= 320)
	{
		//Put in Backward condition
		Port_Forward	&= ~(1 << Forward);  //Clear Bit
		Port_Back		|= 1 << Backward;
		
		return 13; //third level backward
	}
	else if(Speed_value >= 120 && Speed_value <= 220)
	{
		//Put in Backward condition
		Port_Forward	&= ~(1 << Forward);  //Clear Bit
		Port_Back		|= 1 << Backward;
		
		return 14; //fourth level backward
	}
	else if(Speed_value >= 70 && Speed_value <= 120)
	{
		//Put in Backward condition
		Port_Forward	&= ~(1 << Forward);  //Clear Bit
		Port_Back		|= 1 << Backward;
		
		return 15; //fifth level backward
	}
	else if(Speed_value >= 0 && Speed_value <= 70)
	{
		//Put in Backward condition
		Port_Forward	&= ~(1 << Forward);  //Clear Bit
		Port_Back		|= 1 << Backward;
		
		return 16; //sixth level backward
	}
	return 0;
}

/***********************Get_speed_num_Left_Right*************************/
/*This Function check the value of the AD conversion and convert it to  */
/*convert it to a lower number between 0-6 while 0 is no drive and      */
/*numbers 1-6 worth the speed of the motor while 1 is the slowest and   */
/*6 is the highest. number over 10 in the return function that mean     */
/*that the motor will turn left, num 1-6 turn right. 6 = max. 1 = min.  */
/************************************************************************/

unsigned char Get_speed_num_Left_Right()
{
	//variable to get the value which is bigger then 255
	//the value is between 0-1024 that the reason i use int variable
	unsigned int Speed_value;
	
	//call function of converting A to D on Left/Right pin
	Speed_value = Get_AD_Value(Left_and_Right);
	
	if(Speed_value >= 490 && Speed_value <= 520)
	{
		//Put in Break condition
		Port_Left	&= ~(1 << Left); //Clear Bit
		Port_Right	&= ~(1 << Right); //Clear Bit
		
		return 0; //don't turn left or right
	}
	else if(Speed_value >= 520 && Speed_value <= 600)
	{
		//Put in Right condition
		Port_Left	&= ~(1 << Left); //Clear Bit
		Port_Right	|= 1 << Right;
		
		return 1; //first level right
	}
	else if(Speed_value >= 600 && Speed_value <= 700)
	{
		//Put in Right condition
		Port_Left	&= ~(1 << Left); //Clear Bit
		Port_Right	|= 1 << Right;
		
		return 2; //second level right
	}
	else if(Speed_value >= 700 && Speed_value <= 800)
	{
		//Put in Right condition
		Port_Left	&= ~(1 << Left); //Clear Bit
		Port_Right	|= 1 << Right;
		
		return 3; //third level right
	}
	else if(Speed_value >= 800 && Speed_value <= 900)
	{
		//Put in Right condition
		Port_Left	&= ~(1 << Left); //Clear Bit
		Port_Right	|= 1 << Right;
		
		return 4; //fourth level right
	}
	else if(Speed_value >= 900 && Speed_value <= 980)
	{
		//Put in Right condition
		Port_Left	&= ~(1 << Left); //Clear Bit
		Port_Right	|= 1 << Right;
		
		return 5; //fifth level right
	}
	else if(Speed_value >= 980 && Speed_value <= 1024)
	{
		//Put in Right condition
		Port_Left	&= ~(1 << Left); //Clear Bit
		Port_Right	|= 1 << Right;
		
		return 6; //sixth level right
	}
	//now left will be checked
	else if(Speed_value >= 420 && Speed_value <= 520)
	{
		//Put in Left condition
		Port_Left	|= 1 << Left;
		Port_Right	&= ~(1 << Right); //Clear Bit
		
		return 11; //first level left
	}
	else if(Speed_value >= 320 && Speed_value <= 420)
	{
		//Put in Left condition
		Port_Left	|= 1 << Left;
		Port_Right	&= ~(1 << Right); //Clear Bit
		
		return 12; //second level left
	}
	else if(Speed_value >= 220 && Speed_value <= 320)
	{
		//Put in Left condition
		Port_Left	|= 1 << Left;
		Port_Right	&= ~(1 << Right);  //Clear Bit
		
		return 13; //third level left
	}
	else if(Speed_value >= 120 && Speed_value <= 220)
	{
		//Put in Left condition
		Port_Left	|= 1 << Left;
		Port_Right	&= ~(1 << Right);  //Clear Bit
		
		return 14; //fourth level left
	}
	else if(Speed_value >= 70 && Speed_value <= 120)
	{
		//Put in Left condition
		Port_Left	|= 1 << Left;
		Port_Right	&= ~(1 << Right);  //Clear Bit
		
		return 15; //fifth level left
	}
	else if(Speed_value >= 0 && Speed_value <= 70)
	{
		//Put in Left condition
		Port_Left	|= 1 << Left;
		Port_Right	&= ~(1 << Right);  //Clear Bit
		
		return 16; //sixth level left
	}
	return 0;
}

/*******************************PWM_Setup********************************/
/*This Function set the PWM mode, in this mode we have one counter and  */
/*two register to compare the value because we have 2 motors. each of   */
/*PWM generate difrent Duty Cycle. the value of the joystick will       */
/*determent the value of the Duty Cycle. more information is in the book*/
/*in sobject of Timers/counters.										*/
/************************************************************************/


void PWM_Setup()
{
	//set pin 5 and 6 as Output, they will generate the PWM
	DDRD = (1 << PORTD5 | 1 << PORTD6);

	//define compare of register A & B on mode 3
	//mode 3 is setting the bit while up counting and clear the bit in down counting
	TCCR0A = (1 << COM0A1 | 1 << COM0B1 | 1 << WGM00 | 1 << COM0B0 | 1 << COM0A0);

	//make prscaler of clk by 2 Fclk/2
	TCCR0B = 1 << CS00;

	//clear all interrupts
	TIFR0 = 7;

	//setting register A as 255 for 0V
	OCR0A = 255;

	//setting register A as 255 for 0V
	OCR0B = 255;
}

void Read_Speed_and_send_to_Motor_Forword_Backward(unsigned char speed_value_Forword_Backward)
{
	
	switch(speed_value_Forword_Backward)
	{
		case  0: OCR0A = 255; break;
		case  1: OCR0A = 199; break;
		case  2: OCR0A = 156; break;
		case  3: OCR0A = 116; break;
		case  4: OCR0A =  75; break;
		case  5: OCR0A =  43; break;
		case  6: OCR0A =   0; break;
		case 11: OCR0A = 199; break;
		case 12: OCR0A = 156; break;
		case 13: OCR0A = 116; break;
		case 14: OCR0A =  75; break;
		case 15: OCR0A =  43; break;
		case 16: OCR0A =   0; break;
	}
}

void Read_Speed_and_send_to_Motor_Left_Right(unsigned char speed_value_Left_Right)
{
	
	switch(speed_value_Left_Right)
	{
		case  0: OCR0B = 255; break;
		case  1: OCR0B = 199; break;
		case  2: OCR0B = 156; break;
		case  3: OCR0B = 116; break;
		case  4: OCR0B =  75; break;
		case  5: OCR0B =  43; break;
		case  6: OCR0B =   0; break;
		case 11: OCR0B = 199; break;
		case 12: OCR0B = 156; break;
		case 13: OCR0B = 116; break;
		case 14: OCR0B =  75; break;
		case 15: OCR0B =  43; break;
		case 16: OCR0B =   0; break;
	}
}
