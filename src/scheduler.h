/***********************************************************************
 *@file        scheduler.c
 *
 *@version     0.0.1
 *
 *@brief       scheduler header files
 *
 *@author      Jiabin Lin, jili9036@Colorado.edu
 *
 *@date        Sep 9, 2021
 *
 *@institution University of Colorado Boulder (UCB)
 *
 *@course      ECEN 5823-001: IoT Embedded Firmware (Fall 2021)
 *
 *@instructor  David Sluiter
 *
 *@assignment  ecen5823-assignment2-JiabinLin12
 *
 *@due         Sep 17, 2020
 *
 *@resources   Utilized Silicon Labs' EMLIB peripheral libraries to
 *             implement functionality.
 *
 *
 *@copyright   All rights reserved. Distribution allowed only for the
 *             use of assignment grading. Use of code excerpts allowed at the
 *             discretion of author. Contact for permission.  */
#ifndef SRC_SCHEDULER_H_
#define SRC_SCHEDULER_H_

#include "app.h"
#include "src/gpio.h"


#define CIR_QUEUE_SIZE      10

//add events here
/* DOS temp, not using circular queue, some kind of timing/space bug there
enum {
  no_event = 0,
  letimer_underflow_expired,
  letimer_comp1_expired,
  i2c_done
};
*/

enum {
  no_event                  = 0,
  letimer_underflow_expired = 1,
  letimer_comp1_expired     = 2,
  i2c_done                  = 4
};

typedef enum uint32_t {
  STATE_IDLE,
  STATE_SENSOR_POWER_ON,
  STATE_I2C_WRITING,
  STATE_I2C_READING,
  STATE_SENSOR_CLEAN_UP,
  MY_STATES_NUM
}state_t;


//structure for event circular queue
typedef struct EventQueue_s{
  int32_t head;
  int32_t tail;
  uint32_t event[CIR_QUEUE_SIZE];
}EventQueue_t;

//Function prototype
EventQueue_t *getEvQ();
void EvtCirQ_init();
void schedulerSetEventReadTemp();
void schedulerSetEventWaitUs();
void schedulerSetEventI2Cdone();
void temp_measure_state_machine(uint32_t evt);
uint32_t getNextEvent();

#endif /* SRC_SCHEDULER_H_ */
