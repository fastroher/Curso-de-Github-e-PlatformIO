#include <Arduino.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <LiquidCrystal_I2C.h>

const int PINLED = 13; //testando
const int BUTTON1 = 10;
const int BUTTON2 = 11;
const int ONE_WIRE_BUS = 9;
const int I2C_ARRD = 0x27;
const int COLUMNS = 20;
const int LINES = 4;

DeviceAddress S1 = {0x28, 0xFF, 0xCE, 0x86, 0x86, 0x16, 0x04, 0x8C};
DeviceAddress S2 = {0x28, 0xFF, 0xEB, 0xDD, 0x52, 0x16, 0x04, 0x6D};

OneWire onewire(ONE_WIRE_BUS);
DallasTemperature sensors(&onewire);
LiquidCrystal_I2C lcd(I2C_ARRD, COLUMNS, LINES);

int tela = 0;
int telaAnterior = -1;

float s1, s2;

void setup(){
  lcd.init();
  lcd.backlight();
  lcd.setCursor(1, 1);
  lcd.print("Iniciando sistema!");
  sensors.begin();
  sensors.setWaitForConversion(false);
  sensors.requestTemperatures();
  pinMode(BUTTON1, INPUT_PULLUP);
  pinMode(BUTTON2, INPUT_PULLUP);
  delay(500);
  s1 = sensors.getTempC(S1);
  s2 = sensors.getTempC(S2);
}

void loop(){
  static bool b1Ant = false;
  static bool b2Ant = false;
  static bool aguardandoConversao = false;
  static unsigned long instantePedido = 0;
  static float s1f, s2f;

  bool b1 = !digitalRead(BUTTON1);
  bool b2 = !digitalRead(BUTTON2);
  bool b1Pressionado = (b1 && !b1Ant);
  bool b2Pressionado = (b2 && !b2Ant);

  b1Ant = b1;
  b2Ant = b2;

  if (!aguardandoConversao && (millis() - instantePedido >= 1000)){
    sensors.requestTemperatures();
    aguardandoConversao = true;
    instantePedido = millis();
  }

  if (aguardandoConversao && sensors.isConversionComplete()){
    s1  = sensors.getTempC(S1);
    s2  = sensors.getTempC(S2);
    s1f = sensors.getTempF(S1);
    s2f = sensors.getTempF(S2);
    aguardandoConversao = false;
  }

  if (tela == 1){
    if (b1Pressionado)      tela = 2;
    else if (b2Pressionado) tela = 4;
  }
  else if (tela == 2){
    if (b1Pressionado)      tela = 3;
    else if (b2Pressionado) tela = 0;
  }
  else if (tela == 3){
    if (b2Pressionado) tela = 0;
  }
  else if (tela == 4){
    if (b2Pressionado) tela = 0;
  }

  if (tela != telaAnterior){
    lcd.clear();
  }

  switch (tela){

    case 0:
      lcd.setCursor(0,0); lcd.print("MENU:               ");
      lcd.setCursor(0,1); lcd.print("B1 - Temperatura    ");
      lcd.setCursor(0,2); lcd.print("B2 - Endereco       ");
      lcd.setCursor(0,3); lcd.print("                    ");
      tela = 1;
      break;

    case 1:
      break;

    case 2:
      lcd.setCursor(0,0); lcd.print("TEMP. (");
      lcd.write(0xDF);    lcd.print("C)       ");
      lcd.setCursor(0,1); lcd.print("Temp. 1: ");
      lcd.print(s1, 1);   lcd.write(0xDF); lcd.print("C   ");
      lcd.setCursor(0,2); lcd.print("Temp. 2: ");
      lcd.print(s2, 1);   lcd.write(0xDF); lcd.print("C   ");
      lcd.setCursor(0,3); lcd.print("1: F  2: Sair       ");
      break;

    case 3:
      lcd.setCursor(0,0); lcd.print("TEMP. (F)           ");
      lcd.setCursor(0,1); lcd.print("Temp. 1: ");
      lcd.print(s1f, 1);  lcd.print(" F  ");
      lcd.setCursor(0,2); lcd.print("Temp. 2: ");
      lcd.print(s2f, 1);  lcd.print(" F  ");
      lcd.setCursor(0,3); lcd.print("2: Sair             ");
      break;

    case 4:
      lcd.setCursor(0,0); lcd.print("ENDERECOS           ");
      lcd.setCursor(0,1); lcd.print("S1:");
      for (int i = 0; i < 8; i++){
        if (S1[i] < 16) lcd.print('0');
        lcd.print(S1[i], HEX);
      }

      lcd.setCursor(0,2); lcd.print("S2:");
      for (int i = 0; i < 8; i++){
        if (S2[i] < 16) lcd.print('0');
        lcd.print(S2[i], HEX);
      }

      lcd.setCursor(0,3); lcd.print("2: Sair             ");
      break;
  }

  telaAnterior = tela;

  delay(20);
}