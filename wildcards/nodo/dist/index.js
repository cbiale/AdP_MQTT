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
Object.defineProperty(exports, "__esModule", { value: true });
const mqtt = __importStar(require("mqtt"));
const cliente = mqtt.connect('mqtt://localhost:1883');
cliente.on('connect', () => {
    console.log('Conectado al broker MQTT');
    // cada 10 segundos
    setInterval(() => {
        // se simula el envio de un topico con id aleatorio
        const id = Math.floor(Math.random() * 3);
        // valor aleatorio a enviar
        const valor = Math.floor(Math.random() * 40);
        // mensaje a enviar
        const mensaje = JSON.stringify({ valor });
        // publico mensaje
        cliente.publish(`medicion/${id}`, mensaje);
        console.log(`Nuevo mensaje publicado por el id ${id}: ${mensaje}`);
    }, 10000);
});
