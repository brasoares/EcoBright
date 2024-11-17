#include <WiFi.h>
#include <Adafruit_MQTT.h>
#include <Adafruit_MQTT_Client.h>

// Substitua pelas suas credenciais do Adafruit IO
#define IO_USERNAME  "brasoares"
#define IO_KEY       "aio_SPID08iIlSc5cXx0pf1q1Z5Er7D0"

// Configurações WiFi (se necessário)
#define WIFI_SSID "seu_ssid"
#define WIFI_PASS "sua_senha"

// Configurações MQTT
#define AIO_SERVER      "io.adafruit.com" 
#define AIO_SERVERPORT  1883                   // use 8883 for SSL
#define AIO_USERNAME    IO_USERNAME
#define AIO_KEY        IO_KEY

// Pinos
const int pirPin = 2;    // Pino do sensor PIR
const int ledPin = 13;   // Pino do LED

// Variáveis
unsigned long lastDebounceTime = 0;  // Último tempo de debounce
unsigned long debounceDelay = 50;    // Tempo de debounce (ms)
int pirState;             // Estado atual do sensor PIR
int lastPirState = LOW;   // Estado anterior do sensor PIR

// Cria o cliente WiFi
WiFiClient client;

// Cria o cliente MQTT
Adafruit_MQTT_Client mqtt(&client, AIO_SERVER, AIO_SERVERPORT, AIO_USERNAME, AIO_KEY);

// Cria o feed MQTT para publicar o estado do sensor
Adafruit_MQTT_Publish movimento = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/movimento"); 

void setup() {
  Serial.begin(9600);
  pinMode(pirPin, INPUT);
  pinMode(ledPin, OUTPUT);

  // Conecta ao WiFi (se necessário)
  Serial.print("Conectando ao WiFi: ");
  Serial.println(WIFI_SSID);
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("WiFi conectado");

  // Conecta ao Adafruit IO
  connectAdafruitIO();
}

void loop() {
  // Mantém a conexão com o Adafruit IO
  if(! mqtt.ping()) {
    if(! mqtt.connected()) {
      connectAdafruitIO();
    }
  }

  // Verifica o estado do sensor PIR com debounce
  int reading = digitalRead(pirPin);
  if (reading != lastPirState) {
    lastDebounceTime = millis();
  }

  if ((millis() - lastDebounceTime) > debounceDelay) {
    if (reading != pirState) {
      pirState = reading;
      if (pirState 1  == HIGH) {
        // Movimento detectado!
        Serial.println("Movimento detectado!");
        digitalWrite(ledPin, HIGH); // Acende o LED
        movimento.publish(1); // Publica "1" no feed "movimento"
      } else {
        // Sem movimento
        Serial.println("Sem movimento");
        digitalWrite(ledPin, LOW); // Apaga o LED
        movimento.publish(0); // Publica "0" no feed "movimento"
      }
    }
  }

  lastPirState = reading; 
}


// Função para conectar ao Adafruit IO
void connectAdafruitIO() {
  Serial.print("Conectando ao Adafruit IO... ");
  int8_t ret;

  while ((ret = mqtt.connect()) != 0) { 
    switch (ret) {
      case 1: Serial.println("Protocolo errado"); break;
      case 2: Serial.println("ID rejeitado"); break;
      case 3: Serial.println("Servidor indisponível"); break;
      case 4: Serial.println("Usuário ou senha inválidos"); break;
      case 5: Serial.println("Não autorizado"); break;
      case 6: Serial.println("Falha ao se inscrever"); break;
      default: Serial.println("Erro desconhecido"); break;
    }

    Serial.println("Tentando novamente em 5 segundos...");
    delay(5000);  
  }
  Serial.println("Adafruit IO Conectado!");
}
