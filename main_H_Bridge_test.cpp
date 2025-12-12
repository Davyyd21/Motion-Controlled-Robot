#include <Arduino.h>

int R1=2;
int R2=3;
int R3=4;
int R4=5;

void setup() {
  Serial.begin(115200);
  pinMode(R1, OUTPUT);
  pinMode(R2, OUTPUT);
  pinMode(R3, OUTPUT);
  pinMode(R4, OUTPUT);
  digitalWrite(R1, LOW);
  digitalWrite(R2, LOW);
  digitalWrite(R3, LOW);
  digitalWrite(R4, HIGH);
  }

void loop(){
  digitalWrite(R1, LOW);
  digitalWrite(R2, HIGH);
  digitalWrite(R3, HIGH);
  digitalWrite(R4, HIGH);
  delay(1000);
  digitalWrite(R1, LOW);
  digitalWrite(R2, LOW);
  digitalWrite(R3, LOW);
  digitalWrite(R4, HIGH);
  delay(1000);
}
