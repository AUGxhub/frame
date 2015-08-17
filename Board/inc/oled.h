#ifndef	_LCD_H_
#define _LCD_H_

#define RESET  PTA16_OUT
#define DC     PTA17_OUT
#define D1     PTA15_OUT
#define D0     PTA14_OUT

 typedef unsigned char byte;
 typedef unsigned int word;
 extern byte beyond96x64[512];
 void LCD_WrDat(uint8 data);
 void LCD_Set_Pos(byte x, byte y);
 void LCD_Init(void);
 void LCD_CLS(void);
 void LCD_CLS_Later(void);
 void LCD_P6x8Str(byte x,byte y,byte ch[]);
 void LCD_P8x16Str(byte x,byte y,byte ch[]);
 void LCD_P6x8uint16(byte x,byte y,uint16 num);
 void LCD_P14x16Str(byte x,byte y,byte ch[]);
 void LCD_Print(byte x, byte y, byte ch[]);
 void LCD_PutPixel(byte x,byte y);
 void LCD_Rectangle(byte x1,byte y1,byte x2,byte y2,byte gif);
 void Draw_BMP(byte x0,byte y0,byte x1,byte y1,byte bmp[]); 
 void LCD_Fill(byte dat);
 void LED_PrintImage(uint8 *pucTable, uint16 usRowNum, uint16 usColumnNum);

//ÐÂ¼ÓµÄ 
void OLED_P8x16Str(uint8 x,uint8 y,uint8 t,uint8 ch[]);
void OLED_P14x16Str(uint8 x,uint8 y,uint8 t,uint8 ch[]);
void OLED_Print(uint8 x, uint8 y,uint8 mode, uint8 ch[]);
void LCD_P8x16uint16(byte x,byte y,uint16 num);
#endif