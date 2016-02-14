#include "Display.h"

const int32_t CircleX[180] = {115, 115, 115, 114, 114, 114, 114, 113, 113, 112, 112, 111, 110, 110, 109, 108, 107, 106, 105, 104, 
	103, 102, 100, 99, 98, 96, 95, 94, 92, 91, 89, 88, 86, 84, 83, 81, 79, 78, 76, 74, 72, 71, 69, 67, 65, 64, 62, 60, 58, 56, 55, 53, 
	51, 49, 48, 46, 44, 43, 41, 39, 38, 36, 35, 33, 32, 31, 29, 28, 27, 25, 24, 23, 22, 21, 20, 19, 18, 17, 17, 16, 15, 15, 14, 14, 13, 
	13, 13, 13, 12, 12, 12, 12, 12, 13, 13, 13, 13, 14, 14, 15, 15, 16, 17, 17, 18, 19, 20, 21, 22, 23, 24, 25, 27, 28, 29, 31, 32, 33, 
	35, 36, 38, 39, 41, 43, 44, 46, 48, 49, 51, 53, 55, 56, 58, 60, 62, 64, 65, 67, 69, 71, 72, 74, 76, 78, 79, 81, 83, 84, 86, 88, 
	89, 91, 92, 94, 95, 96, 98, 99, 100, 102, 103, 104, 105, 106, 107, 108, 109, 110, 110, 111, 112, 112, 113, 113, 114, 114, 114, 114, 115, 115};
	
	const int32_t CircleY[180] = {88, 86, 84, 82, 80, 79, 77, 75, 73, 72, 70, 68, 67, 65, 63, 62, 60, 59, 57, 56, 55, 53, 52, 51, 49, 48, 
		47, 46, 45, 44, 43, 42, 41, 41, 40, 39, 39, 38, 38, 37, 37, 37, 37, 36, 36, 36, 36, 36, 37, 37, 37, 37, 38, 38, 39, 39, 40, 41, 41, 
		42, 43, 44, 45, 46, 47, 48, 49, 51, 52, 53, 55, 56, 57, 59, 60, 62, 63, 65, 67, 68, 70, 72, 73, 75, 77, 79, 80, 82, 84, 86, 88, 89, 
		91, 93, 95, 96, 98, 100, 102, 103, 105, 107, 108, 110, 112, 113, 115, 116, 118, 119, 120, 122, 123, 124, 126, 127, 128, 129, 130, 
		131, 132, 133, 134, 134, 135, 136, 136, 137, 137, 138, 138, 138, 138, 139, 139, 139, 139, 139, 138, 138, 138, 138, 137, 137, 136, 
		136, 135, 134, 134, 133, 132, 131, 130, 129, 128, 127, 126, 124, 123, 122, 120, 119, 118, 116, 115, 113, 112, 110, 108, 107, 105, 
		103, 102, 100, 98, 96, 95, 93, 91, 89};
	
		//x is 115 - 12 = 103 / 2 = 51.5, mid 63.5
		//y is 139 - 36, mid 87.5
		//cos(30 = 25.75
		//cos60 = 42.05
		
		//x is 89.25 - 37.75, mid 63.5
		//y is 113.25-61.75, mid 87.5
		//cos(30) = 22.3
		//cos(60) = 12.875
		const int32_t hourX[13] = {0, 76, 85, 89, 85, 76, 63, 51, 41, 38, 41, 51, 63 };
			
		const int32_t hourY[13] = {0, 65, 75, 88, 100, 109, 113, 109, 100, 88, 75, 65, 61};
		

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
		ST7735_DrawPixel(CircleX[i], CircleY[i] - 2, color);
	}
}

void drawClockHands(uint32_t hour, uint32_t minute, char fillHand){
	int32_t minIndex;
	uint16_t color1, color2;
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
	ST7735_Line(64, 84, CircleX[minIndex], CircleY[minIndex] - 2, color1);
	ST7735_Line(64, 84, hourX[hour], hourY[hour] -2, color2);
}

void drawDigTime(uint32_t hour, uint32_t minute, char AMPM){
	uint32_t hour0, min0;
	ST7735_SetCursor(8, 14);
	hour0 = hour % 10;
	hour /= 10;
	min0 = minute % 10;
	minute /= 60;
	if (!AMPM){
	printf("%d%d:%d%d AM", hour, hour0, minute, min0);
	}
	else{
	printf("%d%d:%d%d PM", hour, hour0, minute, min0);	
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

