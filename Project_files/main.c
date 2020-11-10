
/*Begining of Auto generated code by Atmel studio */

#include <UTFT.h>
#include <UTouch.h>
#include "LCD_Interface.h"
#include "LCD_Pages.h"

void Timer_1_Setup();


/************************************************************************/
/*		Memory Control And Communication Between Controllers            */
/************************************************************************/
unsigned char Main_Control[20]={0};
//Sell 0  = Battery in percent
//Sell 1  = Battery Voltage value
//Sell 2  = Speedometer real value according to the sensor
//Sell 3  = Speed of MOTOR A Forward (sowens on the Right Side)
//Sell 4  = Speed of MOTOR B Forward (sowens on the Left Side)
//Sell 5  = Speed of MOTOR A Backward (sowens on the Right Side)
//Sell 6  = Speed of MOTOR B Backward (sowens on the Left Side)
//Sell 7  = Back light Control 1 or 0 (ON or OFF)
//Sell 8  = Front light Control 1 or 0 (ON or OFF)
//Sell 9  = Touch control, Enable or Disable (1 or 0)
//Sell 10 = JoyStick Enable or Disable (1 or 0)
//Sell 11 = Send Speed of MOTOR A Forward (Value from touched Screen)
//Sell 12 = Send Speed of MOTOR B Forward (Value from touched Screen)
//Sell 13 =
//Sell 14 =
//Sell 15 =
//Sell 16 =
//Sell 17 =
//Sell 18 =
//Sell 19 = Counter For Timer, Set the Sending rate
/************************************************************************/
/*	   End Of Memory Control And Communication Between Controllers      */
/************************************************************************/




void setup()
{
	 //Setup the LCD
	 myGLCD.InitLCD();
	 myTouch.InitTouch();
	 myTouch.setPrecision(PREC_LOW);
	 myGLCD.setFont(BigFont);
	 myGLCD.clrScr();
	 USART_init();
	 USART_init_3();
	 Timer_1_Setup();
	 Print_Hebrew_String(0,0,0,0);
}

// the loop function runs over and over again forever
void loop() 
{
	   int x,y,i =1;
	   sei();
	   unsigned char arr[] = {"tcdsvuzjyhfknbxgpmera,"};
	   
	   //myGLCD.setColor(0,0,0);
	   //Set_Hebrew_Font_Size(Medium);
	   //scroll_Menu_Bar(100,50,1,4,arr,7);
	   /*
	   for (char i = 0; i <= 10; i++)
	   {
		   Send_num(Main_Control[i]);
	   }
	   */
	  Main_Page();
	   /*
	   Reverse_and_Forward_Sign(30,30,Big,Reverse);
	   _delay_ms(5000);
	   myGLCD.clrScr();
	   Reverse_and_Forward_Sign(30,30,Medium,Forward);
	   _delay_ms(5000);
	   myGLCD.clrScr();
	   Reverse_and_Forward_Sign(30,30,Small,Reverse);
	    _delay_ms(5000);
		myGLCD.clrScr();
		*/
}

//this is interrupt to sign as that new data has been received.
ISR (USART3_RX_vect)
{
	unsigned char JoyStickINPUT = UDR3;
	
    if (JoyStickINPUT >= 0x70)
	{
		Main_Control[6] = JoyStickINPUT - 0x70;
	}
	else if (JoyStickINPUT >= 0x60)
	{
		Main_Control[5] = JoyStickINPUT - 0x60;
	}
	else if (JoyStickINPUT >= 0x40)
	{
		Main_Control[4] = JoyStickINPUT - 0x40;
		//USART_send_3(Main_Control[12]);
	}
	else if (JoyStickINPUT >= 0x30)
	{
		Main_Control[3] = JoyStickINPUT - 0x30;
		//USART_send_3(Main_Control[11]);
	}
	else if (UDR3 & (0 << 4) && UDR3 & (1 << 5) && UDR3 & (0 << 6))
	{
		Main_Control[2] = UDR3 - 0x20;
	}
	else if (UDR3 & (1 << 4) && UDR3 & (0 << 5) && UDR3 & (0 << 6))
	{
		Main_Control[1] = UDR3 - 0x10;
	}
	else if (UDR3 & (0 << 4) && UDR3 & (0 << 5) && UDR3 & (0 << 6))
	{
		Main_Control[0] = UDR3;
	}
	
}
ISR(TIMER2_OVF_vect)
{
	//here we have a counter that count to 255, sins we dont need this loop working so fest,
	//this variable will slow that down
	Main_Control[19]++;
	
	if (Main_Control[19] > 20)
	{
		//sending touched value to the master controller, there he decide who to print
		USART_send_3(Main_Control[5]+ 0x70);
		USART_send_3(Main_Control[6]+ 0x60);
		USART_send_3(Main_Control[11]+(unsigned char)(0x40));
		USART_send_3(Main_Control[12]+(unsigned char)(0x30));
		//setting those variable to zero so we can put the last value in the memory,
		//you can read in the master controller code.
		Main_Control[12] = 0;
		Main_Control[11] = 0;
		
		//set beck to zero, so we will get to the loop in 20 times of counting from 0 to 255 of the timer
		Main_Control[19] = 0;
	}
	
}
void Timer_1_Setup()
{
	// enable timer overflow interrupt for both Timer0 and Timer1
	TIMSK2=(1<<TOIE1);
	
	// lets turn on 16 bit timer1 also with /1024
	TCCR2B |= (1 << CS10) | (1 << CS12);
}



