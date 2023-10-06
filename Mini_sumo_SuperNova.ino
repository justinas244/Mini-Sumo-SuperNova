#include <IRremote.h>
#include <Wire.h>
const int RECV_PIN=4;
IRrecv irrecv(RECV_PIN);
decode_results results;

int flag_pin = 5;


int programa = 0;
bool startas = false;
bool Stopas = false;
bool reikalingas_pultas = true;

bool pilnas_speed = false;
const byte program_led = 13;
//const byte led = 13;
//LINE begin
const byte LINE_LEFT = 3;
const byte LINE_RIGHT = 2;
//LINE end

//FRONT SENSOR begin
const byte FRONT_LEFT = A0;
const byte FRONT_CENTER = A1;
const byte FRONT_RIGHT = A2;
const byte BACK = A3;
//FRONT SENSOR end

//START MODULE begin
const byte START_MODULE = A5;
//START MODULE end

//SENSORS SUMS
byte line_sum = 0;
byte sharp_sum = 0;

//MOTOR CONTROL VARIABLES
double INTEGRAL = 0;
int V = 0;

//MOTORS begin

int pwm_a = 9;
int ain2  = 7;
int ain1  = 8;

int pwm_b = 10;
int bin2  = 12;
int bin1  = 11;
//MOTORS end

//VALDYMO KINTAMIEJI begin
const byte SPEED_STOP = 0;
 byte SPEED_NORMAL = 70;//80
 byte Default_speed = 70;//80
const byte SPEED_ATTACK2 = 180;//180
const byte SPEED_ATTACK = 255;//255

 byte SPEED_PUOLIMO = 255;//255

const byte SPEED_GO_BACK_LINE = 255;//255
const byte SPEED_GO_BACK_LINE_TURN = 170;//200
const byte SPEED_GO_BACK_LINE_TURN2 = 170;//200

const unsigned short TIME_FOR_GO_BACK = 55;
const unsigned short TIME_FOR_TURNING = 115;
const unsigned short TIME_FOR_TURNING_BOTH = 130;
unsigned short TURNING_TIME = TIME_FOR_TURNING;
unsigned long LINE_TIME = 2;

const unsigned short Matau_priesa = 3000;

//STATES
byte RUN_STATE = 0;
byte LINE_AVOID_STATE = 0;
byte LAST_SEEN = 0;
byte LAST_STATE = 0;
byte STRATEGY_STATE = 0;

//STATE TIMERS
unsigned long LAST_SEEN_TIME = 0;
unsigned long STRATEGY_START_TIME = 0;
unsigned long TIME_FOR_STRATEGY = 0;
unsigned long Matymas = 0;

unsigned long bejudesio = 0;
unsigned long greicio_perjungimo_interval = 5000;

unsigned long TIME_FOR_ATACK_SET = 0;
unsigned long TIME = 0;

int naudoti_veliava = 1;
int flag_puse = 0;     // 0- kaire 1-vidurys 2-desine
int mygtuko_pin = 6;
bool buvo_priesas = false;
void setup() {
  pinMode(RECV_PIN,INPUT_PULLUP);
  Serial.begin(9600);
  irrecv.enableIRIn(); // Start the receiver
  irrecv.blink13(true);
  pinMode(flag_pin,OUTPUT);
  pinMode(mygtuko_pin,INPUT_PULLUP);
  //myservo.attach(5);
  //myservo.writeMicroseconds(1500);  // set servo to mid-point
  pinMode(LINE_LEFT, INPUT);
  pinMode(LINE_RIGHT, INPUT);
  
  pinMode(FRONT_LEFT, INPUT_PULLUP);
  pinMode(FRONT_CENTER, INPUT_PULLUP);
  pinMode(FRONT_RIGHT, INPUT_PULLUP);
  pinMode(BACK, INPUT_PULLUP);
  
  pinMode(START_MODULE, INPUT);
  pinMode(program_led,OUTPUT);
 // pinMode(led,OUTPUT);
  
    pinMode(pwm_a,OUTPUT);
    pinMode(ain2,OUTPUT);
    pinMode(ain1,OUTPUT);
    pinMode(pwm_b,OUTPUT);
    pinMode(bin2,OUTPUT);
    pinMode(bin1,OUTPUT);
   pinMode(START_MODULE, INPUT);

  motor(0,0);
 
  /*veliava(0);
  delay(2000);*/
  veliava(90);
  while(true){
    pulto_nuskaitymas();
   if(digitalRead(START_MODULE)){ reikalingas_pultas = false;  break; }
    if(startas == true){
        pinMode(START_MODULE,OUTPUT);
        delay(2);
        digitalWrite(START_MODULE,HIGH);
        digitalWrite(program_led, HIGH);
        delay(1);
        break;
       }
  }
  if(naudoti_veliava == 1){
      if(flag_puse == 0){ veliava(0);}
      else if(flag_puse == 1){ veliava(90);}
      else if(flag_puse == 2){ veliava(180);}
  }
  else{
    veliava(90);
  }
  
  STRATEGY_STATE = programa;
  STRATEGY_START_TIME = millis();
  bejudesio = millis();
}

void loop() {
  if (millis() - LINE_TIME > TURNING_TIME) LINE_AVOID_STATE = 0;
  if (millis() - STRATEGY_START_TIME > TIME_FOR_STRATEGY) STRATEGY_STATE = 0;
  if ((millis() - Matymas > Matau_priesa)&&buvo_priesas == true) { SPEED_NORMAL = 255; SPEED_PUOLIMO = 255;}
  if (millis() - bejudesio > greicio_perjungimo_interval && STRATEGY_STATE !=2 && STRATEGY_STATE !=4 && STRATEGY_STATE !=5 && STRATEGY_STATE !=6 && STRATEGY_STATE !=10){
    SPEED_NORMAL = 255;}
    else{
      SPEED_NORMAL  = Default_speed;
    }
  if(reikalingas_pultas == true){ STOPAS(); }
  if(digitalRead(START_MODULE) ==LOW){ STOPAS();}
  readSensors();
  
  switch (RUN_STATE) {
    case 0: switch(LINE_AVOID_STATE) {
              case 0: switch(STRATEGY_STATE) {
                        case 0: motor(SPEED_NORMAL, SPEED_NORMAL); break;

                        case 1: TIME_FOR_STRATEGY = 35;
                                motor(-SPEED_ATTACK, SPEED_ATTACK);
                                break;

                        case 2: SPEED_NORMAL = 30;
                                motor(SPEED_NORMAL, SPEED_NORMAL);
                                break;        
                                
                        case 3: TIME_FOR_STRATEGY = 35;
                                motor(SPEED_ATTACK,-SPEED_ATTACK);
                                break;
                                
                                
                        case 4: motor(-SPEED_ATTACK, SPEED_ATTACK);
                                delay(60);
                                motor(SPEED_ATTACK, SPEED_ATTACK);
                                delay(120);
                                motor(SPEED_ATTACK, -SPEED_ATTACK);
                                delay(120);
                                if(naudoti_veliava == 1){ veliava(0);}
                                taktika_3kg();
                                break; 
                                
                        case 5: if(naudoti_veliava == 1){ veliava(180);}
                                taktika_3kg();
                                break;  
                                
                        case 6: motor(SPEED_ATTACK, -SPEED_ATTACK);
                                delay(60);
                                motor(SPEED_ATTACK, SPEED_ATTACK);
                                delay(120);
                                motor(-SPEED_ATTACK, SPEED_ATTACK);
                                delay(120);
                                if(naudoti_veliava == 1){ veliava(180);}
                                taktika_3kg();
                                break;
                                
                        case 7: motor(-SPEED_ATTACK, SPEED_ATTACK);
                                delay(80);
                                motor(255, 80);
                                delay(400);
                                break; 
                               
                         case 8: TIME_FOR_STRATEGY = 200;
                                motor(SPEED_ATTACK, SPEED_ATTACK);
                               break; 
                               
                         case 9: motor(SPEED_ATTACK, -SPEED_ATTACK);
                                delay(80);
                                motor(90, 230);
                                delay(600);
                                break;
                                
                        case 10:taisono_taktika();
                                break; 
                                           
                      } break;
              case 1: motor(SPEED_GO_BACK_LINE_TURN, -SPEED_GO_BACK_LINE_TURN2); bejudesio=millis(); break;
              case 2: motor(-SPEED_GO_BACK_LINE_TURN2, SPEED_GO_BACK_LINE_TURN); bejudesio=millis(); break;
              case 3: switch(LAST_SEEN) {
                        case 0: motor(SPEED_GO_BACK_LINE_TURN, -SPEED_GO_BACK_LINE_TURN2); bejudesio=millis(); break;
                        case 1: motor(-SPEED_GO_BACK_LINE_TURN2, SPEED_GO_BACK_LINE_TURN); bejudesio=millis(); break;
                        case 2: motor(SPEED_GO_BACK_LINE_TURN, -SPEED_GO_BACK_LINE_TURN2); bejudesio=millis(); break;
                      } break;   
            } 
            INTEGRAL = 0;
            break;
            
    case 1: INTEGRAL = 0;
            LAST_STATE = 1;
            LAST_SEEN_TIME = millis();
            Matymas = millis();
            LAST_SEEN = 1;
            if(naudoti_veliava == 1){ veliava(180);}
            pasisukimas(120,0);
              Matymas = millis();
              SPEED_NORMAL = Default_speed;
              buvo_priesas = false;
            break;

    case 3: INTEGRAL = 0;
            LAST_SEEN_TIME = millis();
            LAST_STATE = 3;
            LAST_SEEN = 0;
            TIME_FOR_STRATEGY = 0;
            motor(SPEED_PUOLIMO,SPEED_PUOLIMO);
            buvo_priesas = true;
            break;
            
    case 5: INTEGRAL = 0;
            LAST_STATE = 5;
            LAST_SEEN_TIME = millis();
            LAST_SEEN = 2;
            if(naudoti_veliava == 1){ veliava(0);}
            pasisukimas(120,1);
              Matymas = millis();
              SPEED_NORMAL = Default_speed;
              buvo_priesas = false;
            break;
            
    case 6: motor(-SPEED_GO_BACK_LINE, -SPEED_GO_BACK_LINE);
            delay(TIME_FOR_GO_BACK);
            LINE_AVOID_STATE = 1;
            TIME_FOR_STRATEGY = 0;
            TURNING_TIME = TIME_FOR_TURNING;
            LINE_TIME = millis();
             Matymas = millis();
              SPEED_NORMAL = Default_speed;
              buvo_priesas = false;
            break;
    case 7: motor(-SPEED_GO_BACK_LINE, -SPEED_GO_BACK_LINE);
            delay(TIME_FOR_GO_BACK);
            LINE_AVOID_STATE = 2;
            TIME_FOR_STRATEGY = 0;
            TURNING_TIME = TIME_FOR_TURNING;
            LINE_TIME = millis();
            Matymas = millis();
              SPEED_NORMAL = Default_speed;
              buvo_priesas = false;
            break;
    case 8: motor(-SPEED_GO_BACK_LINE, -SPEED_GO_BACK_LINE);
            delay(TIME_FOR_GO_BACK);
            LINE_AVOID_STATE = 3;
            TURNING_TIME = TIME_FOR_TURNING_BOTH;
            TIME_FOR_STRATEGY = 0;
            LINE_TIME = millis();
            Matymas = millis();
              SPEED_NORMAL = Default_speed;
              buvo_priesas = false;
            break;

     case 9: INTEGRAL = 0;
             LAST_STATE = 5;
             LAST_SEEN_TIME = millis();
             //LAST_SEEN = 2;
             if(naudoti_veliava == 1){ veliava(0);}
             pasisukimas(200,1);
             if(buvo_priesas == false){
              Matymas = millis();
              SPEED_NORMAL = Default_speed;
             }
             break;
            
    case 10: INTEGRAL = 0;
            LAST_STATE = 5;
            LAST_SEEN_TIME = millis();
            //LAST_SEEN = 1;
            if(naudoti_veliava == 1){ veliava(180);}
            pasisukimas(200,0);
            if(buvo_priesas == false){
              Matymas = millis();
              SPEED_NORMAL = Default_speed;
            }
            break;
  }
}
void readSensors() {
  line_sum =  PIND&0b1100;
  sharp_sum = PINC&0B1111;
  //sharp_sum = 0;
 // Serial.println(sharp_sum, BIN);
   switch(sharp_sum) {
    case 0b0111:  
    if(buvo_priesas == false){
      if(LAST_SEEN == 1){ RUN_STATE = 10;} else{ RUN_STATE = 9;}    // galas
    }
    else{ RUN_STATE = 3; }       break;     //galas
    case 0b0101:  RUN_STATE = 3; break;    // galas ir centras
    case 0b0110:  RUN_STATE = 9; break;    // galas ir desine
    case 0b0011:  RUN_STATE = 10; break;    // galas ir kaire
    case 0b1110:  RUN_STATE = 5; break;    // desine
    case 0b1100:  RUN_STATE = 5; break;    // desine ir centras
    case 0b1101:  RUN_STATE = 3; break;    // centras
    case 0b1011:  RUN_STATE = 1; break;    // kaire
    case 0b1001:  RUN_STATE = 1; break;    // kaire ir centras
    case 0b1111:  case 0b0000: case 0b1000:  case 0b0100: case 0b0010:
                     switch(line_sum) {
                           case 0b1000: RUN_STATE = 6; break;
                           case 0b0100: RUN_STATE = 7; break;
                           case 0b1100: RUN_STATE = 8; break;
                           case 0b0000: RUN_STATE = 0; break;
                      }break;
   }
   delay(10);
}

void motor(int left, int right){ 
  right = constrain(right, -255, 255);
  left  = constrain(left, -255, 255);

  if(right>=0){
    analogWrite(pwm_b, right);
    digitalWrite(bin2,HIGH);
    digitalWrite(bin1,LOW);
  }
  else{
    analogWrite(pwm_b, right*(-1));
    digitalWrite(bin1,HIGH);
    digitalWrite(bin2,LOW);
  }
 if(left>=0){
    analogWrite(pwm_a, left);
    digitalWrite(ain2,HIGH);
    digitalWrite(ain1,LOW);
  }
  else{
    analogWrite(pwm_a, left*(-1));
    digitalWrite(ain1,HIGH);
    digitalWrite(ain2,LOW);
  }
}





