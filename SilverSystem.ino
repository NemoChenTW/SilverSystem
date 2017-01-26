#include <DHT.h>
#include <Wire.h>  // Arduino IDE 內建
#include <LiquidCrystal_I2C.h>

#define DHTPIN 2 //定義訊號要從PIN2 進來
DHT dht(DHTPIN, DHT22);

// I2C LCD 設定 
LiquidCrystal_I2C lcd(0x3F, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);  // 設定 LCD I2C 位址

char temptureHeaderString[6] = "Cur: ";
int relay1 = 4; //設定繼電器1為第4孔

#define LOWTEMP 23.5
#define HIGHTEMP 26.5

// Relay Status
bool isRelayOpen = false;
int  RelayOpenCounter = 0;


String getTemptureStr(float tempture)
{
  String retStr = String("") + temptureHeaderString + tempture;
  return retStr;
}

String getCounterStr(int counter)
{
  char numStr[4];

  if(counter == 0)
    sprintf(numStr, "%2d", counter);
  else
    sprintf(numStr, "%02d", counter);
  
  String retStr = String("") + numStr;
  return retStr;
}

void printCounter()
{
  lcd.setCursor(0, 1); // 設定游標位置在第二行第0位
  lcd.print(getCounterStr(RelayOpenCounter));
}

void setup() {
  
  lcd.begin(16, 2);      // 初始化 LCD，一行 16 的字元，共 2 行，預設開啟背光
  lcd.backlight(); // 開啟背光
  lcd.setCursor(0, 0); // 設定游標位置在第一行行首
  lcd.print("Silver System!");
  
  //   DHT22
  Serial.begin(9600);
  delay(300);             //Let system settle
  Serial.println("Humidity and temperature\n\n");
  delay(700);             //Wait rest of 1000ms recommended delay before
  //accessing sensor
  dht.begin();

  // 二路繼電器

  pinMode(relay1, OUTPUT);         //設定繼電器1为输出

  delay(1000);
  lcd.clear();

  lcd.setCursor(5, 0); // 設定游標位置在第一行第5位
  lcd.print(temptureHeaderString);
  
  lcd.setCursor(3, 1); // 設定游標位置在第二行第3位
  String tempSetStr = String("") + LOWTEMP + " ~ " + HIGHTEMP;
  lcd.print(tempSetStr);

  // 繼電器預設關閉
  isRelayOpen = false;
  digitalWrite(relay1, LOW);          //繼電器1開關斷開
  Serial.println("Relay Off");
  lcd.setCursor(0, 0); // 設定游標位置在第一行第0位
  lcd.print("OFF");

  printCounter();
}

void loop() {
  // Wait a few seconds between measurements.
  delay(2000);

  // Read temperature as Celsius (the default)
  float t = dht.readTemperature();

  // Check if any reads failed and exit early (to try again).
  if (isnan(t)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  //  String
  Serial.println(getTemptureStr(t));
  lcd.setCursor(10, 0); // 設定游標位置在第一行第10位
  lcd.print(t); 

  delay(300);
  
  if (t <= LOWTEMP)
  {
    digitalWrite(relay1, HIGH);          //繼電器1開關導通
    Serial.println("Relay Open");
    lcd.setCursor(0, 0); // 設定游標位置在第一行第0位
    lcd.print("ON ");

    if(isRelayOpen == false)
    {
      isRelayOpen = true;
      
      RelayOpenCounter++;      
      printCounter();
    }
  }
  else if (t > HIGHTEMP)
  {
    digitalWrite(relay1, LOW);          //繼電器1開關斷開
    Serial.println("Relay Off");
    lcd.setCursor(0, 0); // 設定游標位置在第一行第0位
    lcd.print("OFF");

    if(isRelayOpen == true)
    {
      isRelayOpen = false;
    }
  }
}



