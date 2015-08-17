#ifndef _KEY_H_
#define _KEY_H_


#define   K1     gpio_get (PTE0)
#define   K2     gpio_get (PTE1)
#define   K3     gpio_get (PTE2)
#define   K4     gpio_get (PTE3)



/****四个按键初始化*****/
void Key_Init();

void Menu_display(); 
void Keyscan();
void Display();

/******按键菜单控制程序******/
void Key_Control();

#endif