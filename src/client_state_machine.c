/*
 * client_state_machine.c
 *
 *  Created on: Nov 23, 2021
 *      Author: 17207
 */
#include "client_state_machine.h"
#define INCLUDE_LOG_DEBUG 1
#include "src/log.h"


void discovery_state_machine(sl_bt_msg_t *evt)
{
  conn_state_t  current_conn_state;
  static conn_state_t  next_conn_state=scanning;
  current_conn_state=next_conn_state;
  sl_status_t sc_c;

  static const uint8_t sml_char[2]={0x1c, 0x2a};
  static const uint8_t sml_service[2]={0x09, 0x18};
  static const uint8_t distance_char[4] = {0x89, 0x62, 0x13, 0x2d};
  static const uint8_t distance_service[4]={0x89, 0x62, 0x13, 0x2d };
  ble_data_struct_t *bleDataPtr = get_ble_data();
  switch (current_conn_state)
     {
      case scanning:
        if( SL_BT_MSG_ID(evt->header)== sl_bt_evt_connection_opened_id)
          {
            printf("Scanning");
            displayPrintf(DISPLAY_ROW_CONNECTION, "Connected");
            sc_c = sl_bt_gatt_discover_primary_services_by_uuid(bleDataPtr->connection_handle,
                                                                          sizeof(sml_service),
                                                                          (const uint8_t*)sml_service);
            if(sc_c!=SL_STATUS_OK)
              {
               LOG_INFO("sl_bt_gatt_discover_primary_services_by_uuid()  returned != 0 status=0x%04x", (unsigned int) sc_c);
               }
            next_conn_state=sml_discover_services;
          }
      break;

      case sml_discover_services:
         if(SL_BT_MSG_ID(evt->header)==sl_bt_evt_gatt_procedure_completed_id )
           {
             printf("discover_services");
             sc_c = sl_bt_gatt_discover_characteristics_by_uuid(bleDataPtr->connection_handle,
                                                             bleDataPtr->sml_service_handle,
                                                             sizeof(sml_char),
                                                             (const uint8_t*)sml_char);
             if(sc_c!=SL_STATUS_OK)
               {
                LOG_INFO("sl_bt_gatt_discover_characteristics_by_uuid()  returned != 0 status=0x%04x", (unsigned int) sc_c);
               }
          next_conn_state =sml_discover_characteristics;
           }
      break;

      case sml_discover_characteristics:
        if(SL_BT_MSG_ID(evt->header)==sl_bt_evt_gatt_procedure_completed_id)
         {
          printf("discover_characteristics");
          sc_c = sl_bt_gatt_set_characteristic_notification(bleDataPtr->connection_handle,
                                                           bleDataPtr->sml_characteristic_handle,
                                                          sl_bt_gatt_indication);

          if(sc_c!=SL_STATUS_OK)
            {
            LOG_INFO("sl_bt_gatt_set_characteristic_notification()  returned != 0 status=0x%04x", (unsigned int) sc_c);
            }
          next_conn_state=sml_enable_indication;
         }
      break;

      case sml_enable_indication:
         if(SL_BT_MSG_ID(evt->header)==sl_bt_evt_gatt_procedure_completed_id)
           {
             printf("enable_indication");
            //sc_c = sl_bt_gatt_send_characteristic_confirmation(bleDataPtr->connection_handle);
            //if(sc_c!=SL_STATUS_OK)
              //{
              //LOG_INFO("sl_bt_gatt_send_characteristic_confirmation()  returned != 0 status=0x%04x", (unsigned int) sc_c);
              //}
             sc_c = sl_bt_gatt_discover_primary_services_by_uuid(bleDataPtr->connection_handle,
                                                                           sizeof(distance_service),
                                                                           (const uint8_t*)distance_service);
            next_conn_state=distance_discover_services;
           }
      break;

      case distance_discover_services:
        if(SL_BT_MSG_ID(evt->header)==sl_bt_evt_gatt_procedure_completed_id)
          {
            printf("btn_service");
            sc_c = sl_bt_gatt_discover_characteristics_by_uuid(bleDataPtr->connection_handle,
                                                                       bleDataPtr->sml_service_handle,
                                                                       sizeof(distance_char),
                                                                       (const uint8_t*)distance_char);
            next_conn_state=distance_discover_characteristics;
          }

      break;

      case distance_discover_characteristics:
        if(SL_BT_MSG_ID(evt->header)==sl_bt_evt_gatt_procedure_completed_id)
          {
            printf("btn_characteristics");
            sc_c = sl_bt_gatt_set_characteristic_notification(bleDataPtr->connection_handle,
                                                                     bleDataPtr->distance_characteristic_handle,
                                                                    sl_bt_gatt_indication);

            if(sc_c!=SL_STATUS_OK)
             {
             LOG_INFO("sl_bt_gatt_set_characteristic_notification()  returned != 0 status=0x%04x", (unsigned int) sc_c);
             }
             next_conn_state=distance_enable_indication;
          }

      break;

      case distance_enable_indication:
        if(SL_BT_MSG_ID(evt->header)==sl_bt_evt_gatt_procedure_completed_id)
          {
            printf("enable_indication");
            next_conn_state=running;
          }

      break;
      case running:
        if(SL_BT_MSG_ID(evt->header)==sl_bt_evt_gatt_characteristic_value_id)
         {
       printf("running");
          if(evt->data.evt_gatt_characteristic.characteristic == bleDataPtr->sml_characteristic_handle)
            {
         sc_c = sl_bt_gatt_send_characteristic_confirmation(bleDataPtr->connection_handle);
         displayPrintf(DISPLAY_ROW_CONNECTION, "Handling Indications");

         if(evt->data.evt_gatt_characteristic_value.value.data[0]==1)
           {
          displayPrintf(DISPLAY_ROW_SMLVALUE,"Light On");
          gpioLed0SetOn();
           }
          else if(evt->data.evt_gatt_characteristic_value.value.data[0]==0)
          displayPrintf(DISPLAY_ROW_SMLVALUE,"Light Off");
         gpioLed0SetOff();

        }
          if(evt->data.evt_gatt_characteristic.characteristic == bleDataPtr->distance_characteristic_handle)
            {
              sc_c = sl_bt_gatt_send_characteristic_confirmation(bleDataPtr->connection_handle);
              if(evt->data.evt_gatt_characteristic_value.value.data[0]==1)
              displayPrintf(DISPLAY_ROW_9,"Too Close");
              else if(evt->data.evt_gatt_characteristic_value.value.data[0]==0)
              displayPrintf(DISPLAY_ROW_9,"Safe");
            }
          next_conn_state=running;
         }
        if(SL_BT_MSG_ID(evt->header)==sl_bt_evt_connection_closed_id)
          {
           sc_c = sl_bt_scanner_start(sl_bt_gap_1m_phy, sl_bt_scanner_discover_generic);
           if(sc_c!=SL_STATUS_OK)
             {
              displayPrintf(DISPLAY_ROW_SMLVALUE,"");
              displayPrintf(DISPLAY_ROW_BTADDR, "");
              displayPrintf(DISPLAY_ROW_9,"");
              LOG_INFO("sl_bt_scanner_start()  returned != 0 status=0x%04x", (unsigned int) sc_c);
             }
           next_conn_state =scanning;
          }
      break;

      default:
      break;
     }//end of switch
}
