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
#define fre 5  //�������¶ȵ�ѹ�Ĳɼ�Ƶ�ʣ��������Ļˢ��Ƶ�ʣ�5��ʾ��Ļˢ��5�βɼ�һ���¶�

uint oled_flag=1; //led�Ŀ���״̬ 0 �� 1 ��

uint temp_tx[3],vol_tx[3]; //����ͨ���õ��¶Ⱥ͵�ѹ��Ϣ

uint led_fre = 1000; //����LED��˸Ƶ��

uint timera_counter = 0; //�ⲿ��ʱ��������

int main(void)
{
	WDTCTL = WDTPW | WDTHOLD;	// stop watchdog timer
	
	//��ʼ���������ƶ���
	LED_Init();

	//��ʼ��OLED����ʾ��ӭ����
	OLED_Init();
	OLED_Clear();
	OLED_ShowString(30,0,"HELLO!");
	OLED_ShowString(15,4,"Starting...");

	//��ʼ��ds18b20����һ���¶ȣ�����С�����ַֿ�
	DS18b20_Init();  //��ʼ���¶ȴ�����
	unsigned int temp,temp_h,temp_l;
	temp=get_one_temperature();
	temp_h = temp/10; //��������
	temp_l = temp%10; //С������

	//��ʼ��AD
	uint sum,voltage,voltage_h,voltage_l;
	float vol_raw=0.0;
	ADC_Init();                                  //ADC��ʼ��
	DCO_Init();                                  //DCO��ʼ��
	//��һ�ε�ѹ
    sum = Mesure_Voltage();
    vol_raw = sum >> 5; //�ظ�����32�κ�ȡƽ��ֵ����λ��msp430оƬ�������ٶȸ���
    voltage = vol_raw*500/1023;  //v*2.5*10*2/1023
    voltage_h = voltage/100; //����
    voltage_l = voltage%100; //С����������λ

    //��ʼ���ⲿ��ʱ��������LED��˸����
    TIMERA_init();

    //���ڳ�ʼ��
    UART_Init();

    //��ʾLED״̬���ַ���
    unsigned char *led1_status;
    unsigned char *led2_status;

    //�¶ȵ�ѹˢ�¼�����
    uint info_refresh_counter=0;

    //��������ѭ������ȫ���ж�
    OLED_Clear();
    _BIS_SR(GIE);

    while(1)
    {
        if(oled_flag == 1)
        {
            //LED״̬���
            if(P2IN&BIT4)  led1_status = "OFF";
            else led1_status = "ON ";
            if(P2IN&BIT5)  led2_status = "OFF";
            else  led2_status = "ON ";

            //ָ����ʽ��ʾ�ַ���
            OLED_ShowCHinese(0,0,0);//��
            OLED_ShowCHinese(18,0,1);//��
            OLED_ShowString(0,2,"Temp:");
            OLED_ShowString(0,4,"Vol:");
            OLED_ShowString(0,6,"LED");
            OLED_ShowString(40,6,led1_status);
            OLED_ShowString(70,6,led2_status);

            OLED_ShowNum(40,2,temp_h,3,16);//��ʾ�¶�
            OLED_ShowNum(65,2,temp_l,3,16);//��ʾ�¶�
            OLED_ShowString(71,2,".");
            OLED_ShowCHinese(90,2,2);

            OLED_ShowNum(30,4,voltage_h,3,16);//��ʾ��ѹ
            OLED_ShowNum(60,4,voltage_l,3,16);//��ʾ�¶�
            OLED_ShowString(55,4,".");
            OLED_ShowString(90,4,"V");
        }

        info_refresh_counter++;

        if(info_refresh_counter>fre)
        {
            //�����¶Ⱥ͵�ѹ
            temp_h = temp/10;
            temp_l = temp%10;
            sum = Mesure_Voltage();
            vol_raw = sum >> 5;
            voltage = vol_raw*500/1023;  //v*2.5*10*2/1023
            voltage_h = voltage/100;
            voltage_l = voltage%100;


            //���͵�PC�����ݵĴ���
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
            delay_ms(150); //��ʱȷ����������
        }
        UCA0TXBUF = ';';
        delay_ms(150);
        for(i = 0; i<3;i++)
        {
            UCA0TXBUF = 48 + vol_tx[i];
            delay_ms(150);
        }
    }

    //ʵ�ֹر�oled�ƴ��룬����ƬѡΪ0����
    else if(UCA0RXBUF == '2'){
        oled_flag = 0; //��
        OLED_Display_Off();
    }
    else if(UCA0RXBUF == '3'){
        oled_flag = 1; //��
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
  if(timera_counter >= led_fre)  //1s=1000ms=20*50 ��ʱ
  {
    P2OUT^=BIT4; //P2.4 LEDȡ��
    timera_counter=0;
  }
//  TACCTL0&=~ CCIE;
}


