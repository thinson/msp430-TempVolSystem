/*
 * vol.c
 *
 *  Created on: 2018年7月22日
 *      Author: wk176
 */
#include "msp430g2553.h"
#include "vol.h"

/****************************************************************************
                  ADC初始化
*****************************************************************************/
void ADC_Init(void)
{
    ADC10CTL0 = ADC10ON + ADC10IE + REFON + REF2_5V + SREF_1 + MSC + ADC10SHT_2;;  // ADC10ON, interrupt enabled
    ADC10CTL1 = INCH_3;                          // input A1
    ADC10AE0 |= BIT3;                            //
}

/****************************************************************************
                  DCO时钟初始化  设为1MHz
*****************************************************************************/
void DCO_Init(void)
{
    if(CALBC1_1MHZ==0xFF||CALDCO_1MHZ==0xFF)
        {
            while(1);
        }
    BCSCTL1 = CALBC1_1MHZ;
    DCOCTL  = CALDCO_1MHZ;
    BCSCTL2 = SELM_0 +DIVM_0;
}
/****************************************************************************
                  测量电压
*****************************************************************************/
uint Mesure_Voltage(void){
    uint i=0,sum=0;
    for(i=0;i<32;i++){
        ADC10CTL0 |= ENC + ADC10SC;             // Sampling and conversion start
//       __bis_SR_register(CPUOFF + GIE);  //没必要进行cpu休眠
        sum += ADC10MEM;// Move results
    }
    return sum;
}

