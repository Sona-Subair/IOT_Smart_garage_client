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
#include "i2c.h"

#define INCLUDE_LOG_DEBUG 1
#include "src/log.h"



/**
 * This function initialize LE-TIMER0
 * @param: void
 * @return void
 */
void letimer_init(){
  LETIMER_Init_TypeDef letimer_init_args = LETIMER_INIT_DEFAULT;
  letimer_init_args.topValue = ACTUAL_COMP0_LOAD;         //Set value to 3000ms
  letimer_init_args.comp0Top = true;                      //Reset CNT when underflow
  LETIMER_Init(LETIMER0,&letimer_init_args);              // DOS moved this up to here

  LETIMER_CompareSet(LETIMER0,0,ACTUAL_COMP0_LOAD);       //Set comparator0 period
  LETIMER_IntClear(LETIMER0,IF_UF);                       //Clear UF Interrupt
  LETIMER_IntEnable(LETIMER0,IF_UF);                      //Enable UF Interrupt

  NVIC_ClearPendingIRQ(LETIMER0_IRQn);                    //Clear IRQ
  NVIC_EnableIRQ(LETIMER0_IRQn);                          //Enable IRQ

  LETIMER_Enable(LETIMER0,true);                          //Enable LETIMER
}

/**
 * Verify if the timer is roll over
 * @param[in]: current tick
 * @param[in]: previous tick
 * @return: true if roll over happened
 *         false otherwise
 * */
static inline bool is_rollover(uint32_t cur, uint32_t pre){
  return ((int)(cur-pre))>0? true:false;
}

/**
 * delay amount of time based on the user input
 * @param[in]: millisecond to wait
 *             input range [1000, 3000000]
 *             1000:
 *             we are using ULFRCO(1kHz) in EM3
 *             it is not possible to reach to microsecond level
 *
 *             LETIMER_PERIOD_MS:
 *             Period is 3 second, busying polling more than
 *             3 second would be bad for power consumption +
 *             polling more than 3 second CPU will spend all
 *             its time waiting
 *
 * */
void timerWaitUs_polled(uint32_t us_wait){
  uint32_t pre_tik = LETIMER_PERIOD_MS, cur_tik=0,tik_waited=0;

  if((us_wait < (1*MS_TO_US)) || (us_wait >= LETIMER_PERIOD_MS*MS_TO_US)){
      LOG_ERROR("ULFRCO only has precision of millisecond or Sleep time close to LETIMER period");
      LOG_INFO("FUNCTION DEFAULT TO 80MS");
      us_wait = SI7021_ENABLE_TIME_US;
  }

  uint32_t tik_in = LETIMER_CounterGet(LETIMER0);
  uint32_t t_us = S_TO_US/ACTUAL_CLK_FREQ;
  uint32_t tik_req = us_wait/t_us;

  while(tik_waited <= tik_req)
  {
    cur_tik = LETIMER_CounterGet(LETIMER0);
    if(is_rollover(cur_tik, pre_tik)){
        tik_waited = tik_in + (LETIMER_PERIOD_MS - cur_tik);
    }else{
        tik_waited = tik_in-cur_tik;
    }
    pre_tik = LETIMER_CounterGet(LETIMER0);
  }

}

/**
 * sleep amount of time based on the user input. Noticed in timerWaitUs_irq
 * CPU is not polling, it only wakes up when the comparator 1 values matches
 * the value we set to.
 * @param[in]: millisecond to wait
 *             input range [1000, 3000000]
 *             1000:
 *             we are using ULFRCO(1kHz) in EM3
 *             it is not possible to reach to microsecond level
 *
 *             LETIMER_PERIOD_MS:
 *             Period is 3 second, busying polling more than
 *             3 second would be bad for power consumption +
 *             polling more than 3 second CPU will spend all
 *             its time waiting
 * */
void timerWaitUs_irq(uint32_t us_wait){

  if((us_wait < (1*MS_TO_US)) || (us_wait >= LETIMER_PERIOD_MS*MS_TO_US)){
       LOG_ERROR("ULFRCO only has precision of millisecond or Sleep time close to LETIMER period");
       LOG_INFO("FUNCTION DEFAULT TO 80MS");
       us_wait = SI7021_ENABLE_TIME_US;
   }
  //Convert from us time request to the number of ticks required
  float clk_period = (float)1/(float)(ACTUAL_CLK_FREQ);
  float s_wait = (float)us_wait/(float)US_TO_S;
  uint16_t tik_req = (uint16_t)(s_wait/clk_period);

  //Calculate the final tik needed
  uint16_t cur_tik = LETIMER_CounterGet(LETIMER0);
  uint16_t final_tik = (cur_tik - tik_req);         // target COMP1 value

  //Rollover case
  if(final_tik > TIK_PER_PERIOD) {
      final_tik = LETIMER_PERIOD_MS - (0xFFFF-final_tik);
  }

#if DEBUG
  LOG_INFO("c=%d, f=%d", (int) cur_tik, (int) final_tik);
#endif

  //Enable Comparator 1 interrupts
  LETIMER_CompareSet(LETIMER0,1,final_tik);
  LETIMER_IntClear(LETIMER0,IF_COMP1);
  LETIMER0->IEN |= IF_COMP1;
}







