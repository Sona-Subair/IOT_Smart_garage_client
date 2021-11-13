/***********************************************************************
 *@file        sml_adc.h
 *
 *@version     0.0.1
 *
 *@brief       light sensor adc input file
 *
 *@author      Jiabin Lin, jili9036@Colorado.edu
 *
 *@date        Nov 13rd, 2021
 *
 *@institution University of Colorado Boulder (UCB)
 *
 *@course      ECEN 5823-001: IoT Embedded Firmware (Fall 2021)
 *
 *@instructor  David Sluiter
 *
 *@assignment  ecen5823-assignment2-JiabinLin12
 *
 *@due         Dec 8th, 2021
 *
 *@resources   Utilized Silicon Labs' EMLIB peripheral libraries to
 *             implement functionality.
 *
 *
 *@copyright   All rights reserved. Distribution allowed only for the
 *             use of assignment grading. Use of code excerpts allowed at the
 *             discretion of author. Contact for permission.  */

#ifndef SRC_SML_ADC_H_
#define SRC_SML_ADC_H_
void IntEnable_ADC();
void IntDisable_ADC();
void initADC (void);
#endif /* SRC_SML_ADC_H_ */
