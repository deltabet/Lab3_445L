#include <stdint.h>
#include "tm4c123gh6pm.h"
#include "FruitsCandy.h"

unsigned long ADCMailbox;      //input from ADC
unsigned char averageCount;    
unsigned long average;
unsigned long Button;						//if PE0 pressed or not
unsigned char play;							//play music or not.
unsigned long note;							//which note
unsigned long noteCount;				//count vaule of note memory array
unsigned char noteTime;					//duration of current note.
unsigned char songSet;


void Timer2_Init(void){
	 volatile uint32_t delay;
	//DisableInterrupts();
  SYSCTL_RCGCTIMER_R |= 0x04;   // 0) activate TIMER2
	  delay = SYSCTL_RCGCTIMER_R;  //allow timer1 to finish activating
  //PeriodicTask = task;          // user function
  TIMER2_CTL_R = 0x00000000;    // 1) disable TIMER1A during setup
  TIMER2_CFG_R = 0x00000000;    // 2) configure for 32-bit mode
	TIMER2_TAMR_R = 0x00000002;   // 3) configure for periodic mode, default down-count settings
  TIMER2_TAILR_R = 2083333;    // 4) reload value, 10Khz
  TIMER2_TAPR_R = 0;            // 5) bus clock resolution
  TIMER2_ICR_R = 0x00000001;    // 6) clear TIMER1A timeout flag
  TIMER2_IMR_R = 0x00000001;    // 7) arm timeout interrupt
  NVIC_PRI5_R = (NVIC_PRI5_R&0x00FFFFFF)|0x20000000; // 8) priority 1
// interrupts enabled in the main program after all devices initialized
// vector number 37, interrupt number 21
  NVIC_EN0_R |= 1<<23;           // 9) enable IRQ 23 in NVIC
  TIMER2_CTL_R = 0x00000001;    // 10) enable TIMER1A
}
void Timer2A_Handler(void){
	TIMER2_ICR_R = TIMER_ICR_TATOCINT;// acknowledge TIMER3A timeout
	if ((GPIO_PORTE_DATA_R & 0x01) == 1)
			Button = 1;
	else
			Button = 0;
	if (play == 1){
		if (!songSet){
		if (noteTime == 0){
			if (fruitscandy[noteCount] != 0){
				if (fruitTime[noteCount] == 0){
					noteCount++;
				}
				TIMER1_TAILR_R = fruitscandy[noteCount];
				noteTime = fruitTime[noteCount]; 
				noteCount++;
			}
			if (fruitscandy[noteCount] == 0){
				noteCount = 0;
				noteTime = 0;
			}
		}
		else{
			noteTime--;
		}
	}
		else{
			TIMER1_TAILR_R = 4778;
		}
	}
}
/*
//Input: none
//Output: none
//Initalizes systick to sample 40 hz.
void Systick_Init(void){
	NVIC_ST_CTRL_R = 0;         // disable SysTick during setup
  NVIC_ST_RELOAD_R = 2083333;// reload value
  NVIC_ST_CURRENT_R = 0;      // any write to current clears it
  NVIC_SYS_PRI3_R = (NVIC_SYS_PRI3_R&0x00FFFFFF)|0x20000000; // priority 1
  NVIC_ST_CTRL_R = 0x0007; // enable SysTick with core clock and interrupts
	noteCount = 0;
	noteTime = 0;
	averageCount = 0;
	average = 0;
}

//Input: none
//Output: none
//Interrupt for systick. Read ADC.
//Handle timing for music. For memory array with time, if time counts down, then switch to next note.
void SysTick_Handler(void){
	if ((GPIO_PORTE_DATA_R & 0x01) == 1)
			Button = 1;
	else
			Button = 0;
	if (play == 1){
		if (noteTime == 0){
			if (fruitscandy[noteCount] != 0){
				if (fruitTime[noteCount] == 0){
					noteCount++;
				}
				TIMER0_TAILR_R = fruitscandy[noteCount];
				noteTime = fruitTime[noteCount]; 
				noteCount++;
			}
			if (fruitscandy[noteCount] == 0){
				noteCount = 0;
				noteTime = 0;
			}
		}
		else{
			noteTime--;
		}
	}
}
*/


