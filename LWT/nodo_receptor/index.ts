import * as mqtt from 'mqtt';

const cliente = mqtt.connect('mqtt://localhost:1883');

// se ejecuta al conectarse al broker
cliente.on('connect', () => {
    console.log('Conectado al broker MQTT');
    // se suscribe al tópico 'medicion'
    cliente.subscribe('medicion');
    cliente.subscribe('estado');
});

// muestra el mensaje que recibe en la consola
cliente.on('message', (topico, mensaje) => {
    console.log(`Nuevo mensaje recibido: ${mensaje.toString()}`);
});
