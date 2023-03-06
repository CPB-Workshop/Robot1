#include <SoftwareSerial.h>
#define rxPin 2
#define txPin 3
#define baudrate 38400
//#define baudrate 9600 //si l'autre débit ne marche pas (ne renvois pas ERROR ou OK avec les commandes AT

String msg;

SoftwareSerial hc05(rxPin ,txPin);

void setup(){
  pinMode(rxPin,INPUT);
  pinMode(txPin,OUTPUT);
  
  Serial.begin(9600);
  Serial.println("ENTER AT Commands:");
  hc05.begin(baudrate);
}

void loop(){
    readSerialPort();
    if(msg!="") hc05.println(msg);
    
    if (hc05.available()>0){
      Serial.write(hc05.read());
    }
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
