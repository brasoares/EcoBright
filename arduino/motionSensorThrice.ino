int calibrationTime = 30;     
long unsigned int lowIn;     
long unsigned int pause = 5000;  
boolean lockLow = true;   
boolean takeLowTime;   

int pirPin = 2;    
int ledPin = 13;

void setup(){
  Serial.begin(9600);
  pinMode(pirPin, INPUT);
  pinMode(ledPin, OUTPUT);
  digitalWrite(pirPin, LOW);

  // Calibrando o sensor
  Serial.print("calibrando sensor ");
  for(int i = 0; i < calibrationTime; i++){
    Serial.print(".");
    delay(1000);
  }
  Serial.println(" pronto");
  Serial.println("SENSOR ATIVO");
  delay(50);
}

void loop(){
  if(digitalRead(pirPin) == HIGH){
    // Piscar 3 vezes rápidas
    for(int i = 0; i < 3; i++){
      digitalWrite(ledPin, HIGH);
      delay(100);
      digitalWrite(ledPin, LOW);
      delay(100);
    }

    if(lockLow){
      lockLow = false;
      Serial.println("---");
      Serial.print("movimento detectado em ");
      Serial.print(millis()/1000);
      Serial.println(" seg");
      delay(50);
    }
    takeLowTime = true;
  }

  if(digitalRead(pirPin) == LOW){    
    digitalWrite(ledPin, LOW);
    if(takeLowTime){
      lowIn = millis();
      takeLowTime = false;
    }
    if(!lockLow && millis() - lowIn > pause){
      lockLow = true;
      Serial.print("movimento terminou em ");
      Serial.print((millis() - pause)/1000);
      Serial.println(" seg");
      delay(50);
    }
  }
}
