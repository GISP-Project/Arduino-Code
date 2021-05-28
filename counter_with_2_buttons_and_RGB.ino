#include <LiquidCrystal.h>

//pin buttons
const int buttonUp = 13;
const int buttonDown = 12;

//pin led RGB
const int redPin = 11;
const int bluePin = 10;
const int greenPin = 9;

//pin display LCD
const int rs = 2;
const int en = 7;
const int d4 = 6;
const int d5 = 5;
const int d6 = 4;
const int d7 = 3;

LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

int numPeople = 0;
const int maxPeople = 5;

int buttonUpState = 0;
int buttonUpPrevState = 0;
int buttonDownState = 0;
int buttonDownPrevState = 0;

bool buttonPressed = false;

void setup()
{
  Serial.begin(9600);
  
  pinMode(buttonUp, INPUT);
  pinMode(buttonDown, INPUT);
  
  pinMode(redPin, OUTPUT);
  pinMode(greenPin, OUTPUT);
  pinMode(bluePin, OUTPUT);
  
  lcd.begin(16,2);
  lcd.setCursor(0,0); 
  lcd.print("Num. persone: ");
  lcd.setCursor(15,0);
  lcd.print(numPeople);
  lcd.setCursor(0,1);
  lcd.print("Max persone: ");
  lcd.setCursor(15,1);
  lcd.print(maxPeople);
  
}

void loop(){
  
  if(numPeople<maxPeople)
    color(0,255,0); //green
  
  
  if(numPeople==maxPeople)
    color(255,0,0); //red
  
  
  //checkUp()
   buttonUpState = digitalRead(buttonUp);
   if(buttonUpState != buttonUpPrevState){
    if(buttonUpState == HIGH){ //HIGH
      buttonPressed = true;
      if(numPeople<maxPeople)numPeople++;
      }
    delay(250);
  	}
   buttonUpPrevState = buttonUpState;
   
  
  //checkDown()
   buttonDownState = digitalRead(buttonDown);
   if(buttonDownState != buttonDownPrevState){
     if(buttonDownState == HIGH){ 
      	buttonPressed = true;
       	if(numPeople > 0) numPeople--;
     }
    delay(250);
   }
    buttonDownPrevState = buttonDownState;
  
  if(buttonPressed){
    lcd.setCursor(15,0);
    lcd.print(numPeople);
    buttonPressed = false;
  }
  
}

void color(int red, int green, int blue){
  analogWrite(redPin, red);
  analogWrite(greenPin, green);
  analogWrite(bluePin, blue);
}              
