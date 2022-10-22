#include <OneWire.h>
#include <Wire.h> 
#include <OneWire.h>
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27,16,2);  // Устанавливаем  дисплей

// Номер пина Arduino с подключенным датчиком
#define PIN_DS18B20 5
#define Relay 4 // Реле модуль подключен к цифровому выводу 5


OneWire  ds(PIN_DS18B20);  // подключен к 3 пину (резистор на 4.7к обязателен) 
int last_time;
bool on;

void setup(void) {
  pinMode(Relay, OUTPUT);     
  lcd.init();                     
  lcd.backlight();// Включаем подсветку дисплея
  on = false; // пол выключен
  last_time = 0;

  //lcd.setCursor(0, 0);
  //выводим строку 1
  //lcd.print("Initialization..");
  //delay(250);
  //аналогично выводим вторую строку
  //lcd.setCursor(0, 1);
  //lcd.print("Floor is OFF");
  //delay(2250);

  
}

void loop(void) {
  byte i;
  byte present = 0;
  byte type_s;
  byte data[12];
  byte addr[8];
  float celsius, fahrenheit;

  //lcd.setCursor(0, 0);
  //выводим строку 1
  //lcd.print("Start..             ");
  //delay(1000);

  
  if ( !ds.search(addr)) {
    ds.reset_search();
    delay(250);
    return;
  }
  if (OneWire::crc8(addr, 7) != addr[7]) {
      return;
  }
  
  // the first ROM byte indicates which chip
  switch (addr[0]) {
    case 0x10:
      type_s = 1;
      break;
    case 0x28:
      type_s = 0;
      break;
    case 0x22:
      type_s = 0;
      break;
    default:
      return;
  } 

  ds.reset();
  ds.select(addr);
  ds.write(0x44, 1);        // начало коммуникации
  
  //lcd.setCursor(0, 0);
  //выводим строку 1
  //lcd.print("Communication..");
  
  delay(1000);
   
  present = ds.reset();
  ds.select(addr);    
  ds.write(0xBE);         // читаем значение


  for ( i = 0; i < 9; i++) {           // смотрим 9 байтов
    data[i] = ds.read();
  }
    //Преобразуем получненный данные в температуру
    // Используем int16_t тип, т.к. он равен 16 битам
    // даже при компиляции под 32-х битный процессор  
  int16_t raw = (data[1] << 8) | data[0];
  if (type_s) {
    raw = raw << 3;
    if (data[7] == 0x10) {
      raw = (raw & 0xFFF0) + 12 - data[6];
    }
  } else {
    byte cfg = (data[4] & 0x60);
    if (cfg == 0x00) raw = raw & ~7;
    else if (cfg == 0x20) raw = raw & ~3;
    else if (cfg == 0x40) raw = raw & ~1;
  }
  celsius = (float)raw / 16.0;
  fahrenheit = celsius * 1.8 + 32.0;

  char msg[128];
  int temp = celsius;
  int tempd = celsius*100-temp*100;
  sprintf(msg, "T = %d.%d C     ",temp, tempd);
  lcd.setCursor(0, 0);
  lcd.print(msg);
  lcd.setCursor(0, 1);
  if (on) {
    digitalWrite(Relay, LOW); // реле включено если нужно
     sprintf(msg, "ON   %d    ",last_time);
     last_time++;
     lcd.print(msg); 
     }

    else {
      digitalWrite(Relay, HIGH);
      sprintf(msg, "OFF  %d    ",last_time);
     lcd.print(msg); 
    }
  if (celsius>29)
    on = false;   //heat off  
  if (celsius<27)
    on = true;  //heat on

  //lcd.setCursor(0, 0);
  //выводим строку 1
  //lcd.print("Run..");
  //delay(250);
  
  
  }
