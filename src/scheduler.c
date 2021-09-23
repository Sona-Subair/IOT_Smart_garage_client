/***********************************************************************
 *@file        scheduler.c
 *
 *@version     0.0.1
 *
 *@brief       implement scheduler for interrupt events
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

#include "scheduler.h"
#include "em_core.h"
#include "i2c.h"

// Include logging for this file
#define INCLUDE_LOG_DEBUG 1
#include "src/log.h"


EventQueue_t EvtQ;

EventQueue_t *getEvQ(){
  return (&EvtQ);
}

/**
 * This function initialize the circular queue struct
 * @param: void
 * return void
 */
void EvtCirQ_init(){
  CORE_DECLARE_IRQ_STATE;
  CORE_ENTER_CRITICAL();
  EvtQ.head = -1;
  EvtQ.tail = -1;
  CORE_EXIT_CRITICAL();
}
//Attribute:
//CirQ idea: https://www.geeksforgeeks.org/circular-queue-set-1-introduction-array-implementation/
/**
 * Enqueue the event
 * @param[in]:  event to be enqueue
 * @return: return true if the event is enqueue
 *          false if the queue is full and cant
 *          be enqueued
 * */
static bool EvtCirQ_EnQ(uint32_t event){
  //Case 1: FIFO is full
  if(((EvtQ.head==0) &&( EvtQ.tail==CIR_QUEUE_SIZE-1)) ||
      ((EvtQ.head!=0)&&(EvtQ.tail==(EvtQ.head-1)))){
      return false;
  }
  //Case 2: FIFO is empty
  if((EvtQ.head==-1)){
      EvtQ.head=0;
      EvtQ.tail=0;
      EvtQ.event[EvtQ.tail] = event;
  }
  //Case 3: FIFO is neither empty or full
  else{
      EvtQ.tail = (EvtQ.tail==CIR_QUEUE_SIZE-1)? 0:EvtQ.tail+1;
      EvtQ.event[EvtQ.tail] = event;
  }
  return true;
}
/**
 * dequeue the event
 * @return: return the head of the event in the queue
 * */
static int EvtCirQ_DeQ(){
  //Empty Queue
  if((EvtQ.head==-1) && (EvtQ.tail==-1)){
      return -1;
  }
  int event = EvtQ.event[EvtQ.head];                                //get event

  EvtQ.event[EvtQ.head] = no_event;                                 //clear Event
  //Last element
  if(EvtQ.head==EvtQ.tail){
      EvtQ.head=-1;
      EvtQ.tail=-1;
  }else{
      EvtQ.head = (EvtQ.head==CIR_QUEUE_SIZE-1)? 0:EvtQ.head+1;     //Move head to the next element
  }
  return event;
}
/**
 * set read temperature event to the queue
 * */
void schedulerSetEventReadTemp(){
  CORE_DECLARE_IRQ_STATE;
  CORE_ENTER_CRITICAL();                                            //Enter critical section while processing the queue
  EvtCirQ_EnQ(letimer_underflow_expired);
  CORE_EXIT_CRITICAL();
}

/**
 * set WaitUs event to the queue
 * */
void schedulerSetEventWaitUs(){
  CORE_DECLARE_IRQ_STATE;
  CORE_ENTER_CRITICAL();                                            //Enter critical section while processing the queue
  EvtCirQ_EnQ(letimer_comp1_expired);
  CORE_EXIT_CRITICAL();
}

/**
 * set I2C0 event to the queue
 * */
void schedulerSetEventI2Cdone(){
  CORE_DECLARE_IRQ_STATE;
  CORE_ENTER_CRITICAL();                                            //Enter critical section while processing the queue
  EvtCirQ_EnQ(i2c_done);
  CORE_EXIT_CRITICAL();
}

/**
 * get the next event from the queue
 * */
uint32_t getNextEvent(){
  uint32_t theEvent;
  CORE_DECLARE_IRQ_STATE;
  CORE_ENTER_CRITICAL();                                             //Enter critical section while processing the queue
  theEvent = EvtCirQ_DeQ();
  CORE_EXIT_CRITICAL();
  return theEvent;
}

void temp_measure_state_machine(uint32_t evt){
  state_t current_state;
  static state_t next_state = STATE_IDLE;
  current_state = next_state;

  switch(current_state){

    case STATE_IDLE:
      next_state = STATE_IDLE;

      if(evt== letimer_underflow_expired ){
          //si7021_enable();
          timerWaitUs_irq(SI7021_ENABLE_TIME_US);
          next_state = STATE_SENSOR_POWER_ON;
          LOG_INFO("To 1");                   //3000
      }
      break;

    case STATE_SENSOR_POWER_ON:
      next_state = STATE_SENSOR_POWER_ON;

      if(evt == letimer_comp1_expired){
          //sl_power_manager_add_em_requirement(EM1);
          //si7021_send_temp_cmd();
          //next_state = STATE_I2C_WRITING;
          next_state = STATE_IDLE;
          LOG_INFO("To 2");                  //3080+
      }
      break;
//
//    case STATE_I2C_WRITING:
//      next_state = STATE_I2C_WRITING;
//      if(evt == i2c_done){
//          timerWaitUs_irq(SI7021_CNVRT_TIME_US);
//          sl_power_manager_remove_em_requirement(EM1);
//          next_state = STATE_I2C_READING;
//          LOG_INFO("To 3");
//      }
//      break;
//
//    case STATE_I2C_READING:
//      next_state = STATE_I2C_READING;
//      if(evt == letimer_comp1_expired){
//          sl_power_manager_add_em_requirement(EM1);
//          si7021_read_temp_cmd();
//          next_state = STATE_SENSOR_CLEAN_UP;
//          LOG_INFO("To 4");
//      }
//      break;
//
//    case STATE_SENSOR_CLEAN_UP:
//      next_state = STATE_SENSOR_CLEAN_UP;
//      if(evt == i2c_done){
//          si7021_disable();
//          log_temp();
//          NVIC_DisableIRQ(I2C0_IRQn);
//          sl_power_manager_remove_em_requirement(EM1);
//          next_state = STATE_IDLE;
//          LOG_INFO("To 0");
//      }
//      break;



    default:
      break;
   }
}
