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
 *@assignment  final project
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
#include "ble.h"
#include "app_assert.h"

// Include logging for this file
#define INCLUDE_LOG_DEBUG 1
#include "src/log.h"
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
  ble_data_loc->connection_enable = false;
  ble_data_loc->indication_in_flight = false;
  ble_data_loc->motion_indication_enable = false;
  ble_data_loc->light_indication_enable = false;
  ble_data_loc->smart_garage_bonded = false;
  ble_data_loc->smart_garage_confirmation_require = false;
}

bd_addr server_addr=SERVER_BT_ADDRESS;

/**
 * Handle ble event based on the ble stack signal
 * @param[in] ble stack signal
 * */
void handle_ble_event(sl_bt_msg_t *evt){
  ble_data_struct_t *ble_data_loc = get_ble_data();
  sl_status_t sc;
  bd_addr addr;
  uint8_t address_type;
  uint8_t flag;
  ble_data.server_address=server_addr;
  switch(SL_BT_MSG_ID(evt->header)){
    //Events common to both servers and clients

    case sl_bt_evt_system_boot_id:
        sc = sl_bt_system_get_identity_address(&addr , &address_type);
        displayInit();
        displayPrintf(DISPLAY_ROW_NAME,"Client");
                displayPrintf(DISPLAY_ROW_BTADDR,"%02X:%02X:%02X:%02X:%02X:%02X",ble_data.server_address.addr[5],
                              ble_data.server_address.addr[4],
                              ble_data.server_address.addr[3],
                              ble_data.server_address.addr[2],
                              ble_data.server_address.addr[1],
                              ble_data.server_address.addr[0]);
                sc = sl_bt_scanner_set_mode(sl_bt_gap_1m_phy, 0);
                if(sc!=SL_STATUS_OK)
                  {
                  LOG_INFO("sl_bt_scanner_set_mode()  returned != 0 status=0x%04x", (unsigned int) sc);
                  }

                sc = sl_bt_scanner_set_timing(sl_bt_gap_1m_phy, 80, 40);
                if(sc!=SL_STATUS_OK)
                  {
                   LOG_INFO("sl_bt_scanner_set_timing()  returned != 0 status=0x%04x", (unsigned int) sc);
                  }
                displayPrintf(DISPLAY_ROW_CONNECTION, "Discovering");
                //ble_data.PB0client_button_pressed=false;
                sc = sl_bt_connection_set_default_parameters(60,
                                                             60,
                                                             3,
                                                             850,
                                                             0,
                                                             0);
                if(sc!=SL_STATUS_OK)
                    {
                    LOG_INFO("sl_bt_scanner_default_parameters()  returned != 0 status=0x%04x", (unsigned int) sc);
                    }

                sc= sl_bt_scanner_start(sl_bt_gap_1m_phy, sl_bt_scanner_discover_generic);
                if(sc!=SL_STATUS_OK)
                    {
                    LOG_INFO("sl_bt_scanner_start()  returned != 0 status=0x%04x", (unsigned int) sc);
                    }
                sc = sl_bt_sm_delete_bondings();
                sc = sl_bt_sm_configure (
                     SM_CONFIG_FLAG,
                     sm_io_capability_displayyesno
                   );
                   if(sc!=SL_STATUS_OK)
                      {
                     LOG_INFO(" sl_bt_sm_configure() returned != 0 status=0x%04x", (unsigned int) sc);
                      }
        break;

    case sl_bt_evt_scanner_scan_report_id:
       for(int i=0;i<=5;i++)
      {
        if(evt->data.evt_scanner_scan_report.address.addr[i]!=addr.addr[i])
        {
         flag=0;
        }
        else
         flag=1;
       }
        if(flag!=0)
         {
          sc=sl_bt_connection_open(evt->data.evt_scanner_scan_report.address,
                                   evt->data.evt_scanner_scan_report.address_type,
                                   sl_bt_gap_1m_phy,
                                   NULL);
          if(sc!=SL_STATUS_OK)
            {
             LOG_INFO("sl_bt_connection_open()  returned != 0 status=0x%04x", (unsigned int) sc);
            }
            sc=sl_bt_scanner_stop(evt->data.evt_scanner_scan_report.address);
            if(sc!=SL_STATUS_OK)
             {
              LOG_INFO("sl_bt_scanner_stop()  returned != 0 status=0x%04x", (unsigned int) sc);
             }
            flag=0;
            }
       break;
      /**Call when client connected**/
    case sl_bt_evt_connection_opened_id:
   ble_data.connection_handle=evt->data.evt_connection_opened.connection;
      break;

      /**Call when a connection closed**/
    case sl_bt_evt_connection_closed_id:
      sc = sl_bt_sm_delete_bondings();
      break;



    /**Call when parameters are set**/
     case sl_bt_evt_sm_confirm_bonding_id:
        if(evt->data.evt_sm_confirm_bonding.connection==connection_handle){
          sc = sl_bt_sm_bonding_confirm(connection_handle,1);
        }else{
          sc = sl_bt_sm_bonding_confirm(connection_handle,0);
        }

        if(sc!=SL_STATUS_OK){
          LOG_ERROR("bonding confirm failed %d", sc);
        }
        break;

    case sl_bt_evt_sm_confirm_passkey_id:
      displayPrintf(DISPLAY_ROW_PASSKEY, "%d", evt->data.evt_sm_confirm_passkey.passkey);
      displayPrintf(DISPLAY_ROW_ACTION, "Confirm with PB0");
      break;

    case sl_bt_evt_sm_bonded_id:
       displayPrintf(DISPLAY_ROW_CONNECTION, "Bonded");
       displayPrintf(DISPLAY_ROW_PASSKEY, "");
       displayPrintf(DISPLAY_ROW_ACTION, "");

       break;

     case  sl_bt_evt_sm_bonding_failed_id:
       LOG_ERROR("Bonding Failed");
       break;
     case sl_bt_evt_gatt_service_id:
        if(evt->data.evt_gatt_service.uuid.len==2)
          {
        ble_data.sml_service_handle=evt->data.evt_gatt_service.service;
          }//16
        if(evt->data.evt_gatt_service.uuid.len==4)
             {
           ble_data.distance_service_handle=evt->data.evt_gatt_service.service;
             }
        break;

      case sl_bt_evt_gatt_characteristic_id:
        if(evt->data.evt_gatt_characteristic.uuid.len==2)
          {
        ble_data.sml_characteristic_handle=evt->data.evt_gatt_characteristic.characteristic;
          }
        if(evt->data.evt_gatt_characteristic.uuid.len==4)
            {
          ble_data.distance_characteristic_handle=evt->data.evt_gatt_characteristic.characteristic;
            }
        break;


    case sl_bt_evt_system_external_signal_id:
      switch(evt->data.evt_system_external_signal.extsignals){
          case pb0_pressed:
           {
          sl_bt_sm_passkey_confirm(ble_data.connection_handle, 1);
           }}

      break;

      /**Call when change of CCCD or indication confirmation received by client**/

       case sl_bt_evt_gatt_procedure_completed_id:
       if(evt->data.evt_gatt_procedure_completed.result==0x110F)
               {
                sc=sl_bt_sm_increase_security(ble_data.connection_handle);
               }
     break;

  }
}
