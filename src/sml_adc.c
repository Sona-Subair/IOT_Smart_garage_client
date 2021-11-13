/***************************************************************************//**
 * @file main_s1.c
 * @brief Use the ADC to take repeated blocking measurements on a single pin
 *******************************************************************************
 * # License
 * <b>Copyright 2020 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * SPDX-License-Identifier: Zlib
 *
 * The licensor of this software is Silicon Laboratories Inc.
 *
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 *
 *******************************************************************************
 * # Evaluation Quality
 * This code has been minimally tested to ensure that it builds and is suitable
 * as a demonstration for evaluation purposes only. This code will be maintained
 * at the sole discretion of Silicon Labs.
 ******************************************************************************/

#include <stdio.h>
#include "em_device.h"
#include "em_chip.h"
#include "em_cmu.h"
#include "em_adc.h"

#define adcFreq   16000000


/****
 * Set up ADC input for EXP 10
 * @param[in]: None
 * @return[out]: None
 * Attribute: Silicon LAB adc single interrupt example
 */
void initADC (void)
{
  CMU_ClockEnable(cmuClock_ADC0, true);

  ADC_Init_TypeDef init = ADC_INIT_DEFAULT;
  ADC_InitSingle_TypeDef initSingle = ADC_INITSINGLE_DEFAULT;

  init.prescale = ADC_PrescaleCalc(adcFreq, 0); // Init to max ADC clock for Series 1
  init.timebase = ADC_TimebaseCalc(0);

  initSingle.diff       = false;           // single ended
  initSingle.reference  = adcRef2V5;       // internal 2.5V reference
  initSingle.resolution = adcRes12Bit;     // 12-bit resolution
  initSingle.acqTime    = adcAcqTime4;     // set acquisition time to meet minimum requirements

  initSingle.posSel = adcPosSelAPORT2XCH9; //EXP 10

  ADC_Init(ADC0, &init);
  ADC_InitSingle(ADC0, &initSingle);

  // Enable ADC interrupts
  NVIC_ClearPendingIRQ(ADC0_IRQn);
  NVIC_EnableIRQ(ADC0_IRQn);
}

/**
 * Enable ADC interrupt
 * */
void IntEnable_ADC(){
  ADC_IntEnable(ADC0, ADC_IEN_SINGLE);
}
/**
 * Diable ADC interrupt
 * */
void IntDisable_ADC(){
  ADC_IntDisable(ADC0, ADC_IEN_SINGLE);
}

