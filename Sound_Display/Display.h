#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include "ST7735.h"




		
void ST7735_Line(uint32_t x1, uint32_t y1, uint32_t x2, uint32_t y2, uint16_t color);
void	ST7735_Circle(uint16_t color);
void drawClockHands(uint32_t hour, uint32_t minute, char fillHand);
void drawDigTime(uint32_t hour, uint32_t minute, char AMPM);
void drawClock(uint32_t hour, uint32_t minute);
void drawMode(char mode);
void drawInfo(char mode, char alarmSet, char songSet);
		


