#include <LoRa.h>               //inisialisasi LoRa
#include <LiquidCrystal_I2C.h>  //inisialisasi LCDI2C
#include <Wire.h>
#include <OneWire.h>
#include <DallasTemperature.h>  //inisialisasi Suhu
OneWire pin_DS18B20(4);
DallasTemperature DS18B20(&pin_DS18B20);

const int tdsPin = A0;
const int pinPH = A1;
const int ledPin = 13;
int nilaiSensor = 0;
unsigned long int rataADCPH;
int bufADCPH[10],temp;
LiquidCrystal_I2C lcd(0x27, 16, 2);

void setup() {
  
  Serial.begin(9600);
  DS18B20.begin();
  
  //setting LCD I2C
  lcd.init();
  lcd.backlight();
  
  //setting pin LED
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);
  while (!Serial) {
    ;
  }
  if (!LoRa.begin(915E6)) {
    Serial.println("Starting LoRa failed!");
    while (1);
  }
}

void loop() {
  //pengukuran pH 
  for(int i=0;i<10;i++)
  {
     bufADCPH[i]=analogRead(pinPH);
     delay(10);
  }
    for (int i=0;i<9;i++) 
    {
      for (int j=i+1;j<10;j++)
      {
        if(bufADCPH[i]>bufADCPH[j])
        {
          temp=bufADCPH[i];
          bufADCPH[i]=bufADCPH[j];
          bufADCPH[j]=temp;
        }
      }
    }
    rataADCPH=0;
    for (int i=2;i<8;i++) {
      rataADCPH+=bufADCPH[i];
    }
  float teganganPH = (float)rataADCPH*5.0/1024/6;
  float nilaipH = 2.101628 * teganganPH + 2.917;
  //pengukuran TDS
  int tdsValue = analogRead(tdsPin);
  float nilaiTDS = map(tdsValue, 0, 1023, 0, 1000);
  
  float suhu = DS18B20.getTempCByIndex(0);

  //menampilkan pada LCD
  lcd.clear();
  lcd.setCursor(0, 0);  
  lcd.print("pH : ");
  lcd.print(nilaipH);
  lcd.setCursor(0,1);
  lcd.print("TDS: ");
  lcd.print(nilaiTDS);
  lcd.setCursor(8, 0);
  lcd.print("Suhu: ");
  lcd.print(suhu);

  

  //kirim data ke gateway
  String data = String(nilaipH)+" "+String(nilaiTDS)+" "+String(suhu);
  LoRa.beginPacket();
  LoRa.print(data);
  LoRa.endPacket();
  Serial.println("Kirim data"+data);

  //indikasi Data dikirim ke gateway
  digitalWrite(ledPin, HIGH);
  delay(1000);
  digitalWrite(ledPin, LOW);
  delay(3000);
}
