#include <WiFi.h>
#include <PubSubClient.h>

// Configurações Wi-Fi
const char* ssid = "WIFI";     
const char* password = "@999"; 

// Configurações MQTT para Adafruit IO
const char* mqttServer = "io.adafruit.com";
const int mqttPort = 1883;
const char* mqttUser = "brasoares";   // Usuário Adafruit IO
const char* mqttPassword = "aio_SPID08iIlSc5cXx0pf1q1Z5Er7D0";  // Chave Adafruit IO
const char* mqttTopicMovimento = "brasoares/feeds/movimento";  // Feed para detecção de movimento
const char* mqttTopicTempo = "brasoares/feeds/tempo_resposta"; // Feed para o tempo de resposta

WiFiClient espClient;
PubSubClient client(espClient);

// Parâmetros do sensor PIR e LED
int pirPin = 2;    // Pino digital do sensor PIR
int ledPin = 13;   // Pino digital do LED
int calibrationTime = 30;
long unsigned int lowIn;
long unsigned int pause = 5000;

boolean lockLow = true;
boolean takeLowTime;

/////////////////////////////
// Conexão WiFi
void connectWiFi() {
  Serial.print("Conectando-se ao WiFi...");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println(" Conectado ao WiFi");
}

// Conexão MQTT
void reconnectMQTT() {
  while (!client.connected()) {
    Serial.print("Tentando conectar ao MQTT...");
    if (client.connect("ArduinoClient", mqttUser, mqttPassword)) {
      Serial.println("Conectado ao servidor MQTT");
    } else {
      Serial.print("Falha na conexão MQTT, status: ");
      Serial.println(client.state());
      delay(5000);
    }
  }
}

// Função de callback para o MQTT (vazia, pois não estamos recebendo dados do Adafruit IO)
void mqttCallback(char* topic, byte* payload, unsigned int length) {}

// Configuração inicial
void setup() {
  Serial.begin(9600);
  pinMode(pirPin, INPUT);
  pinMode(ledPin, OUTPUT);
  digitalWrite(pirPin, LOW);

  // Conectar ao Wi-Fi
  connectWiFi();

  // Configurar o servidor MQTT e o callback
  client.setServer(mqttServer, mqttPort);
  client.setCallback(mqttCallback);

  // Calibração do sensor PIR
  Serial.print("Calibrando o sensor...");
  for (int i = 0; i < calibrationTime; i++) {
    Serial.print(".");
    delay(1000);
  }
  Serial.println(" Pronto!");
  Serial.println("Sensor ativo.");
  delay(50);
}

////////////////////////////
// Loop principal
void loop() {
  // Conectar ao MQTT
  if (!client.connected()) {
    reconnectMQTT();
  }
  client.loop();

  if (digitalRead(pirPin) == HIGH) {
    digitalWrite(ledPin, HIGH);   // Liga o LED com movimento detectado
    if (lockLow) {
      lockLow = false;            
      Serial.println("---");
      Serial.print("Movimento detectado aos ");
      Serial.print(millis() / 1000);
      Serial.println(" segundos");

      // Publicar "Movimento detectado" no feed do Adafruit IO
      client.publish(mqttTopicMovimento, "Movimento detectado");
      delay(50);         
    }
    takeLowTime = true;
  }

  if (digitalRead(pirPin) == LOW) {       
    digitalWrite(ledPin, LOW);  // Desliga o LED quando não há movimento
    if (takeLowTime) {
      lowIn = millis();
      takeLowTime = false;
    }

    // Se o sensor ficar em LOW por mais que o 'pause', encerra o movimento
    if (!lockLow && millis() - lowIn > pause) {  
      lockLow = true;
      Serial.print("Movimento terminou aos ");
      Serial.print((millis() - pause) / 1000);
      Serial.println(" segundos");

      // Publicar "Movimento terminado" no feed do Adafruit IO
      client.publish(mqttTopicMovimento, "Movimento terminado");
      delay(50);
    }
  }
}