#ifndef _PWM_H_
#define _PWM_H_


/*

    //FindEdge ʹ��
    #define     P_Speed 3       
    #define     P_Duoji 3.59    //��� PID ��P
    #define     D_Duoji 90   //��� PID ��D

    //FindEdge2ʹ��
    #define     P_Speed 3       
    #define     P_Duoji 3.59    //��� PID ��P
    #define     D_Duoji 90   //��� PID ��D

#define zhongxian 40

#define  Zhi_Speed_Set 280        //ֱ���ٶ�����
#define Wan_Speed_Set  150        //����ٶ�����
#define SmallS_Speed_Set 0       //СS���ٶ�����
#define BigS_Speed_Set    0     //��S���ٶ�����
#define SingleDouble_Speed_Set 0  //��˫���ٶ�����
*/

//flash �洢������
//extern float Kp_Duoji, Ki_Duoji, Kd_Duoji;
extern uint16 Kp_Duoji_u, Ki_Duoji_u, Kd_Duoji_u;

extern float Kp_Dianji, Ki_Dianji, Kd_Dianji;
extern uint16 ZhiDao, WanDao, ZhiJiao;

#define     P_Speed 3    
#define     P_Duoji  ( Kp_Duoji)   //��� PID ��P
#define     D_Duoji  (Kd_Duoji)   //��� PID ��D

#define zhongxian 40

#define Zhi_Speed_Set (ZhiDao)        //ֱ���ٶ�����
#define Wan_Speed_Set  (WanDao)        //����ٶ�����
#define SmallS_Speed_Set 0       //СS���ٶ�����
#define BigS_Speed_Set    0     //��S���ٶ�����
#define SingleDouble_Speed_Set 0  //��˫���ٶ�����


// ���PID���� 
/*
#define Kp (Kp_Dianji)
#define Ki (Ki_Dianji)
#define Kd (Kd_Dianji)
*/


/****** ���PID���� ******/
#define Kp 0
#define Ki 3
#define Kd 0.1

static int SpeedFlag =0;

/****** PWM��ʼ�� *******/
void PWM_Init();

/****** ���ֵ������ *******/
void Drive_L(int32 left);

/****** ���ֵ������ *******/
void Drive_R(int32 right);

/****** ���ٳ��� ***********/
void Speed_Ajust(float Speed_Set,float Speed_Ave);

/****** ǰ���������� *******/
void PWM_Go(char duty1);


/****** ���˵������� *******/
void PWM_Back(char duty2);


/****** ���ֹͣ���� *******/
void PWM_Stop();


/****** ���ƫת���� *******/
void PWM_Duoji(char duoji);

/****** ������Գ��� *******/
void Duoji_Test();

/****** ����Ƕȿ��Ƴ��� ******/
void Duoji_Control(int Angle);

/****** ���ѭ�߿��Ƴ��� ******/
void Duoji_Follow(int16 heixian);

/****** �ܿ��Ƴ��� ******/

void All_Control(uint8 jiance_heixian, uint16 Dangqian_Speed,int * Position  );
/****** ���PID���� ********/
int Speed_PID(float Speed_Set_PID, float Speed_Now);
#endif