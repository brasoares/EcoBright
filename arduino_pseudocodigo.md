# Pseudocódigo - Controle de Movimento com MQTT

Início

Definir ssid como "WIFI"                // Nome da rede Wi-Fi  
Definir password como "@999"             // Senha da rede Wi-Fi  

Definir mqttServer como "io.adafruit.com"  
Definir mqttPort como 1883  
Definir mqttUser como "brasoares"       // Usuário Adafruit IO  
Definir mqttPassword como "aio_SPID08iIlSc5cXx0pf1q1Z5Er7D0"  // Chave Adafruit IO  
Definir mqttTopicMovimento como "brasoares/feeds/movimento"  
Definir mqttTopicTempo como "brasoares/feeds/tempo_resposta"  

Criar objeto espClient  
Criar objeto client do tipo PubSubClient com espClient  

Definir pirPin como 2    // Pino do sensor PIR  
Definir ledPin como 13   // Pino do LED  
Definir calibrationTime como 30  // Tempo de calibração do sensor  
Definir lowIn como 0    // Armazenar o momento de baixa  
Definir pause como 5000 // Pausa após detecção de movimento  
Definir lockLow como verdadeiro  
Definir takeLowTime como falso  

## Função connectWiFi:
Imprimir "Conectando-se ao WiFi..."  
Iniciar conexão Wi-Fi com ssid e password  
Enquanto WiFi.status não for WL_CONNECTED:  
    Aguardar 1 segundo  
    Imprimir "."  
Imprimir " Conectado ao WiFi"  

## Função reconnectMQTT:
Enquanto client não estiver conectado:  
    Imprimir "Tentando conectar ao MQTT..."  
    Se client.connect("ArduinoClient", mqttUser, mqttPassword):  
        Imprimir "Conectado ao servidor MQTT"  
    Caso contrário:  
        Imprimir erro da conexão MQTT  
        Aguardar 5 segundos  

## Função mqttCallback (não faz nada no momento)

## Função setup:
Iniciar comunicação serial a 9600  
Definir pirPin como entrada  
Definir ledPin como saída  
Definir pirPin para LOW (inicializar como desligado)

Chamar connectWiFi para conectar ao Wi-Fi  

Configurar client para o servidor MQTT  
Definir callback como mqttCallback  

Imprimir "Calibrando o sensor..."  
Para i de 0 até calibrationTime:  
    Imprimir "."  
    Aguardar 1 segundo  
Imprimir "Pronto! Sensor ativo."  
Aguardar 50 milissegundos  

## Função loop:
Se client não estiver conectado:  
    Chamar reconnectMQTT para reconectar ao MQTT  
Chamar client.loop para manter a comunicação com o servidor MQTT  

Se o sensor PIR detectar movimento (pirPin HIGH):  
    Acender o LED (ledPin HIGH)  
    Se lockLow for verdadeiro:  
        Definir lockLow como falso  
        Imprimir o tempo de detecção de movimento  
        Publicar "Movimento detectado" no feed MQTT  
        Aguardar 50 milissegundos  

    Definir takeLowTime como verdadeiro  

Se o sensor PIR não detectar movimento (pirPin LOW):  
    Apagar o LED (ledPin LOW)  
    Se takeLowTime for verdadeiro:  
        Armazenar o tempo atual em lowIn  
        Definir takeLowTime como falso  

    Se o tempo sem movimento for maior que o valor de pause:  
        Definir lockLow como verdadeiro  
        Imprimir o tempo de término do movimento  
        Publicar "Movimento terminado" no feed MQTT  
        Aguardar 50 milissegundos  

Fim