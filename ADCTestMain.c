// ADCTestMain.c
// Runs on TM4C123
// This program periodically samples ADC channel 0 and stores the
// result to a global variable that can be accessed with the JTAG
// debugger and viewed with the variable watch feature.
// Daniel Valvano
// September 5, 2015

/* This example accompanies the book
   "Embedded Systems: Real Time Interfacing to Arm Cortex M Microcontrollers",
   ISBN: 978-1463590154, Jonathan Valvano, copyright (c) 2015

 Copyright 2015 by Jonathan W. Valvano, valvano@mail.utexas.edu
    You may use, edit, run or distribute this file
    as long as the above copyright notice remains
 THIS SOFTWARE IS PROVIDED "AS IS".  NO WARRANTIES, WHETHER EXPRESS, IMPLIED
 OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
 MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
 VALVANO SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL,
 OR CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 For more information about my classes, my research, and my books, see
 http://users.ece.utexas.edu/~valvano/
 */

// center of X-ohm potentiometer connected to PE3/AIN0
// bottom of X-ohm potentiometer connected to ground
// top of X-ohm potentiometer connected to +3.3V 
#include <stdint.h>
#include <stdlib.h>
#include "ADCSWTrigger.h"
#include "tm4c123gh6pm.h"
#include "PLL.h"
#include "ST7735.h"

#define PF2             (*((volatile uint32_t *)0x40025010))
#define PF1             (*((volatile uint32_t *)0x40025008))
void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts
long StartCritical (void);    // previous I bit, disable interrupts
void EndCritical(long sr);    // restore I bit to previous value
void WaitForInterrupt(void);  // low power mode

volatile uint32_t ADCvalue;
uint32_t first, second, median;
uint32_t times[1000];
uint32_t values[1000];
uint32_t PMFgraph[101];
uint32_t i = 0;
uint32_t k = 127;
uint32_t canrun = 0;
// This debug function initializes Timer0A to request interrupts
// at a 100 Hz frequency.  It is similar to FreqMeasure.c.
void Timer0A_Init100HzInt(void){
  volatile uint32_t delay;
  //DisableInterrupts();
  // **** general initialization ****
  SYSCTL_RCGCTIMER_R |= 0x01;      // activate timer0
  delay = SYSCTL_RCGCTIMER_R;      // allow time to finish activating
  TIMER0_CTL_R &= ~TIMER_CTL_TAEN; // disable timer0A during setup
  TIMER0_CFG_R = 0;                // configure for 32-bit timer mode
  // **** timer0A initialization ****
                                   // configure for periodic mode
  TIMER0_TAMR_R = TIMER_TAMR_TAMR_PERIOD; //x02
  TIMER0_TAILR_R = 799999;         // start value for 100 Hz interrupts
  TIMER0_IMR_R |= TIMER_IMR_TATOIM;// enable timeout (rollover) interrupt, x01
  TIMER0_ICR_R = TIMER_ICR_TATOCINT;// clear timer0A timeout flag, x01
  TIMER0_CTL_R |= TIMER_CTL_TAEN;  // enable timer0A 32-b, periodic, interrupts, x01
  // **** interrupt initialization ****
                                   // Timer0A=priority 2
  NVIC_PRI4_R = (NVIC_PRI4_R&0x00FFFFFF)|0x40000000; // top 3 bits
  NVIC_EN0_R |= 1<<19;              // enable interrupt 19 in NVIC
}
void Timer0A_Handler(void){
  TIMER0_ICR_R = TIMER_ICR_TATOCINT;    // acknowledge timer0A timeout
	 PF2 ^= 0x04;                   // profile
	first = TIMER1_TAR_R;
  ADCvalue = ADC0_InSeq3();
	second = TIMER1_TAR_R;
	first = first - second;
	//PF2 ^= 0x04;
	if (i < 1000){
		times[i] = first;
		values[i] = ADCvalue;
		i += 1;
	}
	else{
		canrun = 1;
	}
	 PF2 ^= 0x04;                   // profile
}

void Timer1_Init(void){
	 volatile uint32_t delay;
	//DisableInterrupts();
  SYSCTL_RCGCTIMER_R |= 0x02;   // 0) activate TIMER1
	  delay = SYSCTL_RCGCTIMER_R;  //allow timer1 to finish activating
  //PeriodicTask = task;          // user function
  TIMER1_CTL_R = 0x00000000;    // 1) disable TIMER1A during setup
  TIMER1_CFG_R = 0x00000000;    // 2) configure for 32-bit mode
	TIMER1_TAMR_R = 0x00000002;   // 3) configure for periodic mode, default down-count settings
  TIMER1_TAILR_R = 0xFFFFFFFF;    // 4) reload value
  TIMER1_TAPR_R = 0;            // 5) bus clock resolution
  TIMER1_ICR_R = 0x00000001;    // 6) clear TIMER1A timeout flag
  //TIMER1_IMR_R = 0x00000001;    // 7) arm timeout interrupt
  NVIC_PRI5_R = (NVIC_PRI5_R&0xFFFF00FF)|0x00008000; // 8) priority 4
// interrupts enabled in the main program after all devices initialized
// vector number 37, interrupt number 21
  NVIC_EN0_R = 1<<21;           // 9) enable IRQ 21 in NVIC
  TIMER1_CTL_R = 0x00000001;    // 10) enable TIMER1A
}

void Timer2_Init(void){
	 volatile uint32_t delay;
	//DisableInterrupts();
  SYSCTL_RCGCTIMER_R |= 0x04;   // 0) activate TIMER2
	  delay = SYSCTL_RCGCTIMER_R;  //allow timer1 to finish activating
  //PeriodicTask = task;          // user function
  TIMER2_CTL_R = 0x00000000;    // 1) disable TIMER1A during setup
  TIMER2_CFG_R = 0x00000000;    // 2) configure for 32-bit mode
	TIMER2_TAMR_R = 0x00000002;   // 3) configure for periodic mode, default down-count settings
  TIMER2_TAILR_R = 0xFFFFFFF;    // 4) reload value, 10Khz
  TIMER2_TAPR_R = 0;            // 5) bus clock resolution
  TIMER2_ICR_R = 0x00000001;    // 6) clear TIMER1A timeout flag
  TIMER2_IMR_R = 0x00000001;    // 7) arm timeout interrupt
  NVIC_PRI5_R = (NVIC_PRI5_R&0x00FFFFFF)|0x20000000; // 8) priority 1
// interrupts enabled in the main program after all devices initialized
// vector number 37, interrupt number 21
  NVIC_EN0_R |= 1<<23;           // 9) enable IRQ 23 in NVIC
  TIMER2_CTL_R = 0x00000001;    // 10) enable TIMER1A
}

void Timer3_Init(void){
	volatile uint32_t delay;
  SYSCTL_RCGCTIMER_R |= 0x08;   // 0) activate TIMER3
  //PeriodicTask = task;          // user function
	delay = SYSCTL_RCGCTIMER_R;  //allow timer1 to finish activating
  TIMER3_CTL_R = 0x00000000;    // 1) disable TIMER3A during setup
  TIMER3_CFG_R = 0x00000000;    // 2) configure for 32-bit mode
  TIMER3_TAMR_R = 0x00000002;   // 3) configure for periodic mode, default down-count settings
  TIMER3_TAILR_R = 0x79999;    // 4) reload value
  TIMER3_TAPR_R = 0;            // 5) bus clock resolution
  TIMER3_ICR_R = 0x00000001;    // 6) clear TIMER3A timeout flag
  TIMER3_IMR_R = 0x00000001;    // 7) arm timeout interrupt
  NVIC_PRI8_R = (NVIC_PRI8_R&0x00FFFFFF)|0x80000000; // 8) priority 1
// interrupts enabled in the main program after all devices initialized
// vector number 51, interrupt number 35
  NVIC_EN1_R = 1<<(35-32);      // 9) enable IRQ 35 in NVIC
  TIMER3_CTL_R = 0x00000001;    // 10) enable TIMER3A
}

void Timer3A_Handler(void){
	TIMER3_ICR_R = TIMER_ICR_TATOCINT;// acknowledge TIMER3A timeout
	//PF1 = (PF1*12345678)/1234567+0x02;
	ADC0_InSeq3();
	k += 1;
}

void ST7735_Line(uint32_t x1, uint32_t y1, uint32_t x2, uint32_t y2, uint16_t color){
	int32_t moveX, moveY, iX, iY, margin;
	if (x1 > 128) x1 = 128;
	if (x2 > 128) x1 = 128;
	if (y1 > 160) y1 = 160;
	if (y2 > 160) y2 = 160;
	moveX = (x2 - x1);
	moveY = (y2 - y1);
	iX = ((moveX > 0) - (moveX < 0));
	iY = ((moveY > 0) - (moveY < 0));
	moveX = abs(moveX) << 1;
	moveY = abs(moveY) << 1;
  ST7735_DrawPixel(x1, y1, color);
	
	if (moveX >= moveY){
		margin = moveY - (moveX >> 1);
		while (x1 != x2){
			if ((margin >= 0) && (margin || (iX > 0))){
				margin -= moveX;
				y1 += iY;
			}		
			margin += moveY;
			x1 += iX;
			ST7735_DrawPixel(x1, y1, color);
		}
	}
	else{
		margin = moveX - (moveY >> 1);
		while (y1 != y2){
			if ((margin >= 0) && (margin || (iY > 0))){
				margin -= moveY;
				x1 += iX;
			}		
			margin += moveX;
			y1 += iY;
			ST7735_DrawPixel(x1, y1, color);
		}
	}
}
int main(void){
	uint32_t timejitter, j, min, max, outsidecount;
	int32_t PMFtemp;
  PLL_Init(Bus80MHz);                   // 80 MHz
  SYSCTL_RCGCGPIO_R |= 0x20;            // activate port F
  ADC0_InitSWTriggerSeq3_Ch9();         // allow time to finish activating
  
  GPIO_PORTF_DIR_R |= 0x06;             // make PF2, PF1 out (built-in LED)
  GPIO_PORTF_AFSEL_R &= ~0x06;          // disable alt funct on PF2, PF1
  GPIO_PORTF_DEN_R |= 0x06;             // enable digital I/O on PF2, PF1
                                        // configure PF2 as GPIO
  GPIO_PORTF_PCTL_R = (GPIO_PORTF_PCTL_R&0xFFFFF00F)+0x00000000;
  GPIO_PORTF_AMSEL_R = 0;               // disable analog functionality on PF
  PF2 = 0;                      // turn off LED
	DisableInterrupts();
	Timer3_Init();
	Timer1_Init();               // set up Timer1A
	Timer0A_Init100HzInt();
	
  EnableInterrupts();
  while(!canrun){
    //PF1 ^= 0x02;
		GPIO_PORTF_DATA_R ^= 0x02;
    	//PF1 = (PF1*12345678)/1234567+0x02;	
		//k += 1;
	}
	min = times[0];
	max = times[0];
	median = 186; //change this value depending on whatever
	median = values[0];
	outsidecount = 0;
	for (j = 1; j < k; j += 1){
		PMFgraph[j] = 0;
	}
	for (j = 1; j < 10; j += 1){
		median += values[j];
	}
	median = median / 10;
	for (j = 1; j < 1000; j += 1){
		if (times[j] > max){
			max = times[j];
		}
		if (times[j] < min){
			min = times[j];
		}
		PMFtemp = ((k - 1) / 2) + values[j] - median;
		if (PMFtemp < 0 || PMFtemp >= k){
		  outsidecount += 1;
		}			
		else{
		  PMFgraph[PMFtemp] += 1;
		}
	}
	timejitter = max - min;
	ST7735_InitR(INITR_REDTAB);
	ST7735_PlotClear(0,160);
	ST7735_Line(4, 125, 10, 40, ST7735_BLACK);
	for (j = 0; j < k; j += 1){
		ST7735_PlotBar(PMFgraph[j] * 5);
		ST7735_PlotNext();
		//ST7735_PlotNext();
		//ST7735_PlotNext();
	}
}

