#include "common.h"
#include "tower.h"
#include "MK60_dma.h"
#include "MK60_PIT.h"
#include "misc.h"
#include "oled.h"
//在一个周期内接受到 1k 信号就为灯台信号
//如果在 2s 内一直接收到信号 则判断信号有效不是 干扰信号
//如果信号  检测到后  熄灭 立刻启动
//如果信号  检测到后  减速 停车
//
uint16 i_exit =0;
uint8 go_flag =0;

uint8 Pit_Count = 0;//定时器进入中断次数
uint8 Flag_Tower = 0;//发车 和 停车标志
void pit2_hander()
{
  i_exit++;
  if(i_exit == 19) go_flag =1;//35 == 15s
	uint32 receive1=255, receive2=255;//连续2S的脉冲数
     Pit_Count ++ ;
//printf("PIT COUNT %d \n\r",Pit_Count);
     if(Pit_Count == 1)
     {
          receive1 = DMA_count_get(DMA_CH1);
          //printf("recev 1 is %d\n\r",receive1);
     }
     
     else if(Pit_Count == 2)
     {
          receive2 = DMA_count_get(DMA_CH1);
          Pit_Count = 0;//重新取数
  //printf("recev 2 is %d\n\r",receive2);
     }
     
     if(  (receive2 == 0) )//两次计数不一致，第二次为灭灯
     {
          Flag_Tower ++ ;
          
          if(go_flag ==0)
		  {
          LCD_P6x8uint16(0, 0, Flag_Tower);
		   LCD_Print(0,2,"wait to start ");
			  
		  }
          //printf("rece %d",Flag_Tower);
          if(Flag_Tower == 1)
          {
               //发车
            //go_flag =1;
             //printf("go !!");
          }
          else if(Flag_Tower == 2)
          {
               //停车
               Flag_Tower = 0;
          }
     }
	 //LCD_P6x8uint16(0,0,receive);
	 DMA_count_reset(DMA_CH1);
	 PIT_Flag_Clear(PIT2);       //清中断标志位
}
void tower_init()
{
     DMA_count_Init(DMA_CH1, PTB3, 30000, DADDR_RECOVER);//DMA初始化 左轮转数计数
     port_init(PTB3,ALT1 | DMA_RISING);
     pit_init(PIT2, 90000*500*1);                          //定时 1s
     set_vector_handler(PIT2_VECTORn,pit2_hander);   // 设置中断复位函数到中断向量表里
          enable_irq(PIT2_IRQn);                         // 使能LPTMR中断
  DMA_EN(DMA_CH1);//使能两个计数通道  
}





