/*
  MPU6050_WLED Switch test to check wiring hookup
*/

unsigned long switchLastdebounceTime = 0;
unsigned long debounceDelay = 50;  // switchTest = digitalRead(sw); delay in ms 
int sw = 0;  //Switch being tested
int swTest = 0;
int switchTest = HIGH;



void setup() {
  Serial.begin(9600);
  while(!Serial){};  
  delay(5000);
  Serial.println("\n\n\nTesting Switches...\n");

  pinMode(13, INPUT_PULLUP);
  pinMode(14, INPUT_PULLUP);
  pinMode(23, INPUT_PULLUP);
  pinMode(27, INPUT_PULLUP);
  pinMode(26, INPUT_PULLUP);
  
  switchesTest();
  
}

void loop(){}

void switchesTest(){
    
  if(swTest == 0){  

    //Switch 1 test
    Serial.println("Test Switch 1");
    Serial.println("Waiting...");
    sw = 13;
    //digitalWrite(sw, HIGH);
    switchTest = digitalRead(sw);
    while (switchTest == HIGH) {
      debounceSwitch(sw);
    }
    if(switchTest == LOW){
      Serial.println("Switch 1 Good");
      switchTest = HIGH;
    }

    //Switch 2 test
    Serial.println("Test Switch 2");
    Serial.println("Waiting...");
    sw = 14;
    digitalWrite(sw, HIGH);
    switchTest = digitalRead(sw);
    while (switchTest == HIGH) {
      debounceSwitch(sw);
    }
    if(switchTest == LOW){
      Serial.println("Switch 2 Good");
      switchTest = HIGH;
    }

    //Switch 3 test
    Serial.println("Test Switch 3");
    Serial.println("Waiting...");
    sw = 23;
    digitalWrite(sw, HIGH);
    switchTest = digitalRead(sw);
    while (switchTest == HIGH) {
      debounceSwitch(sw);
    }
    if(switchTest == LOW){
      Serial.println("Switch 3 Good");
      switchTest = HIGH;
    }
    //Switch 4 test
    Serial.println("Test Switch 4");
    Serial.println("Waiting...");
    sw = 26;
    digitalWrite(sw, HIGH);
    switchTest = digitalRead(sw);
    while (switchTest == HIGH) {
      debounceSwitch(sw);
    }
    if(switchTest == LOW){
      Serial.println("Switch 4 Good");
      switchTest = HIGH;
    }

    //Switch 5 test
    Serial.println("Test Switch 5");
    Serial.println("Waiting...");
    sw = 27;
    digitalWrite(sw, HIGH);
    switchTest = digitalRead(sw);
    while (switchTest == HIGH) {
      debounceSwitch(sw);
    }
    if(switchTest == LOW){
      Serial.println("Switch 5 Good\n\n\n");
      switchTest = HIGH;
    }
  }
  swTest = 1;
}

void debounceSwitch(int sw){  
  // Read/switchTest = digitalRead(sw); switch (sw) being tested
  if ((millis() - switchLastdebounceTime) > debounceDelay) {
    switchTest = digitalRead(sw);
    switchLastdebounceTime = millis();
  }
}
