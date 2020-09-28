void SystemInit()
{}

#define SYSCTL_RCGCUART_R  	(*((volatile unsigned long *) 0x400FE618))
#define SYSCTL_RCGCGPIO_R		(*((volatile unsigned long *) 0x400FE608))

#define GPIO_PORTD_AFSEL_R   (*((volatile unsigned long *) 0x40007420))
#define GPIO_PORTD_PCTL_R    (*((volatile unsigned long *) 0x4000752C))
#define GPIO_PORTD_DEN_R 		(*((volatile unsigned long *) 0x4000751C))
#define GPIO_PORTD_DIR_R  	 (*((volatile unsigned long *) 0x40007400))
#define GPIO_PORTD_LOCK_R		(*((volatile unsigned long *) 0x40007520))
#define GPIO_PORTD_CR_R			(*((volatile unsigned long *) 0x40007524))

#define UART2_CTL_R			(*((volatile unsigned long *) 0x4000E030))
#define UART2_IBRD_R 		(*((volatile unsigned long *) 0x4000E024))
#define UART2_FBRD_R 		(*((volatile unsigned long *) 0x4000E028))
#define UART2_LCRH_R 		(*((volatile unsigned long *) 0x4000E02C))
#define UART2_CC_R		(*((volatile unsigned long *) 0x4000EFC8))
#define UART2_FR_R			(*((volatile unsigned long *) 0x4000E018))
#define UART2_DR_R 			(*((volatile unsigned long *) 0x4000E000))

#define UART2_CLK_EN 		0x04
#define GPIO_PORTD_CLK_EN		0x08
#define GPIO_PORTD_UART2_CFG  0x0C0

#define GPIO_PCTL_PD6_U2RX  0x01000000
#define GPIO_PCTL_PD7_U2TX 0x10000000
#define GPIO_PORTD_UNLOCK_CR 0x4C4F434B
#define GPIO_PORTD_CR_EN	0x000000FF

#define UART_CS_SysClk			0x00000000
#define UART_CS_PIOSC			0x00000005
#define UART_LCRH_WLEN_8  0x00000060
#define UART_LCRH_FEN  0x00000010
#define UART_FR_TXFF  0x00000020
#define UART_FR_RXFE	0x00000010
#define UART_CTL_UARTEN  0x00000001
#define UART_LB_EN	0x00000080


unsigned char UART_Rx(void);
void UART_Tx(unsigned char data);
void UART_Tx_String(char *pt);
void UART_Rx_String(char *bufPt, unsigned short max);

void UART_Init(void){
	SYSCTL_RCGCUART_R |= UART2_CLK_EN;
	SYSCTL_RCGCGPIO_R |= GPIO_PORTD_CLK_EN;
	
	GPIO_PORTD_LOCK_R = GPIO_PORTD_UNLOCK_CR;
	GPIO_PORTD_CR_R |= GPIO_PORTD_CR_EN;
	
	GPIO_PORTD_DEN_R |= GPIO_PORTD_UART2_CFG;
	GPIO_PORTD_AFSEL_R |= GPIO_PORTD_UART2_CFG;
	GPIO_PORTD_PCTL_R |= (GPIO_PCTL_PD6_U2RX | GPIO_PCTL_PD7_U2TX);
	
	UART2_CTL_R &= ~(UART_CTL_UARTEN);
	UART2_IBRD_R=8;
	UART2_FBRD_R=44;
	UART2_LCRH_R = (UART_LCRH_WLEN_8 |UART_LCRH_FEN);
	UART2_CC_R = UART_CS_SysClk;
	//UART2_CTL_R= UART_LB_EN;
	UART2_CTL_R |= UART_CTL_UARTEN;
}

unsigned char UART_Rx(void){
	while ((UART2_FR_R & UART_FR_RXFE) !=0);
	return ((unsigned char) (UART2_DR_R & 0xFF));
}

void UART_Rx_String(char *pt, unsigned short max) {
	int length =0;
	char character;
	character = UART_Rx();
	if(length <max){
		*pt=character;
		pt++;
		length++;
		UART_Tx(character);
	}
	*pt=0;
}

void UART_Tx(unsigned char data){
	while ((UART2_FR_R & UART_FR_TXFF) !=0);
	UART2_DR_R = data;
}

void UART_Tx_String (char *pt){
	while (*pt){
		UART_Tx(*pt);
		pt++;
	}
}

int main(void)
{
	char string [17];
	UART_Init();
	
	while (1){
		UART_Tx_String("Enter text: ");
		UART_Rx_String(string,16);
	}
}
	