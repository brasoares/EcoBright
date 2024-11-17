#include <WiFi.h>
#include <PubSubClient.h>

// Configurações Wi-Fi
const char* ssid = "WIFI";            // Substitua pelo nome da sua rede Wi-Fi
const char* password = "SENHA";    // Substitua pela senha da sua rede Wi-Fi

// Configurações MQTT Adafruit IO
const char* mqttServer = "io.adafruit.com";  // Servidor MQTT do Adafruit IO
const int mqttPort = 1883;                   // Porta do servidor MQTT
const char* mqttUser = "brasoares";      // Substitua pelo seu usuário Adafruit IO
const char* mqttPassword = "aio_SPID08iIlSc5cXx0pf1q1Z5Er7D0";  // Substitua pela sua chave do Adafruit IO
const char* mqttTopic = "brasoares/feeds/motion_detectionnn";  // Substitua pelo seu feed no Adafruit IO

WiFiClient espClient;
PubSubClient client(espClient);

// Pinos do sensor PIR e LED
int pirPin = 2;       // Pino digital conectado ao sensor PIR
int ledPin = 13;      // Pino digital conectado ao LED

// Configurações do sensor PIR
int calibrationTime = 30;      // Tempo de calibração (em segundos)
long unsigned int lowIn;      // Tempo quando o sensor volta ao estado LOW
long unsigned int pause = 5000; // Tempo para considerar o movimento terminado (em ms)
boolean lockLow = true;        // Impede a repetição de eventos enquanto o movimento é detectado
boolean takeLowTime;           // Marca o momento que o sensor detecta LOW

void setup() {
  Serial.begin(9600);          // Inicializa a comunicação serial
  pinMode(pirPin, INPUT);      // Configura o pino do PIR como entrada
  pinMode(ledPin, OUTPUT);     // Configura o pino do LED como saída
  digitalWrite(pirPin, LOW);  // Garante que o pino do PIR está em LOW inicialmente

  // Conectar ao Wi-Fi
  connectWiFi();

  // Configura o servidor MQTT
  client.setServer(mqttServer, mqttPort);
  client.setCallback(mqttCallback);

  // Calibração do sensor PIR
  Serial.print("Calibrando o sensor...");
  for (int i = 0; i < calibrationTime; i++) {
    Serial.print(".");
    delay(1000);
  }
  Serial.println(" pronto");
}

void loop() {
  if (!client.connected()) {
    reconnectMQTT();
  }
  client.loop();

  if (digitalRead(pirPin) == HIGH) {
    digitalWrite(ledPin, HIGH);  // Acende o LED quando o movimento é detectado

    if (lockLow) {
      lockLow = false;  // Impede novos eventos até o sensor voltar a LOW
      Serial.println("Movimento detectado");
      client.publish(mqttTopic, "Movimento detectado");
      delay(50);  // Delay para evitar múltiplas publicações rápidas
    }
    takeLowTime = true;  // Marca que foi detectado um movimento
  }

  if (digitalRead(pirPin) == LOW) {
    digitalWrite(ledPin, LOW);  // Apaga o LED quando não há movimento

    if (takeLowTime) {
      lowIn = millis();  // Armazena o tempo de transição para LOW
      takeLowTime = false;
    }

    if (!lockLow && millis() - lowIn > pause) {
      lockLow = true;
      Serial.println("Movimento terminou");
      client.publish(mqttTopic, "Movimento terminado");
      delay(50);  // Delay para evitar múltiplas publicações rápidas
    }
  }
}

void connectWiFi() {
  Serial.print("Conectando-se ao WiFi...");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println(" Conectado ao WiFi");
}

void reconnectMQTT() {
  while (!client.connected()) {
    Serial.print("Tentando conectar ao MQTT...");
    if (client.connect("ArduinoClient", mqttUser, mqttPassword)) {
      Serial.println("Conectado ao servidor MQTT");
      client.subscribe(mqttTopic);
    } else {
      Serial.print("Falha na conexão com o MQTT, status: ");
      Serial.println(client.state());
      delay(5000);
    }
  }
}

void mqttCallback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Mensagem recebida no tópico: ");
  Serial.print(topic);
  Serial.print(" | Mensagem: ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
}