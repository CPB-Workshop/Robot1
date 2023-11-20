#include <Servo.h> //Inclu avec arduino
#include "ezButton.h"
#include "RemoteXY/RemoteXY.h"

//REGLAGES
#define vitesseMinA 200 //à tester, a augmenter 1 par 1 si le moteur tourne pas a la même vitesse que l'autre quand ca doit aller droit (diff physique)
#define vitesseMinB 170 //à tester, a augmenter 1 par 1 si le moteur tourne pas a la même vitesse que l'autre quand ca doit aller droit (diff physique)
#define pinceFerme 0 //normalement est calibré au montage (diff physique) peux pas être négatif
#define pinceOuverte 140 //à tester, a augmenter 1 par 1 si la pince s'ouvre pas totalement (diff physique)

////////////////////////////////////////////////////////////////////////////////////////
//ACTIVATION CONTROLLE BLUETOOTH SUR LA SERIAL HARD (0-1)
#define REMOTEXY_SERIAL_SPEED 9600
//RemoteXY configurate  
#pragma pack(push, 1)
uint8_t RemoteXY_CONF[] =   // 126 bytes
  { 255,4,0,16,0,148,0,16,27,2,5,33,66,30,30,30,29,63,30,30,
  2,26,31,2,0,36,39,22,11,31,7,22,11,2,26,31,31,65,117,116,
  111,0,77,97,110,117,101,108,0,68,51,9,4,58,26,5,26,53,31,8,
  36,135,94,71,97,117,99,104,101,0,68,114,111,105,116,101,0,67,101,110,
  116,114,101,0,1,0,10,39,18,18,4,68,21,21,2,31,80,105,110,99,
  101,0,72,100,70,4,27,27,4,4,24,24,2,26,121,37,0,0,0,0,
  0,0,250,68,0,0,0,0,1,0,0,0,0,0,0,200,66,4,0,0,
  200,66,0,0,122,68,95,0,0,122,68,0,0,250,68 };
// this structure defines all the variables and events of your control interface 
struct {
  int8_t joystick_1_x; // from -100 to 100  
  int8_t joystick_1_y; // from -100 to 100  
  uint8_t switch_1; // =1 if switch ON and =0 if OFF 
  uint8_t button_1; // =1 if button pressed, else =0 
    // output variables
  float onlineGraph_1_var1;
  float onlineGraph_1_var2;
  float onlineGraph_1_var3;
  float circularbar_1;  // from 0 to 2000 
    // other variable
  uint8_t connect_flag;  // =1 if wire connected, else =0 
} RemoteXY;
#pragma pack(pop)
//////////////////////////////////////////////////////////////////////////////////////////

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

//PHOTORESISTANCES
#define RD A0  // composante photoresistor sur la pin A0
#define RC A1  // composante photoresistor sur la pin A1
#define RG A2  // composante photoresistor sur la pin A2

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
CRemoteXY *remotexy; //interface bluetooth

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

  //PHOTORESISTANCES
  pinMode(RD, INPUT);
  pinMode(RC, INPUT);
  pinMode(RG, INPUT);


  //init bluetooth interface
  remotexy = new CRemoteXY (
    RemoteXY_CONF_PROGMEM, 
    &RemoteXY, 
    "", 
    new CRemoteXYStream_HardSerial (
      &Serial,       
      REMOTEXY_SERIAL_SPEED     // REMOTEXY_SERIAL_SPEED
    )
  ); 
}


void loop() {
  //d'abord on fait nos mesure
  float distanceMM = mesureSonar();
  RemoteXY.circularbar_1 = distanceMM;
  RemoteXY.onlineGraph_1_var1 = analogRead(RG);
  RemoteXY.onlineGraph_1_var2 = analogRead(RD);
  RemoteXY.onlineGraph_1_var3 = analogRead(RC);
  //readSerialPort();
  checkParechoc(RemoteXY.switch_1);
  //TODO lux sensor
  //ensuite on rafraichis l'interface
  remotexy->handler();
  //et enfin on écris sur nos moteurs et pince ce qu'on a recus comme ordre
  autoPince(RemoteXY.switch_1, RemoteXY.button_1, distanceMM);

  
  lancerRotationMoteurPontA(map(RemoteXY.joystick_1_y-RemoteXY.joystick_1_x,-150,150,-(vitesseMinA*1.5),(vitesseMinA*1.5))); //1.5 car 150 max (75+75 -> digonal) donc le max doit être de 150%
  lancerRotationMoteurPontB(map(RemoteXY.joystick_1_y+RemoteXY.joystick_1_x,-150,150,-(vitesseMinB*1.5),(vitesseMinB*1.5)));
  ///////////////////////////////////////
  //       100
  //
  //-100    *    100
  //
  //      -100
  //////////////////////////////////////
}

void tournerGauche(){
  lancerRotationMoteurPontA(-vitesseMinA);
  lancerRotationMoteurPontB(vitesseMinB);
}

void tournerDroite(){
  lancerRotationMoteurPontA(vitesseMinA);
  lancerRotationMoteurPontB(-vitesseMinB);
}

float mesureSonar(){
 /* 1. Lance une mesure de distance en envoyant une impulsion HIGH de 10µs sur la broche TRIGGER */
  digitalWrite(TRIGGER_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIGGER_PIN, LOW);
  
  /* 2. Mesure le temps entre l'envoi de l'impulsion ultrasonique et son écho (si il existe) */
  long measure = pulseIn(ECHO_PIN, HIGH, MEASURE_TIMEOUT);
   
  /* 3. Calcul et renvois de la distance en mm à partir du temps mesuré */
  return (measure / 2.0 * SOUND_SPEED);
}

void lancerRotationMoteurPontA(long vitesse) {
  bool marcheAvant = vitesse > -0.1 ? true : false;
  digitalWrite(borneIN2, marcheAvant ? HIGH : LOW);                
  digitalWrite(borneIN1, marcheAvant ? LOW : HIGH);
  analogWrite(borneENA, vitesse);       // Active l'alimentation du moteur 1
}

void arreterRotationMoteurPontA(){
  analogWrite(borneENA, 0);        // Désactive l'alimentation du moteur 1
}

void lancerRotationMoteurPontB(long vitesse) {
  bool marcheAvant = vitesse > -0.1 ? true : false;
  digitalWrite(borneIN3, marcheAvant ? HIGH : LOW);                
  digitalWrite(borneIN4, marcheAvant ? LOW : HIGH);
  analogWrite(borneENB, vitesse);       // Active l'alimentation du moteur 2
}

void arreterRotationMoteurPontB(){
  analogWrite(borneENB, 0);        // Désactive l'alimentation du moteur 2
}

void actionPince(int angle){
   myservo.write(angle);
}

////////////////////////////////////////AUTOMATIC MODE

/**
 * Fonction pour piloter la pince, prend en compte un flag du mode auto et ferme la pince automatiquement. pour la rouvrir, presser le bouton
 */
void autoPince(bool autoMode, bool buttonManual, float sonarDistanceObjct){
  if(autoMode){//Si le mode auto ALORS
    if(buttonManual){ //SI je presse le button 
       actionPince(pinceOuverte);//on ouvre
    } else if (sonarDistanceObjct <= 30 ) { // SINON Si l'objet est a moins ou égale à 30mm (3cm) ALORS
      actionPince(pinceFerme);
    }
  }else{//SINON (Si pas mode Auto ALORS
    if(buttonManual){//SI le bouton est pressé ALORS
      actionPince(pinceFerme); // pince fermé
    }else{ // Sinon
      actionPince(pinceOuverte); // pince ouverte
    }
  }
}
/**
 * Routine auto des parechoc. Doit être désctivé en mode auto (la pince en mode auto gene
 */
void checkParechoc(bool autoMode){
  limitSwitchG.loop(); // MUST call the loop() function first
  limitSwitchD.loop(); // MUST call the loop() function first
  if(!autoMode && (limitSwitchG.getState() == LOW || limitSwitchD.getState() == LOW)){ // SI(INVERSE du mode auto (donc le manuel) ET (Switch gauche pressé OU Switch droit pressé)) ALORS ->
    lancerRotationMoteurPontB(-vitesseMinB); //roule en arriere pour le moteur A
    lancerRotationMoteurPontA(-vitesseMinA); //roule en arriere pour le moteur B
    delay(1000);//pendant 1 sec
    arreterRotationMoteurPontA(); //on arrete A
    arreterRotationMoteurPontB(); //on arrete B
  }
}
