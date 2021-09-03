/***********************************************************************
 *@file        irq.c
 *
 *@version     0.0.1
 *
 *@brief       interrupt function file.
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
/***
 * Handle LETIMER0 interrupt
 * @param: void
 * */
void LETIMER0_IRQHandler(void){
  int letimer_flag = LETIMER_IntGetEnabled(LETIMER0);  //determine source of IRQ
  LETIMER_IntClear(LETIMER0,letimer_flag);             //clear source of IRQ set
  if(letimer_flag&IF_COMP1){
      gpioLed0SetOn();                                 //LED0 is set on when COMP0 is set
  }else if (letimer_flag&IF_UF){
      gpioLed0SetOff();                                //LED0 is set off when underflow is set
  }
}
