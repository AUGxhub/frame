
#include "common.h"
#include "include.h"

//git for github
/***************����ͷ
*SDA PTB1
*SCL PTB0
*H PTC8
*V PTC9
*P PTC10
*D0-D7 PTC0-PTC7
****************/

/***************
*D4 D5 D6 D7 ��� 
*A12 ��� 
*E24 E25  uart
*D0 E26 ���Ҽ���
*oled A14 - A17
*B3 �����������
****************/


uint8 imgbuff[CAMERA_SIZE];                             //����洢����ͼ�������
uint8 img[CAMERA_W*CAMERA_H];
uint8 imgo[CAMERA_W*CAMERA_H];                          //ת��λ��
//��������
void PORTC_IRQHandler();
void DMA0_IRQHandler();
void sendimg(uint8 *imgaddr, uint32 imgsize);
void img_extract(uint8 *dst, uint8 *src, uint32 srclen);
void all_init();
void pit0_hander(void);
void shutdown();
//

extern uint8 ledDisplay ;
int lcd_refrash_count=0;//lcd ˢд����
uint16 Left_Speed,Right_Speed;//��õ�����ת��
int16 err;//
static int position =0;
extern uint8 go_flag ;
char ch_blue,ch1;
int main_control =0;//�޸�����ͷ������־
 void  main(void)
{

        all_init();//�ܳ�ʼ�� 
        
        while(!main_control)
        {
        Key_Control();
        systick_delay_ms(50);
        
        }

        tower_init();  
        
        //����ͷ��ʼ��
        camera_init(imgbuff);//����ͷ��ʼ��
    	set_vector_handler(PORTC_VECTORn , PORTC_IRQHandler);   //����LPTMR���жϸ�λ����Ϊ PORTC_IRQHandler
	set_vector_handler(DMA0_VECTORn , DMA0_IRQHandler);     //����LPTMR���жϸ�λ����Ϊ PORTC_IRQHandler
           enable_irq(PIT0_IRQn);                         // ʹ��LPTMR�ж�
        while(1)
	{ 
          if(go_flag == 1){
        //     disable_irq( PIT0_IRQn);   
	camera_get_img();                                   //����ͷ��ȡͼ��

	img_extract(img, imgbuff, CAMERA_SIZE);             //�ڰ׶�ֵ�� 

        err =  FindEdge(img,80,60,&position);                                //���ұ�Ǳ�Ե   
	if(ledDisplay == 1)
          LED_PrintImage(img,60,80);                          //����ͼ��

          All_Control(err,0,&position);
         // systick_delay_ms(70);
          
          // enable_irq(PIT0_IRQn);                         // ʹ��LPTMR�ж�  
           
          }//end if 
        }//end main while

}
//��ʱ��0 �ж� ����������
void pit0_hander()
{
      
     Left_Speed = DMA_count_get(DMA_CH4);
    // Right_Speed = DMA_count_get(DMA_CH3);  //�ұ�ת������ ����ʱ��ʹ��
     DMA_count_reset(DMA_CH4);
     if(lcd_refrash_count==20){
      // LCD_P6x8uint16(100,0,Left_Speed);
       lcd_refrash_count=0;
       if(go_flag == 1){
         Key_Control();}
     }
     lcd_refrash_count++;   
    
     PIT_Flag_Clear(PIT0);       //���жϱ�־λ
}


// ��ʼ��һ��
void all_init()
{
 
     PWM_Init();         //PWM��ʼ��    
     Key_Init();        //������ʼ��

     DMA_count_Init(DMA_CH4, PTE26, 30000, DADDR_RECOVER);//DMA��ʼ�� ����ת������
     port_init(PTE26,ALT1 | DMA_RISING);
     DMA_count_Init(DMA_CH3, PTD0, 30000, DADDR_RECOVER);//DMA��ʼ��  ����ת������
     port_init(PTD0,ALT1 | DMA_RISING);
     
     pit_init(PIT0, 900000);                          //��ʱ 90 W��busʱ�� ���ж�
     set_vector_handler(PIT0_VECTORn,pit0_hander);   // �����жϸ�λ�������ж���������

    
       

    
     DMA_EN(DMA_CH3);//ʹ����������ͨ��
     DMA_EN(DMA_CH4);//ʹ����������ͨ��  
    
     
     
     
     PWM_Duoji(0x2f);//����ص���λ
     gpio_init(PTD7,GPO,0);
     printf("init all finish !\n\r");
     
}

/*!
*  @brief      PORTA�жϷ�����
*  @since      v5.0
*/
void PORTC_IRQHandler()
{
	uint8  n;    //���ź�
	uint32 flag;

	while(!PORTC_ISFR);
	flag = PORTC_ISFR;
	PORTC_ISFR  = ~0;                                   //���жϱ�־λ

	n = 9;                                             //���ж�
	if(flag & (1 << n))                                 //PTC9�����ж�
	{
		camera_vsync();
	}
#if ( CAMERA_USE_HREF == 1 )                            //ʹ�����ж�
	n = 8;
	if(flag & (1 << n))                                 //PTC8�����ж�
	{
		camera_href();
	}
#endif


}


/*!
*  @brief      DMA0�жϷ�����
*  @since      v5.0
*/
void DMA0_IRQHandler()
{
	camera_dma();
}





//����ͼ����λ����ʾ
//��ͬ����λ������ͬ���������ʹ�� yy_����ͷ���ڵ��� ���
//���ʹ��������λ��������Ҫ�޸Ĵ���
void sendimg(uint8 *imgaddr, uint32 imgsize)
{
	uint8 cmd[4] = {0, 255, 1, 0 };    //yy_����ͷ���ڵ��� ʹ�õ�����

	uart_putbuff(FIRE_PORT, cmd, sizeof(cmd));    //�ȷ�������

	uart_putbuff(FIRE_PORT, imgaddr, imgsize); //�ٷ���ͼ��
}

//ѹ����ֵ��ͼ���ѹ���ռ� �� ʱ�� ��ѹ��
//srclen �Ƕ�ֵ��ͼ���ռ�ÿռ��С
void img_extract(uint8 *dst, uint8 *src, uint32 srclen)
{
	uint8 colour[2] = {255, 0}; //0 �� 1 �ֱ��Ӧ����ɫ
	//ע��Ұ�������ͷ 0 ��ʾ ��ɫ��1��ʾ ��ɫ
	uint8 tmpsrc;
	while(srclen --)
	{
		tmpsrc = *src++;
		*dst++ = colour[ (tmpsrc >> 7 ) & 0x01 ];
		*dst++ = colour[ (tmpsrc >> 6 ) & 0x01 ];
		*dst++ = colour[ (tmpsrc >> 5 ) & 0x01 ];
		*dst++ = colour[ (tmpsrc >> 4 ) & 0x01 ];
		*dst++ = colour[ (tmpsrc >> 3 ) & 0x01 ];
		*dst++ = colour[ (tmpsrc >> 2 ) & 0x01 ];
		*dst++ = colour[ (tmpsrc >> 1 ) & 0x01 ];
		*dst++ = colour[ (tmpsrc >> 0 ) & 0x01 ];
	}
}

//����ͣ������ ������Ӧ����������ֹͣ 
void shutdown()
{
     if( uart_querychar (UART4,&ch_blue) == 1)     //��ѯ����1���ַ������浽 ch��
        {
                if(ch_blue == 's')
                {
                  PWM_Stop();
                  uart_putchar (UART4, ch_blue); 
                }
        }
     
     else if( uart_querychar (UART4,&ch_blue) == 1)     //��ѯ����1���ַ������浽 ch��
        {
                if(ch_blue == 'g')
                {
                  SpeedFlag = 0;
                  uart_putchar (UART4, ch_blue); 
                }
        }
}
