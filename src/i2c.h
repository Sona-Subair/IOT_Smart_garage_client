/***********************************************************************
 *@file        i2c.h
 *
 *@version     0.0.1
 *
 *@brief       I2C initialization;
 *@brief       si7021 sensor enable/disable, read/write from/to sensor
 *@brief       function declaration
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

#ifndef SRC_I2C_H_
#define SRC_I2C_H_

#include "sl_i2cspm.h"
//I2C0 define
#define I2C0_ROUTELOC_SCL   14
#define I2C0_ROUTELOC_SDA   16
#define I2C0_SLCPORT        gpioPortC
#define I2C0_SLCPIN         10
#define I2C0_SDAPORT        gpioPortC
#define I2C0_SDAPIN         11

//si7021 sensor define
#define SI7021_ADDRESS      0x40
#define CMD_NO_HOLD_MASTER  0xF3
#define CMD_READ_TEMP       0xE0
#define ENABLE              1
#define DISABLE             0
#define SI7021_PORT gpioPortD
#define SI7021_PIN  15


//Function prototype
void i2c_init();
void si7021_enable();
void si7021_disable();
void si7021_send_temp_cmd();
void si7021_read_temp_cmd();
#endif /* SRC_I2C_H_ */

