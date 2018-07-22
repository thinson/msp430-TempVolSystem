/*
 * serial.c
 *
 *  Created on: 2018年7月22日
 *      Author: wk176
 */

#include "msp430g2553.h"
#include "serial.h"
/****************************************************************************
                                                                           串口通信
****************************************************************************/

void UART_Init(void){

    P1SEL |= BIT1+BIT2;                                    // Timer function for TXD pin
    P1SEL2 |=  BIT1 + BIT2;  // P1.2 UCA0TXD output

    //------------UART(USCI_A0) 的寄存器和波特率配置----------------//
    UCA0CTL1 |=  UCSSEL_2 + UCSWRST;  // USCI Clock = SMCLK,USCI_A0 disabled
    UCA0BR0   =  104;                 // 104 From datasheet table-
    UCA0BR1   =  0;                   // -selects baudrate =9600,clk = SMCLK
    UCA0MCTL  =  UCBRS_1;             // Modulation value = 1 from datasheet
    //UCA0STAT |=  UCLISTEN;            // loop back mode enabled，配置为内部循环模式，即自己的TXD传输给自己的RXD接受
    UCA0CTL1 &= ~UCSWRST;             // Clear UCSWRST to enable USCI_A0

    //---------------- 开启传输中断和接受中断，同时开启全局中断，然后在UCA0TXBUF寄存器中写入一个A ------------------//
//    IE2 |= UCA0TXIE;                  // Enable the Transmit interrupt
    IE2 |= UCA0RXIE;                  // Enable the Receive  interrupt
    IFG2 &= ~UCA0RXIFG;
    IFG2 &= ~UCA0TXIFG;
    _BIS_SR(GIE);                     // Enable the global interrupt
}
