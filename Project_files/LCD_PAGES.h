/*
 * LCD_Pages.h
 *
 * Created: 25/11/2016 11:34:28
 *  Author: Azriel
 */ 

//NOTE *1
/*
Here we add one, because we always set his value to zero right after sending, and also when
we get to opposite condition, those place are signed with "Reset LCD Send Value" at the beginning
of if condition. and now we don't know when it is really zero when we receiving the data in the master.
so we adding one here, and decrees it when receiving on the master controller, so now we know, when we get one
value, the user pressed on 0, and when we get 0, it is non pressed at all and we continue printing from memory.
more about memory. you can read in the master controller code. (in general, 1=Speed 0, 0=NULL)
*/

#ifndef LCD_PAGES_H_
#define LCD_PAGES_H_

#include <UTFT.h>
#include <util/delay.h>
#include <math.h>
#include <UTouch.h>
#include "UART.h"
#include "LCD_Interface.h"

extern unsigned char Main_Control[];

void Main_Page();
void Motor_Control_Page();
void Page_Of_Setting();
void Print_Motor_Speed_By_Touch(unsigned char Speed, unsigned char Speed1);


void Main_Page()
{
	word x,y;
	
	//set arr value for the main menu
	//the value of the arr is in GEMATRIA, while 0x30 = sapce
	word Main_Page_arr_1[]={400,80,200,10,9,0x30,200,1,300,10};		//(תפריט ראשי (10
	word Main_Page_arr_2[]={5,3,4,200,6,400};						//( הגדרות (6
	word Main_Page_arr_3[]={2,100,200,400,0x30,40,50,6,70,10,41};	//(בקרת מנועים (11
	word Main_Page_arr_4[]={40,90,2,0x30,1,9,6,50,6,40,10};		    //(מצב אוטונומי (11
	word Main_Page_arr_5[]={3,10,0x30,80,10,0x30,1,60};			    //( גי פי אס (8
	word Main_Page_arr_6[]={40,90,30,40,5};						    //( מצלמה (5
	
	//clear screen before printing
	myGLCD.clrScr();
	
	//Print battery value according to memory value
	myGLCD.setColor(46,192,100);
	Battery(Main_Control[0]);
	
	myGLCD.setColor(255,0,0);
	myGLCD.drawHLine(40,40,400);
	
	//3 Left Buttons
	Create_button(200,80,30,50,255,255,255);
	Create_button(200,80,30,140,255,255,255);
	Create_button(200,80,30,230,255,255,255);
	
	//3 Right Buttons
	Create_button(200,80,250,50,255,255,255);
	Create_button(200,80,250,140,255,255,255);
	Create_button(200,80,250,230,255,255,255);
	
	myGLCD.setColor(64,80,90);
	Set_Hebrew_Font_Size(Big);
	Print_Hebrew_String(Main_Page_arr_1,160,20,10);
	
	Set_Hebrew_Font_Size(Big);
	myGLCD.setColor(0,0,200);
	Print_Hebrew_String(Main_Page_arr_3,265,80,11);
	
	Set_Hebrew_Font_Size(Big);
	myGLCD.setColor(0,0,200);
	Print_Hebrew_String(Main_Page_arr_2,85,80,6);
	
	while(1)
	{
		if (myTouch.dataAvailable())
		{
			myTouch.read();
			x=myTouch.getX();
			y=myTouch.getY();
			
			if (  (x >= 30 && x <= 250)  )
			{
				if(    (y >= 80 && y <= (60+40))   )
				{
					myGLCD.clrScr();
					Motor_Control_Page();
					
				}
			}
			else if (  (x >= 250 && x <= 450)  )
			{
				if(    (y >= 60 && y <= (60+40))   )
				{
					myGLCD.clrScr();
					Page_Of_Setting();
					
				}
			}
		}
	}
}

void Motor_Control_Page()
{
	//create x&y variable for fixed location of the touch
	word x,y,check = 0 ,check1 = 0;
	
	//this variable is static because we want the controller to remember what is
	//the value of the last speed if we move to other pages (meaning:go out of the function)
	unsigned char Speed = 0, Speed1 = 0;
	
	//those arr of the page, and they are not is the loop
	word arr[]={40,50,6,70,0x30,1}; //מנוע א
	word arr1[]={40,50,6,70,0x30,2};//מנוע ב
	
	//Print battery value according to memory value
	myGLCD.setColor(46,192,100);
	Battery(Main_Control[0]);
	
	//Print Home Sign, pressing on that will lead you to main menu
	//the touch condition for that is the condition of the big while loop of this function
	Home_Sign(225,2);
	
	//Set printing to show that driving direction is Forward
	Reverse_and_Forward_Sign(85,60,Medium,Forward);
	
	//Set printing to show that driving direction is Forward
	Reverse_and_Forward_Sign(355,60,Medium,Forward);
	
	//print the arr in whit color
	Set_Hebrew_Font_Size(Big);
	myGLCD.setColor(255,255,255);
	Print_Hebrew_String(arr,10,35,6);
	Print_Hebrew_String(arr1,390,35,6);
	
	
	//as long as the user didn't press on the home button, stay in this loop
	while(!(x >= 220 && x <= 260 && y <= 32))
	{
		
		/************************************************************************/
		/*	   Check what direction (F/R) and Speed the JoyStick is in          */
		/************************************************************************/
		
		//check value of motor A on the right Side
		//it will set the screen according to the JoyStick
		if (Main_Control[3] >= 0)
		{
			//this variable help as to print the Forward OR Reverse only once
			static char Print_F_R_Motor_A = 0;
			
				//here we check if the value is forward and we check if we printed the value already
				if (Main_Control[5] == 0 && Print_F_R_Motor_A == 0)
				{
					//Set printing to show that driving direction is Forward
					Reverse_and_Forward_Sign(355,60,Medium,Forward);
					
					//after printing the value of forward, the only value that could be printed is Reverse
					Print_F_R_Motor_A = 1;
				}
				else if (Main_Control[5] && Print_F_R_Motor_A)
				{
					//Set printing to show that driving direction is Reverse
					Reverse_and_Forward_Sign(355,60,Medium,Reverse);
					
					//now we set the condition to print forward, all we wating for is to Sell 5 to be chanched
					Print_F_R_Motor_A = 0;
				}
				
			//check if Speed value is equal to zero, if so avoid reprinting this value
			//NOTE: if we will not check this condition, it will not allow the user to change value
			//      by touch, it will always print 0. (it will show what the user choose and print 0 right after it)
			if (Main_Control[3] == 0)
			{
				
				//this variable remember if we already printed 0 sins it was zero
				check++;
				
				//here we check if we printed once already, if so, we will not reprint zero again
				//so the user can use the touch pad to change values
				if (check > 2)
				{
					//we will set it to 3, because it will increase until it will be 255, and then it become 0 again
					//and ruin are condition, so we stack it at a number that bigger then 1
					check = 3;
				}
				//if it is the first time, we will print it for one time until there is a change of value by the JoyStick
				else
				{
					//move received value from micro controller (from JoyStick) to appear on the LCD
					//Speed = Main_Control[3];
					
					//Enable loop of changing value
					Print_Motor_Speed_By_Touch(0, Main_Control[4]);
					
				}
			}
			else
			{	
				//move received value from micro controller (from JoyStick) to appear on the LCD
				//Speed = Main_Control[3];
				
				//Enable loop of changing value
				Print_Motor_Speed_By_Touch(Main_Control[3], Main_Control[4]);
				
				check = 1;
			}
		}
		//check value of motor B on the Left Side
		//it will set the screen according to the JoyStick
		if (Main_Control[4] >= 0)
		{
			//this variable help as to print the Forward OR Reverse only once
			static char Print_F_R_Motor_B = 0;
			
			//here we check if the value is forward and we check if we printed the value already
			if (Main_Control[6] == 0 && Print_F_R_Motor_B == 0)
			{
				//Set printing to show that driving direction is Forward
				Reverse_and_Forward_Sign(85,60,Medium,Forward);
				
				//after printing the value of forward, the only value that could be printed is Reverse
				Print_F_R_Motor_B = 1;
			}
			else if (Main_Control[6] && Print_F_R_Motor_B)
			{
				//Set printing to show that driving direction is Reverse
				Reverse_and_Forward_Sign(85,60,Medium,Reverse);
				
				//now we set the condition to print forward, all we waiting for is to Sell 6 to be chanced
				Print_F_R_Motor_B = 0;
			}
			
			//check if Speed value is equal to zero, if so avoid reprinting this value
			//NOTE: if we will not check this condition, it will not allow the user to change value
			//      by touch, it will always print 0. (it will show what the user choose and print 0 right after it)
			if (Main_Control[4] == 0)
			{
				//this variable remember if we already printed 0 sins it was zero
				check1++;
				
				//here we check if we printed once already, if so, we will not reprint zero again
				//so the user can use the touch pad to change values
				if (check1 > 1)
				{
					//we will set it to 3, because it will increase until it will be 255, and then it become 0 again
					//and ruin are condition, so we stack it at a number that bigger then 1
					check1 = 3;
				}
				//if it is the first time, we will print it for one time until there is a change of value by the JoyStick
				else
				{
					//move received value from micro controller (from JoyStick) to appear on the LCD
					//Speed1 = Main_Control[4];
					
					//Enable loop of changing value
					Print_Motor_Speed_By_Touch(Main_Control[3], 0);
				}
			}
			else
			{
				check1 = 0;
				
				//move received value from micro controller (from JoyStick) to appear on the LCD
				//Speed1 = Main_Control[4];
				
				//Enable loop of changing value
				Print_Motor_Speed_By_Touch(Main_Control[3], Main_Control[4]);
			}
		}
		
		/************************************************************************/
		/*	   END of Check what direction (F/R) and Speed the JoyStick is in   */
		/************************************************************************/
		
		
		//check if the touch is not busy
		if (myTouch.dataAvailable())
		{
			
			//read the value of x and y
			myTouch.read();
			
			//get the value that the controller read on x line
			x=myTouch.getX();
			
			//get the value that the controller read on y line
			y=myTouch.getY();
		}
		
		//Enable or Disable Touch according to Main_Control Variable
		//if it is Disable, The if condition of the controlling area will be disabled
		//there for all the touch conditions are unavailable
		if (Main_Control[7])
		{
			/************************************************************************/
			/*				check reverse or forward condition                      */
			/************************************************************************/
			//check if reverse or forward button has been touched on one of the motors
			if((x >= 120 && x <= 160) || (x >= 340 && x <= 380))
			{
				//check if the right Button is touched
				if ((y >= 60 && y <= 100) && (x >= 120 && x <= 160))
				{
					//check it value and do NOT operation on the button
					if (Main_Control[6])
					{
						//if it is entered here it mean the motor is in reverse mode
						//now we changing it to forward because setting it back to value of 0
						Main_Control[6] = 0;
						
						//Set printing to show that driving direction is Forward
						Reverse_and_Forward_Sign(85,60,Medium,Forward);
					} 
					//if it is entered here it mean the motor is in Forward mode
					//now we changing it to reverse because setting it back to value of 1
					else
					{
						Main_Control[6] = 1;
						
						//Set printing to show that driving direction is Forward
						Reverse_and_Forward_Sign(85,60,Medium,Reverse);
					}
					
				} 
				//check if the left Button is touched
				else if ((x >= 340 && x <= 380) && (y >= 60 && y <= 100))
				{
						//check it value and do NOT operation on the button
						if (Main_Control[5])
						{
							//if it is entered here it mean the motor is in reverse mode
							//now we changing it to forward because setting it back to value of 0
							Main_Control[5] = 0;
							
							//Set printing to show that driving direction is Forward
							Reverse_and_Forward_Sign(355,60,Medium,Forward);
						}
						//if it is entered here it mean the motor is in Forward mode
						//now we changing it to reverse because setting it back to value of 1
						else
						{
							Main_Control[5] = 1;
							
							//Set printing to show that driving direction is Forward
							Reverse_and_Forward_Sign(355,60,Medium,Reverse);
						}
				}
				//here we make a delay of 200ms because when  the user press the button it will enter this loop a few times
				//since are pinger gonna be the around 100ms so it will change a few time between condition, the delay solve the problem
				_delay_ms(200);
				
				//because those variable are stay in the same value, it will enter here as long as the user didn;t press any other 
				//area on the screen, setting it back to zero, break the if condition
				x = y = 0;
			}
			/************************************************************************/
			/*	    	END of check reverse or forward condition                   */
			/************************************************************************/
			
			
			/************************************************************************/
			/*					check the speed level of motor A                    */
			/************************************************************************/
			//check if the right Speed_Control is touched
			if (x >= 400 && x <= 460    &&     y >= 50 && y <= (260))
			{
				//Reset LCD Send Value
				Main_Control [11] = 0;
				
				
				//check if level 6 as been touched
				if (y >=   (50)    &&    y <= (80))
				{
					//Set speed of Left Speed_Controller to 6
					Main_Control[12] = Speed = 6;
					
					//NOTE *1
					Main_Control[12]++;
					
					//Allow the loop condition in order to change the value
					//Print_Motor_Speed_By_Touch(Speed, Speed1);
					
					//Since The X stay the Same if we will not change it, it will stock are
					//if condition of the data from the JoyStick, so we Restarting it
					x = y = 0;
				}
				
				//check if level 5 as been touched
				else if (y >= (80)    &&    y <= (110))
				{
					//Set speed of Left Speed_Controller to 5
					Main_Control[12] = Speed = 5;
					
					//NOTE *1
					Main_Control[12]++;
					
					//Allow the loop condition in order to change the value
					//Print_Motor_Speed_By_Touch(Speed, Speed1);
					
					//Since The X stay the Same if we will not change it, it will stock are
					//if condition of the data from the JoyStick, so we Restarting it
					x = y = 0;
				}
				
				//check if level 4 as been touched
				else if (y >= (110)    &&    y <= (140))
				{
					//Set speed of Left Speed_Controller to 4
					Main_Control[12] = Speed = 4;
					
					//NOTE *1
					Main_Control[12]++;
					
					//Allow the loop condition in order to change the value
					//Print_Motor_Speed_By_Touch(Speed, Speed1);
					
					//Since The X stay the Same if we will not change it, it will stock are
					//if condition of the data from the JoyStick, so we Restarting it
					x = y = 0;
				}
				
				//check if level 3 as been touched
				else if (y >= (140)    &&    y <= (170))
				{
					//Set speed of Left Speed_Controller to 3
					Main_Control[12] = Speed = 3;
					
					//NOTE *1
					Main_Control[12]++;
					
					//Allow the loop condition in order to change the value
					//Print_Motor_Speed_By_Touch(Speed, Speed1);
					
					//Since The X stay the Same if we will not change it, it will stock are
					//if condition of the data from the JoyStick, so we Restarting it
					x = y = 0;
				}
				
				//check if level 2 as been touched
				else if (y >= (170)    &&    y <= (195))
				{
					//Set speed of Left Speed_Controller to 2
					Main_Control[12] = Speed = 2;
					
					//NOTE *1
					Main_Control[12]++;
					
					//Allow the loop condition in order to change the value
					//Print_Motor_Speed_By_Touch(Speed, Speed1);
					
					//Since The X stay the Same if we will not change it, it will stock are
					//if condition of the data from the JoyStick, so we Restarting it
					x = y = 0;
				}
				
				//check if level 1 as been touched
				else if (y >= (195)    &&    y <= (225))
				{
					//Set speed of Left Speed_Controller to 1
					Main_Control[12] = Speed = 1;
					
					//NOTE *1
					Main_Control[12]++;
					
					//Allow the loop condition in order to change the value
					//Print_Motor_Speed_By_Touch(Speed, Speed1);
					
					//Since The X stay the Same if we will not change it, it will stock are
					//if condition of the data from the JoyStick, so we Restarting it
					x = y = 0;
				}
				
				//check if level 0 as been touched
				else if (y >= (225)    &&    y <= (260))
				{
					//Set speed of Left Speed_Controller to 0
					Main_Control[12] = Speed =  0;
					
					//NOTE *1
					Main_Control[12]++;
					
					//Allow the loop condition in order to change the value
					//Print_Motor_Speed_By_Touch(Speed, Speed1);
					
					//Since The X stay the Same if we will not change it, it will stock are
					//if condition of the data from the JoyStick, so we Restarting it
					x = y = 0;
				}
				
			}
			/************************************************************************/
			/*				END of check the speed level of motor A                 */
			/************************************************************************/
			
			
			/************************************************************************/
			/*			    	check the speed level of motor  B                   */
			/************************************************************************/
			
			//if the right Speed_Control is not touched
			//check if the left Speed_Control is touched
			else if (x >= 20 && x <= 80    &&      y >= 50 && y <= (260))
			{
				
				//Reset LCD Send Value
				Main_Control [12] = 0;
				
				
				//check if level 6 as been touched
				if (y >=   (50)    &&    y <= (80))
				{
					//Set speed of Right Speed_Controller to 6
					Main_Control[11] = Speed1 = 6;
					Main_Control[11]++;
					//Allow the loop condition in order to change the value
					//Print_Motor_Speed_By_Touch(Speed, Speed1);
					
					//Since The X stay the Same if we will not change it, it will stock are
					//if condition of the data from the JoyStick, so we Restarting it
					x = y = 0;
				}
				
				//check if level 5 as been touched
				else if (y >= (80)    &&    y <= (110))
				{
					//Set speed of Right Speed_Controller to 5
					Main_Control[11] = Speed1 = 5;
					
					//NOTE *1
					Main_Control[11]++;
					
					//Allow the loop condition in order to change the value
					//Print_Motor_Speed_By_Touch(Speed, Speed1);
					
					//Since The X stay the Same if we will not change it, it will stock are
					//if condition of the data from the JoyStick, so we Restarting it
					x = y = 0;
				}
				
				//check if level 4 as been touched
				else if (y >= (110)    &&    y <= (140))
				{
					//Set speed of Right Speed_Controller to 4
					Main_Control[11] = Speed1 = 4;
					
					//NOTE *1
					Main_Control[11]++;
					
					//Allow the loop condition in order to change the value
					//Print_Motor_Speed_By_Touch(Speed, Speed1);
					
					//Since The X stay the Same if we will not change it, it will stock are
					//if condition of the data from the JoyStick, so we Restarting it
					x = y = 0;
				}
				
				//check if level 3 as been touched
				else if (y >= (140)    &&    y <= (170))
				{
					//Set speed of Right Speed_Controller to 3
					Main_Control[11] = Speed1 = 3;
					
					//NOTE *1
					Main_Control[11]++;
					
					//Allow the loop condition in order to change the value
					//Print_Motor_Speed_By_Touch(Speed, Speed1);
					
					//Since The X stay the Same if we will not change it, it will stock are
					//if condition of the data from the JoyStick, so we Restarting it
					x = y = 0;
				}
				
				//check if level 2 as been touched
				else if (y >= (170)    &&    y <= (195))
				{
					//Set speed of Right Speed_Controller to 2
					Main_Control[11] = Speed1 = 2;
					
					//NOTE *1
					Main_Control[11]++;
					
					//Allow the loop condition in order to change the value
					//Print_Motor_Speed_By_Touch(Speed, Speed1);
					
					//Since The X stay the Same if we will not change it, it will stock are
					//if condition of the data from the JoyStick, so we Restarting it
					x = y = 0;
				}
				
				//check if level 1 as been touched
				else if (y >= (195)    &&    y <= (225))
				{
					//Set speed of Right Speed_Controller to 1
					Main_Control[11] = Speed1 = 1;
					
					//NOTE *1
					Main_Control[11]++;
					
					//Allow the loop condition in order to change the value
					//Print_Motor_Speed_By_Touch(Speed, Speed1);
					
					//Since The X stay the Same if we will not change it, it will stock are
					//if condition of the data from the JoyStick, so we Restarting it
					x = y = 0;
				}
				
				//check if level 0 as been touched
				else if (y >= (225)    &&    y <= (260))
				{
					//Set speed of Right Speed_Controller to 0
					Main_Control[11] = Speed1 = 0;
					
					//NOTE *1
					Main_Control[11]++;
					
					//Allow the loop condition in order to change the value
					//Print_Motor_Speed_By_Touch(Speed, Speed1);
					
					//Since The X stay the Same if we will not change it, it will stock are
					//if condition of the data from the JoyStick, so we Restarting it
					x = y = 0;
				}
			}
			
		}
		/************************************************************************/
		/*			   	END of check the speed level of motor  B                */
		/************************************************************************/
	}
	
	//when we get out of the function, the Spidometer will not be print again
	//in order to print it again we will reset the condition by calling this function
	RePrint_Spidometer();
	
	//when we get out of the function, the Speed_Controllers will not be print again
	//in order to print it again we will reset the condition by calling this function
	RePrint_Speed_Control();
	
	//to avoid the printing the next page on the previous one, we clear the screen
	myGLCD.clrScr();
	
	//we printing the main page
	Main_Page();
}

void Page_Of_Setting()
{
	word x,y;
	static char V_All,V0,V1,V2;
	
	x=y=0;
	
	//Print battery value according to memory value
	myGLCD.setColor(46,192,100);
	
	//print the battery on the screen, the value is transfered by the Main_Control arr which is global
	Battery(Main_Control[0]);
	
	//Print Home Sign, pressing on that will lead you to main menu
	//the touch condition for that is the condition of the big while loop of this function
	Home_Sign(225,2);
	
	unsigned char arr[]={"cjr vfk"};  //בחר הכל
	myGLCD.setColor(200,00,60);
	Set_Hebrew_Font_Size(Big);
	Print_Hebrew_String_in_EN_HE_Mode(arr,10,50,7);
	Squre_Confirm(120,40,35,V_All);
	
	unsigned char arr0[]={"akhyv cnvhru, nbug cndg"};  // שליטה במהירות מנוע במגע
	myGLCD.setColor(55,00,255);
	Set_Hebrew_Font_Size(Big);
	Print_Hebrew_String_in_EN_HE_Mode(arr0,10,100,23);
	Squre_Confirm(20,90,35,V0);
	
	unsigned char arr1[]={",turv ehsnh,"};  //תאורה קידמית
	myGLCD.setColor(55,00,255);
	Set_Hebrew_Font_Size(Big);
	Print_Hebrew_String_in_EN_HE_Mode(arr1,10,150,12);
	Squre_Confirm(20,140,35,V1);
	
	unsigned char arr2[]={",turv tjurh,"};  // תאורה אחורית
	myGLCD.setColor(55,00,255);
	Set_Hebrew_Font_Size(Big);
	Print_Hebrew_String_in_EN_HE_Mode(arr2,10,200,12);
	Squre_Confirm(20,190,35,V2);
	
	unsigned char arr3[]={"tpaur duhxyheho"};   //אפשור גויסטיקים
	myGLCD.setColor(55,00,255);
	Set_Hebrew_Font_Size(Big);
	Print_Hebrew_String_in_EN_HE_Mode(arr3,10,250,15);
	Squre_Confirm(20,240,35,V3);
	
	while(!(x >= 220 && x <= 260 && y <= 32))
	{
		
		//check if the touch is not busy
		if (myTouch.dataAvailable())
		{
			
			//read the value of x and y
			myTouch.read();
			
			//get the value that the controller read on x line
			x=myTouch.getX();
			
			//get the value that the controller read on y line
			y=myTouch.getY();
		}
		if (x >= 19 && x <= 57)
		{
			if (y >= 90 && y <= 125)
			{
				V0 = !V0;
				Squre_Confirm(20,90,35,V0);
				x = y = 0;
				_delay_ms(200);
				if (V0 == 0)
				{
					V_All = 0;
					Squre_Confirm(120,40,35,V_All);
				}
				else if (V0 > 0 && V1 > 0 && V2 > 0 && V3 > 0)
				{
					V_All = 1;
					Squre_Confirm(120,40,35,V_All);
				}
			}
			else if (y >= 140 && y <= 175)
			{
				V1 = !V1;
				Squre_Confirm(20,140,35,V1);
				x = y = 0;
				_delay_ms(200);
				if (V1 == 0)
				{
					V_All = 0;
					Squre_Confirm(120,40,35,V_All);
				}
				else if (V0 > 0 && V1 > 0 && V2 > 0 && V3 > 0)
				{
					V_All = 1;
					Squre_Confirm(120,40,35,V_All);
				}
			}
			else if (y >= 190 && y <= 225)
			{
				V2 = !V2;
				Squre_Confirm(20,190,35,V2);
				x = y = 0;
				_delay_ms(200);
				if (V2 == 0)
				{
					V_All = 0;
					Squre_Confirm(120,40,35,V_All);
				}
				else if (V0 > 0 && V1 > 0 && V2 > 0 && V3 > 0)
				{
					V_All = 1;
					Squre_Confirm(120,40,35,V_All);
				}
			}
			else if (y >= 240 && y <= 275)
			{
				V3 = !V3;
				Squre_Confirm(20,240,35,V3);
				x = y = 0;
				_delay_ms(200);
				if (V3 == 0)
				{
					V_All = 0;
					Squre_Confirm(120,40,35,V_All);
				}
				else if (V0 > 0 && V1 > 0 && V2 > 0 && V3 > 0)
				{
					V_All = 1;
					Squre_Confirm(120,40,35,V_All);
				}
			}
		}
		else if (x >= 130 && x <= 175)
		{
			if (y >= 40 && y <= 75)
			{
				V_All = !V_All;
				V0 = V1 = V2 = V3 = V_All;
				Squre_Confirm(120,40,35,V_All);
				
				Squre_Confirm(20,240,35,V3);
				Squre_Confirm(20,190,35,V2);
				Squre_Confirm(20,140,35,V1);
				Squre_Confirm(20,90,35,V0);
				_delay_ms(200);
				x = y = 0;
			}
		}
	}
	
	//Enable or Disable Touch of controlling speed motor according to V1 value
	Main_Control[7] = V0;
	
	//to avoid the printing the next page on the previous one, we clear the screen
	myGLCD.clrScr();
	
	//we printing the main page
	Main_Page();
}

void Print_Motor_Speed_By_Touch(unsigned char Speed, unsigned char Speed1)
{
	//this Function avoid of reprinting the Speed_Controls over and over again
	//it will enter this Function only if there is a change in the touch
	
	//	static char Last_Speed = 0, Last_Speed1 = 0;
	/*
		//check if motor is in Forward mode
		if (Speed1 < 10)
		{	
			if (Last_Speed1 < 2)
			{
				//Set printing to show that driving direction is Forward
				Reverse_and_Forward_Sign(85,60,Medium,Forward);
				
				//break the condition so it will be printed only once
				Last_Speed1 = 3;
			}
		}
		else
		{
			//if motor in reverse decries 10 from the value in order to get value from 1-6
			Speed1 -= 10;
			
			if (Last_Speed1 == 3)
			{
			
			//Set printing to show that driving direction is reverse
			Reverse_and_Forward_Sign(85,60,Medium,Reverse);
			
			//break the condition so it will be printed only once
			Last_Speed1 = 1;
			}
		}
		
		//check if motor is in Forward mode
		if (Speed < 10)
		{
			if (Last_Speed < 2)
			{
				//Set printing to show that driving direction is reverse
				Reverse_and_Forward_Sign(355,60,Medium,Forward);
				
				//break the condition so it will be printed only once
				Last_Speed = 3;
			}
		}
		
		else
		
		{
			//if motor in reverse decries 10 from the value in order to get value from 1-6
			Speed -= 10;
			
			if (Last_Speed == 3)
			{
				
				//Set printing to show that driving direction is reverse
				Reverse_and_Forward_Sign(355,60,Medium,Reverse);
				
				//break the condition so it will be printed only once
				Last_Speed = 1;
			}
		}
		*/
		//enter the new speed that got touched of the right Speed_Control
		Speed_Control(400,60,60,6,Speed);
		
		//enter the new speed that got touched of the left Speed_Control
		Speed_Control1(20,60,60,6,Speed1);
		
		//set the speedometer wright location according to the sensor
		Spidometer(235,200,140,(100/6*Speed),22/6*Speed);
		
		//in order to avoid the reentering of the loop we setting it back to zero
	
}

#endif /* LCD_PAGES_H_ */
