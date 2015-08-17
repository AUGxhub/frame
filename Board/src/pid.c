/*PID = PID->UK_REAL + PID->KP*[E(k)-E(k-1)]+PID->KI*E(k)+PID->KD*[E(k)-2E(k-1)+E(k-2)];(增量型PID算式)
函数入口: PID->RK(设定值),PID->CK(实际值),PID->KP,PID->KI,PID->KD
函数出口: U(K)*/
#include"common.h"
#include"pid.h"
#define MAXOUT 0xff
//#define MAXGAP 100
 
void pid_exe(pid_str*PID)
{
    PID->EK[2]=PID->EK[1];
    PID->EK[1]=PID->EK[0];
    PID->EK[0]=PID->RK-PID->CK;
    PID->UK_REAL=PID->UK_REAL
        +PID->KP*(PID->EK[0]-PID->EK[1])//微分一次后积分即原数
        +(float)PID->KI*PID->EK[0]/PID->F//直接积分
        +(float)PID->KD*(PID->EK[0]-2*PID->EK[1]+PID->EK[2])*PID->F;//二阶微分后积分即一阶微分
    if((PID->UK_REAL>>PID->BITMOV)>=MAXOUT)
    {
        PID->UK=MAXOUT;
    }else if(PID->UK_REAL>>PID->BITMOV<=-MAXOUT)
    {
        PID->UK=-MAXOUT;
    }else
    {
        PID->UK=PID->UK_REAL>>PID->BITMOV;
    }
         
}