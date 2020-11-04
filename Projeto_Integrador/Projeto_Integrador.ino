#include <Arduino.h>
#include <U8g2lib.h>
#include "TinyDHT.h"
#include <Adafruit_Sensor.h>

#define DHTPIN 6
#define DHTTYPE DHT11

DHT dht(DHTPIN, DHTTYPE);

U8G2_SH1106_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0);

//Linhas e colunas
int ln1 = 31;
int ln2 = 47;
int ln3 = 63;
int cl1 = 42;
int cl2 = 105;

int pin2 = 3;
int pin1 = 2;
int mq2 = 4;
int mq9 = 5;
//int limMq2 = 1000;

unsigned long duration1;
unsigned long duration2;

unsigned long starttime;
unsigned long sampletime_ms = 3000;//sampe 1s ;
unsigned long lowpulseoccupancy1 = 0;
unsigned long lowpulseoccupancy2 = 0;
float ratio1 = 0;
float ratio2 = 0;
float concentration1 = 0;
float concentration2 = 0;

void setup() {

  Serial.begin(9600);
  
  pinMode(2,INPUT); //Sensor de poeira menor que 1,5 micrômetros
  pinMode(3,INPUT); //Sensor de Poeira maior que 1,5 micrômetos
  pinMode(4,INPUT); //Sensor MQ-2 (fumaça)
  pinMode(5,INPUT); //Sensor MQ-9 (Monóxido de Carbono)
  pinMode(6,INPUT); //Sensor DTH (Humidade e Temperatura)
  
  starttime = millis();
  
  
  u8g2.begin();
  dht.begin();
}

void loop() {

  duration1 = pulseIn(pin1, LOW);
  duration2 = pulseIn(pin2, LOW);
  
  lowpulseoccupancy1 = lowpulseoccupancy1+duration1;
  lowpulseoccupancy2 = lowpulseoccupancy2+duration2;
  
  int sensorFumaca = digitalRead(mq2);
  int sensorCO2 = digitalRead(mq9);
  float umidade = dht.readHumidity();
  float temp = dht.readTemperature();
  
  if ((millis()-starttime) > sampletime_ms)
  {
    ratio1 = lowpulseoccupancy1/(sampletime_ms*10.0);  // Integer percentage 0=>100
    concentration1 = 1.1*pow(ratio1,3)-3.8*pow(ratio1,2)+520*ratio1+0.62; // using spec sheet curve

    ratio2 = lowpulseoccupancy2/(sampletime_ms*10.0);  // Integer percentage 0=>100
    concentration2 = 1.1*pow(ratio2,3)-3.8*pow(ratio2,2)+520*ratio2+0.62; // 

    
    u8g2.clearBuffer();
    u8g2.setFont(u8g2_font_7x14B_tf ); //FONTE 10PX
    u8g2.setCursor(0,10);
    u8g2.print("  Qualidade do Ar");
    u8g2.setFont(u8g2_font_micro_tr); //FONTE 5PX
    u8g2.setCursor(0,ln1);
    u8g2.print("Umidade%:");
    u8g2.setCursor(0,ln2);
    u8g2.print("Temp(C):");
    u8g2.setCursor(0,ln3);
    u8g2.print("Fumaca:");
    u8g2.drawVLine(64,20,42);
    u8g2.setCursor(70,ln1);
    u8g2.print("Poeira1:");
    u8g2.setCursor(70,ln2);
    u8g2.print("Poeira2:");
    u8g2.setCursor(70,ln3);
    u8g2.print("CO2:");
    

     //Inserindo Valores
    u8g2.setCursor(cl1,ln1); //Valor da Humidade
    u8g2.print(umidade); 
    u8g2.setCursor(cl1,ln2); //Valor da Temperatura
    u8g2.print(temp); 
    u8g2.setCursor(cl1,ln3); //Valor da fumaça
    if (sensorFumaca == LOW){
      u8g2.print("S");
    }else{
      u8g2.print("N");
    }
    u8g2.setCursor(cl2,ln1); // Valor da Poeira 1
    u8g2.print(concentration1);
    u8g2.setCursor(cl2,ln2); // Valor Poeira 2
    u8g2.print(concentration2);
    u8g2.setCursor(cl2,ln3); //Valor CO2
    if (sensorCO2 == LOW){
      u8g2.print("S");
    }else{
      u8g2.print("N");
    }
    
   
   u8g2.sendBuffer();
   delay(starttime);

    lowpulseoccupancy1 = 0;
    lowpulseoccupancy2 = 0;
    starttime = millis();

  }


}
