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


#define CIR_QUEUE_SIZE      10

//add events here
enum {
  no_event = 0,
  read_temp_from_si7021 = 1
};


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
uint32_t getNextEvent();

#endif /* SRC_SCHEDULER_H_ */
