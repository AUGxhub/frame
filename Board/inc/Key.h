#ifndef _KEY_H_
#define _KEY_H_


#define   K1     gpio_get (PTE0)
#define   K2     gpio_get (PTE1)
#define   K3     gpio_get (PTE2)
#define   K4     gpio_get (PTE3)



/****�ĸ�������ʼ��*****/
void Key_Init();

void Menu_display(); 
void Keyscan();
void Display();

/******�����˵����Ƴ���******/
void Key_Control();

#endif