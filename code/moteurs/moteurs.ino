#define borneENA        6       // On associe la borne "ENA" du L298N à la pin D8 de l'arduino
#define borneIN1        7       // On associe la borne "IN1" du L298N à la pin D7 de l'arduino
#define borneIN2        8       // On associe la borne "IN2" du L298N à la pin D6 de l'arduino
#define borneIN3        5       // On associe la borne "IN3" du L298N à la pin D5 de l'arduino
#define borneIN4        4       // On associe la borne "IN4" du L298N à la pin D4 de l'arduino
#define borneENB        3       // On associe la borne "ENB" du L298N à la pin D3 de l'arduino
//vitesse min : 100
void setup() {
  
  // Configuration de toutes les pins de l'Arduino en "sortie" (car elles attaquent les entrées du module L298N)
  pinMode(borneENA, OUTPUT);
  pinMode(borneIN1, OUTPUT);
  pinMode(borneIN2, OUTPUT);
  pinMode(borneIN3, OUTPUT);
  pinMode(borneIN4, OUTPUT);
  pinMode(borneENB, OUTPUT);

}


void loop() {
}

void lancerRotationMoteurPontA(bool marcheAvant, char vitesse) {
  digitalWrite(borneIN1, marcheAvant ? LOW : HIGH);                
  digitalWrite(borneIN2, marcheAvant ? HIGH : LOW);
  
  analogWrite(borneENA, vitesse);       // Active l'alimentation du moteur 1
}

void arreterRotationMoteurPontA(){
  analogWrite(borneENA, 0);        // Désactive l'alimentation du moteur 1
}

void lancerRotationMoteurPontB(bool marcheAvant, char vitesse) {
  digitalWrite(borneIN4, marcheAvant ? LOW : HIGH);                
  digitalWrite(borneIN3, marcheAvant ? HIGH : LOW);
  
  analogWrite(borneENB, vitesse);       // Active l'alimentation du moteur 2
}

void arreterRotationMoteurPontB(){
  analogWrite(borneENB, 0);        // Désactive l'alimentation du moteur 2
}
