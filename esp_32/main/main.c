#include <stdio.h>
#include <string.h>
#include "nvs_flash.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_http_client.h"

#include "freertos/semphr.h"

#include "wifi.h"
#include "mqtt.h"
#include "cJSON.h"

// define el ID del dispositivo y topico
#define DISPOSITIVO_ID "1"
#define TOPICO "medicion/" DISPOSITIVO_ID

// semáforos
xSemaphoreHandle semaforo_conexion_wifi;
xSemaphoreHandle semaforo_conexion_mqtt;


void conectado_wifi(void * params) 
{
    while (true) 
    {
        if (xSemaphoreTake(semaforo_conexion_wifi, portMAX_DELAY))
        {
            // procesar al conectarse a internet
            iniciar_mqtt();
        }
    }
}

void comunicacion_broker(void * params) 
{
    // esperar hasta que se libere el semaforo 
    // espera bloqueante por uso de portMAX_DELAY
    if (xSemaphoreTake(semaforo_conexion_mqtt, portMAX_DELAY)) 
    {
        while (true) 
        {
            // Generar un valor aleatorio entre 0 y 40
            int valor = rand() % 41;

            // Crea objeto JSON
            cJSON *objeto_json = cJSON_CreateObject();
            cJSON_AddNumberToObject(objeto_json, "valor", valor);

            // Convierte objeto JSON a una cadena
            char *json_string = cJSON_PrintUnformatted(objeto_json);

            // enviar mensaje MQTT
            enviar_mensaje_mqtt(TOPICO, json_string);

            // Libera memoria
            free(json_string);
            cJSON_Delete(objeto_json);

            // espera 10 segundos
            vTaskDelay(10000 / portTICK_PERIOD_MS);
        }
    }
}

void app_main() 
{
    // inicia NVS
    esp_err_t ret = nvs_flash_init();
    ESP_ERROR_CHECK(esp_netif_init());
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
      ESP_ERROR_CHECK(nvs_flash_erase());
      ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    // iniciamos semáforos
    semaforo_conexion_wifi = xSemaphoreCreateBinary();
    semaforo_conexion_mqtt = xSemaphoreCreateBinary();

    // se inicia WiFi
    iniciar_wifi();

    // tareas
    xTaskCreate(&conectado_wifi, "Conexion a MQTT", 4096, NULL, 1, NULL);
    xTaskCreate(&comunicacion_broker, "Comunicacion con el broker", 4096, NULL, 1, NULL);
}


