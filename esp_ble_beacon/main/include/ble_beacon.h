#include <stdio.h>
#include <string.h>
#include "esp_err.h"
#include "nvs_flash.h"
#include "esp_log.h"
#include "services/gap/ble_svc_gap.h"
#include "host/ble_hs.h"
#include "host/ble_hs_adv.h"
#include "host/util/util.h"
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include "nimble/nimble_port.h"



//defines
#define TAG                             "APP"
#define BLE_DEVICE_NAME                 "BLE_Beacon"
#define BLE_GAP_APPEARANCE_GENERIC_TAG  0x0200
#define BLE_GAP_LE_ROLE_PERIPHERAL      0x00
#define BLE_GAP_URI_PREFIX_HTTPS        0x17

//global
static uint8_t own_addr_type;
static uint8_t addr_val[6] = {0};
static uint8_t esp_uri[] = {BLE_GAP_URI_PREFIX_HTTPS, '/', '/', 'e', 's', 'p', 'r', 'e', 's', 's', 'i', 'f', '.', 'c', 'o', 'm'};

//functions
inline static void format_addr(char *addr_str, uint8_t addr[]);
static int gap_init(void);
static void start_advertising();
static void adv_init();
static void on_stack_reset(int reason);
static void on_stack_sync();
static void nimble_host_config_init();
static void nimble_host_task(void *param);
static void ble_store_config_init(void);

