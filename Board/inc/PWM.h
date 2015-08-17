#ifndef _PWM_H_
#define _PWM_H_


/*

    //FindEdge 使用
    #define     P_Speed 3       
    #define     P_Duoji 3.59    //舵机 PID 的P
    #define     D_Duoji 90   //舵机 PID 的D

    //FindEdge2使用
    #define     P_Speed 3       
    #define     P_Duoji 3.59    //舵机 PID 的P
    #define     D_Duoji 90   //舵机 PID 的D

#define zhongxian 40

#define  Zhi_Speed_Set 280        //直道速度设置
#define Wan_Speed_Set  150        //弯道速度设置
#define SmallS_Speed_Set 0       //小S道速度设置
#define BigS_Speed_Set    0     //大S道速度设置
#define SingleDouble_Speed_Set 0  //单双线速度设置
*/

//flash 存储的数据
//extern float Kp_Duoji, Ki_Duoji, Kd_Duoji;
extern uint16 Kp_Duoji_u, Ki_Duoji_u, Kd_Duoji_u;

extern float Kp_Dianji, Ki_Dianji, Kd_Dianji;
extern uint16 ZhiDao, WanDao, ZhiJiao;

#define     P_Speed 3    
#define     P_Duoji  ( Kp_Duoji)   //舵机 PID 的P
#define     D_Duoji  (Kd_Duoji)   //舵机 PID 的D

#define zhongxian 40

#define Zhi_Speed_Set (ZhiDao)        //直道速度设置
#define Wan_Speed_Set  (WanDao)        //弯道速度设置
#define SmallS_Speed_Set 0       //小S道速度设置
#define BigS_Speed_Set    0     //大S道速度设置
#define SingleDouble_Speed_Set 0  //单双线速度设置


// 电机PID控制 
/*
#define Kp (Kp_Dianji)
#define Ki (Ki_Dianji)
#define Kd (Kd_Dianji)
*/


/****** 电机PID控制 ******/
#define Kp 0
#define Ki 3
#define Kd 0.1

static int SpeedFlag =0;

/****** PWM初始化 *******/
void PWM_Init();

/****** 左轮电机控制 *******/
void Drive_L(int32 left);

/****** 右轮电机控制 *******/
void Drive_R(int32 right);

/****** 调速程序 ***********/
void Speed_Ajust(float Speed_Set,float Speed_Ave);

/****** 前进调档函数 *******/
void PWM_Go(char duty1);


/****** 后退调档函数 *******/
void PWM_Back(char duty2);


/****** 电机停止函数 *******/
void PWM_Stop();


/****** 舵机偏转函数 *******/
void PWM_Duoji(char duoji);

/****** 舵机测试程序 *******/
void Duoji_Test();

/****** 舵机角度控制程序 ******/
void Duoji_Control(int Angle);

/****** 舵机循线控制程序 ******/
void Duoji_Follow(int16 heixian);

/****** 总控制程序 ******/

void All_Control(uint8 jiance_heixian, uint16 Dangqian_Speed,int * Position  );
/****** 电机PID调速 ********/
int Speed_PID(float Speed_Set_PID, float Speed_Now);
#endif