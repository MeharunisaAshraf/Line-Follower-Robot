void SystemInit()
{}

#define SYSCTL_RCGCGPIO_R 	(*((volatile unsigned long *)0x400FE608))
#define SYSCTL_RCGCADC_R 	(*((volatile unsigned long *)0x400FE638))	

#define GPIO_PORTA_DATA_R 	(*((volatile unsigned long *)0x400043FC))
#define GPIO_PORTA_DIR_R 	(*((volatile unsigned long *)0x40004400))	
#define GPIO_PORTA_DEN_R 	(*((volatile unsigned long *)0x4000451C))

#define GPIO_PORTB_DATA_R 	(*((volatile unsigned long *)0x400053FC))
#define GPIO_PORTB_DIR_R 	(*((volatile unsigned long *)0x40005400))	
#define GPIO_PORTB_DEN_R 	(*((volatile unsigned long *)0x4000551C))

#define GPIO_PORTF_DATA_R 	(*((volatile unsigned long *)0x400253FC))
#define GPIO_PORTF_DIR_R 	(*((volatile unsigned long *)0x40025400))	
#define GPIO_PORTF_DEN_R 	(*((volatile unsigned long *)0x4002551C))

#define ADC_BASE 	0x40038000

#define ADC0_PC_R 			*(volatile long *)(ADC_BASE +0xFC4)
#define ADC0_SSPRI_R		*(volatile long *)(ADC_BASE+0x020)
#define ADC0_ACTSS_R  		*(volatile long *)(ADC_BASE+0x000)
#define ADC0_IM_R  			*(volatile long *)(ADC_BASE+0x008)
#define ADC0_RIS_R			*(volatile long *)(ADC_BASE +0x004)
#define ADC0_ISC_R			*(volatile long *)(ADC_BASE +0x00C)
#define ADC0_SAC_R			*(volatile long *)(ADC_BASE +0x030)
#define ADC0_PSSI_R			*(volatile long *)(ADC_BASE+0x028)
#define ADC0_SSCTL3_R		*(volatile long *)(ADC_BASE+0x0A4)
#define ADC0_SSFIFO3_R		*(volatile long *)(ADC_BASE +0x0A8)

unsigned char Lookup_7Seg_Disp[12]={0xC0,0xF9,0xA4,0xB0,0x99, 0x92,0x82,0xF8,0x80,0x90,0xC6};
unsigned char Temperature_Value[3]={0,0,0xA};
unsigned char i,value=0;
unsigned int ADC_value = 0,temperature=0;


void ADC_Init(void)
{
	volatile unsigned long delay; 
	
	SYSCTL_RCGCGPIO_R |= 0x23;
	SYSCTL_RCGCADC_R |=0x01;
	
	delay=SYSCTL_RCGCADC_R;
	
	GPIO_PORTA_DIR_R |= 0x38;
	GPIO_PORTA_DEN_R |= 0x38;
	
	GPIO_PORTB_DIR_R |= 0xFF;
	GPIO_PORTB_DEN_R |=0xFF;
	GPIO_PORTB_DATA_R |=0xFF;
	
	GPIO_PORTF_DIR_R |=0x08;
	GPIO_PORTF_DEN_R |=0x08;
	GPIO_PORTF_DATA_R |=0x08;
	
	
	ADC0_PC_R &=0x00;
	ADC0_PC_R |= 0x01;
	ADC0_SSPRI_R |=0x3210;
	ADC0_ACTSS_R &= ~(0x08);
	ADC0_SSCTL3_R |= 0x0E;
	ADC0_SAC_R |= 0x04;
	ADC0_IM_R &= ~(0x08);
	
	ADC0_ACTSS_R |= 0x08;
}

void ADC_Temperature(void){
	ADC0_PSSI_R |=0x08;
	
	while ((ADC0_RIS_R & 0x08)==0);
	ADC_value = (ADC0_SSFIFO3_R & 0xFFF);
	
	temperature = (unsigned char)(147.5-(ADC_value*3.3*75)/4096);
	Temperature_Value[0]=temperature%10;
	Temperature_Value[1]=temperature%10;
	
	ADC0_ISC_R |= 0x08;
}

void Delay(unsigned long count) {
	int i;
	for(i=0;i<count;i++)
	{}
}

int main()
{
	ADC_Init();
	
	while(1)
	{
		for(i=0;i<=2;i++)
		{
			value=Temperature_Value[i];
			GPIO_PORTB_DATA_R=Lookup_7Seg_Disp[value];
			
			GPIO_PORTA_DATA_R = (0x38-(1<<(i+3)));
			Delay(1000);
		}
		ADC_Temperature();
	}
}
	
	
