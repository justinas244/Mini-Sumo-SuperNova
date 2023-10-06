int kodas=0;
int start_kodas = 0x95;
int programos_rinkimasis = 0xE0;
int programos_uzdarymas = 0xE5;
int programos_check = 0xBF;
int stop_kodas = 0xBA;

int veliavos_atjungimas= 0x3BE0;
int veliavos_ijungimas= 0xB8;
int ledState = LOW;
unsigned long blyksejimo_praejo = 0; 
const long intervalas_blyksejimo = 50;
bool ledbusena= LOW;

void pulto_nuskaitymas(){
   int programos_mygtukas = digitalRead(mygtuko_pin);
   
  if (IrReceiver.decode()>0) {
    kodas = IrReceiver.decodedIRData.decodedRawData;
    Serial.println(kodas,HEX);
    if(kodas == start_kodas){
      Serial.println("STARTAS");
      startas = true;
      digitalWrite(program_led,HIGH);
    }
    else if( kodas == programos_check){
      Serial.println("Programos pasirinkimo check");
      check_program();
    }
    else if( kodas == veliavos_ijungimas){
      Serial.println("Veliava ijungta");
      naudoti_veliava = 1;
    }
    else if( kodas == veliavos_atjungimas){
      Serial.println("Veliava isjungta");
      naudoti_veliava = 0;
    }
    irrecv.resume(); // Receive the next value
    delay(10);
  }

  if(programos_mygtukas == LOW){
    disableLEDFeedback();
    digitalWrite(program_led,HIGH);
    programa++;
    delay(300);
    digitalWrite(program_led,LOW);
    delay(300);
    if(programa>10){ programa = 0; }
    programos();
  }
  
}

void check_program(){
  disableLEDFeedback();
  if(programa == 0){
    for(int i =0; i<5; i++){
      digitalWrite(program_led,HIGH);
      delay(100);
      digitalWrite(program_led,LOW);
     // pinMode(RECV_PIN,INPUT_PULLUP);
      delay(100);
    }
    if(naudoti_veliava == false){
        for(int i =0; i<3; i++){
        digitalWrite(program_led,HIGH);
        delay(1000);
        digitalWrite(program_led,LOW);
        delay(1000);
       }
    }
  }
  else{
    for(int i =0; i<programa; i++){
      digitalWrite(program_led,HIGH);
      delay(500);
      digitalWrite(program_led,LOW);
      delay(500);
    }
    if(naudoti_veliava == false){
      for(int i =0; i<3; i++){
      digitalWrite(program_led,HIGH);
      delay(1000);
      digitalWrite(program_led,LOW);
      delay(1000);
     }
    }
  }
  enableLEDFeedback();
}

void STOPAS(){
  if (IrReceiver.decode()) {
    kodas = IrReceiver.decodedIRData.decodedRawData;
   // Serial.println(kodas,HEX);
    if(kodas == stop_kodas){
      Serial.println("STOP");
      while(true){
       Stopas = true;
       digitalWrite(START_MODULE,LOW);
       digitalWrite(program_led,LOW);
       motor(0,0);
       veliava(90);
       delay(5000);
      }
    }
    irrecv.resume(); // Receive the next value
    //delay(100);
  }
}
void programos(){
   switch(programa){
    case 0: flag_puse = 1; break;
    case 1: flag_puse = 0; break;
    case 2: flag_puse = 0; break;
    case 3: flag_puse = 2; break;
    case 4: flag_puse = 1; break;
    case 5: flag_puse = 1; break;
    case 6: flag_puse = 1; break;
    case 7: flag_puse = 0; break;
    case 8: flag_puse = 2; break;
    case 9: flag_puse = 2; break;
    case 10: flag_puse = 1; break;
   }
}

