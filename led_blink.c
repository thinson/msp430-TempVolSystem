/*
 * led_blink.c
 *
 *  Created on: 2018年7月22日
 *      Author: wk176
 */

#include "led_blink.h"
/****************************************************************************
                                                                        外部计数器
****************************************************************************/
void TIMERA_init(void)
{
  TACCR0 =125;  //计到2500，约20ms
  TACTL = TASSEL_2+ID_3+MC_1+TACLR;  //设置TIMERA的时钟源为SMCLK(1M),8分频后（125K）,计数模式为up,到CCR0再自动从0开始计数
  TACCTL0 |= CCIE;                             // CCR0 interrupt enabled
}

void LED_Init(void)
{
    //点亮P2.4和P2.5两个led
    P2DIR |= BIT4 +BIT5;
    P2OUT &=~ (BIT4+BIT5);
}

