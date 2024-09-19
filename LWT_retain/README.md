# Arquitectura de Protocolos

## MQTT simple con un nodo emisor (con LWT) y otro receptor

En el directorio `nodo_emisor` se encuentra un ejemplo de aplicación  implementada en TypeScript que realiza publicaciones usando el tópico `medicion` y retiene el mensaje en el broker.

En el directorio `nodo_receptor` se encuentra un ejemplo de aplicación  implementada en TypeScript que se suscribe al tópico `medicion` e imprime el mensaje recibido. 

El archivo `iniciar_broker.sh` inicia un contenedor con EMQX.
