/*
 * serial.c
 *
 *  Created on: 2018��7��22��
 *      Author: wk176
 */

#include "msp430g2553.h"
#include "serial.h"
/****************************************************************************
                                                                           ����ͨ��
****************************************************************************/

void UART_Init(void){

    P1SEL |= BIT1+BIT2;                                    // Timer function for TXD pin
    P1SEL2 |=  BIT1 + BIT2;  // P1.2 UCA0TXD output

    //------------UART(USCI_A0) �ļĴ����Ͳ���������----------------//
    UCA0CTL1 |=  UCSSEL_2 + UCSWRST;  // USCI Clock = SMCLK,USCI_A0 disabled
    UCA0BR0   =  104;                 // 104 From datasheet table-
    UCA0BR1   =  0;                   // -selects baudrate =9600,clk = SMCLK
    UCA0MCTL  =  UCBRS_1;             // Modulation value = 1 from datasheet
    //UCA0STAT |=  UCLISTEN;            // loop back mode enabled������Ϊ�ڲ�ѭ��ģʽ�����Լ���TXD������Լ���RXD����
    UCA0CTL1 &= ~UCSWRST;             // Clear UCSWRST to enable USCI_A0

    //---------------- ���������жϺͽ����жϣ�ͬʱ����ȫ���жϣ�Ȼ����UCA0TXBUF�Ĵ�����д��һ��A ------------------//
//    IE2 |= UCA0TXIE;                  // Enable the Transmit interrupt
    IE2 |= UCA0RXIE;                  // Enable the Receive  interrupt
    IFG2 &= ~UCA0RXIFG;
    IFG2 &= ~UCA0TXIFG;
    _BIS_SR(GIE);                     // Enable the global interrupt
}
