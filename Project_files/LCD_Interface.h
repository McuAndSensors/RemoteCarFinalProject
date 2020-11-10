/*
 * LCD_Interface.h
 *
 * Created: 18/11/2016 08:42:20
 *  Author: Azriel
 */ 


#ifndef LCD_INTERFACE_H_
#define LCD_INTERFACE_H_
/************************************************************************/
/*							Global Variables                            */
/************************************************************************/
unsigned char	*RePrint_Speedometer;

unsigned char	 Print_Speed_Control1 = 1;

unsigned char	 Print_Speed_Control = 1;

//this variable remember when the last location of the arrow was in the scale in order
//to delete it and place the new one(if we will m=not delete the last one the screen will be gray)
unsigned char Spidometer_Line_Cleaner;

char *Font_Size = (char *)Big;
/************************************************************************/
/*						End Of Global Variables	                        */
/************************************************************************/

#define  Big		3
#define  Medium		2
#define  Small		1

void Create_button(word Button_X_Size, word Button_Y_Size, word X_Location, word Y_Location,unsigned char Red,unsigned char Green,unsigned char Blue);
void Main_Seven_Segment(unsigned char Size_1_To_5, word X_Location, word Y_Location, char Display_Num);
void Battery(unsigned char Capacity_in_Precent);
void Hebrew_Letters_Big_Size(word Letter_Num, word X_Location, word Y_Location);
void Print_Hebrew_String(word *Hebrew_String, word X_location, word Y_Location, unsigned char Size_String);
void Print_Letter_16_Bit(unsigned char *Bit_value,word X,word Y);
void Line_With_angle(word X_Location, word Y_Location, word Line_Size, word Line_angle);
void Line_With_angle_v2(word X_Location, word Y_Location, word Line_Size, word Line_angle);
void Spidometer(word X_Location,word Y_Location, unsigned char Size_Radios, unsigned char Speed_in_Precent, unsigned char Running_Speed);
void Seven_Segment_Decimal_To_99(unsigned char Size_1_To_5, word X_Location, word Y_Location, char Display_Num);
void Speed_Control(word X_Location,word Y_Location, unsigned char Size_OF_X,char Max_Speed, char Present_Speed);
void Speed_Control1(word X_Location,word Y_Location, unsigned char Size_OF_X,char Max_Speed, char Present_Speed);
void Squre_Confirm(word X_Location, word Y_Location,  unsigned char X_Size, char V_On_Off);
void Print_Hebrew_String_in_EN_HE_Mode(unsigned char *Hebrew_String, word X_location, word Y_Location, unsigned char Size_String);
void scroll_Menu_Bar(word X_Location, word Y_Location,  unsigned char Size_1_OR_2,unsigned char Options_Amount, unsigned char *String, char Max_Line_Chars);
void Pointing_Down_Arrow(word X_Location, word Y_Location,  unsigned char X_Size);
void Pointing_Up_Arrow(word X_Location, word Y_Location,  unsigned char X_Size);
void Print_Letter_8_Bit(unsigned char *Bit_value,word X,word Y);
void Hebrew_Letters_Medium_Size(word Letter_Num, word X_Location,word Y_Location);
void Set_Font_Size(char Size);

#endif /* LCD_INTERFACE_H_ */