#include <Arduino.h>
// ESP-NOW libraries
#include <WiFi.h>
#include <esp_now.h>

// Struct for incoming data
typedef struct struct_message {
  int x;
  int y;
} struct_message;

// Initialize struct
struct_message data;

// Motor A connections
const int enA = 14;  // PWM pin for motor A
const int in1 = 27;
const int in2 = 26;

// Motor B connections
const int enB = 32;  // PWM pin for motor B
const int in3 = 25;
const int in4 = 33;

// PWM properties
const int freq = 1000;
const int pwmChannelA = 0;
const int pwmChannelB = 1;
const int resolution = 8;  // 8-bit (0-255)

// Callback for receiving data
void OnDataRecv(const uint8_t *mac, const uint8_t *incoming_data, int len) {
  memcpy(&data, incoming_data, sizeof(data));
   Serial.print("x= "); Serial.println(data.x);
  Serial.print("y= "); Serial.println(data.y);
  Serial.println("~~~~~~");
  if (data.y==0){
    if (data.x>0){
      ledcWrite(pwmChannelA, data.x);  
      ledcWrite(pwmChannelB, data.x);
      // Reverse
      digitalWrite(in1, LOW);
      digitalWrite(in2, HIGH);
      digitalWrite(in3, LOW);
      digitalWrite(in4, HIGH);
    }else if (data.x<0){
      ledcWrite(pwmChannelA, abs(data.x));  
      ledcWrite(pwmChannelB, abs(data.x));
       // Forward
      digitalWrite(in1, HIGH);
      digitalWrite(in2, LOW);
      digitalWrite(in3, HIGH);
      digitalWrite(in4, LOW);
    }else{
      digitalWrite(in1,LOW);
      digitalWrite(in2,LOW);
      digitalWrite(in3,LOW);
      digitalWrite(in4,LOW);
    }
  }else if (data.y<0){
       ledcWrite(pwmChannelA, abs(data.y));  
      ledcWrite(pwmChannelB, abs(data.y));
      digitalWrite(in1, LOW);
      digitalWrite(in2, HIGH);
      digitalWrite(in3, HIGH);
      digitalWrite(in4, LOW);
  }else if (data.y>0){
      ledcWrite(pwmChannelA, data.y);  
      ledcWrite(pwmChannelB, data.y);
      digitalWrite(in1, HIGH);
      digitalWrite(in2, LOW);
      digitalWrite(in3, LOW);
      digitalWrite(in4, HIGH);
  }
}

void setup() {
  Serial.begin(115200);

  // Initialize WiFi and ESP-NOW
  WiFi.mode(WIFI_STA);
  Serial.println(WiFi.macAddress());

  if (esp_now_init() != ESP_OK) {
    Serial.println("Eroare initializare esp-now");
    return;
  }

  // Setup motor control pins
  pinMode(in1, OUTPUT);
  pinMode(in2, OUTPUT);
  pinMode(in3, OUTPUT);
  pinMode(in4, OUTPUT);


  // Configure PWM channels
  ledcSetup(pwmChannelA, freq, resolution);
  ledcSetup(pwmChannelB, freq, resolution);

  // Attach PWM pins to channels
  ledcAttachPin(enA, pwmChannelA);
  ledcAttachPin(enB, pwmChannelB);

  // Turn off motors initially
  digitalWrite(in1, LOW);
  digitalWrite(in2, LOW);
  digitalWrite(in3, LOW);
  digitalWrite(in4, LOW);

  // Set initial PWM duty cycle to 0
  ledcWrite(pwmChannelA, 0);
  ledcWrite(pwmChannelB, 0);

  esp_now_register_recv_cb(OnDataRecv);
}

// Control motor direction
void directionControl() {
  ledcWrite(pwmChannelA, 255);  // Full speed
  ledcWrite(pwmChannelB, 255);

  // Forward
  digitalWrite(in1, HIGH);
  digitalWrite(in2, LOW);
  digitalWrite(in3, HIGH);
  digitalWrite(in4, LOW);
  delay(2000);

  // Reverse
  digitalWrite(in1, LOW);
  digitalWrite(in2, HIGH);
  digitalWrite(in3, LOW);
  digitalWrite(in4, HIGH);
  delay(2000);

  // Stop
  digitalWrite(in1, LOW);
  digitalWrite(in2, LOW);
  digitalWrite(in3, LOW);
  digitalWrite(in4, LOW);
}

// Control motor speed
void speedControl() {
  // Reverse direction
  digitalWrite(in1, LOW);
  digitalWrite(in2, HIGH);
  digitalWrite(in3, LOW);
  digitalWrite(in4, HIGH);

  // Accelerate
  for (int i = 0; i < 256; i++) {
    ledcWrite(pwmChannelA, i);
    ledcWrite(pwmChannelB, i);
    delay(20);
  }
    // Decelerate
  for (int i = 255; i >= 0; --i) {
    ledcWrite(pwmChannelA, i);
    ledcWrite(pwmChannelB, i);
    delay(20);
  }

  // Stop
  digitalWrite(in1, LOW);
  digitalWrite(in2, LOW);
  digitalWrite(in3, LOW);
  digitalWrite(in4, LOW);
}


void loop() {
  //directionControl();
  //delay(1000);
  //speedControl();
  //delay(1000);
}
