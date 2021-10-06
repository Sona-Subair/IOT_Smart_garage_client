/***********************************************************************
 *@file        ble.c
 *
 *@version     0.0.1
 *
 *@brief       function to handle ble event
 *@brief
 *
 *@author      Jiabin Lin, jili9036@Colorado.edu
 *
 *@date        Sep 26, 2021
 *
 *@institution University of Colorado Boulder (UCB)
 *
 *@course      ECEN 5823-001: IoT Embedded Firmware (Fall 2021)
 *
 *@instructor  David Sluiter
 *
 *@assignment  ecen5823-assignment2-JiabinLin12
 *
 *@due         Oct 3, 2020
 *
 *@resources   Utilized Silicon Labs' EMLIB peripheral libraries to
 *             implement functionality.
 *
 *
 *@copyright   All rights reserved. Distribution allowed only for the
 *             use of assignment grading. Use of code excerpts allowed at the
 *             discretion of author. Contact for permission.  */
#include "ble.h"
#include "app_assert.h"

// Include logging for this file
#define INCLUDE_LOG_DEBUG 1
#include "src/log.h"

#include "i2c.h"
#include "autogen/gatt_db.h"

#include "ble_device_type.h"

ble_data_struct_t ble_data;

static uint8_t advertising_set_handle = 0xff;
uint8_t connection_handle = 0xff;
/**Get private ble data*/
ble_data_struct_t *get_ble_data(){
  return (&ble_data);
}
/**Get private connection handle*/
uint8_t *get_connection_handle(){
  return (&connection_handle);
}

/**private ble data initialization*/
void ble_data_init(){
  ble_data_struct_t *ble_data_loc = get_ble_data();
  ble_data_loc->htm_indication_enable = false;
  ble_data_loc->htm_connection_enable = false;
  ble_data_loc->htm_indication_on_flight = false;
}

/**
 * Handle ble event based on the ble stack signal
 * @param[in] ble stack signal
 * */
void handle_ble_event(sl_bt_msg_t *evt){
  ble_data_struct_t *ble_data_loc = get_ble_data();
  sl_status_t sc;
  bd_addr addr;
  uint8_t address_type;
  uint8_t system_id[6];
  switch(SL_BT_MSG_ID(evt->header)){
    //Events common to both servers and clients

    case sl_bt_evt_system_boot_id:
        ble_data_init();
        displayInit();
        sc = sl_bt_advertiser_create_set(&advertising_set_handle);
        app_assert_status(sc);

        sc = sl_bt_advertiser_set_timing( advertising_set_handle,
                                          MIN_ADV_INTERVAL,
                                          MAX_ADV_INTERVAL,
                                          ADV_DURATION,
                                          MAX_ADV_EVENT);
        app_assert_status(sc);


        sc = sl_bt_advertiser_start(advertising_set_handle,
                                    sl_bt_advertiser_general_discoverable,
                                    sl_bt_advertiser_connectable_scannable);
        app_assert_status(sc);

        sc = sl_bt_system_get_identity_address(&addr, &address_type);
        app_assert_status(sc);
        displayPrintf(DISPLAY_ROW_NAME, "Server");
        displayPrintf(DISPLAY_ROW_CONNECTION, "Advertising");
        displayPrintf(DISPLAY_ROW_ASSIGNMENT, "A6");
        displayPrintf(DISPLAY_ROW_BTADDR, "%02X:%02X:%02X:%02X:%02X:%02X",addr.addr[0],
                      addr.addr[1],addr.addr[2],addr.addr[3],addr.addr[4], addr.addr[5]);
      break;

      /**Call when client connected**/
    case sl_bt_evt_connection_opened_id:
      ble_data_loc->htm_connection_enable = true;
      connection_handle = evt->data.evt_connection_opened.connection;
      sc = sl_bt_connection_set_parameters( connection_handle,
                                            MIN_CNT_INTERVAL,
                                            MAX_CNT_INVERVAL,
                                            SLAVE_LATENCY,
                                            SUPERVISON_TIMEOUT,
                                            MIN_CNT_EVT_LENGTH,
                                            MAX_CNT_EVT_LENGTH);
      app_assert_status(sc);

      sc = sl_bt_advertiser_stop(advertising_set_handle);
      app_assert_status(sc);

      displayPrintf(DISPLAY_ROW_CONNECTION, "Connected");


      break;

      /**Call when a connection closed**/
    case sl_bt_evt_connection_closed_id:
      ble_data_loc->htm_connection_enable = false;
      sc = sl_bt_advertiser_start(
             advertising_set_handle,
             sl_bt_advertiser_general_discoverable,
             sl_bt_advertiser_connectable_scannable);
      app_assert_status(sc);
      displayPrintf(DISPLAY_ROW_CONNECTION, "Advertising");
      displayPrintf(DISPLAY_ROW_TEMPVALUE, "");
      break;


    /**Call when parameters are set**/
    case sl_bt_evt_connection_parameters_id:
#if (PRINT_PARAMS) // DOS: compiler error in formatting these parameters, included with ( ) and typecast to int
      LOG_INFO("interval=%d, latency=%d, timeout=%d",
               (int) (evt->data.evt_connection_parameters.interval*1.25),
               (int) (evt->data.evt_connection_parameters.latency),
               (int) (evt->data.evt_connection_parameters.timeout) );
#endif
      break;

    case sl_bt_evt_system_external_signal_id:
      break;

      /**Call when change of CCCD or indication confirmation received by client**/
    case sl_bt_evt_gatt_server_characteristic_status_id:
      if((evt->data.evt_gatt_server_characteristic_status.characteristic ==gattdb_temperature_measurement))
        {
          if(evt->data.evt_gatt_server_characteristic_status.status_flags ==sl_bt_gatt_server_client_config){            //Configuration changed
            if(evt->data.evt_gatt_server_characteristic_status.client_config_flags == sl_bt_gatt_server_indication) {     //Indication enable
              ble_data_loc->htm_indication_enable = true;
            } else if(evt->data.evt_gatt_server_characteristic_status.client_config_flags == sl_bt_gatt_server_disable) { // Indication disable
              ble_data_loc->htm_indication_enable = false;
            }
          }else if(evt->data.evt_gatt_server_characteristic_status.status_flags ==sl_bt_gatt_server_confirmation){       //confirmation from client
              ble_data_loc->htm_indication_on_flight = false;
          }
      }
      break;

    case sl_bt_evt_system_soft_timer_id:
        displayUpdate();
        break;
      /**Should never reach here**/
    case sl_bt_evt_gatt_server_indication_timeout_id:
      LOG_ERROR("Client did not respond");
      break;


    //Event just for clients
  }
}
