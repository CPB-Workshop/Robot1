#define LDRAG A0  // composante photoresistor sur la pin A0
#define LDRAD A1  // composante photoresistor sur la pin A1
#define LDRAR A2  // composante photoresistor sur la pin A2

unsigned int value;
 
void setup() {
   // initialise la communication avec le PC
   Serial.begin(9600);

   // initialise les broches
   pinMode(LDRAG, INPUT);
   pinMode(LDRAD, INPUT);
   pinMode(LDRAR, INPUT);
}
 
void loop() {
   // mesure la tension sur la broche A0
   value = analogRead(LDRAG);
   Serial.println(value);
   // mesure la tension sur la broche A1
   value = analogRead(LDRAD);
   Serial.println(value);
   // mesure la tension sur la broche A2
   value = analogRead(LDRAR);
   Serial.println(value);

   delay(200);
}
