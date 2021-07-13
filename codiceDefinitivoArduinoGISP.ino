#include <LiquidCrystal.h>
#include <WiFi.h>
#include <HTTPClient.h>

String link_string ;
const long interval = 90000;
unsigned long previousMillis =0;
 
//pin buttons
const int buttonUp = 18;
const int buttonDown = 17;

//pin LEDs
const int redPin = 26;
const int greenPin = 25;

//pin display LCD
const int rs = 19;
const int en = 23;
const int d4 = 5;
const int d5 = 13;
const int d6 = 12;
const int d7 = 14;

LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

int numPeople = 0;
const int maxPeople = 10;


int buttonUpState = 0;
int buttonUpPrevState = 0;
int buttonDownState = 0;
int buttonDownPrevState = 0;

bool buttonPressed = false;


//set il wifi
//nome wifi
const char* ssid="InserireNomeWiFi";
//password wifi
const char* password="InserirePassword";
//chiave delle nostre API thingspeak
String api_key="C8EOELR4TOE5HMUC";

void setup()
{
  
  Serial.begin(115200);
//connessione al wifi
    WiFi.begin(ssid, password);
    while(WiFi.status() !=WL_CONNECTED){
      delay(1000);
      Serial.println("Connecting to Wifi...");
    }

    Serial.println("connected to the wifi nework");
//fine connessione al wifi
  
  pinMode(buttonUp, INPUT);
  pinMode(buttonDown, INPUT);
  
  pinMode(redPin, OUTPUT);
  pinMode(greenPin, OUTPUT);
  
  lcd.begin(16,2);
  lcd.setCursor(0,0); 
  lcd.print("Num.Persone:");
  lcd.setCursor(14,0);
  lcd.print(numPeople);
  lcd.setCursor(0,1);
  lcd.print("Max.Persone:");
  lcd.setCursor(14,1);
  lcd.print(maxPeople);
  
}

void loop(){
  
  if(numPeople<maxPeople){
    digitalWrite(greenPin,HIGH);
    digitalWrite(redPin, LOW);
  }
  
  if(numPeople==maxPeople){
    digitalWrite(greenPin,LOW);
    digitalWrite(redPin,HIGH);
  }
  
  //checkUp()
   buttonUpState = digitalRead(buttonUp);
   if(buttonUpState != buttonUpPrevState){
    if(buttonUpState == HIGH){ //HIGH
      buttonPressed = true;
      if(numPeople<maxPeople){
        numPeople++;
            if((WiFi.status()==WL_CONNECTED)){
                HTTPClient http;
                link_string = String("https://api.thingspeak.com/update?api_key="+String(api_key)+"&field1="+numPeople);
                http.begin(link_string);
                int httpCode = http.GET();
                if(httpCode > 0){
                  String payload = http.getString();
              }else {
            Serial.println("Error on HTTP request");
            }
          }
        }
      }
      
    delay(25);
    }
   buttonUpPrevState = buttonUpState;
   
   
  
  //checkDown()
   buttonDownState = digitalRead(buttonDown);
   if(buttonDownState != buttonDownPrevState){
     if(buttonDownState == HIGH){ 
        buttonPressed = true;
        if(numPeople > 0){
          numPeople--;
           if((WiFi.status()==WL_CONNECTED)){
                HTTPClient http;
                link_string = String("https://api.thingspeak.com/update?api_key="+String(api_key)+"&field1="+numPeople);
                http.begin(link_string);
                int httpCode = http.GET();
                if(httpCode > 0){
                  String payload = http.getString();
              }else {
            Serial.println("Error on HTTP request");
            }
          }
        }
     }
    delay(25);
   }
    buttonDownPrevState = buttonDownState;
  
  if(buttonPressed){
    lcd.setCursor(15,0);
    lcd.print(numPeople);
    buttonPressed = false;
  }
 unsigned long currentMillis = millis();
   
  if (currentMillis - previousMillis >= interval) {
    // registra ultimo momento
    previousMillis = currentMillis;
    
  if((WiFi.status()==WL_CONNECTED)){
    HTTPClient http;
    link_string = String("https://api.thingspeak.com/update?api_key="+String(api_key)+"&field1="+numPeople);
    http.begin(link_string);
    int httpCode = http.GET();
    if(httpCode > 0){
      String payload = http.getString();
      }else {
        Serial.println("Error on HTTP request");
        }
    }
  }

  
}
