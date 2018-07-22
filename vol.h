/*
 * vol.h
 *
 *  Created on: 2018Äê7ÔÂ22ÈÕ
 *      Author: wk176
 */

#ifndef VOL_H_
#define VOL_H_

#include "msp430g2553.h"
#define uchar unsigned char
#define uint unsigned int

void DCO_Init(void);
void ADC_Init(void);
uint Mesure_Voltage(void);

#endif /* VOL_H_ */
