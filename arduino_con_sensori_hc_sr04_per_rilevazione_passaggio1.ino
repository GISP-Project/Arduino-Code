#include <LiquidCrystal.h>

//pin sensore 1
const int pin_trig1 = 11; 
const int pin_echo1 = 10; 

//pin sensore 2
const int pin_trig2 = 13; 
const int pin_echo2 = 12; 

//pin led RGB -> acceso quando basso, pin Blu non collegato perchè non ci serve
const int redPin = 9;
const int greenPin = 8;

//pin display LCD
const int rs = 6;
const int en = 5;
const int d4 = 4;
const int d5 = 3;
const int d6 = 2;
const int d7 = 7;

LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

long curr_time;
long time1,time2; // tempo ritorno segnale ultrasuoni (us)
long tmax1,tmax2; //tempo max ritorno segnale (us)
const long timeTrans = 10000; //tempo max (ms) tra l'attivazione di un sensore e di quello successivo
							  //per considerare avvenuto il passaggio. Se il secondo sensore non viene attivato entro il tempo limite,
							  //l'attivazione del primo viene ignorata
long tpass = 0; //tempo trascorso dal passaggio su uno dei sensori (ms)

int cons = 2; //almeno 2 loop consecutivi per essere considerata attivazione valida
int cont1=1, cont2=1; //contatore loop consecutivi

int flag = 0; //0:nessun passaggio, 1:passaggio sul sensore 1, 2:passaggio sul sensore 2

//variabili conteggio -> andranno nel db
int numPeople = 0;
const int maxPeople = 5;

void setup() {
 
	pinMode(pin_trig1, OUTPUT);
	pinMode(pin_echo1, INPUT);
	
 	pinMode(pin_trig2, OUTPUT);
	pinMode(pin_echo2, INPUT);
  
  	pinMode(redPin, OUTPUT);
  	pinMode(greenPin, OUTPUT);
  
  	lcd.begin(16, 2); //dim schermo
  	lcd.print("Benvenuto!");
  	
  
	Serial.begin(9600);
	Serial.println( "Start");
  
  
  //CALIBRAZIONE DEI SENSORI
  //Avviene con passaggio libero. Si ottiene distanza max (range tra 2 cm e 4 m)
  	digitalWrite(pin_trig1, LOW);  
  	digitalWrite(pin_trig2, LOW);
  
  for(int i=0;i<10;i++){  //vengono eseguite 10 misure di distanza, da cui si prendono i valori minimi dei tempi di ritorno

    //lettura distanza sensore 1
    digitalWrite(pin_trig1, LOW);
    delayMicroseconds(2);
    digitalWrite(pin_trig1, HIGH); //viene inviato l' impulso
    delayMicroseconds(10);          //perchè ciò avvenga il segnale di trig deve essere alto per almeno 10 us
    digitalWrite(pin_trig1, LOW);  
    time1 = pulseIn(pin_echo1, HIGH); //viene misurato il tempo di ritorno del segnale sonoro dal pin echo
   
    //lettura distanza sensore 2
    digitalWrite(pin_trig2, LOW);
    delayMicroseconds(2);
    digitalWrite(pin_trig2, HIGH);
    delayMicroseconds(10);
    digitalWrite(pin_trig2, LOW);
    time2 = pulseIn(pin_echo2, HIGH);

    if(i==0){ //inizializzazione variabili (prima misurazione)
      tmax1 = time1;
      tmax2 = time2;
    }
    else{
      if(time1<tmax1){  //alle iterazioni successive le variabili tempo vengono sovrascritte solo se è stato misurato un tempo di ritorno minore
        tmax1=time1;
      }  
      else
        tmax2=time2;
    }
  }
  
  Serial.println("Calibrazione effettuata!");
  Serial.print("Distanza max sensori (cm):sens1= ");
  Serial.print(tmax1 * 0.034/2); //velocita suono nell'aria: 343 m/s circa
  Serial.print(" /sens2= ");
  Serial.println(tmax2 * 0.034/2);
  
  	tmax1-=3000;
  	tmax2-=300;
}

 
void loop() {
  	
  	lcd.clear();
  	lcd.setCursor(0,0); 
  	lcd.print("Num. persone: ");
  	lcd.setCursor(15,0);
  	lcd.print(numPeople);
  	lcd.setCursor(0,1);
  	lcd.print("Max persone: ");
  	lcd.setCursor(15,1);
  	lcd.print(maxPeople);
  
  	if(numPeople < maxPeople){
    	digitalWrite(greenPin,LOW);
  		delay(500);
  		digitalWrite(greenPin,HIGH);
  	}
  
  	if(numPeople == maxPeople){
      digitalWrite(redPin, LOW);
  	  delay(500);
  	  digitalWrite(redPin, HIGH);
  	}
  	
  
  	
  
 	//lettura distanza dal sensore 1
	digitalWrite(pin_trig1, LOW); 
	delayMicroseconds(2);
	digitalWrite(pin_trig1, HIGH);
	delayMicroseconds(10);
	digitalWrite(pin_trig1, LOW);
  	time1 = pulseIn(pin_echo1, HIGH); 
  
  	//lettura distanza dal sensore 2
	digitalWrite(pin_trig2, LOW); 
	delayMicroseconds(2);
	digitalWrite(pin_trig2, HIGH); 
	delayMicroseconds(10);
	digitalWrite(pin_trig2, LOW);
   	time2 = pulseIn(pin_echo2, HIGH);
  
  
	//long distance1 = 0.034 * time1/2;
  	//long distance2 = 0.034 * time2/2;
 
 	if(time1<tmax1){ //registrato passaggio su sensore 1
        cont1++;
      }else cont1=1;  
      
	if(time2<tmax2){
        cont2++;
      }else cont2=1;  
       

    if(flag == 0){
      if(cont1 == cons){ //registrato passaggio su sensore 1
        //Serial.println("Passaggio sensore 1");
        flag = 1;
        tpass = millis(); //salvato il tempo corrente
      }else if(cont2 == cons){
        //Serial.println("Passaggio sensore 2");
        flag = 2;
        tpass = millis();
      } 
    }else{
      if((millis()-tpass)>timeTrans){  //troppo tempo transcorso senza l' attivazione dell' altro sensore
        Serial.println("Passaggio annullato");
        flag = 0;  
      }else{
        if(flag == 1 && cont2 == cons){  
          //PASSAGGIO AVVENUTO con direzione sensore1->sensore2 - ENTRATA
          Serial.println("Passaggio 1 -> 2");
          //Serial.println(time1 * 0.034/2);
          //Serial.println(time2 * 0.034/2);
          curr_time = millis();
          flag = 0;
          cont1 = 0; 
          cont2 = 0;
          numPeople++;
        }
        
        if(flag == 2 && cont1 == cons){  
          //PASSAGGIO AVVENUTO con direzione sensore2->sensore1 - USCITA
          Serial.println("Passaggio 2 -> 1");
          //Serial.println(tempo1 * 0.034/2);
          //Serial.println(tempo2 * 0.034/2);
          curr_time = millis();
          flag = 0;
          cont1 = 0;
          cont2 = 0;
          if(numPeople>0) numPeople--;
        }
      }
    }

}
