#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <string.h>
#include "esp_system.h"
#include "esp_netif.h"
#include "esp_event.h"
#include "esp_log.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "freertos/queue.h"

#include "lwip/sockets.h"
#include "lwip/dns.h"
#include "lwip/netdb.h"

#include "mqtt_client.h"

#include "mqtt.h"

// URL del broker MQTT
// ver lo devuelto por el comando ip addr (parte inet)
#define MQTT_BROKER_URL "mqtt://192.168.0.104:1883" 

// TAG para mensajes de registro de MQTT
static const char *TAG = "MQTT";

// semáforo definido en main.c
extern xSemaphoreHandle semaforo_conexion_mqtt;

// cliente MQTT
esp_mqtt_client_handle_t cliente;

static esp_err_t manejador_eventos_mqtt_cb (esp_mqtt_event_handle_t event) 
{
    esp_mqtt_client_handle_t cliente = event->client;
    int msg_id;
    // eventos MQTT
    switch (event->event_id)
    {
        case MQTT_EVENT_CONNECTED:
            ESP_LOGI(TAG, "MQTT_EVENT_CONNECTED");
            xSemaphoreGive(semaforo_conexion_mqtt);
            break;
        case MQTT_EVENT_DISCONNECTED:
            ESP_LOGI(TAG, "MQTT_EVENT_DISCONNECTED");
            break;
        case MQTT_EVENT_SUBSCRIBED:
            ESP_LOGI(TAG, "MQTT_EVENT_SUBSCRIBED, msj_id=%d", event->msg_id);
            break;
        case MQTT_EVENT_UNSUBSCRIBED:
            ESP_LOGI(TAG, "MQTT_EVENT_UNSUBSCRIBED, msj_id=%d", event->msg_id);
            break;
        case MQTT_EVENT_PUBLISHED:
            ESP_LOGI(TAG, "MQTT_EVENT_PUBLISHED, msj_id=%d", event->msg_id);
            break;
        case MQTT_EVENT_DATA:
            ESP_LOGI(TAG, "MQTT_EVENT_DATA");
            printf("Tópico=%.*s\r\n", event->topic_len, event->topic);
            printf("Datos=%.*s\r\n", event->data_len, event->data);
            break;
        case MQTT_EVENT_ERROR:
            ESP_LOGI(TAG, "MQTT_EVENT_ERROR");
        default:
            ESP_LOGI(TAG, "Otro evento id:%d", event->event_id);
            break;
    }
    return ESP_OK;
}

// manejador de eventos MQTT
static void manejador_eventos_mqtt(void *handler_args, esp_event_base_t event_base, int32_t event_id, void *event_data)
{
    manejador_eventos_mqtt_cb(event_data);
}

void iniciar_mqtt()
{
    // configuración de broker
    esp_mqtt_client_config_t mqtt_cfg = {
        .uri = MQTT_BROKER_URL,
    };
    // Crea un manejador de cliente MQTT basado en la configuración pasada como argumento
    cliente = esp_mqtt_client_init(&mqtt_cfg);
    // Registra eventos MQTT
    // - Manejador de cliente MQTT
    // - Tipo de eventos (todos)
    // - Función llamada cuando un evento se publica en la cola
    // - argumento al manejador de eventos
    esp_mqtt_client_register_event(cliente, ESP_EVENT_ANY_ID, manejador_eventos_mqtt, cliente);
    // Inicia el cliente MQTT con el identificador de cliente ya creado
    esp_mqtt_client_start(cliente);
}

void enviar_mensaje_mqtt(char * topico, char * mensaje) 
{
    int mensaje_id = esp_mqtt_client_publish(cliente, topico, mensaje, 0, 1, 0);
    ESP_LOGI(TAG, "Mensaje enviado, msj_id=%d, topico=%s, mensaje=%s", mensaje_id, topico, mensaje);
}