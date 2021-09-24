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
#include "i2c.h"

// Include logging for this file
#define INCLUDE_LOG_DEBUG 1
#include "src/log.h"

static uint32_t timestamp = 0;                         //allow to call within irq.c

/*
static void ts_increment(){
  CORE_DECLARE_IRQ_STATE;
  CORE_ENTER_CRITICAL();                               //Enter critical section
  timestamp += ACTUAL_COMP0_LOAD;
  CORE_EXIT_CRITICAL();
}
*/

/***
 * Handle LETIMER0 interrupt
 * @param: void
 * */
void LETIMER0_IRQHandler(void){

  int letimer_flag = LETIMER_IntGetEnabled(LETIMER0);  //determine source of IRQ
  LETIMER_IntClear(LETIMER0,letimer_flag);

  if(letimer_flag & LETIMER_IF_UF) {
    //gpioLed1SetOn();
    //ts_increment();
    timestamp += ACTUAL_COMP0_LOAD;
    schedulerSetEventReadTemp();                       //Set read Si7021 event when UF
  }

  if(letimer_flag & LETIMER_IF_COMP1){
    //gpioLed1SetOff();
    LETIMER_IntDisable(LETIMER0, LETIMER_IF_COMP1);
    schedulerSetEventWaitUs();
  }

}


void I2C0_IRQHandler(void){
  I2C_TransferReturn_TypeDef transferStatus;
  transferStatus = I2C_Transfer(I2C0);
  if(transferStatus==i2cTransferDone){
      schedulerSetEventI2Cdone();
  }
  if(transferStatus < 0){
      LOG_ERROR("%d",transferStatus);
  }
}


uint32_t letimerMilliseconds(){
  //uint32_t time_ms = 0, tik = 0;
  //CORE_DECLARE_IRQ_STATE;
  //CORE_ENTER_CRITICAL();                               //Enter critical section
  //tik = LETIMER_PERIOD_MS - LETIMER_CounterGet(LETIMER0);
  //time_ms = (tik*S_TO_MS)/ACTUAL_CLK_FREQ;
  //timestamp += time_ms;
  //CORE_EXIT_CRITICAL();
  return timestamp;
}
