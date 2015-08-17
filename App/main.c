
#include "common.h"
#include "include.h"

//git for github
/***************摄像头
*SDA PTB1
*SCL PTB0
*H PTC8
*V PTC9
*P PTC10
*D0-D7 PTC0-PTC7
****************/

/***************
*D4 D5 D6 D7 电机 
*A12 舵机 
*E24 E25  uart
*D0 E26 左右计数
*oled A14 - A17
*B3 红外脉冲计数
****************/


uint8 imgbuff[CAMERA_SIZE];                             //定义存储接收图像的数组
uint8 img[CAMERA_W*CAMERA_H];
uint8 imgo[CAMERA_W*CAMERA_H];                          //转存位置
//函数声明
void PORTC_IRQHandler();
void DMA0_IRQHandler();
void sendimg(uint8 *imgaddr, uint32 imgsize);
void img_extract(uint8 *dst, uint8 *src, uint32 srclen);
void all_init();
void pit0_hander(void);
void shutdown();
//

extern uint8 ledDisplay ;
int lcd_refrash_count=0;//lcd 刷写计数
uint16 Left_Speed,Right_Speed;//测得的左右转速
int16 err;//
static int position =0;
extern uint8 go_flag ;
char ch_blue,ch1;
int main_control =0;//修改摄像头参数标志
 void  main(void)
{

        all_init();//总初始化 
        
        while(!main_control)
        {
        Key_Control();
        systick_delay_ms(50);
        
        }

        tower_init();  
        
        //摄像头初始化
        camera_init(imgbuff);//摄像头初始化
    	set_vector_handler(PORTC_VECTORn , PORTC_IRQHandler);   //设置LPTMR的中断复位函数为 PORTC_IRQHandler
	set_vector_handler(DMA0_VECTORn , DMA0_IRQHandler);     //设置LPTMR的中断复位函数为 PORTC_IRQHandler
           enable_irq(PIT0_IRQn);                         // 使能LPTMR中断
        while(1)
	{ 
          if(go_flag == 1){
        //     disable_irq( PIT0_IRQn);   
	camera_get_img();                                   //摄像头获取图像

	img_extract(img, imgbuff, CAMERA_SIZE);             //黑白二值化 

        err =  FindEdge(img,80,60,&position);                                //查找标记边缘   
	if(ledDisplay == 1)
          LED_PrintImage(img,60,80);                          //绘制图像

          All_Control(err,0,&position);
         // systick_delay_ms(70);
          
          // enable_irq(PIT0_IRQn);                         // 使能LPTMR中断  
           
          }//end if 
        }//end main while

}
//定时器0 中断 计数脉冲数
void pit0_hander()
{
      
     Left_Speed = DMA_count_get(DMA_CH4);
    // Right_Speed = DMA_count_get(DMA_CH3);  //右边转速用来 当定时器使用
     DMA_count_reset(DMA_CH4);
     if(lcd_refrash_count==20){
      // LCD_P6x8uint16(100,0,Left_Speed);
       lcd_refrash_count=0;
       if(go_flag == 1){
         Key_Control();}
     }
     lcd_refrash_count++;   
    
     PIT_Flag_Clear(PIT0);       //清中断标志位
}


// 初始化一切
void all_init()
{
 
     PWM_Init();         //PWM初始化    
     Key_Init();        //按键初始化

     DMA_count_Init(DMA_CH4, PTE26, 30000, DADDR_RECOVER);//DMA初始化 左轮转数计数
     port_init(PTE26,ALT1 | DMA_RISING);
     DMA_count_Init(DMA_CH3, PTD0, 30000, DADDR_RECOVER);//DMA初始化  右轮转数计数
     port_init(PTD0,ALT1 | DMA_RISING);
     
     pit_init(PIT0, 900000);                          //定时 90 W个bus时钟 后中断
     set_vector_handler(PIT0_VECTORn,pit0_hander);   // 设置中断复位函数到中断向量表里

    
       

    
     DMA_EN(DMA_CH3);//使能两个计数通道
     DMA_EN(DMA_CH4);//使能两个计数通道  
    
     
     
     
     PWM_Duoji(0x2f);//舵机回到正位
     gpio_init(PTD7,GPO,0);
     printf("init all finish !\n\r");
     
}

/*!
*  @brief      PORTA中断服务函数
*  @since      v5.0
*/
void PORTC_IRQHandler()
{
	uint8  n;    //引脚号
	uint32 flag;

	while(!PORTC_ISFR);
	flag = PORTC_ISFR;
	PORTC_ISFR  = ~0;                                   //清中断标志位

	n = 9;                                             //场中断
	if(flag & (1 << n))                                 //PTC9触发中断
	{
		camera_vsync();
	}
#if ( CAMERA_USE_HREF == 1 )                            //使用行中断
	n = 8;
	if(flag & (1 << n))                                 //PTC8触发中断
	{
		camera_href();
	}
#endif


}


/*!
*  @brief      DMA0中断服务函数
*  @since      v5.0
*/
void DMA0_IRQHandler()
{
	camera_dma();
}





//发送图像到上位机显示
//不同的上位机，不同的命令，这里使用 yy_摄像头串口调试 软件
//如果使用其他上位机，则需要修改代码
void sendimg(uint8 *imgaddr, uint32 imgsize)
{
	uint8 cmd[4] = {0, 255, 1, 0 };    //yy_摄像头串口调试 使用的命令

	uart_putbuff(FIRE_PORT, cmd, sizeof(cmd));    //先发送命令

	uart_putbuff(FIRE_PORT, imgaddr, imgsize); //再发送图像
}

//压缩二值化图像解压（空间 换 时间 解压）
//srclen 是二值化图像的占用空间大小
void img_extract(uint8 *dst, uint8 *src, uint32 srclen)
{
	uint8 colour[2] = {255, 0}; //0 和 1 分别对应的颜色
	//注：野火的摄像头 0 表示 白色，1表示 黑色
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

//紧急停车函数 按下相应按键后立刻停止 
void shutdown()
{
     if( uart_querychar (UART4,&ch_blue) == 1)     //查询接收1个字符，保存到 ch里
        {
                if(ch_blue == 's')
                {
                  PWM_Stop();
                  uart_putchar (UART4, ch_blue); 
                }
        }
     
     else if( uart_querychar (UART4,&ch_blue) == 1)     //查询接收1个字符，保存到 ch里
        {
                if(ch_blue == 'g')
                {
                  SpeedFlag = 0;
                  uart_putchar (UART4, ch_blue); 
                }
        }
}
