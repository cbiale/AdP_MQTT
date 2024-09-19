import express, { Express, Request, Response } from 'express';
import * as mqtt from 'mqtt';

// Tipos de datos
interface Medicion {
    fecha: Date;
    valor: number;
}

interface Dispositivo {
    id : number;
    mediciones: Medicion[];
}

// arreglo de dispositivos
let  dispositivos : Dispositivo[] = [];

// defino express y puerto
const app: Express = express();
const puerto : number = 3000;

// conexión a broker MQTT
const cliente = mqtt.connect('mqtt://localhost:1883');

// defino rutas
app.get('/', (req: Request, res: Response) => {
    res.send('Servidor');
});

app.get('/mediciones/:id', (req: Request, res: Response) => {
    // recupero id, pasando a integer
    const idBuscado : number = parseInt(req.params.id);
    // busco el dispositivo por id (idBuscado)
    const dispositivoEncontrado = dispositivos.find(buscado => buscado.id === idBuscado);
    // si se encuentra devuelvo mediciones del dispositivo
    if (dispositivoEncontrado) {
        res.send(dispositivoEncontrado.mediciones);
    } else {
        // sino devuelvo error
        res.status(404).json({ message: `Dispositivo con id ${idBuscado} no encontrado.`});
    }
});

app.get('/mediciones_actuales/:id', (req: Request, res: Response) => {
    // recupero id, pasando a integer
    const idBuscado : number = parseInt(req.params.id);
    // busco el dispositivo por id (idBuscado)
    const dispositivoEncontrado = dispositivos.find(buscado => buscado.id === idBuscado);
    // si se encuentra devuelvo estado actual del dispositivo
    if (dispositivoEncontrado) {
        res.send(dispositivoEncontrado.mediciones[dispositivoEncontrado.mediciones.length-1]);
    } else {
        // sino devuelvo error
        res.status(404).json({ message: `Dispositivo con id ${idBuscado} no encontrado.`});
    }
});

// al conectarse al broker
cliente.on('connect', () => {
    console.log('[Servidor] Conectado al broker MQTT');
    cliente.subscribe('medicion/#', console.log); // client.subscribe('topic', {qos: valor})
});

// al recibir un mensase
cliente.on('message', (topico : string, mensaje : Buffer) => {
    // controlo tópico
    if (topico.indexOf('medicion') !== -1) {
        // imprimo mensaje recibido
        console.log(`[Servidor] Mensaje recibido: ${mensaje}. Topico: ${topico}`);

        // paso a formato JSON
        // devuelve un tipo any, se pueden usar bibliotecas como zod para validaciones
        const datos = JSON.parse(mensaje.toString());

        // genero una constante de tipo JSON
        const medicion : Medicion = { 
            fecha : new Date(), // fecha y hora actual
            valor : datos.valor, // campo valor del mensaje en formato JSON
        };

        // recupero id, pasando a integer (el id viene en el topico)
        const idBuscado : number = parseInt(topico.substring(9));
        // busco el dispositivo por id (idBuscado)
        let dispositivoEncontrado = dispositivos.find(buscado => buscado.id === idBuscado);
        // si se encuentra agrego medicion al dispositivo
        if (dispositivoEncontrado) {
            dispositivoEncontrado.mediciones.push(medicion);
        } else {
            // creo dispositivo
            dispositivoEncontrado = {
                id : idBuscado,
                mediciones : []
            };
            // agrego al arreglo de dispositivos
            dispositivos.push(dispositivoEncontrado);
            // agrego medicion
            dispositivoEncontrado.mediciones.push(medicion);
        }
        console.log(`[Servidor] Nueva medición agregada: ${idBuscado} - ${medicion.fecha} - ${medicion.valor}`);
    }
});

// escucho por peticiones
app.listen(puerto, () => {
    console.log(`[Servidor] Servidor ejecuntandose en el puerto: ${puerto}`);
});