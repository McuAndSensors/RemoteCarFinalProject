/*
 * NRF_Project.c
 *
 * Created: 25/08/2016 11:57:45
 * Author : Azriel
 */ 

#define F_CPU 16000000UL //12MHz connected to 328P MCU(Micro Controller)

/************************************************************************/
/*                              UART SETUP                              */
/************************************************************************/
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
void Send_num(unsigned char num)
{
	Verafy_num(num/16);
	Verafy_num(num%16);
	USART_send(',');
	USART_send(' ');
}


/************************************************************************/
/*                          END OF LCD SETUP                            */
/************************************************************************/


#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <avr/sfr_defs.h>
#include <stdio.h>

/************************************************************************/
/*                          define all registers                        */
/************************************************************************/
#define CONFIG			0x00 //configuration register: control	RX_DR, TX_DS, MAX_RT, CRC, CRC_SIZE, PWR_UP, TX/RX_MODE
#define EN_AA			0x01 //Enhanced ShockBurst: En/Dis Auto ACK on data pipe (0 to 5)
#define EN_RXADDR		0x02 //Enable RX Address: En/Dis RX Addresses on Data pipe (0 to 5)
#define SETUP_AW		0x03 //Setup of Address width: 3/4/5 Byte (common to all data pipes)
#define SETUP_RETR		0x04 //Setup of Auto Retransmission: delay between packets sent (250uS to 4000uS) and ARC(retransmit amount 0-15)
#define RF_CH			0x05 //set the Channel frequency 
#define RF_SETUP		0x06 //RF Setup register: Set data rate speed. 250k/1M/2M or low mode 250Kbps and set up output power(0,-6,-12,-18)dBm
#define STATUS			0x07 //give an indication on: data arrived, data sent, max RT, pipe PAYLOAD available for reading(3 bit), TX FIFO full flag 
#define OBSERVE_TX		0x08 //2 counters of lost packets
#define RPD				0x09 //Received Power Detector: recognize power receiving   
#define RX_ADDR_P0		0x0A //Received Address data pipe (LSByte is written first) up to 5 Byte according to size you defined
#define RX_ADDR_P1		0x0B //Received Address data pipe (LSByte is written first) up to 5 Byte according to size you defined
#define RX_ADDR_P2		0x0C //Received Address data pipe (only LSByte) The MSByte stay the same as pipe 1
#define RX_ADDR_P3		0x0D //Received Address data pipe (only LSByte) The MSByte stay the same as pipe 1
#define RX_ADDR_P4		0x0E //Received Address data pipe (only LSByte) The MSByte stay the same as pipe 1
#define RX_ADDR_P5		0x0F //Received Address data pipe (only LSByte) The MSByte stay the same as pipe 1
#define TX_ADDR			0x10 //Transmit Address data pipe (LSByte is written first) set this address to be equal to Pipe 0
#define RX_PW_P0		0x11 //Set up RX payload size in pipe 0 (1-32 Byte) 0=Pipe not used
#define RX_PW_P1		0x12 //Set up RX payload size in pipe 1 (1-32 Byte) 0=Pipe not used
#define RX_PW_P2		0x13 //Set up RX payload size in pipe 2 (1-32 Byte) 0=Pipe not used
#define RX_PW_P3		0x14 //Set up RX payload size in pipe 3 (1-32 Byte) 0=Pipe not used
#define RX_PW_P4		0x15 //Set up RX payload size in pipe 4 (1-32 Byte) 0=Pipe not used
#define RX_PW_P5		0x16 //Set up RX payload size in pipe 5 (1-32 Byte) 0=Pipe not used
#define FIFO_STATUS		0x17 //give indication on: RX FIFO, TZ FIFO (empty or full) and, give option of reuse of last packet(in PTX)
#define DYNPL			0x1C //Enable Dynamic Payload Length: En/Dis the option for each Pipe
#define FEATURE			0x1D //it allow as to En/Dis all Pipes: Dynamic Payload, ACK Payload, W_TX_PAYLOAD_NOACK command. 
/************************************************************************/
/*                         define all commands                          */
/************************************************************************/
#define READ_RX_PAYLOAD			0x61
#define WRITE_TX_PAYLOAD		0xA0
#define FLUSH_TX				0xE1
#define FLUSH_RX				0xE2
#define REUSE_TX_PL				0xE3
#define READ_RX_PL_WIDTH		0x60
#define WRITE_TX_PAYLOAD_NOACK	0xB0
#define NOP						0xFF
#define  READ_REGISTER			0x00
#define  WRITE_REGISTER			0x20
#define  W_ACK_PAYLOAD			0xA8
/************************************************************************/
/*                    define all options of registers                   */
/************************************************************************/
#define MASK_RX_DR  6
#define MASK_TX_DS  5
#define MASK_MAX_RT 4
#define EN_CRC      3
#define CRCO        2
#define PWR_UP      1
#define PRIM_RX     0
#define ENAA_P5     5
#define ENAA_P4     4
#define ENAA_P3     3
#define ENAA_P2     2
#define ENAA_P1     1
#define ENAA_P0     0
#define ERX_P5      5
#define ERX_P4      4
#define ERX_P3      3
#define ERX_P2      2
#define ERX_P1      1
#define ERX_P0      0
#define AW          0
#define ARD         4
#define ARC         0
#define PLL_LOCK    4
#define RF_DR       3
#define RF_PWR      1
#define LNA_HCURR   0
#define RX_DR       6
#define TX_DS       5
#define MAX_RT      4
#define RX_P_NO     1
#define TX_FULL     0
#define PLOS_CNT    4
#define ARC_CNT     0
#define TX_REUSE    6
#define FIFO_FULL   5
#define TX_EMPTY    4
#define RX_FULL     1
#define RX_EMPTY    0
/************************************************************************/
/*                          PIN definition                              */
/************************************************************************/
#define  CE		0 //PORT B
#define IRQ		2 //PORT B
#define CSN		1 //PORT B
#define MOSI	3 //PORT B
#define MISO	4 //PORT B
#define SCK		5 //PORT B
/*****************PORT DEFINITION*******************/
#define Out_OR_IN	DDRB
#define READ_DATA	PINB
#define Send_Signal PORTB
/****************Define Size Of Payload*************/
#define Payload_Size 6
/************************************************************************/
/*                        End of All Define                             */
/************************************************************************/
void SPI_SETUP (void);
unsigned char SPI_Byte_TX_OR_RX (unsigned char data);
unsigned char Write_Data_To_nRF(unsigned char Reg_Name, unsigned char *arr_of_data, unsigned char arr_Size);
unsigned char Read_Data_From_nRF(unsigned char Reg_Name, unsigned char *arr_of_data, unsigned char arr_Size);
void Set_interrupt(void);
void Main_setup_nRF(void);
void Reset_IRQ(void);
unsigned char Receive_Payload(unsigned char *Receive_Data);
void Transmite_Payload(unsigned char *Data_Transmission);
unsigned char Check_STATUS_Register();
void Set_RX_Mode(void);
void Set_TX_Mode(void);

static unsigned char send_or_receive[6]={0x00,0x01,0x12,0x03,0x04,0x50};
	/*
int main(void)
{
	
	unsigned char STATUS_is_ok;
	Main_setup_nRF();
	USART_init();
	Set_RX_Mode();
	//Set_TX_Mode();
	STATUS_is_ok = 0;
	
	while(1)
	{
		//Send_Signal = 1<<CE;
		//_delay_us(10000);
		//Send_Signal = 1<<CE;
		//Send_Signal = 0<<CSN;
		//Read_Data_From_nRF(FIFO_STATUS,send_or_receive,1);
		//Send_num(send_or_receive[0]);
		Read_Data_From_nRF(STATUS,send_or_receive,1);
		//Send_num(send_or_receive[0]);
		//Send_Signal = 0<<CSN;
		
		if(Receive_Payload(send_or_receive))
		{
			//Send_Signal = 0<<CE;
			Receive_Payload(send_or_receive);
			Send_num(send_or_receive[0]);
			Send_num(send_or_receive[1]);
			Send_num(send_or_receive[2]);
			Send_num(send_or_receive[3]);
			Send_num(send_or_receive[4]);
			Send_num(send_or_receive[5]);
			STATUS_is_ok ++;
			//Reset_IRQ();
			//Send_Signal = 1<<CE;
		}
		
		//Reset_IRQ();
		//_delay_ms(10);
		//send_or_receive[3]++;
		//Transmite_Payload(send_or_receive);
	}
	return 0;
}
*/
/************************************************************************/
/*                        Main Setup Functions                          */
/************************************************************************/

ISR (INT0_vect)
{
	Receive_Payload(send_or_receive);
	Send_num(send_or_receive[0]);
	Send_num(send_or_receive[1]);
	Send_num(send_or_receive[2]);
	Send_num(send_or_receive[3]);
	Send_num(send_or_receive[4]);
	Send_num(send_or_receive[5]);
}

/*******************************SPI_SETUP********************************/
/*Here we define the SPI protocol and define all pins that are used as  */
/*input or an out put, this function is called from the MAIN_SETUP, so  */
/*you don't need to call it in main.									*/
/************************************************************************/
void SPI_SETUP (void)
{
	// turn off interrupts
	cli();
	
	// Set MOSI, SCK, CSN, CE as Output
	DDRB=(1<<SCK)|(1<<MOSI)|(1<<CSN)|(1<<CE)|(1<<2);
	
	// Enable SPI, Set as Master
	// Data speed: Fosc/16, Enable Interrupts
	SPCR = (1<<SPE)|(1<<MSTR)|(1<<SPR0);
	
	//CSN is high, No data to be send now
	Send_Signal = (1<<CSN); 
	
	//CE is LOW, now we don't sending or receiving anything
	Send_Signal = (1<<CE);
	
	// Enable Global Interrupts
	sei();
}
/***************************SPI_Byte_TX_OR_RX****************************/
/*This function sent/received only one byte, this function made only to	*/
/*make the code easer to read, this function is used only in the		*/
/*Write_Data_To_nRF or Read_Data_From_nRF functions.					*/
/************************************************************************/
unsigned char SPI_Byte_TX_OR_RX (unsigned char data)
{
	// Load data into the buffer
	SPDR = data;
	
	//Wait until transmission complete
	while(!(SPSR & (1<<SPIF) ));
	
	// Return received data
	return(SPDR);
}

void Set_interrupt()
{
	DDRD &= ~(1 << DDD2);     // Clear the PD2 pin
	// PD2 (PCINT0 pin) is now an input

	PORTD |= (1 << PORTD2);    // turn On the Pull-up
	// PD2 is now an input with pull-up enabled


	EICRA |= (1 << ISC01);    // set INT0 to trigger on falling adge
	EIMSK |= (1 << INT0);     // Turns on INT0

	sei();                    // turn on interrupts

}
/*************************Write_Data_To_nRF******************************/
/*This function is use to write an max 32 byte of data by using an arr	*/
/*on the main function. by sending the arr you must declare is size, the*/
/*name of the arr is Data_communication and you can move in it any data */
/************************************************************************/
unsigned char Write_Data_To_nRF(unsigned char Reg_Name, unsigned char *arr_of_data, unsigned char arr_Size)
{
	// turn off interrupts
	cli();
	
	unsigned char i, Status_reg;
	// Make sure we don't write after a command that was less then the minimum time
	_delay_us(10);
	
	// turn off interrupts
	cli();                    
	
	//now we get CSN to low so the nRF can listen 
	Send_Signal = 0<<CSN;
	
	//This if function check if we want to write data into register or into the TX FIFO
	//if we put value of '0' in the name of the register that mean that we don't a register to refer to.
	//any number bigger then '0' will be considered as register name by value 
	if(Reg_Name != 0x50 )
	{
			//now the SPI will send command of writing, and the name of the register we will like to write to.
			Status_reg = SPI_Byte_TX_OR_RX(WRITE_REGISTER + Reg_Name);
			_delay_us(10);
	}
	else
	{
			//now the SPI will send command of writing the data to the TX FIFO.
			Status_reg = SPI_Byte_TX_OR_RX(WRITE_TX_PAYLOAD);
			_delay_us(10);
	}
	
	//now we sending the packet one by one till it reach the "arr_Size" value
	//and of course we take 10uS delay between each byte that we send.
	for(i = 0; i < arr_Size; i++)
	{
		SPI_Byte_TX_OR_RX(arr_of_data[i]);
		_delay_us(10);
	}
	
	//now we get CSN to High again and we stop the nRF communication
	Send_Signal = 1<<CSN;
	_delay_us(10);
	
	// turn on interrupts
	sei();                   
	
	//now we send back the state of the status register.
	return(Status_reg);
}


/*************************Read_Data_From_nRF*****************************/
/*This function is use to read an max 32 byte of data by using an arr	*/
/*on the main function. by receiving the arr you must declare is size,  */
/*name of the arr is Data_communication and you can move from it data   */
/************************************************************************/
unsigned char Read_Data_From_nRF(unsigned char Reg_Name, unsigned char *arr_of_data, unsigned char arr_Size)
{
	
	unsigned char i, Status_reg;
	// Make sure we don't read after a command that was less then the minimum time
	_delay_us(10);
	
	// turn off interrupts
	cli();
	
	//now we get CSN to low so the nRF can listen
	Send_Signal = 0<<CSN;
	_delay_us(2);
	if(Reg_Name != 0x50)
	{
	//now the SPI will send command of reading, and the name of the register we will like to read to.
	Status_reg = SPI_Byte_TX_OR_RX(READ_REGISTER + Reg_Name);
	_delay_us(10);
	}
	else
	{
		//now the SPI will send command of reading, and the name of the register we will like to read to.
		Status_reg = SPI_Byte_TX_OR_RX(READ_RX_PAYLOAD);
		_delay_us(10);
	}
	//now we receiving the packet one by one till it reach the "arr_Size" value
	//and of course we take 10uS delay between each byte that we send.
	for(i = 0; i < arr_Size; i++)
	{
		//Here we send dummy byte just to get it to work.
		arr_of_data[i] = SPI_Byte_TX_OR_RX(NOP);
		_delay_us(10);
	}
	
	//now we get CSN to High again and we stop the nRF communication
	Send_Signal = 1<<CSN;
	_delay_us(10);
	
	// turn on interrupts
	sei();
	
	//now we send back the state of the status register.
	return(Status_reg);
}


/***************************Main_setup_nRF*******************************/
/*This function must be called in order to active the nRF24L01+  if you */
/*won't call this function, nothing is going to work. This function set */
/*the SPI communication and the interrupt on Port D, (it can be change) */
/*later on it define all the register, this setup define only one data  */
/*pipe of communication, you can change it for more, 5 byte address size*/
/*and Auto Ack on Pipe 0, you can see every definition down below       */ 
/************************************************************************/
void Main_setup_nRF(void)
{
	unsigned char Value_of_command[5];
	SPI_SETUP(); //Setup the SPI configuration 
	
	//Set_interrupt(); //Enable interrupt on INT0 (PORT D PIN 2)
	
	// turn off interrupts
	cli();
	
	_delay_ms(120);  //The time for the chip to power up and set all registers  
	
	//EN_AA enable (Auto Ack) TX get automatic respond from receiver when a transmission is success
	//work only when the TX and the RX have the same address
	//Enable Auto Acknowledgment on Data Pipe 0  ;bit 0 = '1'
	Value_of_command[0]= 0x01; 
	Write_Data_To_nRF(EN_AA,Value_of_command,1);
	
	//Chose number of Data Pipe
	//Enable one data pipe ; bit 0='1'
	Value_of_command[0]= 0x01; 
	Write_Data_To_nRF(EN_RXADDR,Value_of_command,1);
	 
	//Define the Size of Address (the bigger the size is, the more secure the info will be)
	//we define 5 byte address; bit 1,0 ="11" 
	Value_of_command[0]= 0x03; 
	Write_Data_To_nRF(SETUP_AW,Value_of_command,1); 
	
	//RF channel setup, chose between 2.400GHz t0 2.526GHz
	//we will chose channel 3 2.403GHz, the same mast be on TX and RX
	//RF_CH = 0x03 that will be 0b0000 0011
	Value_of_command[0]= 0x03;
	Write_Data_To_nRF(RF_CH,Value_of_command,1);
	
	//Define the delay between packet retransmission
	//in are case we will choose 750uS just to be sure
	//we will choose also the max of retransmission to be 15
	//bit 3-0 is retransmission amount ="1111" 15 times
	//bit 7-4 is the delay between packets ="0010'  ;0b0010 1111
	Value_of_command[0]= 0x2F;
	Write_Data_To_nRF(SETUP_RETR,Value_of_command,1);
	
	//choose the power mode in dB of transmission and choose the speed of data rate
	//bit 5 and 3 define the speed both bit = '0' it will be on 1Mbps
	//bit 2 and 1 are for power mode "11"=0dB ; bit 0 = don't care
	Value_of_command[0]= 0x26;
	Write_Data_To_nRF(RF_SETUP,Value_of_command,1);
	
	//Receiver address is need to be modified, we make the size of it to 5 Bytes
	//The address will be 0x15,0x35,0x17,0xC5,0x32
	Value_of_command[0]= 0x15;
	Value_of_command[1]= 0x35;
	Value_of_command[2]= 0x17;
	Value_of_command[3]= 0xC5;
	Value_of_command[4]= 0x32;
	Write_Data_To_nRF(RX_ADDR_P0,Value_of_command,5);
	//if you want to use a multiply pipe here is the place to add them
	
	//Write_Data_To_nRF(RX_ADDR_P1,Value_of_command,5);
	
	//Here we set the Transmitter address to be the same as the receiver.
	//it is important to remember that the TX address is must be equal to the
	//receiver of pipe 0 only, and not to the others pipes (in case you use multiply data pipes).
	Write_Data_To_nRF(TX_ADDR,Value_of_command,5);
	
	//here we define how much payload we would like to send each transmit (1-32)
	//we will define to send 6 bytes on each packet on data pipe 0
	//you can change Payload_Size in the define
	//bits 0-5 will define the bit amount.
	Value_of_command[0]= Payload_Size;
	Write_Data_To_nRF(RX_PW_P0,Value_of_command,1);
	
	//Now the function of TX_Mode or Set_RX_Mode must be called in order
	//to complete the Setup process
	
	// turn on interrupts
	sei();
}
unsigned char Check_STATUS_Register()
{
	unsigned char STATUS_reg;
	
	//get STATUS register value
	STATUS_reg = SPI_Byte_TX_OR_RX(NOP);

	//Check if RX_DR is = '0' he will return RX_DR value 
	//that mean the interrupt have been active
	if(!(STATUS_reg & (1<<RX_DR)))
	{
		STATUS_reg = RX_DR;
	}
	
	//Check if TX_DS is = '0' he will return TX_DS value 
	//that mean the interrupt have been active
	if(!(STATUS_reg & (1<<TX_DS)))
	{
		STATUS_reg = TX_DS;
	}
	
	//Check if MAX_RT is = '0' he will return MAX_RT value
	//that mean the interrupt have been active
	if(!(STATUS_reg & (1<<MAX_RT)))
	{
		STATUS_reg = MAX_RT;
	}
	
	//if non of the MAX_RT/TX_DS/RX_DR is true, he will return the register value
	return STATUS_reg;
}
/********************************Reset_IRQ*******************************/
/*This function will be use to clear all kind of interrupts, the word   */
/*IRQ (interrupt request) goes HIGH in 3 cases 1.TX complete 2.RX comp' */
/*3.max retransmit. this function will clear all of those interrupts    */
/************************************************************************/
void Reset_IRQ(void)
{
	_delay_us(10);
	Send_Signal = (0<<CSN);	//CSN low
	_delay_us(10);
	SPI_Byte_TX_OR_RX(WRITE_REGISTER + STATUS);	//Write to STATUS register
	_delay_us(10);
	SPI_Byte_TX_OR_RX(0x70);	//Reset all IRQ interrupt
	_delay_us(10);
	Send_Signal = (1<<CSN);	//CSN IR_High
}
/************************************************************************/
/*                               RX mode                                */
/************************************************************************/
void Set_RX_Mode(void)
{
	//Here i define an arr only because the function must get an arr
	unsigned char Value_of_command[1];
	
	// turn off interrupts
	cli();
	
	//here we use config register to say the device is receiver
	//0x1E = MAX_RT interrupt not be refracted on the IRQ pin
	//0x1E = Enable CRC 2 Byte, POWER_UP = '1', PRX mode
	Value_of_command[0]= 0x0F;
	Write_Data_To_nRF(CONFIG,Value_of_command,1);
	
	//Now we Don't sending or receiving anything
	Send_Signal = (0<<CE);
	
	//Time Delay must is 1.5 mS from POWER_DOWN ==> START_UP i gave 2mS
	_delay_ms(2);
	
	// turn on interrupts
	sei();
}


unsigned char Receive_Payload(unsigned char *Receive_Data)
{
	unsigned char STATUS_reg_condition[1];
	
	// turn off interrupts
	cli();
	
	Send_Signal = (1<<CE);
	
	//wait and listen 5 mS
	_delay_ms(5);
	
	Read_Data_From_nRF(STATUS,STATUS_reg_condition,1);
	if(STATUS_reg_condition[0]  >= 0x40)
	{
		//The first value is '0x50' because we want to read the RX FIFO
		//any other value will make it to read register value and not the RX FIFO 
		Read_Data_From_nRF(0x50,Receive_Data,Payload_Size);

		
		
		//stop listening
		Send_Signal = (0<<CE);
		
	
	
	//after reading the RX FIFO you must clean it other ways you will get dummy data
	//in order to send a command CSN must get low
	Send_Signal = 0<<CSN;
	
	//a small delay just to be sure
	_delay_us(2);
	
	//clean RX FIFO
	SPI_Byte_TX_OR_RX(FLUSH_RX);
	
	//return to high again, no more commands for now
	Send_Signal = 1<<CSN;
	
	//10 uS delay for case that there is command that need that delay
	Reset_IRQ();
	
	return 1;
	}
	
	
	// turn on interrupts
	sei();
	
	return 0;
}
/************************************************************************/
/*                               TX mode                                */
/************************************************************************/
void Set_TX_Mode(void)
{
	//Here i define an arr only because the function must get an arr
	unsigned char Value_of_command[1];
	
	// turn off interrupts
	cli();
	
	//here we use config register to say the device is Transmitter
	//0x1E = MAX_RT interrupt not be refracted on the IRQ pin
	//0x1E = Enable CRC 2 Byte, POWER_UP = '1', PTX mode
	Value_of_command[0]= 0x0E;
	Write_Data_To_nRF(CONFIG,Value_of_command,1);
	
	//Now we Don't sending or receiving anything
	Send_Signal = (0<<CE);
	
	//Time Delay must is 1.5 mS from POWER_DOWN ==> START_UP i gave 2mS
	_delay_ms(2);
	
	// turn on interrupts
	sei();
}

void Transmite_Payload(unsigned char *Data_Transmission)
{
	unsigned char Check_Status[1];
	
	// turn off interrupts
	cli();
	
	//This is the value to clear TX FIFO; 0xE1
	//It is a command that send through SPI
	
	//get the status register condition
	Write_Data_To_nRF(STATUS, Check_Status,1);
	
	//check if the transmitter reached to max of retransmissions
	if((Check_Status[0] & (1<<MAX_RT)) == 0x01)
	{
		//if it reached to max reset it
		Reset_IRQ();
	}
	
	//in order to send a command CSN must go low
	Send_Signal = 0<<CSN;
	
	//according to the data sheet TX FIFO must get clean after transfer
	SPI_Byte_TX_OR_RX(FLUSH_TX);
	
	//after the command CSN get high again
	Send_Signal = 1<<CSN;
	
	//Send the data that is in the Data_Transmission arr
	//By putting the '0x50' value, we saying that the information is for the TX FIFO. (Payload)
	//Payload_size is a DEFINE in the program, you can change it to the size you like (1-32)
	
	Write_Data_To_nRF(0x50, Data_Transmission,Payload_Size);
	//if you change the packet payload to more/less then 6(current value)
	//you must change the value in here too.
	
	//Enable the global interrupt
	sei();
	
	_delay_ms(10);
	//CE is high to transmit data
	Send_Signal = (1<<CE);
	
	_delay_us(15);
	Send_Signal = (0<<CE);
	//we gave here a pulse of 15uS for the data to be transmitted
	
	//once again a long delay before the next move
	_delay_ms(10);
	
		//get the status register condition
		Write_Data_To_nRF(STATUS, Check_Status,1);
		
		//check if the transmitter send the DATA
		if((Check_Status[0] & (1<<TX_DS)) == 0x01)
		{
			//if the data didn't sent, call the transmission function again 
			//in order to send a command CSN must go low
			Send_Signal = 0<<CSN;
	
			//Sending again the DATA in the TX FIFO
			SPI_Byte_TX_OR_RX(REUSE_TX_PL);
	
			//after the command CSN get high again
			Send_Signal = 1<<CSN;
		}
}



