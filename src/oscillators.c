/***********************************************************************
 *@file        oscillators.c
 *
 *@version     0.0.1
 *
 *@brief       clock management unit setup function file.
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


#include "oscillators.h"
#include "app.h"
/**
 * Set up clock management unit for LE timer
 * @param: void
 * @return: void
 * */
void cmu_init(){
#if(LOWEST_ENERGY_MODE<EM3)
  CMU_OscillatorEnable(cmuOsc_LFXO,true,true);        //EM0-EM2: 32kHz
  CMU_ClockSelectSet(cmuClock_LFA, cmuSelect_LFXO);   //Route LFACLK to LETIMER0 or LESENSE
#elif(LOWEST_ENERGY_MODE==EM3)
  CMU_OscillatorEnable(cmuOsc_ULFRCO,true,true);       //EM3: 1kHz
  CMU_ClockSelectSet(cmuClock_LFA, cmuSelect_ULFRCO); //Route LFACLK to LETIMER0 or LESENSE
#endif
  CMU_ClockDivSet(cmuClock_LETIMER0,PRESCALER_VALUE); //Pre-scale clock
  CMU_ClockEnable(cmuClock_LETIMER0,true);            //Enable LETIMER0 clock
  uint32_t temp = CMU_ClockFreqGet(cmuClock_LETIMER0);
}
