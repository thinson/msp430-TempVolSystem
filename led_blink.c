/*
 * led_blink.c
 *
 *  Created on: 2018��7��22��
 *      Author: wk176
 */

#include "led_blink.h"
/****************************************************************************
                                                                        �ⲿ������
****************************************************************************/
void TIMERA_init(void)
{
  TACCR0 =125;  //�Ƶ�2500��Լ20ms
  TACTL = TASSEL_2+ID_3+MC_1+TACLR;  //����TIMERA��ʱ��ԴΪSMCLK(1M),8��Ƶ��125K��,����ģʽΪup,��CCR0���Զ���0��ʼ����
  TACCTL0 |= CCIE;                             // CCR0 interrupt enabled
}

void LED_Init(void)
{
    //����P2.4��P2.5����led
    P2DIR |= BIT4 +BIT5;
    P2OUT &=~ (BIT4+BIT5);
}

