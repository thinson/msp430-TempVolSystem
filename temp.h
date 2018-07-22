/*
 * temp.h
 *
 *  Created on: 2018年7月18日
 *      Author: wk176
 */

#ifndef TEMP_H_
#define TEMP_H_

#include"msp430g2553.h"
/*********************************
**          引脚定义            **
*********************************/
#define DQ_1 P1OUT |= BIT0
#define DQ_0 P1OUT &= ~BIT0
#define DQ_in   P1DIR &= ~BIT0
#define DQ_out  P1DIR |= BIT0
#define DQ_val  (P1IN & BIT0)
/**********************************
****       命令字符定义       *****
**********************************/
#define Read_ROM  0x33  //读ROM
#define Match_ROM 0x55  //匹配ROM
#define Skip_ROM 0xcc   //跳过ROM
#define Search_ROM 0xf0   //搜索ROM
#define Alarm_Search 0xec   //告警搜索
#define Convert_Temperature 0x44 //温度转换
#define Read_Scratchpad 0xbe  //读暂存存储器9字节内容
#define Write_Scratchpad 0x4e //写暂存存储器，写的是TH and TL ，接着发送两位数据就可以
/*********************************
**          定义变量            **
*********************************/
//extern unsigned int Check_val;//初始化检测变量
//extern unsigned int Temp;//存放温度
//extern unsigned int Temp_l;//存放温度低四位
//extern unsigned int Temp_h;//存放温度高四位
/************************************
****         函数定义           ****
************************************/
extern void DS18b20_Port_Init(void);
extern unsigned int  DS18b20_Init(void);
extern void DS18b20_write_byte(unsigned int dat);
extern unsigned int DS18b20_read_byte(void);
extern unsigned int get_one_temperature(void);







#endif /* TEMP_H_ */
