#include <Arduino.h>
//librarii accelerometru
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>
//librarii espnow
#include <esp_now.h>
#include <WiFi.h>

//declarare senzor
Adafruit_MPU6050 mpu;

//reciever mac address
uint8_t broadcastAddress[] = {0xD4, 0x8A, 0xFC, 0xA2, 0x6D, 0x14};

//structura date trimise
typedef struct struct_message {
    int x;
    int y;
}struct_message;

struct_message data;
esp_now_peer_info_t peerInfo;

// callback when data is sent
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("\r\nLast Packet Send Status:\t");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
}

void setup(void) {
  //serial begin
  Serial.begin(115200);
  while (!Serial) {
    delay(10); 
  }

  // initializare senzor
  if (!mpu.begin()) {
    Serial.println("Failed to find MPU6050 chip");
    while (1) {
      delay(10);
    }
  }
  mpu.setAccelerometerRange(MPU6050_RANGE_16_G);
  mpu.setGyroRange(MPU6050_RANGE_250_DEG);
  mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);
  Serial.println("");
  delay(100);

  //initializare wifi
  WiFi.mode(WIFI_STA);
  Serial.println(WiFi.macAddress());

  //initializare espnow
  if (esp_now_init() != ESP_OK) {
    Serial.println("Eroare initializare ESP-NOW");
    return;
  }

  // Once ESPNow is successfully Init, we will register for Send CB to
  // get the status of Transmitted packet
  esp_now_register_send_cb(OnDataSent);

  // Register peer
  memcpy(peerInfo.peer_addr, broadcastAddress, 6);
  peerInfo.channel = 0;  
  peerInfo.encrypt = false;

  // Add peer        
  if (esp_now_add_peer(&peerInfo) != ESP_OK){
    Serial.println("Failed to add peer");
    return;
  }

}

void loop() {

  //sensor reading
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);
  //interpretare date
  /*
  if( a.acceleration.z > 9 && a.acceleration.x > -2 && a.acceleration.x < 2 && a.acceleration.y > -2 && a.acceleration.y < 2  ) Serial.println("!STOP!");
  else if( a.acceleration.x <= -2  && a.acceleration.y > -2 && a.acceleration.y < 2 ) Serial.println("FATA");
  else if( a.acceleration.x >= 2 && a.acceleration.y > -2 && a.acceleration.y < 2 ) Serial.println("SPATE");
  else if( a.acceleration.x > -2 && a.acceleration.x < 2 && a.acceleration.y <= -2 ) Serial.println("STANGA");
  else if( a.acceleration.x > -2 && a.acceleration.x < 2 && a.acceleration.y > 2  ) Serial.println("DREAPTA");
  else Serial.println("~ EROARE EROARE EROARE ~");
  */
  //Serial.print("pozitie fata-spate:"); Serial.println(a.acceleration.x);
  //Serial.print("pozitie stanga-dreapta:"); Serial.println(a.acceleration.y);
  //Serial.println("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~`");
  int px=0,py=0,pwmx=0,pwmy=0,okx=0,oky=0;
  if (a.acceleration.x>2){
    okx=1;
    px = map(a.acceleration.x, 2, 7, 0, 100); 
    pwmx = map(a.acceleration.x, 2, 7, 0, 255); 
  }else if (a.acceleration.x<-2){
    px = map(a.acceleration.x, -2, -7, 0, 100); 
    pwmx = map(a.acceleration.x, -2, -7, 0, 255); 
    okx=2;
  }

  if (a.acceleration.y>2){
    py = map(a.acceleration.y, 2, 9, 0, 100); 
    pwmy = map(a.acceleration.y, 2, 9, 0, 255); 
    oky=1;
  }else if (a.acceleration.y<-2){
    py = map(a.acceleration.y, -2, -9, 0, 100); 
    pwmy = map(a.acceleration.y, -2, -9, 0, 255); 
    oky=2;
  }

  if(px>100)px=px-px%100;
  if(pwmx>255)pwmx=pwmx-pwmx%255;
  if(py>100)py=py-py%100;
  if(pwmy>255)pwmy=pwmy-pwmy%255;
  int x_send=0,y_send=0;
  //printare date serial monitor
  Serial.print("Axa OX: ");
  if (okx==1){
    Serial.print("Spate "); Serial.print(a.acceleration.x); Serial.print(" "); Serial.print(px); Serial.print("% "); Serial.println(pwmx);
    x_send=pwmx;
  }else if (okx==2){
    Serial.print("Fata "); Serial.print(a.acceleration.x); Serial.print(" "); Serial.print(px); Serial.print("% "); Serial.println(pwmx);
    x_send=-pwmx;
  }else Serial.println(".........");

  Serial.print("Axa OY: ");
  if (oky==1){
    Serial.print("Dreapta "); Serial.print(a.acceleration.y); Serial.print(" "); Serial.print(py); Serial.print("% "); Serial.println(pwmy);
    y_send=pwmy;
  }else if (oky==2){
    Serial.print("Stanga "); Serial.print(a.acceleration.y); Serial.print(" "); Serial.print(py); Serial.print("% "); Serial.println(pwmy);
    y_send=-pwmy;
  }else Serial.println(".........");

  //initializare date structura
  data.x = x_send;
  data.y = y_send;
  //trimitere date
    esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &data, sizeof(data));
  //mesaj de verificare
   if (result==ESP_OK){
    Serial.println("Sent with success");
  }else{
    Serial.println("Error sending the data");}
  Serial.println("~~~~~~");
  delay(1000);
}
