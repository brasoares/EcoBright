#include <WiFi.h>
#include <PubSubClient.h>

// Configurações Wi-Fi
const char* ssid = "SUA_REDE_WIFI";     // Substitua pelo nome da sua rede Wi-Fi
const char* password = "SUA_SENHA_WIFI"; // Substitua pela senha da sua rede Wi-Fi

// Configurações MQTT Adafruit IO
const char* mqttServer = "io.adafruit.com";  // Servidor MQTT do Adafruit IO
const int mqttPort = 1883;                  // Porta do servidor MQTT
const char* mqttUser = "SEU_USUARIO";      // Substitua pelo seu usuário Adafruit IO
const char* mqttPassword = "SUA_CHAVE_AIO"; // Substitua pela sua chave do Adafruit IO
const char* mqttTopicMovimento = "brasoares/feeds/deteccao_movimento";  // Feed para detecção de movimento
const char* mqttTopicTempo = "brasoares/feeds/tempo_resposta"; // Feed para o tempo de resposta

WiFiClient espClient;
PubSubClient client(espClient);

// Pinos do sensor PIR e LED
const int pirPin = 2;    // Pino digital conectado ao sensor PIR
const int ledPin = 13;   // Pino digital conectado ao LED

// Configurações do sensor PIR
const int calibrationTime = 30;    // Tempo de calibração (em segundos)
unsigned long lowIn;                // Tempo quando o sensor volta ao estado LOW
const unsigned long pause = 5000;  // Tempo para considerar o movimento terminado (em ms)
boolean lockLow = true;             // Impede a repetição de eventos enquanto o movimento é detectado
boolean takeLowTime;               // Marca o momento que o sensor detecta LOW

void setup() {
  Serial.begin(9600);       // Inicializa a comunicação serial
  pinMode(pirPin, INPUT);    // Configura o pino do PIR como entrada
  pinMode(ledPin, OUTPUT);   // Configura o pino do LED como saída
  digitalWrite(pirPin, LOW); // Garante que o pino do PIR está em LOW inicialmente

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
    unsigned long tempoInicial = millis(); // Registra o tempo inicial

    digitalWrite(ledPin, HIGH);  // Acende o LED quando o movimento é detectado

    if (lockLow) {
      lockLow = false;  // Impede novos eventos até o sensor voltar a LOW
      Serial.println("Movimento detectado");
      client.publish(mqttTopicMovimento, "Movimento detectado");
      delay(50);  // Delay para evitar múltiplas publicações rápidas
    }
    takeLowTime = true;  // Marca que foi detectado um movimento

    // Aguarda a confirmação de recebimento do Adafruit IO (implementar lógica)
    // ... (lógica para receber a confirmação do Adafruit IO)

    unsigned long tempoFinal = millis(); // Registra o tempo final
    unsigned long tempoResposta = tempoFinal - tempoInicial;

    // Publica o tempo de resposta no feed
    char tempoRespostaStr[10];
    dtostrf(tempoResposta, 6, 2, tempoRespostaStr);
    client.publish(mqttTopicTempo, tempoRespostaStr);

    Serial.print("Tempo de resposta do sensor e MQTT: ");
    Serial.print(tempoResposta);
    Serial.println(" ms");
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
      client.publish(mqttTopicMovimento, "Movimento terminado");
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
      client.subscribe(mqttTopicMovimento);
      // Se precisar receber mensagens do Adafruit IO, inscreva-se em um tópico
      // client.subscribe("brasoares/feeds/comando"); 
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

  // Implementar lógica para identificar a confirmação de recebimento do Adafruit IO
  // ... (lógica para identificar a confirmação, por exemplo, comparando o tópico e a mensagem)
}