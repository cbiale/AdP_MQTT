import * as mqtt from 'mqtt';

const cliente = mqtt.connect('mqtt://localhost:1883');

// se ejecuta al conectarse al broker
cliente.on('connect', () => {
    console.log('Conectado al broker MQTT');
    // se suscribe al tópico 'medicion' de forma compartida
    // el formato es: $share/{nombre compartido}/{filtro de tema o tópico}
    // se debe considerar que el agrupamiento se da por:
    //- el nombre compartido y
    //- el filtro de tópico.
    cliente.subscribe('$share/grupo/medicion');
});

// muestra el mensaje que recibe en la consola
cliente.on('message', (topico, mensaje) => {
    console.log(`Nuevo mensaje recibido: ${mensaje.toString()}`);
});