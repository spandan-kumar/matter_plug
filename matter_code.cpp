#include "esp_log.h"
#include "esp_matter.h"
#include "app_priv.h"
#include "driver/gpio.h"

#define TAG "SMART_PLUG"

#define GPIO_RELAY 33  
#define GPIO_LED   34  

static esp_matter_endpoint_t *endpoint;

static esp_err_t app_relay_control_handler(const esp_matter_attr_val_t *val, void *priv_data)
{
    ESP_LOGI(TAG, "State Change Requested: %s", val->val.b ? "ON" : "OFF");

    gpio_set_level(GPIO_RELAY, val->val.b ? 1 : 0);

    gpio_set_level(GPIO_LED, val->val.b ? 1 : 0);

    return ESP_OK;
}

static void app_driver_init()
{
    gpio_config_t io_conf = {
        .intr_type = GPIO_INTR_DISABLE,
        .mode = GPIO_MODE_OUTPUT,
        .pin_bit_mask = (1ULL << GPIO_RELAY),
        .pull_down_en = 0,
        .pull_up_en = 0,
    };
    gpio_config(&io_conf);

    io_conf.pin_bit_mask = (1ULL << GPIO_LED);
    gpio_config(&io_conf);

    gpio_set_level(GPIO_RELAY, 0);
    gpio_set_level(GPIO_LED, 0);
}

static esp_err_t app_create_device()
{
    esp_matter_node_t *node = esp_matter_node_create(NULL, "Matter-SmartPlug");
    if (!node) {
        ESP_LOGE(TAG, "Node creation failed");
        return ESP_FAIL;
    }

    endpoint = esp_matter_endpoint_create(node, ESP_MATTER_ENDPOINT_PLUG, NULL);
    if (!endpoint) {
        ESP_LOGE(TAG, "Endpoint creation failed");
        return ESP_FAIL;
    }

    esp_matter_cluster_create(endpoint, ESP_MATTER_CLUSTER_ON_OFF, NULL, app_relay_control_handler);

    ESP_LOGI(TAG, "Matter Device Created Successfully");
    return ESP_OK;
}

void app_main()
{
    ESP_LOGI(TAG, "Initializing Smart Plug...");
    app_driver_init();   
    app_create_device(); 

    esp_matter_start();
    ESP_LOGI(TAG, "Matter Smart Plug Running!");
}
