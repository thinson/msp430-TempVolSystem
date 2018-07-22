/*
 * temp.h
 *
 *  Created on: 2018��7��18��
 *      Author: wk176
 */

#ifndef TEMP_H_
#define TEMP_H_

#include"msp430g2553.h"
/*********************************
**          ���Ŷ���            **
*********************************/
#define DQ_1 P1OUT |= BIT0
#define DQ_0 P1OUT &= ~BIT0
#define DQ_in   P1DIR &= ~BIT0
#define DQ_out  P1DIR |= BIT0
#define DQ_val  (P1IN & BIT0)
/**********************************
****       �����ַ�����       *****
**********************************/
#define Read_ROM  0x33  //��ROM
#define Match_ROM 0x55  //ƥ��ROM
#define Skip_ROM 0xcc   //����ROM
#define Search_ROM 0xf0   //����ROM
#define Alarm_Search 0xec   //�澯����
#define Convert_Temperature 0x44 //�¶�ת��
#define Read_Scratchpad 0xbe  //���ݴ�洢��9�ֽ�����
#define Write_Scratchpad 0x4e //д�ݴ�洢����д����TH and TL �����ŷ�����λ���ݾͿ���
/*********************************
**          �������            **
*********************************/
//extern unsigned int Check_val;//��ʼ��������
//extern unsigned int Temp;//����¶�
//extern unsigned int Temp_l;//����¶ȵ���λ
//extern unsigned int Temp_h;//����¶ȸ���λ
/************************************
****         ��������           ****
************************************/
extern void DS18b20_Port_Init(void);
extern unsigned int  DS18b20_Init(void);
extern void DS18b20_write_byte(unsigned int dat);
extern unsigned int DS18b20_read_byte(void);
extern unsigned int get_one_temperature(void);







#endif /* TEMP_H_ */
