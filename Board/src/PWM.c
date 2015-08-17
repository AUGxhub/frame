#include "common.h"
#include "MK60_uart.h"
#include  "MK60_FTM.h"
#include "MK60_port.h"
#include "MK60_gpio.h"
#include "pwm.h"
extern int turn90F;//寻找外部变量
extern int turnF;//
char ch;
static int16 lastDuoji =0;//上次舵机位置

/****** 编码器单位转换（转/秒）*********/
#define OPTICAL_ENCODE_CONSTANT    500  //光电编码盘的刻槽数量
#define SPEED_CONTROL_PEIORD   10       //速度控制周期，ms
#define CAR_SPEED_CONSTANT    1000.0/SPEED_CONTROL_PEIORD/OPTICAL_ENCODE_CONSTANT// 单位转化比例值


/***** 电机PID参数 *****/
static  float ek1=0, ek2=0;
static uint32 PID_Out_New,PID_Out_Old;

/****** PWM初始化 *******/
void PWM_Init()
{
     FTM_PWM_init(FTM0, FTM_CH4, 15000, 0);        //D4    --- 
     FTM_PWM_init(FTM0, FTM_CH5, 15000, 0);        //D5       |  电机 1000u
     FTM_PWM_init(FTM0, FTM_CH6, 15000, 0);        //D6       |  
     FTM_PWM_init(FTM0, FTM_CH7, 15000, 0);        //D7    ---     
     
     FTM_PWM_init(FTM1, FTM_CH0, 50, 0);           //A12    舵机  1000u
}

/****** 左轮电机控制 *******/
void Drive_L(int32 left)
{
     uint32 L;
     if(left > 0)        //正转加速
     {
          if(left > 999)                                    //限幅
          {
               left = 999;
          }
          
          L = left;
          FTM_PWM_Duty(FTM0, FTM_CH6, L);               //调速
          FTM_PWM_Duty(FTM0, FTM_CH7, 0);
     }
     
     if(left < 0)        //反转减速
     {
          if(left < -999)                                    //限幅
          {
               left = -999;
          }
          
          L = -left;
          FTM_PWM_Duty(FTM0, FTM_CH6, 0);               //调速
          FTM_PWM_Duty(FTM0, FTM_CH7, L);
     }     
}

/****** 右轮电机控制 *******/
void Drive_R(int32 right)
{
     uint32 R;
     if(right > 0)       //正转加速
     {
          if(right > 999)                                    //限幅
          {
               right = 999;
          }
          
          R = right;
          FTM_PWM_Duty(FTM0, FTM_CH4, R);               //调速
          FTM_PWM_Duty(FTM0, FTM_CH5, 0);
     }
     
     if(right < 0)       //反转减速
     {
          if(right < -999)                                    //限幅
          {
               right = -999;
          }
          
          R = -right;
          FTM_PWM_Duty(FTM0, FTM_CH4, 0);               //调速
          FTM_PWM_Duty(FTM0, FTM_CH5, R);
     }
     
}

/****** 调速程序 ***********/
void Speed_Ajust(float Speed_Set,float Speed_Ave)
{
     
 float Delta, fP, Delta_Out, Speed_Out_Old, Speed_Out_New;    
      //temp = Speed_Ave *  CAR_SPEED_CONSTANT;        //单位转换（转/秒）
     Delta = Speed_Set - Speed_Ave;          //左右轮电机平均转速与设定值的偏差
     
     fP = (P_Speed) * Delta;                 //偏差乘上一个比例参数
     
     Delta_Out = fP;     //(还加一个D控制)
     Speed_Out_New = Speed_Ave + Delta_Out;//+ Speed_Out_Old
     
     //Speed_Out_Old = Speed_Out_New;
     
     Drive_L(Speed_Out_New);                //电机输出
     Drive_R(Speed_Out_New);
}

/****** 前进调档函数 *******/
void PWM_Go(char duty1)
{
     FTM_PWM_Duty(FTM0, FTM_CH4, duty1);               //调档
     FTM_PWM_Duty(FTM0, FTM_CH5, 0);
               
     FTM_PWM_Duty(FTM0, FTM_CH6, duty1);
     FTM_PWM_Duty(FTM0, FTM_CH7, 0);
}

/****** 后退调档函数 *******/
void PWM_Back(char duty2)
{
     FTM_PWM_Duty(FTM0, FTM_CH4, 0);                   //调档
     FTM_PWM_Duty(FTM0, FTM_CH5, duty2);
               
     FTM_PWM_Duty(FTM0, FTM_CH6, 0);
     FTM_PWM_Duty(FTM0, FTM_CH7, duty2);
}

/****** 电机停止函数 *******/
void PWM_Stop()
{
     SpeedFlag = 1;
     FTM_PWM_Duty(FTM0, FTM_CH4, 0);                  
     FTM_PWM_Duty(FTM0, FTM_CH5, 0);
               
     FTM_PWM_Duty(FTM0, FTM_CH6, 0);
     FTM_PWM_Duty(FTM0, FTM_CH7, 0);
}

/****** 舵机偏转函数 *******/
void PWM_Duoji(char duoji)
{
     if(ch < 0x1E)                           //最左限定值
     {
          ch = 0x1E;
     }
     
     else if(ch > 0x40)                      //最右限定值
     {
          ch = 0x40;
     }
     
     FTM_PWM_Duty(FTM1, FTM_CH0, duoji);     
}

/****** 舵机测试程序 *******/
void Duoji_Test()
{
     uart_getchar (UART4, &ch);                   //等待接收数据
          
     //printf("123");
     printf ("%d",ch);
        
     PWM_Duoji(ch);
}

/****** 舵机控制程序 ******/
void Duoji_Control(int Angle)
{
     uint8 Duoji_Angle;     
  
     if(Angle < -45)
     {
          Angle = -45;
     }
     
     else if(Angle > 45)
     {
          Angle = 45;
     }
     
     Duoji_Angle = (Angle * 7) / 18 + 47;
     
     PWM_Duoji(Duoji_Angle);
}

/****** 舵机循线控制程序 ******/
void Duoji_Follow(int16 heixian)
{	
  
	
     int16  fP_Duoji, fD_Duoji,Duoji_Out,Delta_Position ; //   Duo 
     float Kp_Duoji, Ki_Duoji, Kd_Duoji;
     
     Kp_Duoji = ((float)Kp_Duoji_u)/100;
     Ki_Duoji = ((float)Ki_Duoji_u)/100;
     Kd_Duoji = ((float)Kd_Duoji_u)/100;
     
     Delta_Position = heixian - zhongxian;  //与中线偏差  //算法二使用
     fP_Duoji = Delta_Position * P_Duoji ;    //乘上一个比例系数
     fD_Duoji =(Delta_Position -lastDuoji )*D_Duoji;//乘上一个微分系数
     Duoji_Out = fP_Duoji+fD_Duoji;    //PD
     Duoji_Control(Duoji_Out);//控制舵机偏转
     lastDuoji = Delta_Position ;
}

/****** 总控制程序 ******/
void All_Control(uint8 jiance_heixian,uint16 Dangqian_Speed,int * Position )
{
    
     if(SpeedFlag==0)
     {

       
       /**/
     switch(*Position)
	{	
		case 0://直线跑道时
		{                  
                Duoji_Follow(jiance_heixian);
                  
                  Drive_L(Zhi_Speed_Set);
                  Drive_R(Zhi_Speed_Set);
		}break;
		
		case 1: //直角转弯时  
                  
		{
                  PWM_Stop();
                  Duoji_Follow(jiance_heixian);
                  Drive_L(Wan_Speed_Set);
                  Drive_R(Wan_Speed_Set);
                  SpeedFlag = 0;
                  //systick_delay_ms(50);
                 //*Position=0;
                 
		}break;
		
		case 2: //小s弯
		{
                        // x = SmallS_Speed_Set;
                  Duoji_Follow(jiance_heixian);
               Speed_PID(SmallS_Speed_Set, Dangqian_Speed);
		}break;
		
		case 3: //大s弯
		{
                        // x = BigS_Speed_Set;
                  Duoji_Follow(jiance_heixian);
               Speed_PID(BigS_Speed_Set, Dangqian_Speed);
		}break;
		
		case 4: //单双线变换
		{
		// x = SingleDouble_Speed_Set;	
                  Duoji_Follow(jiance_heixian);
               Speed_PID(SingleDouble_Speed_Set, Dangqian_Speed);
		}break;
          
		default:
               break;
	}
     }
}
/****** 电机PID调速 ********/
int Speed_PID(float Speed_Set_PID, float Speed_Now)
{    
     float ek0, Delta_ek0, Delta_ek1;
     
     ek0 = Speed_Set_PID - Speed_Now;   //   此次速度与设定值的偏差

     Delta_ek0 = ek0 - ek1;        //此次偏差与上一次偏差的差值
     Delta_ek1 = ek1 - ek2;        //上次偏差与上上次偏差的差值
     
     PID_Out_Old = PID_Out_New;//此次的PWM输出作为下一次的基础量
     PID_Out_New = Kp * Delta_ek0 + Ki * ek0 + Kd * (Delta_ek0 - Delta_ek1) + Speed_Now;//此次PWM的输出量
     
     Drive_L(PID_Out_New);    //控制左轮电机
     Drive_R(PID_Out_New);    //控制右轮电机
     
     ek2 = ek1;     //把上一次的偏差传给上上次
     ek1 = ek0;     //把此次的偏差传给上一次   
     
     /************限幅*****/
     if(PID_Out_New<=0) PID_Out_New=0;
     if(PID_Out_New>=999) PID_Out_New=999;
     return PID_Out_New;
}