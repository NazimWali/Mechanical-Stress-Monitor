// Program to Determine Strain on Pipe Configuration
// Nazim Wali & Nick Deoki
// Senior Design I

#include "stm32f4xx.h"
#include <stdio.h>
#include <math.h> 

#define RS 0x20     /* PA5 mask for reg select */
#define EN 0x80     /* PA7 mask for enable */

//Delay Function
void delayMs(int n);

//LCD Functions
void LCD_nibble_write(char data, unsigned char control);
void LCD_command(unsigned char command);
void LCD_data(char data);
void LCD_init(void);
void PORTS_init(void);

//Function to write files
void FilePrint(double r, double strain, double stress); 

int main (void) {
    double result1;
		double result2;
		double voltage1;
		double voltage2;
		double voltageacross;
		double resis1;
		double resis2;
		double resis;
		double strain;
		double stress;
		char conv[4];
		char conv2[5];
//		double r1 = 119.6;
//		double r2 = 119.2;
//		double r3 = 119.3;
		double r1 = 350;
		double r2 = 350;
		double r3 = 350;
		int knownres = 119;
//		double r0 = 120.5;
		double r0 = 350;
		double sum1 = 0;
		double sum2 = 0;
		
		int flag = 0;
		double rstart = 0;
		
		/* initialize LCD controller */
    LCD_init();
	
    /* set up pin PA1 for analog input */
    RCC->AHB1ENR |=  1;	            /* enable GPIOA clock */
    //GPIOA->MODER |=  0xC;           /* PA1 analog */
	
		//set up Pin PA0
		GPIOA->MODER |= 0xF;

		GPIOA->PUPDR |= 0x00010000;
		GPIOA->MODER &= 0xFFFCFFFF;
		GPIOA->MODER |= 0x00040000;


//    /* setup ADC1 */
//    RCC->APB2ENR |= 0x00000100;     /* enable ADC1 clock */
//    ADC1->CR2 = 0;                  /* SW trigger */
//    ADC1->SQR3 = 1;                 /* conversion sequence starts at ch 1 */
//    ADC1->SQR1 = 0;                 /* conversion sequence length 1 */
//		//ADC1->SMPR2 = 0x00000038;
//    ADC1->CR2 |= 1;                 /* enable ADC1 */
			
			
			//setup ADC1 & ADC2
			RCC->APB2ENR |= 0x00000300;
			ADC1->CR2 = 0;
			ADC2->CR2 = 0;
			
			ADC1->SQR3 = 1;
			ADC1->SQR1 = 0;
			
			ADC2->SQR3 = 0;
			ADC2->SQR1 = 0;
			
			ADC1->CR2 |= 1;
			ADC2->CR2 |= 1;
			
    while (1) {
			
				for(int i = 0; i < 1000; i++){
					ADC1->CR2 |= 0x40000000;        /* start a conversion */
					while(!(ADC1->SR & 2)) {}       /* wait for conv complete */
					result1 = ADC1->DR;              /* read conversion result */
						
						sum1 += result1;
						
						
//					ADC2->CR2 |= 0x40000000;        /* start a conversion */
//					while(!(ADC2->SR & 2)) {}       /* wait for conv complete */
//					result2 = ADC2->DR;              /* read conversion result */
						
	//					sum2 += result2;
						
					
					
					delayMs(1);
				}
				
				result1 = sum1/1000;
			//	result2 = sum2/100;
				
				//voltage divider to resistance
				voltage1 = (double)result1/4095*5;								//Equation to convert to Voltage units //////////////////check
				//resis1 = (double)((knownres)*(1/((3.3/voltage1)-1)));			
				
//				voltage2 = (double)result2/4095*3.3;								
//				resis2 = (double)((knownres)*(1/((3.3/voltage2)-1)));
				
				//wheatstone to resistance
	//			voltageacross = voltage2 - voltage1;
	//			resis = r3*(((r2*3.3) - ((r1 + r2)*voltageacross))/((r1*3.3) + ((r1 + r2)*voltageacross)));
	//			resis += 3;
	//			voltageacross = (fabs(2.5 - voltage1));
	//		voltageacross = voltage1;
				voltageacross = (2.5 - voltage1); //3-9
	//			resis1 = r3*(((r2*3.3) - ((r1 + r2)*voltageacross))/((r1*3.3) + ((r1 + r2)*voltageacross)));
				resis1 = r3*(((r2*5) - ((r1 + r2)*voltageacross))/((r1*5) + ((r1 + r2)*voltageacross))); //3-9
				//resis = ((r1*3.3) - ((r1 + r2)*voltageacross));
				//resis = r3*(((r2(3.3))-(res1+r2)(voltageacross)));
				//resis = resis/((res1(3.3))-(res1+r2)(voltageacross))
				
				
				if(flag == 0){
							rstart = resis1;
							flag = 1;
											
					}
				
	//			strain = (double)((r0-resis1)/(2.11*r0));									//Equation to calculate strain
		//		strain = (double)((r0-resis1)/(2.155));	//3-9
					strain = (double)((rstart-resis1)/(2.155*rstart));	//3-9
				strain = (double)(strain*(1*pow(10,6)));
			//		strain = fabs(strain);
				//stress = (double)((70*pow(10,9))*strain);										//Equation to calculate stress
				stress = (double)((4)*strain);
				sum1 = 0;
				sum2 = 0;
				
				 /* clear LCD display */
        LCD_command(1);
        //delayMs(1000) hater;
					
				sprintf(conv, "%f", resis1);					//Use sprintf to store int results in a char array
				
				LCD_data('V');
				LCD_data('o');
				LCD_data('l');
				LCD_data(':');
				LCD_data(' ');
					
				for(int i = 0; i < 8; i++){						//For loop to output char array contents to LCD
					LCD_data(conv[i]);
					//delayMs(100);
				}
								
				sprintf(conv2, "%f", strain);					//Store in char array

				LCD_command(0xC0);										//Start print to second line of LCD
				
				LCD_data('S');
				LCD_data('t');
				LCD_data('r');
				LCD_data(':');
				LCD_data(' ');
				
				for(int i = 0; i < 10; i++){						//Print contents of char array
					LCD_data(conv2[i]);
					//delayMs(100);
				}
				
				sum1 = 0;
				result1 = 0;
				
    }
	}


void FilePrint(double r, double strain, double stress){
		FILE *values;
		values = (fopen("E:/values.txt", "w"));
		
		printf("Resistance: %lf\n", r);
		printf("Strain: %lf\n", strain);
		printf("Stress: %lf\n", stress);
		
		fclose(values);
	
}

/* initialize GPIOB/C then initialize LCD controller */
void LCD_init(void) {
    PORTS_init();

    delayMs(20);                /* LCD controller reset sequence */
    LCD_nibble_write(0x30, 0);
    delayMs(5);
    LCD_nibble_write(0x30, 0);
    delayMs(1);
    LCD_nibble_write(0x30, 0);
    delayMs(1);

    LCD_nibble_write(0x20, 0);  /* use 4-bit data mode */
    delayMs(1);
    LCD_command(0x28);          /* set 4-bit data, 2-line, 5x7 font */
    LCD_command(0x06);          /* move cursor right */
    LCD_command(0x01);          /* clear screen, move cursor to home */
    LCD_command(0x0F);          /* turn on display, cursor blinking */
}

void PORTS_init(void) {
    RCC->AHB1ENR |=  0x06;          /* enable GPIOB/C clock */
	
		RCC->AHB1ENR |=  1;                	/* enable GPIOA clock */

    /* PORTA 5 for LCD R/S */
    /* PORTA 7 for LCD EN */
    GPIOA->MODER &= ~0x0000CC00;    /* clear pin mode */
    GPIOA->MODER |=  0x00004400;    /* set pin output mode */
    GPIOA->BSRR = 0x00800000;       /* turn off EN */

    /* PB4-PB7 for LCD D4-D7, respectively. */
    GPIOB->MODER &= ~0x0000FF00;    /* clear pin mode */
    GPIOB->MODER |=  0x00005500;    /* set pin output mode */
}

void LCD_nibble_write(char data, unsigned char control) {
    /* populate data bits */
    GPIOB->BSRR = 0x00F00000;       /* clear data bits */
    GPIOB->BSRR = data & 0xF0;      /* set data bits */

    /* set R/S bit */
    if (control & RS)
        GPIOA->BSRR = RS;
    else
        GPIOA->BSRR = RS << 16;

    /* pulse E */
    GPIOA->BSRR = EN;
    delayMs(0);
    GPIOA->BSRR = EN << 16;
}

void LCD_command(unsigned char command) {
    LCD_nibble_write(command & 0xF0, 0);    /* upper nibble first */
    LCD_nibble_write(command << 4, 0);      /* then lower nibble */

    if (command < 4)
        delayMs(2);         /* command 1 and 2 needs up to 1.64ms */
    else
        delayMs(1);         /* all others 40 us */
}

void LCD_data(char data) {
    LCD_nibble_write(data & 0xF0, RS);      /* upper nibble first */
    LCD_nibble_write(data << 4, RS);        /* then lower nibble */

    delayMs(1);
}

/* 16 MHz SYSCLK */
void delayMs(int n) {
    int i;

    /* Configure SysTick */
    SysTick->LOAD = 16000;  /* reload with number of clocks per millisecond */
    SysTick->VAL = 0;       /* clear current value register */
    SysTick->CTRL = 0x5;    /* Enable the timer */

    for(i = 0; i < n; i++) {
        while((SysTick->CTRL & 0x10000) == 0) /* wait until the COUNTFLAG is set */
            { }
    }
    SysTick->CTRL = 0;      /* Stop the timer (Enable = 0) */
}