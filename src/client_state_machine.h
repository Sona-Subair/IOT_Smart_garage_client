/*
 * client_state_machine.h
 *
 *  Created on: Nov 23, 2021
 *      Author: 17207
 */

#ifndef SRC_CLIENT_STATE_MACHINE_H_
#define SRC_CLIENT_STATE_MACHINE_H_

#include <stdint.h>
#include "sl_bt_api.h"
#include "gatt_db.h"
#include "ble.h"
#include "sl_status.h"
#include "math.h"
#include "gpio.h"

typedef enum
{
   scanning,
   sml_discover_services,
   sml_discover_characteristics,
   sml_enable_indication,
   distance_discover_services,
   distance_discover_characteristics,
   distance_enable_indication,
   running
 } conn_state_t;

 void discovery_state_machine(sl_bt_msg_t *evt);


#endif /* SRC_CLIENT_STATE_MACHINE_H_ */
