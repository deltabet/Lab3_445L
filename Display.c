#include "Display.h"

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

void ST7735_Circle(uint16_t color){
	int i;
	for (i = 0; i < 180; i += 1){
		ST7735_DrawPixel(CircleX[i], CircleY[i], color);
	}
}

void drawClockHands(uint32_t hour, uint32_t minute, char fillHand){
	int32_t minIndex;
	uint16_t color1, color2
	if (fillHand){
		color1 = ST7735_BLACK;
		color2 = ST7735_BLUE;
	}
	else{
		color1 = ST7735_WHITE;
		color2 = ST7735_WHITE;
	}
	minIndex = -minute;
	minIndex = ((minIndex + 75) % 60) * 3;
	ST7735_Line(64, 80, CircleX[minIndex], CircleY[minIndex], color1);
	ST7735_Line(64, 80, hourX[hour], hourY[hour], color2);
}

void drawDigTime(uint32_t hour, uint32_t minute, char AMPM){
	uint32_t hour0, min0;
	ST7735_SetCursor(8, 14);
	hour0 = hour % 10;
	hour /= 10;
	min0 = minute % 10;
	minute /= 60;
	if (!AMPM){
	printf("%d%d:%d%d AM", hour0, hour, min0, minute);
	}
	else{
	printf("%d%d:%d%d PM", hour0, hour, min0, minute);	
	}
	
}


void drawMode(char mode){
	ST7735_SetCursor(0,0);
	printf("                   ");
	ST7735_SetCursor(0,0);
	if (mode == 0){
		printf("Main Menu");
	}
	else if (mode == 1){
		printf("Set Time");
	}
	else if (mode == 2){
		printf("Set Alarm");
		/*if (1){
			
		}
		else{
			printf("Alarm set");
		}*/
	}
	else if (mode == 4){
		printf("Set Tone");
	}
	else if (mode == 5){
		printf("ALARM!");
	}
}

void drawInfo(char mode, char alarmSet, char songSet){
	ST7735_SetCursor(0,1);
	printf("                   ");
	ST7735_SetCursor(0,1);
	if (mode == 2){
		if (!alarmSet){
			printf("Alarm not set");
		}
		else{
			printf("Alarm set");
		}
	}
	else if (mode == 3){
		if (!songSet){
			printf("Fruits Candy");
		}
		else if (songSet == 1){
			printf("Monotone");
		}
		else if (songSet == 2){
			printf("Set");
		}
	}
	else if (mode == 1 && alarmSet == 2){
		printf("Time set");
	}
	else{}
}