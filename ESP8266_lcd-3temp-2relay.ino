
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <OneWire.h> 
#include <DallasTemperature.h>

#define ONE_WIRE_BUS_1 D3
#define ONE_WIRE_BUS_2 D4
#define ONE_WIRE_BUS_3 D5

int k = 1;
float TankTemp,NormalTemp,OutletTemp;
boolean changeStatus = 0;
boolean vms = 0;
boolean itr_block = 0;
int ValveMode;
boolean Old_ValveMode = 0;
int ValveSwitchTime = 11;
String ModeText = "";
int period = 1000;
int spin_period = 250;
unsigned long time_now = 0;
unsigned long cursor_time;
int loop_count1 = 0;
int loop_count2 = 0;

LiquidCrystal_I2C lcd(0x27, 16, 2);
OneWire oneWire1(ONE_WIRE_BUS_1); 
OneWire oneWire2(ONE_WIRE_BUS_2);
OneWire oneWire3(ONE_WIRE_BUS_3); 
DallasTemperature sensors_1(&oneWire1);
DallasTemperature sensors_2(&oneWire2);
DallasTemperature sensors_3(&oneWire3);

byte Heart[8] = {
  0b00000,
  0b01010,
  0b11111,
  0b11111,
  0b01110,
  0b00100,
  0b00000,
  0b00000
};

byte BackSlash[8] = {
  0b00000,
  0b10000,
  0b01000,
  0b00100,
  0b00010,
  0b00001,
  0b00000,
  0b00000
};

void Mode_Home(){
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Mode: ");
  ValveModeCheck();
  lcd.setCursor(6, 0);
  lcd.print(ModeText);
  lcd.setCursor(0, 1);
  lcd.print("WaterTemp: ");
  lcd.setCursor(11, 1);
  lcd.print(int(OutletTemp));
  lcd.setCursor(13, 1);
  lcd.print((char)223);
  lcd.setCursor(14, 1);
  lcd.print("C");
}

void Mode_TankTemp(){
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Tank Water");
  lcd.setCursor(0, 1);
  lcd.print("Temp: ");
  lcd.setCursor(6, 1);
  lcd.print(TankTemp);
  lcd.setCursor(11, 1);
  lcd.print((char)223);
  lcd.setCursor(12, 1);
  lcd.print("C");
}

void Mode_NormalTemp(){
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Normal Water");
  lcd.setCursor(0, 1);
  lcd.print("Temp: ");
  lcd.setCursor(6, 1);
  lcd.print(NormalTemp);
  lcd.setCursor(11, 1);
  lcd.print((char)223);
  lcd.setCursor(12, 1);
  lcd.print("C");
}

void Mode_OutletTemp(){
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Outlet Water");
  lcd.setCursor(0, 1);
  lcd.print("Temp: ");
  lcd.setCursor(6, 1);
  lcd.print(OutletTemp);
  lcd.setCursor(11, 1);
  lcd.print((char)223);
  lcd.setCursor(12, 1);
  lcd.print("C");
}

void ValveModeCheck(){
  if (ValveMode == 0){
    ModeText = "Tank Only";
  }
  else{
    ModeText = "Mixed";
  }
}

void Mode_ModeSelect(){
  if (changeStatus == 1){
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Mode Selecting:");
    lcd.setCursor(2, 1);
    lcd.print("Tank | ");
    lcd.setCursor(10, 1);
    lcd.print("Mixed ");
    if (ValveMode == 0){
      lcd.setCursor(1, 1);
      lcd.print((char)126);
    }
    else{
      lcd.setCursor(9, 1);
      lcd.print((char)126);
    }
  }
}

void cursor_loop(){
  if (loop_count2 == 0){
    lcd.setCursor(1, 0);
    lcd.print("|");
    lcd.setCursor(14, 0);
    lcd.print("|");
    delay(cursor_time);
  }
  else if (loop_count2 == 1){
    lcd.setCursor(1, 0);
    lcd.print("/");
    lcd.setCursor(14, 0);
    lcd.print("/");
    delay(cursor_time);
  }
  else if (loop_count2 == 2){
    lcd.setCursor(1, 0);
    lcd.print("-");
    lcd.setCursor(14, 0);
    lcd.print("-");
    delay(cursor_time);
  }
  else{
    lcd.setCursor(1, 0);
    lcd.write(byte(1));
    lcd.setCursor(14, 0);
    lcd.write(byte(1));
    delay(cursor_time);
  }
}

void Mode_toChangeValve(){
  lcd.clear();
  if (ValveMode == 0){
    lcd.setCursor(3, 0);
    lcd.print("Tank Only!");
    lcd.setCursor(0, 1);
    lcd.print(" Mode Switching ");
    digitalWrite(D7, LOW);
    digitalWrite(D6, HIGH);
  }
  else{
    lcd.setCursor(5, 0);
    lcd.print("Mixed!");
    lcd.setCursor(0, 1);
    lcd.print(" Mode Switching ");
    digitalWrite(D6, LOW);
    digitalWrite(D7, HIGH);
  }
  cursor_time = spin_period;
  for (loop_count1 = 0; loop_count1 < ValveSwitchTime; loop_count1++){
    for (loop_count2 = 0; loop_count2 < 4; loop_count2++){
      cursor_loop();
    }
  }
  digitalWrite(D6, LOW);
  digitalWrite(D7, LOW);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(" Mode Switching ");
  lcd.setCursor(1, 1);
  lcd.write(byte(0));
  lcd.setCursor(14, 1);
  lcd.write(byte(0));
  lcd.setCursor(3, 1);
  lcd.print("Completed!");
  delay(period);
  vms = 0;
  k = 1;
  itr_block = 0;
  Mode_Home();
}

void ValveModeNoChanged(){
  lcd.clear();
  lcd.setCursor(1, 0);
  lcd.print("* Valve Mode *");
  lcd.setCursor(1, 1);
  lcd.print("<");
  lcd.setCursor(14, 1);
  lcd.print(">");
  lcd.setCursor(3, 1);
  lcd.print("No Changed");
  delay(period);
  vms = 0;
  k = 1;
  itr_block = 0;
  Mode_Home();
}

void gettemp(){
  sensors_1.requestTemperatures();
  sensors_2.requestTemperatures();
  sensors_3.requestTemperatures();
  TankTemp = sensors_1.getTempCByIndex(0); // Water Tank
  NormalTemp = sensors_2.getTempCByIndex(0); // Normal Water
  OutletTemp = sensors_3.getTempCByIndex(0); // Outlet Water
}

void goto_mode(){
  if (k == 1){
    Mode_Home();
  }
  else if (k == 2){
    Mode_TankTemp();
  }
  else if (k == 3){
    Mode_NormalTemp();
  }
  else{
    Mode_OutletTemp();
  }
}

void cursor_print(){
  if (ValveMode == 0){
    lcd.setCursor(9, 1);
    lcd.print(" ");
    lcd.setCursor(1, 1);
    lcd.print((char)126);
  }
  else{
    lcd.setCursor(1, 1);
    lcd.print(" ");
    lcd.setCursor(9, 1);
    lcd.print((char)126);
  }
}

void ValveMode_print(){
  if (ValveMode == 0){
    lcd.setCursor(1, 1);
    lcd.print((char)155);
    lcd.setCursor(3, 1);
    lcd.print("Tank Only!");
    lcd.setCursor(14, 1);
    lcd.print((char)155);
    time_now = millis();
    delay(period);
  }
  else{
    lcd.setCursor(3, 1);
    lcd.print((char)157);
    lcd.setCursor(5, 1);
    lcd.print("Mixed!");
    lcd.setCursor(12, 1);
    lcd.print((char)157);
    delay(period);
  }
}

void setup()
{
  //Serial.begin(115200);
  lcd.begin();
  lcd.backlight();
  Wire.setClock(10000);
  sensors_1.begin();
  sensors_2.begin(); 
  sensors_3.begin();
  pinMode(D3,INPUT);
  pinMode(D4,INPUT);
  pinMode(D5,INPUT);
  pinMode(3,INPUT);
  pinMode(D8,INPUT);
  pinMode(D1,OUTPUT);
  pinMode(D2,OUTPUT);
  pinMode(D6,OUTPUT);
  pinMode(D7,OUTPUT);
  digitalWrite(D6, LOW);
  digitalWrite(D7, LOW);
  lcd.createChar(0, Heart);
  lcd.createChar(1, BackSlash);
  lcd.clear();
  digitalWrite(D7, LOW);
  digitalWrite(D6, HIGH);
  lcd.setCursor(1, 0);
  lcd.print("Solar Thermal");
  lcd.setCursor(2, 1);
  lcd.print("Water Heater");
  delay(3000);
  lcd.clear();
  lcd.setCursor(2, 0);
  lcd.print("By: B6112950");
  lcd.setCursor(1, 1);
  lcd.print("Paradorn Raks.");
  delay(2000);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("To: 61PPL03");
  lcd.setCursor(1, 1);
  lcd.print("Padej Pao-la-or");
  delay(2000);
  lcd.clear();
  lcd.setCursor(4, 0);
  lcd.print("Starting");
  lcd.setCursor(1, 1);
  lcd.print("Please Wait...");
  cursor_time = 200;
  for (loop_count1 = 0; loop_count1 < 5; loop_count1++){
    for (loop_count2 = 0; loop_count2 < 4; loop_count2++){
      cursor_loop();
    }
  }
  lcd.clear();
  digitalWrite(D6, LOW);
  digitalWrite(D7, LOW);
  ValveMode = 0;
  sensors_3.requestTemperatures();
  OutletTemp = sensors_3.getTempCByIndex(0); // Outlet Water
  lcd.clear();
  attachInterrupt(digitalPinToInterrupt(3), modeselect, FALLING);
  attachInterrupt(digitalPinToInterrupt(D8), modechange, RISING);
  Wire.setClock(400000);
  Mode_Home();
}


ICACHE_RAM_ATTR void modechange() {
  if (itr_block == 0){
    static unsigned long last_interrupt_time_modechange = 0;
    unsigned long interrupt_time_modechange = millis();
    if (interrupt_time_modechange - last_interrupt_time_modechange > 300){
      //Serial.println("INTERRUPT_2 TO Mode Screen");
      lcd.clear();
      k = 0;
      changeStatus = !changeStatus;
      if (changeStatus == 0){
        itr_block = 1;
        lcd.clear();
        lcd.setCursor(1, 0);
        lcd.print("* Valve Mode *");
      }
      else{
        if (vms == 0){
          Old_ValveMode = ValveMode;
          vms = 1;
        }
        lcd.clear();
        Mode_ModeSelect();
      }
    }
    last_interrupt_time_modechange = interrupt_time_modechange;
  }
}


ICACHE_RAM_ATTR void modeselect() {
  if (itr_block == 0){
    static unsigned long last_interrupt_time_modeselect = 0;
    unsigned long interrupt_time_modeselect = millis();
    if (interrupt_time_modeselect - last_interrupt_time_modeselect > 250){
      if (k >= 1 && changeStatus == 0){
        //Serial.println("INTERRUPT_1 TO SW Screen k = ");
        k = k+1;
        if (k > 4){
          k = 1;
        }
        //Serial.println(k);
        goto_mode();
      }
      else if (k < 1 && changeStatus == 1){
        //Serial.println("INTERRUPT_1 TO SW Mode");
        ValveMode = !ValveMode;
        cursor_print();
      }
    }
    last_interrupt_time_modeselect = interrupt_time_modeselect;
  }
}

void loop() {
  if (changeStatus == 0){
    if (k < 1){
      lcd.clear();
      lcd.setCursor(1, 0);
      lcd.print("* Valve Mode *");
      ValveMode_print();
      lcd.clear();
      if (ValveMode == Old_ValveMode){
        ValveModeNoChanged();
      }
      else{
        Mode_toChangeValve();
      }
    }
    else{
      gettemp();
      switch (k){
        case 1:
          lcd.setCursor(11, 1);
          lcd.print("     ");
          lcd.setCursor(11, 1);
          lcd.print(int(OutletTemp));
          lcd.setCursor(13, 1);
          lcd.print((char)223);
          lcd.setCursor(14, 1);
          lcd.print("C");
          break;
    
        case 2:
          lcd.setCursor(6, 1);
          lcd.print("          ");
          lcd.setCursor(6, 1);
          lcd.print(TankTemp);
          lcd.setCursor(11, 1);
          lcd.print((char)223);
          lcd.setCursor(12, 1);
          lcd.print("C");
          break;
    
        case 3:
          lcd.setCursor(6, 1);
          lcd.print("          ");
          lcd.setCursor(6, 1);
          lcd.print(NormalTemp);
          lcd.setCursor(11, 1);
          lcd.print((char)223);
          lcd.setCursor(12, 1);
          lcd.print("C");
          break;
    
        case 4:
          lcd.setCursor(6, 1);
          lcd.print("          ");
          lcd.setCursor(6, 1);
          lcd.print(OutletTemp);
          lcd.setCursor(11, 1);
          lcd.print((char)223);
          lcd.setCursor(12, 1);
          lcd.print("C");
          break;
      }
    }
  }
}
