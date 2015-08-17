#ifndef __PID__
#define __PID__
/*PID = Uk + KP*[E(k)-E(k-1)]+KI*E(k)+KD*[E(k)-2E(k-1)+E(k-2)];(增量型PID算式)
函数入口: RK(设定值),CK(实际值),KP,KI,KD
函数出口: U(K)*/
typedef struct PIDValue
{
    int8 KP;
    int8 KI;
    int8 KD;
    int8 F;
    int8 BITMOV;
    int EK[3];
     
    int UK;//输出值
    int RK;//设定值
    int CK;//实际值
    int UK_REAL;
 
}pid_str;
//PIDValueStr  PID;
void    pid_exe(pid_str *PID)  ;
#endif