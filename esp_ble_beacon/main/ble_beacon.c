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

inline static void format_addr(char *addr_str, uint8_t addr[]){
    sprintf(addr_str, "%02X:%02X:%02X:%02X:%02X:%02X",addr[0], addr[1], addr[2], addr[3], addr[4], addr[5]);
}

/**
 * @brief initialize GAP service
 * @return integer
 */
static int gap_init(void){
    ESP_LOGI(TAG,"starting GAP services initialization");

    //local variable
    int rc = 0;

    //init GAP service
    ble_svc_gap_init();

    //set GAP device name
    rc = ble_svc_gap_device_name_set(BLE_DEVICE_NAME);
    if(0!=rc){
        ESP_LOGE(TAG,"failed to set device name to %s, error code: %d",BLE_DEVICE_NAME, rc);
        return rc;
    }

    //set GAP device appearance
    // we can change this appearance as per specific device, right now we will keep it generic (changed by the address)
    rc = ble_svc_gap_device_appearance_set(BLE_GAP_APPEARANCE_GENERIC_TAG); 
    if(0!=rc){
        ESP_LOGE(TAG,"failed to set device appearance, error code: %d",rc);
        return rc;
    }
    ESP_LOGI(TAG,"completed GAP service initialization");
    return rc;
}

/**
 * @brief start advertising
 */
static void start_advertising(){
    ESP_LOGI(TAG,"starting advertising");
    int rc = 0;
    const char *name;
    struct ble_hs_adv_fields adv_fields = {0};
    struct ble_hs_adv_fields rsp_fields  = {0};
    struct ble_gap_adv_params adv_params = {0};

    //set advertising flags
    adv_fields.flags = BLE_HS_ADV_F_DISC_GEN | BLE_HS_ADV_F_BREDR_UNSUP;

    //set device name
    name = ble_svc_gap_device_name();
    adv_fields.name = (uint8_t*)name;
    adv_fields.name_len = strlen(name);
    adv_fields.name_is_complete = 1;

    //set device tx power
    adv_fields.tx_pwr_lvl = BLE_HS_ADV_TX_PWR_LVL_AUTO;
    adv_fields.tx_pwr_lvl_is_present = 1;

    //set device appearance
    adv_fields.appearance = BLE_GAP_APPEARANCE_GENERIC_TAG;
    adv_fields.appearance_is_present = 1;

    //set device LE role
    adv_fields.le_role = BLE_GAP_LE_ROLE_PERIPHERAL;
    adv_fields.le_role_is_present = 1;

    //set advertisement fields
    rc = ble_gap_adv_set_fields(&adv_fields);
    if(0!=rc){
        ESP_LOGE(TAG,"failed to set advertising data, error code: %d",rc);
        return;
    }

    //set device address
    rsp_fields.device_addr = addr_val;
    rsp_fields.device_addr_type = own_addr_type;
    rsp_fields.device_addr_is_present = 1;

    //set uri
    rsp_fields.uri = esp_uri;
    rsp_fields.uri_len = sizeof(esp_uri);

    //set scan response fields
    rc = ble_gap_adv_rsp_set_fields(&rsp_fields);
    if(0!=rc){
        ESP_LOGE(TAG,"failed to set scan response data, error code: %d",rc);
        return;
    }

    //setting non-connectable and general discoverable mode to a beacon
    adv_params.conn_mode = BLE_GAP_CONN_MODE_NON;
    adv_params.disc_mode = BLE_GAP_DISC_MODE_GEN;

    //start advertising
    rc = ble_gap_adv_start(own_addr_type, NULL, BLE_HS_FOREVER, &adv_params, NULL, NULL);

    if(0!=rc){
        ESP_LOGE(TAG,"failed to start advertising, error code: %d",rc);
        return;
    }
    ESP_LOGI(TAG,"advertising started");
}


/**
 * @brief advertisement init
 */
static void adv_init(){
    //local variable
    int rc = 0;
    char addr_str[18] = {0};

    /*
    -> call the ble_hs_util_ensure_addr API to confirm that a usable Bluetooth address is available
    -> ble_hs_util_ensure_addr(0) indicates a preference for using the Public Address. A public address is a globally unique, 48-bit address assigned by the IEEE, typically pre-programmed into the device hardware.
    -> ble_hs_util_ensure_addr(1) indicates a preference for using a Random Address, specifically a Static Random Address. If a suitable random address is not already available (e.g., loaded from non-volatile storage), this function will generate one and attempt to set it
    */
    rc = ble_hs_util_ensure_addr(0);
    if(0!=rc){
        ESP_LOGE(TAG,"device does not have any available bt address !");
        return;
    }

    /*
    ref: https://mynewt.apache.org/latest/network/ble_hs/ble_hs_id.html#c.ble_hs_id_infer_auto
    */
    rc = ble_hs_id_infer_auto(0, &own_addr_type);
    if(0!=rc){
        ESP_LOGE(TAG,"failed to infer address type, error code: %d",rc);
        return;
    }

    /*
    ref: https://mynewt.apache.org/latest/network/ble_hs/ble_hs_id.html#c.ble_hs_id_copy_addr
    */
   rc = ble_hs_id_copy_addr(own_addr_type, addr_val, NULL);
   if(0!=rc){
    ESP_LOGE(TAG,"failed to copy device address, error code: %d",rc);
    return;
   }

   format_addr(addr_str, addr_val);
   ESP_LOGI(TAG,"device address: %s",addr_str);

   //start advertising
   start_advertising();
}

/**
 * @brief   call when nimble reset 
 * @param   nimble reset reason
 * @return  none
 */
static void on_stack_reset(int reason){
    //on reset
    ESP_LOGI(TAG,"nimble stack reset, reset reason: %d",reason);
}

/**
 * @brief advertising init
 */
static void on_stack_sync(){
    adv_init();
}

/**
 * @brief config nimble host
 */
static void nimble_host_config_init(){
    ESP_LOGI(TAG,"config nimble host");
    
    //ref: https://mynewt.apache.org/latest/network/ble_setup/ble_sync_cb.html
    ble_hs_cfg.reset_cb         = on_stack_reset;
    ble_hs_cfg.sync_cb       = on_stack_sync;
    ble_hs_cfg.store_status_cb = ble_store_util_status_rr; 

    //store host config
    ble_store_config_init();
}

/**
 * 
 */
static void nimble_host_task(void *param){
    ESP_LOGI(TAG,"nimble host task started");

    nimble_port_run();

    vTaskDelete(NULL);
}

void app_main(void)
{
    ESP_LOGI(TAG,"Starting....");
    //local variable
    int rc = 0;
    esp_err_t ret = ESP_OK;  // #include "esp_err.h"

    //NVS flash init
    ret = nvs_flash_init();
    
    //checking if we can initialize nvs flash without any error
    //ref: https://docs.espressif.com/projects/esp-idf/en/stable/esp32/api-reference/storage/nvs_flash.html
    if(ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND){
            ESP_ERROR_CHECK(nvs_flash_erase());
            ret = nvs_flash_init();
    }

    if(ESP_OK!=ret){
        ESP_LOGE(TAG,"failed to initialize nvs flash, error code: %d",ret);
        return;
    }

    //NimBLE host stack initialization
    ret = nimble_port_init();
    if(ESP_OK!=ret){
        ESP_LOGE(TAG,"failed to initialize nimble stack, error code: %d",ret);
        return;
    }

// #if CONFIG_BT_NIMBLE_GAP_SERVICE
    //GAP service init
    rc = gap_init();
    if(0!=rc){
        ESP_LOGE(TAG,"failed to initialize GAP service, error code: %d",rc);
        return;
    }
// #endif

    //NimBLE host configuration init
    nimble_host_config_init();

    //start nimble host task
    xTaskCreate(nimble_host_task, "NimBLE Host", 4*1024, NULL, 5, NULL);
    return;
}
