#ifndef _VAR_H_
#define _VAR_H_
#include "common.h"

#define height 60
#define weight 80
#define LED_IMAGE_WHITE       0
#define LED_IMAGE_BLACK       1
#define Kmark  (15)  //检测到黑线的阈值
#define Kedge (5)  //是否扫到边界的阈值


//计算 斜率
#define h0 5
#define h1 10
#define h2 15
//去噪点
void erasePot(uint8 *data,uint16 width,uint16 hight);
//高斯滤波
void gaussianFilter(uint8 *data,uint16 width,uint16 hight);
//计算出两个跑道边缘 并计算出中心线 中心参考线
uint8 FindEdge(uint8 *data,uint16 width,uint16 hight, int * position);

float FindEdge2(uint8 *data,uint8 width,uint8 hight);
//
void pit1_hander();

#endif