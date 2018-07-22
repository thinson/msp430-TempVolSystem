/**
 * main.c
 *
 */
#include "msp430.h"
#include "temp.h"
#include "vol.h"
#include "oled.h"
#include "serial.h"
#include "led_blink.h"
#define fre 5  //代表了温度电压的采集频率，相对于屏幕刷新频率，5表示屏幕刷新5次采集一次温度

uint oled_flag=1; //led的开关状态 0 关 1 开

uint temp_tx[3],vol_tx[3]; //串口通信用的温度和电压信息

uint led_fre = 1000; //控制LED闪烁频率

uint timera_counter = 0; //外部定时器计数器

int main(void)
{
	WDTCTL = WDTPW | WDTHOLD;	// stop watchdog timer
	
	//初始化，两个灯都亮
	LED_Init();

	//初始化OLED并显示欢迎界面
	OLED_Init();
	OLED_Clear();
	OLED_ShowString(30,0,"HELLO!");
	OLED_ShowString(15,4,"Starting...");

	//初始化ds18b20，测一次温度，整数小数部分分开
	DS18b20_Init();  //初始化温度传感器
	unsigned int temp,temp_h,temp_l;
	temp=get_one_temperature();
	temp_h = temp/10; //整数部分
	temp_l = temp%10; //小数部分

	//初始化AD
	uint sum,voltage,voltage_h,voltage_l;
	float vol_raw=0.0;
	ADC_Init();                                  //ADC初始化
	DCO_Init();                                  //DCO初始化
	//测一次电压
    sum = Mesure_Voltage();
    vol_raw = sum >> 5; //重复测量32次后取平均值，移位再msp430芯片中运行速度更快
    voltage = vol_raw*500/1023;  //v*2.5*10*2/1023
    voltage_h = voltage/100; //整数
    voltage_l = voltage%100; //小数，保留两位

    //初始化外部定时器，用于LED闪烁控制
    TIMERA_init();

    //串口初始化
    UART_Init();

    //显示LED状态的字符串
    unsigned char *led1_status;
    unsigned char *led2_status;

    //温度电压刷新计数器
    uint info_refresh_counter=0;

    //清屏进入循环，开全局中断
    OLED_Clear();
    _BIS_SR(GIE);

    while(1)
    {
        if(oled_flag == 1)
        {
            //LED状态检测
            if(P2IN&BIT4)  led1_status = "OFF";
            else led1_status = "ON ";
            if(P2IN&BIT5)  led2_status = "OFF";
            else  led2_status = "ON ";

            //指定格式显示字符串
            OLED_ShowCHinese(0,0,0);//武
            OLED_ShowCHinese(18,0,1);//康
            OLED_ShowString(0,2,"Temp:");
            OLED_ShowString(0,4,"Vol:");
            OLED_ShowString(0,6,"LED");
            OLED_ShowString(40,6,led1_status);
            OLED_ShowString(70,6,led2_status);

            OLED_ShowNum(40,2,temp_h,3,16);//显示温度
            OLED_ShowNum(65,2,temp_l,3,16);//显示温度
            OLED_ShowString(71,2,".");
            OLED_ShowCHinese(90,2,2);

            OLED_ShowNum(30,4,voltage_h,3,16);//显示电压
            OLED_ShowNum(60,4,voltage_l,3,16);//显示温度
            OLED_ShowString(55,4,".");
            OLED_ShowString(90,4,"V");
        }

        info_refresh_counter++;

        if(info_refresh_counter>fre)
        {
            //测量温度和电压
            temp_h = temp/10;
            temp_l = temp%10;
            sum = Mesure_Voltage();
            vol_raw = sum >> 5;
            voltage = vol_raw*500/1023;  //v*2.5*10*2/1023
            voltage_h = voltage/100;
            voltage_l = voltage%100;


            //发送到PC端数据的处理
            temp_tx[0] = temp_h/10;
            temp_tx[1]= temp_h%10;
            temp_tx[2]= temp_l;

            vol_tx[0] = voltage_h;
            vol_tx[1]= voltage_l/10;
            vol_tx[2] = voltage_l%10;
            info_refresh_counter = 0;
        }
        delay_ms(50);
    }

}

#pragma vector=ADC10_VECTOR
  __interrupt void ADC10_ISR(void)
  {
    __bic_SR_register_on_exit(CPUOFF);        // Clear CPUOFF bit from 0(SR)
  }


#pragma vector = USCIAB0RX_VECTOR
  __interrupt void ReceiveInterrupt(void)
  {
    P2OUT  ^= BIT5;     // light up P1.6 LED on RX
    if(UCA0RXBUF == '1')
    {
        uint i;
        for(i = 0; i<3;i++)
        {
            UCA0TXBUF = 48 + temp_tx[i];
            delay_ms(150); //延时确保不被覆盖
        }
        UCA0TXBUF = ';';
        delay_ms(150);
        for(i = 0; i<3;i++)
        {
            UCA0TXBUF = 48 + vol_tx[i];
            delay_ms(150);
        }
    }

    //实现关闭oled灯代码，设置片选为0即可
    else if(UCA0RXBUF == '2'){
        oled_flag = 0; //灭
        OLED_Display_Off();
    }
    else if(UCA0RXBUF == '3'){
        oled_flag = 1; //亮
        OLED_Display_On();
        OLED_Clear();
    }
    else if(UCA0RXBUF == 'a'){
        led_fre = 1; //500hz
    }
    else if(UCA0RXBUF == 'b'){
        led_fre = 2;   //250hz
    }
    else if(UCA0RXBUF == 'c'){
        led_fre = 50;   //10hz
    }
    else if(UCA0RXBUF == 'd'){
        led_fre = 100;   //5hz
    }
    else if(UCA0RXBUF == 'e'){
        led_fre = 500;   //1hz
    }
    else if(UCA0RXBUF == 'f'){
        led_fre = 1000;   //0.5hz
    }
    IFG2 &= ~UCA0RXIFG; // Clear RX flag

  }

#pragma vector=TIMER0_A0_VECTOR
__interrupt void Timer_A(void)
{
  timera_counter++;
  if(timera_counter >= led_fre)  //1s=1000ms=20*50 定时
  {
    P2OUT^=BIT4; //P2.4 LED取反
    timera_counter=0;
  }
//  TACCTL0&=~ CCIE;
}


