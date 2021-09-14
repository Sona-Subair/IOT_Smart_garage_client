/***********************************************************************
 *@file        i2c.c
 *
 *@version     0.0.1
 *
 *@brief       I2C initialization;
 *@brief       si7021 sensor enable/disable, read/write from/to sensor
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

#include "app.h"
#include "i2c.h"

#define INCLUDE_LOG_DEBUG 1
#include "src/log.h"

#include "stdio.h"
/**
 * Define as global variable for A4 used
 * **/
I2C_TransferReturn_TypeDef status;
uint8_t cmd_data;
uint8_t rd_data[2];

/**
 * Initialize I2C using I2CSPM_Init and route to the si7021 sensor
 * */
void i2c_init(){
  I2CSPM_Init_TypeDef i2c_init_args;
  i2c_init_args.port = I2C0;
  i2c_init_args.sclPort = I2C0_SLCPORT;
  i2c_init_args.sclPin = I2C0_SLCPIN;
  i2c_init_args.sdaPort = I2C0_SDAPORT;
  i2c_init_args.sdaPin = I2C0_SDAPIN;
  i2c_init_args.portLocationScl = I2C0_ROUTELOC_SCL;
  i2c_init_args.portLocationSda = I2C0_ROUTELOC_SDA;
  i2c_init_args.i2cRefFreq = 0;
  i2c_init_args.i2cMaxFreq = I2C_FREQ_STANDARD_MAX;
  i2c_init_args.i2cClhr = i2cClockHLRStandard;
  I2CSPM_Init(&i2c_init_args);
}

/**
 * send no host master command to si7021
 * **/
void si7021_send_temp_cmd(){
  I2C_TransferSeq_TypeDef sequence;
  cmd_data = CMD_NO_HOLD_MASTER;
  sequence.addr = SI7021_ADDRESS<<1;
  sequence.flags = I2C_FLAG_WRITE;
  sequence.buf[0].data = &cmd_data;
  sequence.buf[0].len= sizeof(cmd_data);

  status = I2CSPM_Transfer (I2C0, &sequence);                     //I2C start writing
  if (status != i2cTransferDone) {                                //I2C status checking
      LOG_ERROR ("I2C WRITE FAILED WITH ERR CODE: %d",status);
  }
  timerWaitUs(11000);                                             //wait >10.8ms because of conversion time
}

/**
 * read two byte temperature values from si7021
 * **/
void si7021_read_temp_cmd(){
  I2C_TransferSeq_TypeDef sequence;
  sequence.addr = SI7021_ADDRESS<<1;
  sequence.flags = I2C_FLAG_READ;
  sequence.buf[0].data = rd_data;//(uint8_t*)&rd_data;
  sequence.buf[0].len= 2;

  status = I2CSPM_Transfer (I2C0, &sequence);                     //I2C start writing
  if (status != i2cTransferDone) {                                //I2C status checking
      LOG_ERROR ("I2C READ FAILED WITH ERR CODE: %d",status);
  }
  uint32_t temperature_in_c = rd_data[1] | rd_data[0]<<8;
  temperature_in_c = (uint32_t) ((175.72 * (float)temperature_in_c)/65536.0) - 46.85;
  //uint16_t temp_code = (rd_data[1] | rd_data[0]<<8;
  LOG_INFO("Temperature %d\n", (int)temperature_in_c);
  printf("Temperature %d\n", (int)temperature_in_c);
}


/**
 * Enable SENSOR_ENABLE PIN for si7021 in the schematic
 * to enable the sensor
 * */
void si7021_enable(){
    GPIO_PinModeSet(SI7021_PORT,SI7021_PIN,gpioModePushPull,ENABLE);
    timerWaitUs(80000);                                             //wait 80ms for si7021 to fully turn on
}
/**
 * Disable SENSOR_ENABLE PIN for si7021 in the schematic
 * to disable the sensor
 * */
void si7021_disable(){
    GPIO_PinModeSet(SI7021_PORT,SI7021_PIN,gpioModePushPull,DISABLE);
}

