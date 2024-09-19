#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"

#include "lwip/sys.h"

#include "wifi.h"

// datos de red y reintentos
#define WIFI_SSID           "TP-Link_9471" // "nombre_de_red"
#define WIFI_PASSWORD       "07737145"     // "contraseña_de_red"
#define REINTENTOS_MAXIMOS  10             // cantidad de reintentos de conexión

// se puede definir en un archivo Kconfig.projbuild

#define WIFI_CONNECTED_BIT BIT0            // define si nos conectamos
#define WIFI_FAIL_BIT      BIT1            // define si hay error al conectarnos

// Es una convención común en el código de ESP-IDF utilizar TAG como una etiqueta o  identificador para los mensajes de registro. 
// Proporciona información sobre el origen o el componente del código que está generando el registro.
static const char *TAG = "Wi-Fi";

// Este grupo de eventos de FreeRTOS se usa para indicarnos cuando estamos conectados a Wi-Fi.
static EventGroupHandle_t grupo_eventos_wifi;
// cantidad de reintentos realizados
static int numero_reintentos = 0;

// semáforo externo
extern xSemaphoreHandle semaforo_conexion_wifi;

// manejador de eventos WiFi
static void manejador_eventos_wifi(void *handler_args, esp_event_base_t event_base, int32_t event_id, void *event_data)
{
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {
        esp_wifi_connect();
    } else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) {
        // si fue desconectado... reintenta REINTENTOS_MAXIMOS 
        if (numero_reintentos < REINTENTOS_MAXIMOS) {
            esp_wifi_connect();
            numero_reintentos++;
            ESP_LOGI(TAG, "reintentando conectarse al AP");
        } else {
            xEventGroupSetBits(grupo_eventos_wifi, WIFI_FAIL_BIT);
        }
        ESP_LOGI(TAG,"Fallo al conectarse al AP");
    } else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        // si obtengo IP 
        ip_event_got_ip_t* event = (ip_event_got_ip_t*) event_data;
        ESP_LOGI(TAG, "Se obtiene ip:" IPSTR, IP2STR(&event->ip_info.ip));
        numero_reintentos = 0;
        xEventGroupSetBits(grupo_eventos_wifi, WIFI_CONNECTED_BIT);
        xSemaphoreGive(semaforo_conexion_wifi);
    }
}

void iniciar_wifi() 
{
    // se crea grupo de eventos
    grupo_eventos_wifi = xEventGroupCreate();

    // se inicializa el subsistema de gestión de interfaces de red
    ESP_ERROR_CHECK(esp_netif_init());

    ESP_ERROR_CHECK(esp_event_loop_create_default());
    // se inicializa en modo estación con los parámetros predefinidos
    esp_netif_create_default_wifi_sta();

    // se crea struct de configuración de WiFi con los parámetros predefinidos
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    // se inicializa la configuración WiFi
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    // manejador de eventos de WiFi
    ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &manejador_eventos_wifi, NULL));
    // manejador de eventos de la interfaz de red
    ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &manejador_eventos_wifi, NULL));

    wifi_config_t wifi_config = {
        .sta = {
            .ssid = WIFI_SSID,
            .password = WIFI_PASSWORD,
        },
    };
    // modos
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config));
    // inicio de WiFi
    ESP_ERROR_CHECK(esp_wifi_start());

    ESP_LOGI(TAG, "Inicio Wi-Fi finalizado.");

    // se espera hasta que la conexión se haya establecido (WIFI_CONNECTED_BIT) 
    // o hasta que la conexión haya fallado (WIFI_FAIL_BIT)
    // Los bits se definen en el manejador de eventos 
    EventBits_t bits = xEventGroupWaitBits(grupo_eventos_wifi, WIFI_CONNECTED_BIT | WIFI_FAIL_BIT,
            pdFALSE, pdFALSE, portMAX_DELAY);

    // se verifica que ha sucedido
    if (bits & WIFI_CONNECTED_BIT) {
        ESP_LOGI(TAG, "Conectado al AP con SSID:%s y password:%s", WIFI_SSID, WIFI_PASSWORD);
    } else if (bits & WIFI_FAIL_BIT) {
        ESP_LOGI(TAG, "Fallo al conectarse al AP con SSID:%s y password:%s", WIFI_SSID, WIFI_PASSWORD);
    } else {
        ESP_LOGE(TAG, "Evento no esperado");
    }

//    ESP_ERROR_CHECK(esp_event_handler_unregister(IP_EVENT, IP_EVENT_STA_GOT_IP, &manejador_eventos_wifi));
//    ESP_ERROR_CHECK(esp_event_handler_unregister(WIFI_EVENT, ESP_EVENT_ANY_ID, &manejador_eventos_wifi));
    vEventGroupDelete(grupo_eventos_wifi);
}