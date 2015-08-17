#ifndef __INCLUDE_H__
#define __INCLUDE_H__

#include  "common.h"

/*
 * Include 用户自定义的头文件
 */
#include  "MK60_wdog.h"
#include  "MK60_gpio.h"     //IO口操作
#include  "MK60_uart.h"     //串口
#include  "MK60_SysTick.h"
#include  "MK60_lptmr.h"    //低功耗定时器(延时)
#include  "MK60_i2c.h"      //I2C
#include  "MK60_ftm.h"      //FTM
#include  "MK60_pit.h"      //PIT
#include  "MK60_dma.h"      //DMA
#include  "FIRE_camera.h"       //摄像头总头文件
#include  "oled.h"          //液晶总头文件
#include  "var.h"        //图像处理
#include  "PWM.h"        //电机 舵机 控制 
#include  "tower.h"   //灯台检测
#include  "key.h"

#endif  //__INCLUDE_H__
