#include <LoRa.h>
#include <SPI.h>
#include<Wire.h>
#include<OneWire.h>
#include<WiFiNINA.h>
#include <Arduino_LSM6DS3.h>
#include<Firebase_Arduino_WiFiNINA.h>


char host[] = "watermonitoring-a2979-default-rtdb.asia-southeast1.firebasedatabase.app";
//https://watermonitoring-a2979-default-rtdb.asia-southeast1.firebasedatabase.app/  https://watermonitoring-a2979-default-rtdb.firebaseio.com
char auth[] = "AIzaSyBUEzofhC2BYQ6dpZpqGhPLLFp1_29E-Ck";
//apikey = AIzaSyBUEzofhC2BYQ6dpZpqGhPLLFp1_29E-Ck  IHbGEoMNJTnMPs9flE4fuUXft7lC3UAKfwFd9GwL
char ssid[] =  "WiFi_Keluarga";
char pass[] = "hpkuratarata";

FirebaseData firebaseData;

void setup() {
  Serial.begin(9600);
  while(!Serial);
  Serial.println("LoRa Gateway");
  if (!LoRa.begin(915E6)) {
    Serial.println("Starting LoRa failed!");
    while (1);
  }

  WiFi.begin(ssid, pass);
  Serial.println("Connecting");
  while(WiFi.status()!=WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }

  Serial.println();
  Serial.print("Connected with ip: ");
  Serial.println(WiFi.localIP());
  Firebase.begin(host, auth, ssid, pass);
  Firebase.reconnectWiFi(true);
}


void loop(){
  int packetSize = LoRa.parsePacket();
  if (packetSize) {
    //menerima data
    String LoRaData = LoRa.readString();
    Serial.println(LoRaData);
    //while(LoRa.available()){
      //Serial.print((char)LoRa.read());
    //}

    String pH = LoRaData.substring(0,4);
    String TDS = LoRaData.substring(5,11);
    String suhu = LoRaData.substring(12,20);
    Serial.print("pH: ");
    Serial.print(pH);
    Serial.print(" | TDS: ");
    Serial.print(TDS);
    Serial.print(" | Suhu: ");
    Serial.print(suhu);

    Serial.print(" | with RSSI");
    Serial.print(LoRa.packetRssi());
    Serial.print("dBm");
    Serial.print(" | With SNR");
    Serial.print(LoRa.packetSnr());
    Serial.println("dB");

    if(Firebase.setString(firebaseData,"Hasil/ph",pH)){
      Serial.println("ph terkirim");
    }else {
      Serial.print("ph tidak terkirim ");
      Serial.println("karena "+firebaseData.errorReason());
    }
        if(Firebase.setString(firebaseData,"Hasil/tds",TDS)){
      Serial.println("tds terkirim");
    }else {
      Serial.print("tds tidak terkirim ");
      Serial.println("karena "+firebaseData.errorReason());
    }
    if(Firebase.setString(firebaseData,"Hasil/suhu",suhu)){
      Serial.println("suhu terkirim");
    }else {
      Serial.print("suhu tidak terkirim ");
      Serial.println("karena "+firebaseData.errorReason());
    }

  }
}
