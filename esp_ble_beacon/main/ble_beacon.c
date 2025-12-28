/*
 *  ble_beacon.cpp
 *
 *  Created on: Dec, 27, 2025
 *      Author: Deepanshu Tanwar
 *
 *  Modified on: Dec 27, 2025
 *      Author: Deepanshu Tanwar
 */

#include "ble_beacon.h"

// /**
//  * @brief initialize GAP service
//  * @return integer
//  */
// int gap_init(void){

//     //local variable
//     int rc = 0;

//     //init GAP service
//     ble_svc_gap_init();

//     //set GAP device name
//     rc = ble_svc_gap_device_name_set(BLE_DEVICE_NAME);
// }

void app_main(void)
{
//     //local variable
//     int rc = 0;
//     esp_err_t ret = ESP_OK;  // #include "esp_err.h"

//     //NVS flash init
//     ret = nvs_flash_init();
    
//     //checking if we can initialize nvs flash without any error
//     //ref: https://docs.espressif.com/projects/esp-idf/en/stable/esp32/api-reference/storage/nvs_flash.html
//     if(ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND){
//             ESP_ERROR_CHECK(nvs_flash_erase());
//             ret = nvs_flash_init();
//     }

//     if(ESP_OK!=ret){
//         ESP_LOGE(TAG,"failed to initialize nvs flash, error code: %d",ret);
//         return;
//     }

//     //NimBLE host stack initialization
//     ret = nimble_port_init();
//     if(ESP_OK!=ret){
//         ESP_LOGE(TAG,"failed to initialize nimble stack, error code: %d",ret);
//         return;
//     }

// #if CONFIG_BT_NIMBLE_GAP_SERVICE
//     //GAP service init
//     rc = gap_init();



}
