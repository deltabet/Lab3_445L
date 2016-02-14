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
#include "Systick.h"

#define PF2             (*((volatile uint32_t *)0x40025010))
#define PF1             (*((volatile uint32_t *)0x40025008))
	
const char MAINMENU = 0;
const char SETTIME = 1;
const char SETALARM = 2;
const char ALARMTONE = 3;
const char FRUITSCANDY = 0;
const char MONOTONE = 1;

uint32_t hour = 12;
uint32_t minute = 0;
uint32_t hourTemp = 12;
uint32_t minuteTemp = 0;
uint32_t alarmHour = 12;
uint32_t alarmMinute = 0;
char mode = 0;
char alarmSet = 0;
char alarmGoingOff = 0;
char songSetTemp = 0;
char curSong = 0;
char AMPM = 0;
char AMPMtemp = 0;
char alarmAMPM = 0;

int main(void){
  PLL_Init(Bus80MHz);                   // 80 MHz
	for (alarmAMPM = 1; alarmAMPM < 10; alarmAMPM += 1){
		alarmAMPM = alarmAMPM;
	}
	alarmAMPM = 0;
  //SYSCTL_RCGCGPIO_R |= 0x20;            // activate port F
  //ADC0_InitSWTriggerSeq3_Ch9();         // allow time to finish activating
  
  //GPIO_PORTF_DIR_R |= 0x06;             // make PF2, PF1 out (built-in LED)
  //GPIO_PORTF_AFSEL_R &= ~0x06;          // disable alt funct on PF2, PF1
  //GPIO_PORTF_DEN_R |= 0x06;             // enable digital I/O on PF2, PF1
                                        // configure PF2 as GPIO
  //GPIO_PORTF_PCTL_R = (GPIO_PORTF_PCTL_R&0xFFFFF00F)+0x00000000;
  //GPIO_PORTF_AMSEL_R = 0;               // disable analog functionality on PF
  //PF2 = 0;                      // turn off LED
	DisableInterrupts();
	DAC_Init();
	//Switch_Init();					//interrupts on switch press
	ST7735_InitR(INITR_REDTAB);
	ST7735_PlotClear(0,160);
		play = 0;
		//TIMER3_CTL_R = 0x00000000;
	
	ST7735_Circle(ST7735_BLACK);
	//drawClockHands(hour, minute, 1);
	drawDigTime(hour, minute, AMPM);
	ST7735_SetCursor(2, 14);
	printf("<");
	ST7735_SetCursor(18, 14);
	printf(">");
	drawClockHands(2,30,1);
	Timer0A_Init60HzInt();  //updates time every second
	Timer3_Init();	//sound
	Timer2_Init();
		play = 1;
		EnableInterrupts();	
		//TIMER3_CTL_R = 0x00000001;
	while(1){
		
	}
}

//These are not the actual functions, but templates to implement, possibly in "switch.c"

/*
void ModeButton(void){
	if (!alarmGoingOff){
	mode = (mode + 1) % 4;
	drawMode(mode);
	drawInfo(mode, alarmSet, songSet);
	if (mode == ALARMTONE){
		//play = 1;
		//TIMER1_CTL_R = 0x00000001;
	}
	else if (mode == SETALARM){
		alarmMinute = minute;
		alarmHour = hour;
		alarmAMPM = AMPM;
	}
	else if (mode == MAINMENU){
		drawDigTime(hour, minute, AMPM);
	}
	else if (mode == SETTIME){
		drawDigTime(hour, minute, AMPM);
		hourTemp = hour;
		minuteTemp = minute;
		AMPMtemp = AMPM;
	}
}
}

void ActivateButton(void){
	if (alarmGoingOff){
		alarmGoingOff = 0;
		//play = 0;
		//TIMER1_TAILR_R = 0xFFFFFFFF;
		//TIMER1_CTL_R = 0x00000000;
		drawMode(mode);
		drawInfo(mode, alarmSet, songSet);
	}
	else{
	if (mode == SETALARM){
		alarmSet = !alarmSet;
		drawInfo(mode, alarmSet, songSet);
	}
	else if (mode == ALARMTONE){
		curSong = songSet;
		drawInfo(mode, alarmSet, 2);
		//play = 0;
		//TIMER1_CTL_R = 0x00000000;
	}
	else if (mode == SETTIME){
		drawClockHands(hour, minute, 0);
		hour = hourTemp;
		minute = minuteTemp;
		AMPM = AMPMtemp;
		drawClockHands(hour, minute, 1);
		drawDigTime(hour, minute, AMPM);
		drawInfo(mode, 2, songSet);
	}
	}
}

void UpButton(void){
	if (!alarmGoingOff){
	if (mode == SETTIME){
		minuteTemp = (minuteTemp + 1) % 60;
		if (!minuteTemp){
			hourTemp = (hourTemp + 1) % 13;
			if (hourTemp == 0){
				hourTemp = 1;
				AMPMtemp = !AMPMtemp;
			}
		}
		drawDigTime(hourTemp, minuteTemp, AMPMtemp);
		drawInfo(mode, alarmSet, songSet);
	}
	else if (mode == SETALARM){
		alarmMinute = (alarmMinute + 1) % 60;
		if (!alarmMinute){
			alarmHour = (alarmHour + 1) % 13;
			if (alarmHour == 0){
				alarmHour = 1;
				AMPMtemp = !AMPMtemp;
			}
		}
		drawDigTime(alarmHour, alarmMinute, alarmAMPM);
		drawInfo(mode, alarmSet, songSet);
	}
	else if (mode == ALARMTONE){
		songSet = !songSet;
		drawInfo(mode, alarmSet, songSet);
	}
	}
}

void DownButton(void){
	if (!alarmGoingOff){
	if (mode == SETTIME){
		hourTemp = (hourTemp + 1) % 13;
		if (hourTemp == 0){
			hourTemp = 1;
			AMPMtemp = !AMPMtemp;
		}
		drawDigTime(hourTemp, minuteTemp, AMPMtemp);
		drawInfo(mode, alarmSet, songSet);
	}
	else if (mode == SETALARM){
		alarmHour = (alarmHour + 1) % 13;
			if (alarmHour == 0){
				alarmHour = 1;
				AMPMtemp = !AMPMtemp;
			}
		drawDigTime(alarmHour, alarmMinute, alarmAMPM);
			drawInfo(mode, alarmSet, songSet);
	}
	else if (mode == ALARMTONE){
		songSet = !songSet;
		drawInfo(mode, alarmSet, songSet);
	}
	}
}
*/

