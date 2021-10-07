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
#include "gpio.h"
// Include logging for this file
#define INCLUDE_LOG_DEBUG 1
#include "src/log.h"

#include "ble.h"
#include "gatt_db.h"

#include "lcd.h"
EventQueue_t EvtQ;

EventQueue_t *getEvQ(){
  return (&EvtQ);
}



/**
 * This function initialize the circular queue struct
 * @param: void
 * return void
 */
/*
void EvtCirQ_init(){
  CORE_DECLARE_IRQ_STATE;
  CORE_ENTER_CRITICAL();
  EvtQ.head = -1;
  EvtQ.tail = -1;
  CORE_EXIT_CRITICAL();
}
*/

//Attribute:
//CirQ idea: https://www.geeksforgeeks.org/circular-queue-set-1-introduction-array-implementation/
/**
 * Enqueue the event
 * @param[in]:  event to be enqueue
 * @return: return true if the event is enqueue
 *          false if the queue is full and cant
 *          be enqueued
 * */
/*
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
*/

/**
 * dequeue the event
 * @return: return the head of the event in the queue
 * */
/*
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
*/

/**
 * set read temperature event to the queue
 * */
void schedulerSetEventReadTemp(){
  CORE_DECLARE_IRQ_STATE;
  CORE_ENTER_CRITICAL();                                            //Enter critical section while processing the queue
  sl_bt_external_signal(letimer_underflow_expired);
  CORE_EXIT_CRITICAL();
}

/**
 * set WaitUs event to the queue
 * */
void schedulerSetEventWaitUs(){
  CORE_DECLARE_IRQ_STATE;
  CORE_ENTER_CRITICAL();                                            //Enter critical section while processing the queue
  sl_bt_external_signal(letimer_comp1_expired);
  CORE_EXIT_CRITICAL();
}

/**
 * set I2C0 event to the queue
 * */
void schedulerSetEventI2Cdone(){
  CORE_DECLARE_IRQ_STATE;
  CORE_ENTER_CRITICAL();                                            //Enter critical section while processing the queue
  sl_bt_external_signal(i2c_done);
  CORE_EXIT_CRITICAL();
}

static inline bool ok_to_indicate(){
  ble_data_struct_t *ble_data_loc = get_ble_data();
  return (ble_data_loc->htm_indication_enable &&
          ble_data_loc->htm_connection_enable &&
         !ble_data_loc->htm_indication_on_flight);
}

void update_and_send_indication(){
  sl_status_t sc;
  ble_data_struct_t *ble_data_loc = get_ble_data();
  uint8_t *connection_handle_loc = get_connection_handle();
  set_temp();
  uint32_t *temp_c = get_temperature_in_c();

  uint8_t   htm_temperature_buffer[5];   // Stores the temperature data in the Health Thermometer (HTM) format.
                                         // format of the buffer is: flags_byte + 4-bytes of IEEE-11073 32-bit float
  uint8_t   *p = htm_temperature_buffer; // Pointer to HTM temperature buffer needed for converting values to bitstream.
  uint32_t  htm_temperature_flt;         // Stores the temperature data read from the sensor in the IEEE-11073 32-bit float format
  uint8_t   flags = 0x00;                // HTM flags set as 0 for Celsius, no time stamp and no temperature type.

  // DOS
  displayPrintf(DISPLAY_ROW_TEMPVALUE, "Temp=%d", (int) *temp_c);
#if DEBUG
  LOG_INFO("Temp=%d", (int) *temp_c);
#endif

  // "bitstream" refers to the order of bytes and bits sent. byte[0] is sent first, followed by byte[1]...
  UINT8_TO_BITSTREAM(p, flags);           // put the flags byte in first, "convert" is a strong word, it places the byte into the buffer

  // Convert sensor data to IEEE-11073 32-bit floating point format.
  htm_temperature_flt = UINT32_TO_FLOAT(*temp_c, 0); // Convert temperature to bitstream and place it in the htm_temperature_buffer
  UINT32_TO_BITSTREAM(p, htm_temperature_flt);

  // Write our local GATT DB
  sc = sl_bt_gatt_server_write_attribute_value( gattdb_temperature_measurement, // handle from gatt_db.h
                                                0,                              // offset
                                                5,                              // length
                                                &htm_temperature_buffer[0] );   // pointer to buffer where data is

  if(sc!=SL_STATUS_OK){
    LOG_ERROR("gatt write attribute failed %d", sc);
  }

  sc=sl_bt_gatt_server_send_indication(*connection_handle_loc,
                                       gattdb_temperature_measurement,
                                       5,
                                       &htm_temperature_buffer[0]);
  if(sc!=SL_STATUS_OK){
    LOG_ERROR("gatt send indication failed %d", sc);
  }
  ble_data_loc->htm_indication_on_flight = true;
}

/**
 * get the next event from the queue
 * */
//uint32_t getNextEvent(){
//  uint32_t theEvent;
//  CORE_DECLARE_IRQ_STATE;
//  CORE_ENTER_CRITICAL();                                             //Enter critical section while processing the queue
//  theEvent = EvtCirQ_DeQ();
//  CORE_EXIT_CRITICAL();
//  return theEvent;
//}
/**
 * Event driven state machine for temperature measurement.
 * Scheduler only wakes up when the event is set, otherwise sleep in EM3
 * @param: the event number flag
 * */
void temp_measure_state_machine(sl_bt_msg_t *evt_struct){


  state_t  current_state;
  static   state_t next_state = STATE_IDLE;
  uint32_t signal;

  current_state = next_state; // advance to next on each call!!!


  if(!ok_to_indicate()){
      displayPrintf(DISPLAY_ROW_TEMPVALUE, "");
      return;
  }

  // DOS: This is the signal, not the event ID!!!
  //uint32_t evt = evt_struct->data.evt_system_external_signal.extsignals;

  // This state machine only runs on sl_bt_evt_system_external_signal_id from
  // our scheduler set event functions called by our ISRs!!!
  if (SL_BT_MSG_ID(evt_struct->header) != sl_bt_evt_system_external_signal_id) {
      return;
  }

  // Once we get here, now we can look inside the event data structure and the field extsignals
  signal = evt_struct->data.evt_system_external_signal.extsignals;

  switch(current_state){

    case STATE_IDLE:
      next_state = STATE_IDLE;

      if(signal== letimer_underflow_expired ){
          si7021_enable();
          timerWaitUs_irq(SI7021_ENABLE_TIME_US);
          next_state = STATE_SENSOR_POWER_ON;
      }
      break;

    case STATE_SENSOR_POWER_ON:
      next_state = STATE_SENSOR_POWER_ON;

      if(signal == letimer_comp1_expired){
          sl_power_manager_add_em_requirement(EM1);
          si7021_send_temp_cmd();
          next_state = STATE_I2C_WRITING;
      }
      break;

    case STATE_I2C_WRITING:
      next_state = STATE_I2C_WRITING;
      if(signal == i2c_done){
          timerWaitUs_irq(SI7021_CNVRT_TIME_US);
          sl_power_manager_remove_em_requirement(EM1);
          next_state = STATE_I2C_READING;
      }
      break;

    case STATE_I2C_READING:
      next_state = STATE_I2C_READING;
      if(signal == letimer_comp1_expired){
          sl_power_manager_add_em_requirement(EM1);
          si7021_read_temp_cmd();
          next_state = STATE_SENSOR_CLEAN_UP;
      }
      break;

    case STATE_SENSOR_CLEAN_UP:
      next_state = STATE_SENSOR_CLEAN_UP;
      if(signal == i2c_done){
          //si7021_disable();
          NVIC_DisableIRQ(I2C0_IRQn);
          update_and_send_indication();
          sl_power_manager_remove_em_requirement(EM1);
          next_state = STATE_IDLE;
      }
      break;



    default:
      break;
   }
}
