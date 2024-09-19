import * as mqtt from 'mqtt';

const cliente = mqtt.connect('mqtt://localhost:1883');

cliente.on('connect', () => {
    // Imprimir en la consola el PID del proceso
    console.log(`PID: ${process.pid}`);
    console.log('Conectado al broker MQTT');

    // cada 10 segundos
    setInterval(() => {
        // valor aleatorio a enviar
        const valor : number = Math.floor(Math.random() * 40);
        // mensaje a enviar
        const mensaje : string = JSON.stringify({ valor });
        // publico mensaje en el topico medicion y lo retengo en el broker
        cliente.publish('medicion', mensaje);
        console.log(`Nuevo mensaje publicado: ${mensaje}`);
    }, 10000);
});
