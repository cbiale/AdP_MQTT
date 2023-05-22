"use strict";
var __createBinding = (this && this.__createBinding) || (Object.create ? (function(o, m, k, k2) {
    if (k2 === undefined) k2 = k;
    var desc = Object.getOwnPropertyDescriptor(m, k);
    if (!desc || ("get" in desc ? !m.__esModule : desc.writable || desc.configurable)) {
      desc = { enumerable: true, get: function() { return m[k]; } };
    }
    Object.defineProperty(o, k2, desc);
}) : (function(o, m, k, k2) {
    if (k2 === undefined) k2 = k;
    o[k2] = m[k];
}));
var __setModuleDefault = (this && this.__setModuleDefault) || (Object.create ? (function(o, v) {
    Object.defineProperty(o, "default", { enumerable: true, value: v });
}) : function(o, v) {
    o["default"] = v;
});
var __importStar = (this && this.__importStar) || function (mod) {
    if (mod && mod.__esModule) return mod;
    var result = {};
    if (mod != null) for (var k in mod) if (k !== "default" && Object.prototype.hasOwnProperty.call(mod, k)) __createBinding(result, mod, k);
    __setModuleDefault(result, mod);
    return result;
};
var __importDefault = (this && this.__importDefault) || function (mod) {
    return (mod && mod.__esModule) ? mod : { "default": mod };
};
Object.defineProperty(exports, "__esModule", { value: true });
const express_1 = __importDefault(require("express"));
const mqtt = __importStar(require("mqtt"));
// arreglo de dispositivos
let dispositivos = [];
// defino express y puerto
const app = (0, express_1.default)();
const puerto = 3000;
// conexion a broker MQTT
const cliente = mqtt.connect('mqtt://localhost:1883');
// defino rutas
app.get('/', (req, res) => {
    res.send('Servidor');
});
app.get('/mediciones/:id', (req, res) => {
    // recupero id, pasando a integer
    const idBuscado = parseInt(req.params.id);
    // busco el dispositivo por id (idBuscado)
    const dispositivoEncontrado = dispositivos.find(buscado => buscado.id === idBuscado);
    // si se encuentra devuelvo mediciones del dispositivo
    if (dispositivoEncontrado) {
        res.send(dispositivoEncontrado.mediciones);
    }
    else {
        // sino devuelvo error
        res.status(404).json({ message: `Dispositivo con id ${idBuscado} no encontrado.` });
    }
});
app.get('/mediciones_actuales/:id', (req, res) => {
    // recupero id, pasando a integer
    const idBuscado = parseInt(req.params.id);
    // busco el dispositivo por id (idBuscado)
    const dispositivoEncontrado = dispositivos.find(buscado => buscado.id === idBuscado);
    // si se encuentra devuelvo estado actual del dispositivo
    if (dispositivoEncontrado) {
        res.send(dispositivoEncontrado.mediciones[dispositivoEncontrado.mediciones.length - 1]);
    }
    else {
        // sino devuelvo error
        res.status(404).json({ message: `Dispositivo con id ${idBuscado} no encontrado.` });
    }
});
// al conectarse al broker
cliente.on('connect', () => {
    console.log('[Servidor] Conectado al broker MQTT');
    cliente.subscribe('medicion/#', console.log); // client.subscribe('topic', {qos: valor})
});
// al recibir un mensase
cliente.on('message', (topico, mensaje) => {
    // controlo topico
    if (topico.indexOf('medicion') !== -1) {
        // imprimo mensaje recibido
        console.log(`[Servidor] Mensaje recibido: ${mensaje}. Topico: ${topico}`);
        // paso a formato JSON
        // devuelve un tipo any, se pueden usar bibliotecas como zod para validaciones
        const datos = JSON.parse(mensaje.toString());
        // genero una constante de tipo JSON
        const medicion = {
            fecha: new Date(),
            valor: datos.valor, // campo valor del mensaje en formato JSON
        };
        // recupero id, pasando a integer (el id viene en el topico)
        const idBuscado = parseInt(topico.substring(9));
        // busco el dispositivo por id (idBuscado)
        let dispositivoEncontrado = dispositivos.find(buscado => buscado.id === idBuscado);
        // si se encuentra agrego medicion al dispositivo
        if (dispositivoEncontrado) {
            dispositivoEncontrado.mediciones.push(medicion);
        }
        else {
            // creo dispositivo
            dispositivoEncontrado = {
                id: idBuscado,
                mediciones: []
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
