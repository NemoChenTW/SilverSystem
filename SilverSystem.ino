#include <DHT.h>
#include <Wire.h>  // Arduino IDE 內建
#include <LiquidCrystal_I2C.h>

#define DHTPIN 2 //定義訊號要從PIN2 進來
DHT dht(DHTPIN, DHT22);

// I2C LCD 設定 
LiquidCrystal_I2C lcd(0x3F, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);  // 設定 LCD I2C 位址

const char CurTemptureMessage[] PROGMEM = {"Cur: "};
const char HelloMessage[] PROGMEM = {"Silver System!"};
const char SerialTitleMessage[] PROGMEM = {"Humidity and temperature\n\n"};
const char DHTErrorMessage[] PROGMEM = {"Failed to read from DHT sensor!"};
const char RelayOpenMessage[] PROGMEM = {"Relay Open"};
const char RelayCloseMessage[] PROGMEM = {"Relay Close"};
const char TemptureSettingMark[] PROGMEM = {" ~ "};
const char OnMessage[] PROGMEM = {"ON "};
const char OffMessage[] PROGMEM = {"OFF"};

int relay1 = 4; //設定繼電器1為第4孔

#define LOWTEMP 23.5
#define HIGHTEMP 26.5

// Relay Status
bool isRelayOpen = false;
uint8_t  RelayOpenCounter = 0;
float readTempt = 0.0;
char chBuffer;
uint8_t i = 0; // for counter

void printPGM_lcd(const char *data)
{
  int len = strlen_P(data);

  for(i = 0; i < len; i++)
  {
    chBuffer = pgm_read_byte_near(data + i);
    lcd.print(chBuffer);
  }
}

void printPGM_serial(const char *data)
{
  int len = strlen_P(data);

  for(i = 0; i < len; i++)
  {
    chBuffer = pgm_read_byte_near(data + i);
    Serial.print(chBuffer);
  }
  Serial.print("\n");
}

void printCounter()
{
  lcd.setCursor(0, 1); // 設定游標位置在第二行第0位
  lcd.print(RelayOpenCounter);
}

void printLCDTemptureSetting()
{
  lcd.setCursor(3, 1); // 設定游標位置在第二行第4位
  lcd.print(LOWTEMP);
  
  printPGM_lcd(TemptureSettingMark);

  lcd.print(HIGHTEMP);
}

void setup() {
  
  lcd.begin(16, 2);      // 初始化 LCD，一行 16 的字元，共 2 行，預設開啟背光
  lcd.backlight(); // 開啟背光
  lcd.setCursor(0, 0); // 設定游標位置在第一行行首
  printPGM_lcd(HelloMessage);
  
  //   DHT22
  Serial.begin(9600);
  delay(300);             //Let system settle
  
  #if defined(SERIAL_DEBUG_OUTPUT)
  printPGM_serial(SerialTitleMessage);
  #endif

  delay(700);             //Wait rest of 1000ms recommended delay before
  //accessing sensor
  dht.begin();

  // 二路繼電器

  pinMode(relay1, OUTPUT);         //設定繼電器1为输出

  delay(1000);
  lcd.clear();

  lcd.setCursor(5, 0); // 設定游標位置在第一行第5位
  printPGM_lcd(CurTemptureMessage);
  
  printLCDTemptureSetting();

  // 繼電器預設關閉
  isRelayOpen = false;
  digitalWrite(relay1, LOW);          //繼電器1開關斷開
  
  #if defined(SERIAL_DEBUG_OUTPUT)
  printPGM_serial(RelayCloseMessage);
  #endif

  lcd.setCursor(0, 0); // 設定游標位置在第一行第0位
  printPGM_lcd(OffMessage);

  printCounter();
}

void loop() {
  // Wait a few seconds between measurements.
  delay(2000);

  // Read temperature as Celsius (the default)
  readTempt = dht.readTemperature();

  // Check if any reads failed and exit early (to try again).
  if (isnan(readTempt)) {
    
    #if defined(SERIAL_DEBUG_OUTPUT)
    printPGM_serial(DHTErrorMessage);
    #endif

    return;
  }

  //  String
  
  #if defined(SERIAL_DEBUG_OUTPUT)
  Serial.println(readTempt);
  #endif

  lcd.setCursor(10, 0); // 設定游標位置在第一行第10位
  lcd.print(readTempt); 

  delay(300);
  
  if (readTempt <= LOWTEMP)
  {
    if(isRelayOpen == false)
    {
      digitalWrite(relay1, HIGH);          //繼電器1開關導通
      isRelayOpen = true;
      
      
      #if defined(SERIAL_DEBUG_OUTPUT)
      printPGM_serial(RelayOpenMessage);
      #endif

      lcd.setCursor(0, 0); // 設定游標位置在第一行第0位
      printPGM_lcd(OnMessage);

      delay(200);
      RelayOpenCounter++;      
      printCounter();
    }
  }
  else if (readTempt > HIGHTEMP)
  {
    if(isRelayOpen == true)
    {
      digitalWrite(relay1, LOW);          //繼電器1開關斷開
      isRelayOpen = false;
      
      
      #if defined(SERIAL_DEBUG_OUTPUT)
      printPGM_serial(RelayCloseMessage);
      #endif

      lcd.setCursor(0, 0); // 設定游標位置在第一行第0位
      printPGM_lcd(OffMessage);
    }
  }
}



