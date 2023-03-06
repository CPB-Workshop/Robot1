#include <Servo.h>
#include <ezButton.h> //installer la librairie ezButton

//CABLAGE MOTEUR
#define borneENA        6       // On associe la borne "ENA" du L298N à la pin D8 de l'arduino
#define borneIN1        7       // On associe la borne "IN1" du L298N à la pin D7 de l'arduino
#define borneIN2        8       // On associe la borne "IN2" du L298N à la pin D6 de l'arduino
#define borneIN3        5       // On associe la borne "IN3" du L298N à la pin D5 de l'arduino
#define borneIN4        4       // On associe la borne "IN4" du L298N à la pin D4 de l'arduino
#define borneENB        3       // On associe la borne "ENB" du L298N à la pin D3 de l'arduino

//CABLAGE SONAR
#define TRIGGER_PIN 11 // Broche TRIGGER
#define ECHO_PIN 12    // Broche ECHO

// SERVO
#define servoPin 10

// PARECHOC
#define SWITCHG 9 // Broche Gauche (a échanger en dessous si pas bon)
#define SWITCHD 2 // Broche Droite (a échanger au dessus si pas bon)


//REGLAGES
#define vitesseMinA 170 //à tester, a augmenter 1 par 1 si le moteur tourne pas a la même vitesse que l'autre quand ca doit aller droit (diff physique)
#define vitesseMinB 140 //à tester, a augmenter 1 par 1 si le moteur tourne pas a la même vitesse que l'autre quand ca doit aller droit (diff physique)
#define pinceFerme 140 //à tester, a augmenter 1 par 1 si la pince ferme pas totalement (diff physique)
#define pinceOuverte 0 //normalement est calibré au montage (diff physique)

//CONSTANTES
/* Constantes pour le timeout */
const unsigned long MEASURE_TIMEOUT = 25000UL; // 25ms = ~8m à 340m/s
/* Vitesse du son dans l'air en mm/us */
const float SOUND_SPEED = 340.0 / 1000;

//STOCKAGE
String msg;
// Creation de l'objet servo
Servo myservo;
//Creation des objets parechoc
ezButton limitSwitchG(SWITCHG);  // create ezButton object;
ezButton limitSwitchD(SWITCHD);  // create ezButton object;

void setup() {
  
  // Configuration de toutes les pins de l'Arduino en "sortie" (car elles attaquent les entrées du module L298N)
  pinMode(borneENA, OUTPUT);
  pinMode(borneIN1, OUTPUT);
  pinMode(borneIN2, OUTPUT);
  pinMode(borneIN3, OUTPUT);
  pinMode(borneIN4, OUTPUT);
  pinMode(borneENB, OUTPUT);

  //SONAR
  pinMode(TRIGGER_PIN, OUTPUT);
  digitalWrite(TRIGGER_PIN, LOW); // La broche TRIGGER doit être à LOW au repos
  pinMode(ECHO_PIN, INPUT);

  //SERVO
  // appel de la fonction attach de l'objet
  myservo.attach(servoPin);
  //par defaut on ouvre
  myservo.write(pinceOuverte);

  //PARECHOC
  limitSwitchG.setDebounceTime(50); // set debounce time to 50 milliseconds
  limitSwitchD.setDebounceTime(50); // set debounce time to 50 milliseconds
  
  //USB et Bluetooth
  Serial.begin(9600);
}


void loop() {
 mesureSonar();
 readSerialPort();
 checkParechoc();
 if(msg!=""){
   if(msg=="0"){
      arreterRotationMoteurPontB();
      arreterRotationMoteurPontA();
   }
   else if(msg=="1"){
      lancerRotationMoteurPontA(true, vitesseMinA);
      lancerRotationMoteurPontB(true, vitesseMinB);
   }
   else if(msg=="3"){
      lancerRotationMoteurPontA(false, vitesseMinA);
      lancerRotationMoteurPontB(false, vitesseMinB);
   }
   else if(msg=="4"){
      tournerGauche();
   }
   else if(msg=="2"){
     tournerDroite();
   }
   else if(msg=="open"){
    actionPince(pinceOuverte);
   }
   else if(msg=="close"){
    actionPince(pinceFerme);
   }
 }
}

void tournerGauche(){
  lancerRotationMoteurPontA(false, vitesseMinA);
  lancerRotationMoteurPontB(true, vitesseMinB);
}

void tournerDroite(){
  lancerRotationMoteurPontA(true, vitesseMinA);
  lancerRotationMoteurPontB(false, vitesseMinB);
}

void mesureSonar(){
 /* 1. Lance une mesure de distance en envoyant une impulsion HIGH de 10µs sur la broche TRIGGER */
  digitalWrite(TRIGGER_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIGGER_PIN, LOW);
  
  /* 2. Mesure le temps entre l'envoi de l'impulsion ultrasonique et son écho (si il existe) */
  long measure = pulseIn(ECHO_PIN, HIGH, MEASURE_TIMEOUT);
   
  /* 3. Calcul la distance à partir du temps mesuré */
  float distance_mm = measure / 2.0 * SOUND_SPEED;

  //Envois via le serial sur USB ou Bluetooth
  Serial.print("*S");
  Serial.println(distance_mm);
}

void lancerRotationMoteurPontA(bool marcheAvant, char vitesse) {
  digitalWrite(borneIN2, marcheAvant ? LOW : HIGH);                
  digitalWrite(borneIN1, marcheAvant ? HIGH : LOW);
  
  analogWrite(borneENA, vitesse);       // Active l'alimentation du moteur 1
}

void arreterRotationMoteurPontA(){
  analogWrite(borneENA, 0);        // Désactive l'alimentation du moteur 1
}

void lancerRotationMoteurPontB(bool marcheAvant, char vitesse) {
  digitalWrite(borneIN3, marcheAvant ? LOW : HIGH);                
  digitalWrite(borneIN4, marcheAvant ? HIGH : LOW);
  
  analogWrite(borneENB, vitesse);       // Active l'alimentation du moteur 2
}

void arreterRotationMoteurPontB(){
  analogWrite(borneENB, 0);        // Désactive l'alimentation du moteur 2
}

void readSerialPort(){
  msg="";
  while (Serial.available()) {
    delay(10);  
    if (Serial.available() >0) {
      char c = Serial.read();  //gets one byte from serial buffer
      msg += c; //makes the string readString
    }
  } 
}

void actionPince(int angle){
   myservo.write(angle);
}

void checkParechoc(){
  limitSwitchG.loop(); // MUST call the loop() function first
  limitSwitchD.loop(); // MUST call the loop() function first
  int state = limitSwitchG.getState();
  if(state == LOW)
    Serial.println("*G100");
  state = limitSwitchD.getState();
  if(state == LOW)
    Serial.println("*D100");
}
