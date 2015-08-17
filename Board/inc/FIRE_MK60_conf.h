/*!
 *     COPYRIGHT NOTICE
 *     Copyright (c) 2013,Ұ��Ƽ�
 *     All rights reserved.
 *     �������ۣ�Ұ���ѧ��̳ http://www.chuxue123.com
 *
 *     ��ע�������⣬�����������ݰ�Ȩ����Ұ��Ƽ����У�δ������������������ҵ��;��
 *     �޸�����ʱ���뱣��Ұ��Ƽ��İ�Ȩ������
 *
 * @file       FIRE_MK60_conf.h
 * @brief      Ұ��K60 ƽ̨�����ļ�
 * @author     Ұ��Ƽ�
 * @version    v5.0
 * @date       2013-06-26
 */

#ifndef __FIRE_MK60_CONF_H__
#define __FIRE_MK60_CONF_H__

/*
 * ����ƽ̨[���ڹ���ѡ���ﶨ��]
 */
//#define MK60DZ10
//#define MK60F15

/*
 * ѡ���Ƿ����������Ϣ���������ע������ĺ궨��
 */
#define DEBUG_PRINT

/*
 * ���徧��ʱ�ӣ���λΪMHz
 */
#define EXTAL_IN_MHz            (50)

/*
 * ���� PLL ��Ƶ Ƶ�� (��ͬ��ƽ̨���в�ͬ�ĳ�Ƶ����)
 */
#if defined(MK60DZ10)
/*
 * ���� PLL ��Ƶ Ƶ��
 */
#define CORE_CLK                PLL100      // �� PLL_e ��ѡ�� ���ṩ�� ���÷���
                                            // bus �� flex bus �� flash Ƶ�� ���� core ��������Ƶ����
#define MAX_BUS_CLK             100         // bus      (bus        >= core/16  )
#define MAX_FLEXBUS_CLK         25          // flex bus (flex bus   >= core/16  )
#define MAX_FLASH_CLK           25          // flash    (flash      >= core/16  )

#elif defined(MK60F15)

#define CORE_CLK                PLL244      // �� PLL_e ��ѡ�� ���ṩ�� ���÷���
                                            // bus �� flex bus �� flash Ƶ�� ���� core ��������Ƶ����
#define MAX_BUS_CLK             200         // bus      (bus        >= core/16  )
#define MAX_FLEXBUS_CLK         50          // flex bus (flex bus   >= core/16  )
#define MAX_FLASH_CLK           25          // flash    (flash      >= core/16  )

#endif

/*********************   �Զ��� ʱ��Ƶ�� ��Ƶ����   ********************/
//��� CORE_CLK Ϊ PLLUSR ����Ϊ�Զ���ģʽ ������������Ч
//����� F15 ϵ�У� MCG_CLK_MHZ = 50u*(VDIV+16)/(PRDIV+1)/2
//����� DZ10 ϵ�У�MCG_CLK_MHZ = 50u/*(VDIV+24)(PRDIV+1)
#define PRDIV             10
#define VDIV              29
#define CORE_DIV          0         //  core = mcg/ ( CORE_DIV  + 1 )
#define BUS_DIV           1         //  bus  = mcg/ ( BUS_DIV   + 1 )
#define FLEX_DIV          9         //  flex = mcg/ ( FLEX_DIV  + 1 )
#define FLASH_DIV         8         //  flash= mcg/ ( FLASH_DIV + 1 )

/*
 * ���� printf���� �� ��������˿� �� ������Ϣ
 */
#define FIRE_PORT           UART4
#define FIRE_BAUD           115200

/*
 * ������ʱ����
 */
#if 1
#include "MK60_DWT.h"                       //�Ƽ�ʹ��
#define DELAY()         dwt_delay_ms(500)
#define DELAY_MS(ms)    dwt_delay_ms(ms)
#define DELAY_US(us)    dwt_delay_us(us)
#elif   0
#include "MK60_lptmr.h"
#define     DELAY()         lptmr_delay_ms(500)
#define     DELAY_MS(ms)    lptmr_delay_ms(ms)
#define     DELAY_US(us)    lptmr_delay_us(us)
#elif   0
#include "MK60_pit.h"
#define DELAY()         pit_delay_ms(PIT3,500)
#define DELAY_MS(ms)    pit_delay_ms(PIT3,ms)
#define DELAY_US(us)    pit_delay_us(PIT3,us)
#else
#include "MK60_SysTick.h"
#define DELAY()         systick_delay_ms(500)
#define DELAY_MS(ms)    systick_delay_ms(ms)
#define DELAY_US(us)    systick_delay_us(us)
#endif


/*
 * ���ö��Ժ���ʵ�ֺ���
 */
void assert_failed(char *, int);

#if ( defined( DEBUG ) && defined( DEBUG_PRINT ))
#define ASSERT(expr) \
    if (!(expr)) \
        assert_failed(__FILE__, __LINE__)
#else
#define ASSERT(expr)
#endif

/*
 * ���õ����������
 */
#if( defined(DEBUG) && defined(DEBUG_PRINT))
#define DEBUG_PRINTF(FORMAT,...)        do{printf(FORMAT,##__VA_ARGS__);}while(0)	/*�����ӡ������Ϣʱ���뽫������ע�͵�*/
#else
#define DEBUG_PRINTF(FORMAT,...)
#endif

/*
 * ����ǰ��⣬��ֹ �������ؿ�
 */
void start_check();
#ifdef DEBUG
#define SRART_CHECK()       start_check()
#else
#define SRART_CHECK()
#endif


#endif /* __FIRE_MK60_CONF_H__  */