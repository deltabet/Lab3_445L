/*Connor Lewis and Taiyi Ouyang
Runs on TM4C123
This program runs the alarm clock functions
*/



#include <stdint.h>
#include <stdlib.h>
#include "ADCSWTrigger.h"
#include "tm4c123gh6pm.h"
#include "PLL.h"
#include "ST7735.h"
#include "Timer.h"
#include "Switch.h"
#include "Sound.h"
#include "Display.h"

#define PF2             (*((volatile uint32_t *)0x40025010))
#define PF1             (*((volatile uint32_t *)0x40025008))	

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
	Timer0A_Init60HzInt();  //updates time every second
	Switch_Init();					//interrupts on switch press
	EnableInterrupts();	
	while(1){
		Display();
		Sound();
	}
}
