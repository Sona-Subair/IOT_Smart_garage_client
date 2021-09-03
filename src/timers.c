/***********************************************************************
 *@file        timer.c
 *
 *@version     0.0.1
 *
 *@brief       letimer setup function file.
 *
 *@author      Jiabin Lin, jili9036@Colorado.edu
 *
 *@date        Sep 3, 2021
 *
 *@institution University of Colorado Boulder (UCB)
 *
 *@course      ECEN 5823-001: IoT Embedded Firmware (Fall 2021)
 *
 *@instructor  David Sluiter
 *
 *@assignment  ecen5823-assignment2-JiabinLin12
 *
 *@due         Sep 10, 2020
 *
 *@resources   Utilized Silicon Labs' EMLIB peripheral libraries to
 *             implement functionality.
 *
 *
 *@copyright   All rights reserved. Distribution allowed only for the
 *             use of assignment grading. Use of code excerpts allowed at the
 *             discretion of author. Contact for permission.  */

#include "timers.h"
#include "app.h"
/**
 * This function initialize LE-TIMER0
 * @param: void
 * return void
 */
void letimer_init(){
  LETIMER_Init_TypeDef letimer_init_args = LETIMER_INIT_DEFAULT;
  letimer_init_args.topValue = ACTUAL_COMP0_LOAD;         //Set value to 2250ms
  letimer_init_args.comp0Top = true;                      //Reset CNT when underflow
  LETIMER_CompareSet(LETIMER0,0,ACTUAL_COMP0_LOAD);       //Set comparator0 period to 2250ms
  LETIMER_CompareSet(LETIMER0,1,ACTUAL_COMP1_LOAD);       //Set LED on time to 175ms
  LETIMER_IntClear(LETIMER0,IF_COMP0|IF_COMP1|IF_UF);     //Clear COMP0, COMP1, UF Interrupt
  LETIMER_IntEnable(LETIMER0,IF_COMP0|IF_COMP1|IF_UF);    //Enable COMP0, COMP1, UF Interrupt
  NVIC_ClearPendingIRQ(LETIMER0_IRQn);                    //Clear IRQ
  NVIC_EnableIRQ(LETIMER0_IRQn);                          //Enable IRQ
  LETIMER_Init(LETIMER0,&letimer_init_args);
  LETIMER_Enable(LETIMER0,true);                          //Enable LETIMER
}
