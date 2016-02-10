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

#define PF2             (*((volatile uint32_t *)0x40025010))
#define PF1             (*((volatile uint32_t *)0x40025008))
#define CLOCK_SPEED			80000000
	
void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts
long StartCritical (void);    // previous I bit, disable interrupts
void EndCritical(long sr);    // restore I bit to previous value
void WaitForInterrupt(void);  // low power mode

struct Clock {
	uint8_t seconds;
	uint8_t minutes;
	uint8_t hours;
};

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
  TIMER0_TAILR_R = (CLOCK_SPEED / 60) - 1; // start value for 60 Hz interrupts subtract one because we count down to 0
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
	static struct Clock clock;
	clock.seconds += 1;
	if(clock.seconds == 60){
		clock.seconds = 0;
		clock.minutes += 1;
		if(clock.minutes == 60){
			clock.minutes = 0;
			clock.hours += 1;
		}
		if(clock.hours == 13){
			clock.hours = 1;
		}
	}
	 PF2 ^= 0x04;                   // profile
	
	
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
	Timer0A_Init100HzInt();
	EnableInterrupts();
	
}
