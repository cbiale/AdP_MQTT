#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_event.h"
#include "nvs_flash.h"
#include "esp_wifi.h"
#include "esp_log.h"
#include "mqtt_client.h"

// datos de red y broker
#define WIFI_SSID "TP-Link_9471" // "nombre_de_red"
#define WIFI_PASSWORD "07737145" // "contraseña_de_red"
#define MQTT_BROKER_URL "mqtt://broker.example.com"

// define el ID del dispositivo
#define DISPOSITIVO_ID "1"
#define TOPICO "medicion/" DISPOSITIVO_ID

esp_mqtt_client_handle_t cliente;

// manejador de eventos
// mejorar con respecto a ejemplos de ESP-IDF
void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data)
{
    esp_mqtt_event_handle_t event = event_data;
    switch ((esp_mqtt_event_id_t) event_id)
    {
        case MQTT_EVENT_CONNECTED:
            ESP_LOGI(TAG, "MQTT_EVENT_CONNECTED");
            break;
        case MQTT_EVENT_DISCONNECTED:
            ESP_LOGI(TAG, "MQTT_EVENT_DISCONNECTED");
            break;
        case MQTT_EVENT_SUBSCRIBED:
            ESP_LOGI(TAG, "MQTT_EVENT_SUBSCRIBED, msg_id=%d", event->msg_id);
            break;
        case MQTT_EVENT_PUBLISHED:
            ESP_LOGI(TAG, "MQTT_EVENT_PUBLISHED, msg_id=%d", event->msg_id);
            break;
        case MQTT_EVENT_DATA:
            ESP_LOGI(TAG, "MQTT_EVENT_DATA");
            break;
        default:
            ESP_LOGI(TAG, "Other event id:%d", event_id);
            break;
    }
}

// inicio de WiFi
void wifi_init()
{
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    // inicia WiFi
    esp_wifi_init(&cfg);
    // modo: WiFi station mode
    esp_wifi_set_mode(WIFI_MODE_STA);
    wifi_config_t wifi_config = {
        .sta = {
            .ssid = WIFI_SSID,
            .password = WIFI_PASSWORD,
            .pmf_cfg = {
                .capable = true,
                .required = false
            },
        },
    };
    // configura WiFi
    esp_wifi_set_config(WIFI_IF_STA, &wifi_config);
    // inicia WiFi
    esp_wifi_start();
}

void enviar_medicion() {

    // Generar un valor aleatorio entre 0 y 40
    int valor = rand() % 41;

    // Crea objeto JSON
    cJSON *root = cJSON_CreateObject();
    cJSON_AddNumberToObject(root, "valor", valor);

    // Convierte objeto JSON a una cadena
    char *json_string = cJSON_PrintUnformatted(root);

    // Publica la medición en el topico
    // Argumentos:
    // - Manejador de cliente MQTT
    // - Topico (string)
    // - Carga útil (string)
    // - Largo: si se establece en 0, la longitud se calcula a partir de la carga útil
    // - QoS del mensaje de publicación
    // - Bandera o flag para retener
    esp_mqtt_client_publish(cliente, TOPICO, json_string, 0, 0, 0);

    // Libera memoria
    free(json_string);
    cJSON_Delete(root);
}

void mqtt_app_start()
{
    // configuración de broker
    esp_mqtt_client_config_t mqtt_cfg = {
        .uri = MQTT_BROKER_URL,
    };
    // Crea un manejador de cliente MQTT basado en la configuración pasada como argumento
    cliente = esp_mqtt_client_init(&mqtt_cfg);
    // Registra eventos MQTT
    // - Manejador de cliente MQTT
    // - Tipo de eventos
    // - Función llamada cuando un evento se publica en la cola
    // - Contexto de los controladores
    esp_mqtt_client_register_event(cliente, ESP_EVENT_ANY_ID, mqtt_event_handler, NULL);
    // Inicia el cliente MQTT con el identificador de cliente ya creado
    esp_mqtt_client_start(cliente);
}


void app_main() 
{
    wifi_init();
    mqtt_app_start();

    while (1)
    {
        // Realiza una medición y envía el valor por MQTT
        enviar_medicion();
        
        // Espera 10 segundos antes de enviar la próxima medición
        vTaskDelay(pdMS_TO_TICKS(10000));
    }
}
