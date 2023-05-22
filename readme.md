# AdP 2023

## MQTT usando wildcards en el servidor

En el directorio `esp_32` se encuentra un ejemplo de aplicación para ESP-32 que realiza publicaciones usando el tópico `medicion/id` donde `id` corresponde al identificador que representa al nodo en el sistema.

En el directorio `nodo` se encuentra una aplicación en funcionalidad semejante a la anterior pero implementada en TypeScript.

En el directorio `servidor` se encuentra un aplicación desarrollada en Typescript que define un servidor usando `express` y que se encuentra subscrita al tópico `medicion/#`.

El archivo `iniciar_broker.sh` inicia un contenedor con EMQX.
