//pin LED
const int pinGreen = 26;
// const int pinRed = ; //TO ADD

//pin sensor 1
const int pin_trig1 = 19;
const int pin_echo1 = 18;

//pin sensor 2
const int pin_trig2 = 5;
const int pin_echo2 = 23;

long curr_time;
long time1,time2; //return time ultrasound signal (us)
long tmax1,tmax2; //max return time (us)
const long timeTrans = 10000; //max time (ms) between the activation of both sensors to consider that the transition has taken place.
                              //If sensor2 is not activated within time limit, the activation of sensor1 is ignored.

long tpass = 0; //time elapsed (ms) since passing over one of the sensors
int cons = 2; //at least 2 consecutive loops to be considered valid activation
int cont1=1, cont2=1; //counter consecutive loops
int flag = 0; //0:no passage, 1:passage on sensor1, 2:passage on sensor2

//counting variables -> to the db
int numPeople = 0;
const int maxPeople = 5;

void setup() {
    Serial.begin(115200);
    Serial.println( "Start");

    pinMode(pinGreen, OUTPUT); 
  //pinMode(pinRed, OUTPUT);   //TO ADD
    
    pinMode(pin_trig1, OUTPUT);
    pinMode(pin_echo1, INPUT);

    pinMode(pin_trig2, OUTPUT);
    pinMode(pin_echo2, INPUT);

  //Sensor Calibration
  //Takes place with free passage. Maximum distance(range between 2 cm and 4 m)
  digitalWrite(pin_trig1, LOW); 
  digitalWrite(pin_trig2, LOW);

      for(int i=0;i<10;i++){ //10 distance measurements, we take minimum return times

      //read sensor1 distance
      digitalWrite(pin_trig1, LOW);
      delayMicroseconds(2);
      digitalWrite(pin_trig1, HIGH); //send impulse
      delayMicroseconds(10); //trig signal must be high for at least 10 us
      digitalWrite(pin_trig1, LOW); 
      time1 = pulseIn(pin_echo1, HIGH); //return time measured by echo pin
      
      //read sensor2 distance
      digitalWrite(pin_trig2, LOW);
      delayMicroseconds(2);
      digitalWrite(pin_trig2, HIGH);
      delayMicroseconds(10);
      digitalWrite(pin_trig2, LOW);
      time2 = pulseIn(pin_echo2, HIGH);

        if(i==0){ //variables initialization (first measurement)
          tmax1 = time1;
          tmax2 = time2;
        }
        else{
          if(time1<tmax1){ //at subsequent iterations, time variables are  overwritten only if a shorter return time has been measured
             tmax1=time1;
            }
        else tmax2=time2;
        }
    }

  Serial.println("Calibration done!"); 
  Serial.print("Max distance sensors (cm):sens1= ");
  Serial.print(tmax1 * 0.034/2); //speed of sound in air: 343 m/s about
  Serial.print(" /sens2= ");
  Serial.println(tmax2 * 0.034/2);
  
  tmax1-=3000;
  tmax2-=3000;

}

void loop() {

    if(numPeople < maxPeople){
      digitalWrite(pinGreen,HIGH);
    //digitalWrite(pinRed, LOW);
    }

    if(numPeople == maxPeople){
      digitalWrite(pinGreen, LOW);
      //digitalWrite(pinRed, HIGH);
    }

    //read sensor1 distance
    digitalWrite(pin_trig1, LOW);
    delayMicroseconds(2);
    digitalWrite(pin_trig1, HIGH);
    delayMicroseconds(10);
    digitalWrite(pin_trig1, LOW);
    time1 = pulseIn(pin_echo1, HIGH);
    
    //read sensor2 distance
    digitalWrite(pin_trig2, LOW);
    delayMicroseconds(2);
    digitalWrite(pin_trig2, HIGH);
    delayMicroseconds(10);
    digitalWrite(pin_trig2, LOW);
    time2 = pulseIn(pin_echo2, HIGH);
    
  //long distance1 = 0.034 * time1/2;
  //long distance2 = 0.034 * time2/2;

    if(time1<tmax1){ //recorded passage on sensor1
      cont1++;
    }else cont1=1; 

    if(time2<tmax2){ //recorded passage on sensor2
      cont2++;
    }else cont2=1; 

    
    if(flag == 0){
      if(cont1 == cons){ //passage on sensor1
      Serial.println("Passage on sensor 1");
      flag = 1;
      tpass = millis(); //save current time
      }else if(cont2 == cons){
        Serial.println("Passage on sensor 2");
        flag = 2;
        tpass = millis();
        } 
     }
     
     else{ //flag != 0
        
        if((millis()-tpass)>timeTrans){ //too long since the activation of the other sensor
        Serial.println("No passage");
        flag = 0; 
        }else{
          if(flag == 1 && cont2 == cons){ 
            
          //RECORDED PASSAGE with direction sensor1->sensor2 - ENTRANCE
          Serial.println("Passage 1 -> 2");
          Serial.println(time1 * 0.034/2);
          Serial.println(time2 * 0.034/2);
          curr_time = millis();
          flag = 0;
          cont1 = 0; 
          cont2 = 0;
          numPeople++;
        }

        if(flag == 2 && cont1 == cons){ 
        //RECORDED PASSAGE with direction sensor2->sensor1 - EXIT
        Serial.println("Passage 2 -> 1");
        Serial.println(time1 * 0.034/2);
        Serial.println(time2 * 0.034/2);
        curr_time = millis();
        flag = 0;
        cont1 = 0;
        cont2 = 0;
        if(numPeople>0) numPeople--;
        }
      }
    }

}
