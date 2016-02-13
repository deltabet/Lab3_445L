#include <stdint.h>
#include "tm4c123gh6pm.h"
#include "Systick.h"




const unsigned short SinWave[32] = {			
  8,9,11,12,13,14,14,15,15,15,14,			
  14,13,12,11,9,8,7,5,4,3,2,			
  2,1,1,1,2,2,3,4,5,7};		

	unsigned char nextPlay;
	unsigned long count;
// **************DAC_Init*********************
// Initialize 4-bit DAC, called once 
// Input: none
// Output: none
// Ports E 5-2
void DAC_Init(void){
	unsigned long volatile delay;
	SYSCTL_RCGC2_R |= 0x10;
	delay = SYSCTL_RCGC2_R;
	GPIO_PORTE_AMSEL_R &= ~0x3C;
	GPIO_PORTE_DIR_R |= 0x3C;
	GPIO_PORTE_AFSEL_R &= ~0x3C;
	GPIO_PORTE_DEN_R |= 0x3C;
	count = 0;
	
}

// **************DAC_Out*********************
// output to DAC
// Input: 4-bit data, 0 to 15 
// Output: none
void DAC_Out(unsigned long data){
	//GPIO_PORTA_DATA_R &= ~0x3C;
	data &= 0x0000000F;
	data = data << 2;
	GPIO_PORTE_DATA_R &= ~0x3C;
	GPIO_PORTE_DATA_R |= data;
}

void Timer1A_Handler(void){
	TIMER1_ICR_R = 0x00000001;
	if (play == 1){
		DAC_Out(SinWave[count]);
		count = (count + 1) & 0x01F;	
	}
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
  TIMER1_IMR_R = 0x00000001;    // 7) arm timeout interrupt
  NVIC_PRI5_R = (NVIC_PRI5_R&0xFFFF00FF)|0x00008000; // 8) priority 4
// interrupts enabled in the main program after all devices initialized
// vector number 37, interrupt number 21
  NVIC_EN0_R = 1<<21;           // 9) enable IRQ 21 in NVIC
  TIMER1_CTL_R = 0x00000001;    // 10) enable TIMER1A
}
//void Play(unsigned long musicName, unsigned long period){
//	TIMER0_TAILR_R = period;
//	nextPlay = musicName;
//	count = 0;
//}

/*
//Input/output: none
//Plays next vaule in sinewave array.
void Timer0A_Handler(void){
	TIMER0_ICR_R = 0x00000001;
//	switch(nextPlay){
//		case 0:
//			if (fastinvader[count] != 17)
//				DAC_Out(fastinvader[count]);
//			else
//				TIMER0_TAILR_R = 0xFFFFFFFF;
//			break;
//			
//	}
	if (play == 1){
		DAC_Out(SinWave[count]);
		count = (count + 1) & 0x01F;	
	}
}

//input/output: none
//Initalizes timer 0.
void Timer0_Init(void){ 
  unsigned long volatile delay;
  SYSCTL_RCGCTIMER_R |= 0x01;   // 0) activate timer0
  delay = SYSCTL_RCGCTIMER_R;
  TIMER0_CTL_R = 0x00000000;    // 1) disable timer0A during setup
  TIMER0_CFG_R = 0x00000000;    // 2) configure for 32-bit mode
  TIMER0_TAMR_R = 0x00000002;   // 3) configure for periodic mode, default down-count settings
  TIMER0_TAILR_R = 0xFFFFFFFF;    	    // 4) reload value
  TIMER0_TAPR_R = 0;            // 5) bus clock resolution
  TIMER0_ICR_R = 0x00000001;    // 6) clear timer2A timeout flag
  TIMER0_IMR_R = 0x00000001;    // 7) arm timeout interrupt
  NVIC_PRI4_R = (NVIC_PRI4_R&0x00FFFFFF)|0xA0000000; // 8) priority 5
// interrupts enabled in the main program after all devices initialized
// vector number 39, interrupt number 23
  NVIC_EN0_R |= 0x00080000;           // 9) enable IRQ 19 in NVIC
  TIMER0_CTL_R = 0x00000001;    // 10) enable timer2A
}
*/


