#include <Servo.h>

// Creation de l'objet servo
Servo myservo;

// Definition du port du servo
#define servoPin 10

// Variable globale pour stocker la position
int angle = 0;

void setup() {
  // appel de la fonction attach de l'objet
  myservo.attach(servoPin);
}

void loop() {
  // Aller a un angle précis
  myservo.write(90);
  delay(1000);
  
  myservo.write(140);
  delay(1000);
  
  myservo.write(0);
  delay(1000);
  
  // balayage de 0 a 180 degrees
  for (angle = 0; angle <= 140; angle += 1) {
    myservo.write(angle);
    delay(15);
  }
  
  // Balayage inverse (de 180 à 0)
  for (angle = 140; angle >= 0; angle -= 1) {
    myservo.write(angle);
    delay(30);
  }
  
  delay(1000);
}
