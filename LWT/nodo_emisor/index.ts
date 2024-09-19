import * as mqtt from 'mqtt';

const cliente = mqtt.connect('mqtt://localhost:1883', {
  will: {
    topic: 'estado',
    payload: 'offline',
    qos: 0,
    retain: false
  }
});

cliente.on('connect', () => {
    // Imprimir en la consola el PID del proceso
    console.log(`PID: ${process.pid}`);
    console.log('Conectado al broker MQTT');
    cliente.publish(`estado`, 'online');    

    // cada 10 segundos
    setInterval(() => {
        // valor aleatorio a enviar
        const valor : number = Math.floor(Math.random() * 40);
        // mensaje a enviar
        const mensaje : string = JSON.stringify({ valor });
        // publico mensaje
        cliente.publish(`medicion`, mensaje);
        console.log(`Nuevo mensaje publicado: ${mensaje}`);
    }, 10000);
});

// como hacer para que el nodo receptor detecte el estado del nodo emisor
// al conectarse al broker. 