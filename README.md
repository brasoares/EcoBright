# EcoBright 💡

## Overview

Projeto de IoT - Objetos Inteligentes Conectados 2024.2 | Universidade Presbiteriana Mackenzie
**Grupo:**
* Ana Laura Paiva de Mello
* Henoc Soares Freire
* Mariane Salgado Fritegotto

**Docentes:**
* André Luís de Oliveira
* Leandro Carlos Fernandes

## Descrição do Projeto

O projeto EcoBright utiliza um sensor PIR (Passive Infrared Sensor) para detectar a presença de movimento. Com essa informação, o sistema controla uma lâmpada LED, que acende apenas quando há movimento no ambiente, promovendo a eficiência energética. ♻️

## Componentes de Hardware

* Arduino UNO R4 WiFi 💻
* Sensor PIR: detecta movimento no ambiente. 🚶
* Lâmpada LED: acende em resposta ao movimento detectado. 💡

## Funcionamento

O sensor PIR detecta movimento e envia um sinal ao Arduino. O Arduino controla a lâmpada LED:

* Se houver movimento, a LED acende. ✅
* Caso contrário, a LED permanece apagada. 

## Conexões e Configurações

**PIR Sensor:**

* VCC -> 5V no Arduino
* OUT -> Pino 2 no Arduino
* GND -> GND no Arduino

**LED:**

* Anodo -> Pino 13 no Arduino
* Catodo -> GND no Arduino

## Comunicação MQTT

Este projeto utiliza o protocolo MQTT para enviar dados de movimento para a plataforma Adafruit IO, permitindo monitoramento remoto em tempo real. 📡

## Bibliotecas Necessárias

* PubSubClient: para integração MQTT no Arduino com Adafruit IO. 📚

## Setup Instructions

1. Clone o repositório:

   ```bash
   git clone [https://github.com/brasoares/EcoBright](https://github.com/brasoares/EcoBright)
   cd EcoBright

2. Use o código com as ferramentas da sua preferência. 😉