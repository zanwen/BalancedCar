#ifndef __INT_OLED_H
#define __INT_OLED_H

#include "Dri_SPI.h"
#include "stdlib.h"

//-----------------OLED端口定义----------------

#define OLED_RESET_LOW()  GPIOB->ODR &= ~GPIO_ODR_ODR0
#define OLED_RESET_HIGH() GPIOB->ODR |= GPIO_ODR_ODR0

#define OLED_DC_LOW()  GPIOB->ODR &= ~GPIO_ODR_ODR1
#define OLED_DC_HIGH() GPIOB->ODR |= GPIO_ODR_ODR1

#define OLED_CMD  0 // 写命令
#define OLED_DATA 1 // 写数据

void Int_OLED_ClearPoint(u8 x, u8 y);
void Int_OLED_ColorTurn(u8 i);
void Int_OLED_DisplayTurn(u8 i);
void Int_OLED_WR_Byte(u8 dat, u8 mode);
void Int_OLED_DisPlay_On(void);
void Int_OLED_DisPlay_Off(void);
void Int_OLED_Refresh(void);
void Int_OLED_Clear(void);
void Int_OLED_DrawPoint(u8 x, u8 y, u8 t);
void Int_OLED_DrawLine(u8 x1, u8 y1, u8 x2, u8 y2, u8 mode);
void Int_OLED_DrawCircle(u8 x, u8 y, u8 r);
void Int_OLED_ShowChar(u8 x, u8 y, u8 chr, u8 size1, u8 mode);
void Int_OLED_ShowChar6x8(u8 x, u8 y, u8 chr, u8 mode);
void Int_OLED_ShowString(u8 x, u8 y, u8 *chr, u8 size1, u8 mode);
void Int_OLED_ShowNum(u8 x, u8 y, u32 num, u8 len, u8 size1, u8 mode);
void Int_OLED_ShowChinese(u8 x, u8 y, u8 num, u8 size1, u8 mode);
void Int_OLED_ScrollDisplay(u8 num, u8 space, u8 mode);
void Int_OLED_ShowPicture(u8 x, u8 y, u8 sizex, u8 sizey, u8 BMP[], u8 mode);
void Int_OLED_Init(void);

void Int_OLED_Test(void);

#endif
