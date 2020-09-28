// UARTTestMain.c
// Runs on LM4F120/TM4C123
// Used to test the UART.c driver
// Daniel Valvano
// May 30, 2014


// U0Rx (VCP receive) connected to PA0
// U0Tx (VCP transmit) connected to PA1
void SystemInit()
{}

#include <stdint.h>
//Register definitions for ClockEnable
#define 		SYSCTLRCGCUARTR			(*((volatile unsigned long*)0x400FE618))
#define 		SYSCTLRCGCGPIOR			(*((volatile unsigned long*)0x400FE608))

//Register definitions for UART0 module
#define 	UART0CTLR						(*((volatile unsigned long*)0x4000C030))
#define  	UART0IBRDR					(*((volatile unsigned long*)0x4000C024))
#define  	UART0FBRDR					(*((volatile unsigned long*)0x4000C028))
#define 	UART0LCRHR					(*((volatile unsigned long*)0x4000C02C))
#define 	UART0CCR						(*((volatile unsigned long*)0x4000CFC8))
#define 	UART0FRR						(*((volatile unsigned long*)0x4000C018))
#define 	UART0DRR						(*((volatile unsigned long*)0x4000C000))

//Register definitions for GPIOPortD
#define 	GPIOPORTAAFSELR			(*((volatile unsigned long*)0x40004420))
#define 	GPIOPORTAPCTLR			(*((volatile unsigned long*)0x4000452C))
#define 	GPIOPORTADENR				(*((volatile unsigned long*)0x4000451C))
#define 	GPIOPORTADIRR				(*((volatile unsigned long*)0x40004400))
#define 	GPIOPORTALOCKR			(*((volatile unsigned long*)0x40004520))
#define 	GPIOPORTACRR				(*((volatile unsigned long*)0x40004524))
#define 	GPIOPORTAAMSELR			(*((volatile unsigned long*)0x40004528))


#define UART_FR_TXFF            0x00000020  // UART Transmit FIFO Full
#define UART_FR_RXFE            0x00000010  // UART Receive FIFO Empty
#define UART_LCRH_WLEN_8        0x00000060  // 8 bit word length
#define UART_LCRH_FEN           0x00000010  // UART Enable FIFOs
#define UART_CTL_UARTEN         0x00000001  // UART Enable
#define CR											13
#define BS											8
#define LF											10

//------------UART_Init------------
// Initialize the UART for 115,200 baud rate (assuming 50 MHz UART clock),
// 8 bit word length, no parity bits, one stop bit, FIFOs enabled
// Input: none
// Output: none
void UART_Init(void){
  SYSCTLRCGCUARTR |= 0x01;            		// activate UART0
  SYSCTLRCGCGPIOR |= 0x01;            		// activate port A
 
  UART0CTLR &= ~UART_CTL_UARTEN;      		// disable UART
  UART0IBRDR = 8;                    		// IBRD = int(16,000,000 / (16 * 115,200)) = int(8.6805)
  UART0FBRDR = 44;                     	// FBRD = int(0.6805 * 64 + 0.5) = 44
																					// 8 bit word length (no parity bits, one stop bit, FIFOs)
  UART0LCRHR = (UART_LCRH_WLEN_8|UART_LCRH_FEN);
  UART0CTLR |= UART_CTL_UARTEN;       		// enable UART
  GPIOPORTAAFSELR |= 0x03;           			// enable alt funct on PA1-0
  GPIOPORTADENR |= 0x03;             			// enable digital I/O on PA1-0
																					// configure PA1-0 as UART
  GPIOPORTAPCTLR = (GPIOPORTAPCTLR & 0xFFFFFF00)+0x00000011;
  GPIOPORTAAMSELR &= ~0x03;          // disable analog functionality on PA
}


//------------UART_InChar------------
// Wait for new serial port input
// Input: none
// Output: ASCII code for key typed
char UART_InChar(void){
  while((UART0FRR & UART_FR_RXFE) != 0);
  return((char)(UART0DRR & 0xFF));
}

//------------UART_OutChar------------
// Output 8-bit to serial port
// Input: letter is an 8-bit ASCII character to be transferred
// Output: none
void UART_OutChar(char data){
  while((UART0FRR & UART_FR_TXFF) != 0);
  UART0DRR = data;
}


//------------UART_OutString------------
// Output String (NULL termination)
// Input: pointer to a NULL-terminated string to be transferred
// Output: none
void UART_OutString(char *pt){
  while(*pt){
    UART_OutChar(*pt);
    pt++;
  }
}


//------------UART_InString------------
// Accepts ASCII characters from the serial port
//    and adds them to a string until <enter> is typed
//    or until max length of the string is reached.
// It echoes each character as it is inputted.
// If a backspace is inputted, the string is modified
//    and the backspace is echoed
// terminates the string with a null character
// uses busy-waiting synchronization on RDRF
// Input: pointer to empty buffer, size of buffer
// Output: Null terminated string
// -- Modified by Agustinus Darmawan + Mingjie Qiu --
void UART_InString(char *bufPt, uint16_t max) {
int length=0;
char character;
  character = UART_InChar();
  while(character != CR){
    if(character == BS){
      if(length){
        bufPt--;
        length--;
        UART_OutChar(BS);
      }
    }
    else if(length < max){
      *bufPt = character;
      bufPt++;
      length++;
      UART_OutChar(character);
    }
    character = UART_InChar();
  }
  *bufPt = 0;
}
 // END OF UART.c



//---------------------OutCRLF---------------------
// Output a CR,LF to UART to go to a new line
// Input: none
// Output: none
void OutCRLF(void){
  UART_OutChar(CR);
  UART_OutChar(LF);
}
//debug code
int main(void){
  char ch;
  char string[20];  // global to assist in debugging
  uint32_t n;

  UART_Init();              // initialize UART
  OutCRLF();
  for(ch='A'; ch<='Z'; ch=ch+1){// print the uppercase alphabet
    UART_OutChar(ch);
  }
  OutCRLF();
  UART_OutChar(' ');
  for(ch='a'; ch<='z'; ch=ch+1){// print the lowercase alphabet
    UART_OutChar(ch);
  }
	
	
  OutCRLF();
  UART_OutChar('-');
  UART_OutChar('-');
  UART_OutChar('>');
  while(1){
    UART_OutString("InString: ");
    UART_InString(string,19);
		UART_OutChar(LF);
    UART_OutString(" OutString="); UART_OutString(string); OutCRLF();
  }
}