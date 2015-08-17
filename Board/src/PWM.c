#include "common.h"
#include "MK60_uart.h"
#include  "MK60_FTM.h"
#include "MK60_port.h"
#include "MK60_gpio.h"
#include "pwm.h"
extern int turn90F;//Ѱ���ⲿ����
extern int turnF;//
char ch;
static int16 lastDuoji =0;//�ϴζ��λ��

/****** ��������λת����ת/�룩*********/
#define OPTICAL_ENCODE_CONSTANT    500  //�������̵Ŀ̲�����
#define SPEED_CONTROL_PEIORD   10       //�ٶȿ������ڣ�ms
#define CAR_SPEED_CONSTANT    1000.0/SPEED_CONTROL_PEIORD/OPTICAL_ENCODE_CONSTANT// ��λת������ֵ


/***** ���PID���� *****/
static  float ek1=0, ek2=0;
static uint32 PID_Out_New,PID_Out_Old;

/****** PWM��ʼ�� *******/
void PWM_Init()
{
     FTM_PWM_init(FTM0, FTM_CH4, 15000, 0);        //D4    --- 
     FTM_PWM_init(FTM0, FTM_CH5, 15000, 0);        //D5       |  ��� 1000u
     FTM_PWM_init(FTM0, FTM_CH6, 15000, 0);        //D6       |  
     FTM_PWM_init(FTM0, FTM_CH7, 15000, 0);        //D7    ---     
     
     FTM_PWM_init(FTM1, FTM_CH0, 50, 0);           //A12    ���  1000u
}

/****** ���ֵ������ *******/
void Drive_L(int32 left)
{
     uint32 L;
     if(left > 0)        //��ת����
     {
          if(left > 999)                                    //�޷�
          {
               left = 999;
          }
          
          L = left;
          FTM_PWM_Duty(FTM0, FTM_CH6, L);               //����
          FTM_PWM_Duty(FTM0, FTM_CH7, 0);
     }
     
     if(left < 0)        //��ת����
     {
          if(left < -999)                                    //�޷�
          {
               left = -999;
          }
          
          L = -left;
          FTM_PWM_Duty(FTM0, FTM_CH6, 0);               //����
          FTM_PWM_Duty(FTM0, FTM_CH7, L);
     }     
}

/****** ���ֵ������ *******/
void Drive_R(int32 right)
{
     uint32 R;
     if(right > 0)       //��ת����
     {
          if(right > 999)                                    //�޷�
          {
               right = 999;
          }
          
          R = right;
          FTM_PWM_Duty(FTM0, FTM_CH4, R);               //����
          FTM_PWM_Duty(FTM0, FTM_CH5, 0);
     }
     
     if(right < 0)       //��ת����
     {
          if(right < -999)                                    //�޷�
          {
               right = -999;
          }
          
          R = -right;
          FTM_PWM_Duty(FTM0, FTM_CH4, 0);               //����
          FTM_PWM_Duty(FTM0, FTM_CH5, R);
     }
     
}

/****** ���ٳ��� ***********/
void Speed_Ajust(float Speed_Set,float Speed_Ave)
{
     
 float Delta, fP, Delta_Out, Speed_Out_Old, Speed_Out_New;    
      //temp = Speed_Ave *  CAR_SPEED_CONSTANT;        //��λת����ת/�룩
     Delta = Speed_Set - Speed_Ave;          //�����ֵ��ƽ��ת�����趨ֵ��ƫ��
     
     fP = (P_Speed) * Delta;                 //ƫ�����һ����������
     
     Delta_Out = fP;     //(����һ��D����)
     Speed_Out_New = Speed_Ave + Delta_Out;//+ Speed_Out_Old
     
     //Speed_Out_Old = Speed_Out_New;
     
     Drive_L(Speed_Out_New);                //������
     Drive_R(Speed_Out_New);
}

/****** ǰ���������� *******/
void PWM_Go(char duty1)
{
     FTM_PWM_Duty(FTM0, FTM_CH4, duty1);               //����
     FTM_PWM_Duty(FTM0, FTM_CH5, 0);
               
     FTM_PWM_Duty(FTM0, FTM_CH6, duty1);
     FTM_PWM_Duty(FTM0, FTM_CH7, 0);
}

/****** ���˵������� *******/
void PWM_Back(char duty2)
{
     FTM_PWM_Duty(FTM0, FTM_CH4, 0);                   //����
     FTM_PWM_Duty(FTM0, FTM_CH5, duty2);
               
     FTM_PWM_Duty(FTM0, FTM_CH6, 0);
     FTM_PWM_Duty(FTM0, FTM_CH7, duty2);
}

/****** ���ֹͣ���� *******/
void PWM_Stop()
{
     SpeedFlag = 1;
     FTM_PWM_Duty(FTM0, FTM_CH4, 0);                  
     FTM_PWM_Duty(FTM0, FTM_CH5, 0);
               
     FTM_PWM_Duty(FTM0, FTM_CH6, 0);
     FTM_PWM_Duty(FTM0, FTM_CH7, 0);
}

/****** ���ƫת���� *******/
void PWM_Duoji(char duoji)
{
     if(ch < 0x1E)                           //�����޶�ֵ
     {
          ch = 0x1E;
     }
     
     else if(ch > 0x40)                      //�����޶�ֵ
     {
          ch = 0x40;
     }
     
     FTM_PWM_Duty(FTM1, FTM_CH0, duoji);     
}

/****** ������Գ��� *******/
void Duoji_Test()
{
     uart_getchar (UART4, &ch);                   //�ȴ���������
          
     //printf("123");
     printf ("%d",ch);
        
     PWM_Duoji(ch);
}

/****** ������Ƴ��� ******/
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

/****** ���ѭ�߿��Ƴ��� ******/
void Duoji_Follow(int16 heixian)
{	
  
	
     int16  fP_Duoji, fD_Duoji,Duoji_Out,Delta_Position ; //   Duo 
     float Kp_Duoji, Ki_Duoji, Kd_Duoji;
     
     Kp_Duoji = ((float)Kp_Duoji_u)/100;
     Ki_Duoji = ((float)Ki_Duoji_u)/100;
     Kd_Duoji = ((float)Kd_Duoji_u)/100;
     
     Delta_Position = heixian - zhongxian;  //������ƫ��  //�㷨��ʹ��
     fP_Duoji = Delta_Position * P_Duoji ;    //����һ������ϵ��
     fD_Duoji =(Delta_Position -lastDuoji )*D_Duoji;//����һ��΢��ϵ��
     Duoji_Out = fP_Duoji+fD_Duoji;    //PD
     Duoji_Control(Duoji_Out);//���ƶ��ƫת
     lastDuoji = Delta_Position ;
}

/****** �ܿ��Ƴ��� ******/
void All_Control(uint8 jiance_heixian,uint16 Dangqian_Speed,int * Position )
{
    
     if(SpeedFlag==0)
     {

       
       /**/
     switch(*Position)
	{	
		case 0://ֱ���ܵ�ʱ
		{                  
                Duoji_Follow(jiance_heixian);
                  
                  Drive_L(Zhi_Speed_Set);
                  Drive_R(Zhi_Speed_Set);
		}break;
		
		case 1: //ֱ��ת��ʱ  
                  
		{
                  PWM_Stop();
                  Duoji_Follow(jiance_heixian);
                  Drive_L(Wan_Speed_Set);
                  Drive_R(Wan_Speed_Set);
                  SpeedFlag = 0;
                  //systick_delay_ms(50);
                 //*Position=0;
                 
		}break;
		
		case 2: //Сs��
		{
                        // x = SmallS_Speed_Set;
                  Duoji_Follow(jiance_heixian);
               Speed_PID(SmallS_Speed_Set, Dangqian_Speed);
		}break;
		
		case 3: //��s��
		{
                        // x = BigS_Speed_Set;
                  Duoji_Follow(jiance_heixian);
               Speed_PID(BigS_Speed_Set, Dangqian_Speed);
		}break;
		
		case 4: //��˫�߱任
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
/****** ���PID���� ********/
int Speed_PID(float Speed_Set_PID, float Speed_Now)
{    
     float ek0, Delta_ek0, Delta_ek1;
     
     ek0 = Speed_Set_PID - Speed_Now;   //   �˴��ٶ����趨ֵ��ƫ��

     Delta_ek0 = ek0 - ek1;        //�˴�ƫ������һ��ƫ��Ĳ�ֵ
     Delta_ek1 = ek1 - ek2;        //�ϴ�ƫ�������ϴ�ƫ��Ĳ�ֵ
     
     PID_Out_Old = PID_Out_New;//�˴ε�PWM�����Ϊ��һ�εĻ�����
     PID_Out_New = Kp * Delta_ek0 + Ki * ek0 + Kd * (Delta_ek0 - Delta_ek1) + Speed_Now;//�˴�PWM�������
     
     Drive_L(PID_Out_New);    //�������ֵ��
     Drive_R(PID_Out_New);    //�������ֵ��
     
     ek2 = ek1;     //����һ�ε�ƫ������ϴ�
     ek1 = ek0;     //�Ѵ˴ε�ƫ�����һ��   
     
     /************�޷�*****/
     if(PID_Out_New<=0) PID_Out_New=0;
     if(PID_Out_New>=999) PID_Out_New=999;
     return PID_Out_New;
}