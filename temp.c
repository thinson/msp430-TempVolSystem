/*
 * temp.c
 *
 *  Created on: 2018年7月18日
 *      Author: wk176
 */

#include"msp430g2553.h"
#include"temp.h"

void DS18b20_Port_Init(void)
{
  P1DIR |= BIT0;
}
/*******************************************************************************
                          DS18b20 操作时序：
                          1.DS18b20 初始化
                          2.对64位ROM进行操作
                               读ROM
                               搜索ROM
                               跳过ROM
                               告警搜索
                          3.对寄存器进行操作
                             包括读取温度……
                          4.精度默认的为0.0625 ，无法重新设定，没找到相应的指令
                            相对应的转换时间为750ms
*******************************************************************************
*******************************************************************************
DS18b20 初始化方法：
 1 主机发送 480 - 960 us 的低电平，释放总线
 2 等待 15 - 60 us
 3 检测DQ上是否有低电平出现
      有：复位成功 ，通常时间为 60-240 us
      无：复位失败 ， 继续等待
 4 DQ上出现低电平后，低电平持续15us，然后DS18b20开始对单片机发送的数据进行采样
*******************************************************************************/
unsigned int  DS18b20_Init(void)
{
    unsigned Check_val;
    DQ_out;
    DQ_0;
    __delay_cycles(600);
    DQ_1;
    __delay_cycles(60);
    DQ_in;
    _NOP();

    if(DQ_val==1)
    {
        Check_val = 0;          //初始化失败
    }
    if(DQ_val==0)
    {
        Check_val = 1;          //初始化成功
    }
    __delay_cycles(10);
    DQ_out;
    DQ_1;
    __delay_cycles(100);
    return Check_val;
}
/*******************************************************************************
DS18b20 写数据方法：
 1 DS18b20 是“一位一位”的写‘0’和‘1’
 2 每写一次‘1’或‘0’为一个周期，每个周期约为 45 - 60 us
 3 DQ拉低 1 us ，表示写周期开始，释放总线，让DQ随写入的值变化
 4 若写1： DQ拉高至少60us，保证在采样周期内采到的值均为高
 5 若写0： DQ拉低至少60us，保证在采样周期内采到的值均为低
 6 释放总线
*******************************************************************************
* 单片机发送数据时，是从写的数据的最高位开始发送*/
void DS18b20_write_byte(unsigned int dat)
{
    unsigned int i;
    for(i = 0; i < 8;i++)
    {
        DQ_0;
        __delay_cycles(2);
        if(dat & 0X01)
          DQ_1;
        else
          DQ_0;
        __delay_cycles(60);
        dat >>= 1;;
        DQ_1;
        __delay_cycles(10);
    }
}
/**************************************************************************************************
DS18b20 读数据方法：
 1 DS18b20 是“一位一位”的读‘0’和‘1’
 2 每读一次‘1’或‘0’为一个周期，每个周期约为 45 - 60 us
 3 DQ拉低 1 us ，表示读周期开始，释放总线，让DQ随DS18b20传送的值变化
 4 若传1：则检测到高电平，持续时间为60us左右，所以检测一次后要延时60us，再检测下一位传送的数据
 5 若传0：则检测到低电平，持续时间为60us左右
**************************************************************************************************
*DS18b20 传送数据是从最低位开始传*
*所以单片机在接受数据时，存储变量一共移动8次，将所有数据都接收并回到最高位*/
unsigned int DS18b20_read_byte(void)
{
    unsigned i;
    unsigned int byte = 0;
    for(i = 0;i < 8;i++)
    {
        byte >>= 1;
        DQ_0;
        __delay_cycles(2);
        DQ_1;
        __delay_cycles(2);
        DQ_in;
        _NOP();
        if(DQ_val)
          byte |= 0x80;
        __delay_cycles(60);
        DQ_out;
        DQ_1;
        __delay_cycles(10);
    }
    return  byte;
}
/*******************************************************************************
当用一个DS18b20进行温度测量时步骤
1.初始化
2.跳过ROM
3.控制寄存器：  温度转换  ，读取ROM  ，  读取温度低8位   ，  温度高8位
注意的是，一定要初始化两次
*******************************************************************************/
unsigned int get_one_temperature(void)  //只读取了整数，没读取小数的部分
{
  unsigned int Temp_l=0,Temp_h=0,Temp=0;
  float f_temp;
  DS18b20_Init();
  DS18b20_write_byte(Skip_ROM);
  DS18b20_write_byte(Convert_Temperature);
  __delay_cycles(500000);

  DS18b20_Init();
  DS18b20_write_byte(Skip_ROM);
  DS18b20_write_byte(Read_Scratchpad);
  Temp_l=DS18b20_read_byte();
  Temp_h=DS18b20_read_byte();
 // Temp_l>>=8;
  Temp_h<<=8;
  Temp=Temp_h+Temp_l;
  f_temp=Temp*0.0625;  //18b20的分辨率是0.0625
  Temp=f_temp*10+0.5;  //乘以10表示小数点后面取一位，加0.5是四舍五入
  return Temp;         //Temp是整型
}




