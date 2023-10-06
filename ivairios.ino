unsigned long sukimosi_praejo = 0; 

int judejimo_state = LOW;            
long judejimo_praejo = 0;        
long off_time = 1400;
int on_time = 100;
int off_time1=off_time;

// Taisono taktikos laikai 
long off_timet = 1500;
int on_timet = 1500;
int off_time1t=off_timet;

void pasisukimas(long sukimosi_laikas, bool puse){
  unsigned long laikas = millis();
  sukimosi_praejo = laikas;
  
  while(true){
     pilnas_speed = false;
    laikas = millis();
  if (laikas - sukimosi_praejo >= sukimosi_laikas) {
    sukimosi_praejo = laikas;
    break;
  }
   sharp_sum = PINC&0B111;
    if(sharp_sum == 0b101){
      if(puse == 0){ motor(SPEED_ATTACK,0); delay(20);}
      else{ motor(0,SPEED_ATTACK); delay(20);}
     // motor(0,0);
      break;
    }
    
    if( puse == 0){  motor(-SPEED_ATTACK, SPEED_ATTACK); }
    else { motor(SPEED_ATTACK, -SPEED_ATTACK); }
    
  }
}
void isibegejimas(int start_speed, int end_speed, int delay_time){
  for(int i = start_speed; i<=end_speed; i++){
    motor(i,i);
    delay(delay_time);
  }
  pilnas_speed = true;
}

void taktika_3kg(){
  while(true){
  unsigned long laikas_3kg = millis();
  
  line_sum = PIND & 0b1100;
  sharp_sum = PINC&0B1111;
  
  if(laikas_3kg - judejimo_praejo > off_time) {
    judejimo_praejo = laikas_3kg;
    
    if(judejimo_state==LOW){
    judejimo_state = HIGH;
    off_time=on_time; 
    motor(SPEED_NORMAL,SPEED_NORMAL);
    }
    else{
     judejimo_state = LOW;
     off_time=off_time1;
     motor(0,0);
    }
   }
   
  switch(sharp_sum) {
    case 0b0111:  RUN_STATE = 9; break;    // galas
    case 0b0101:  RUN_STATE = 3; break;    // galas ir centras
    case 0b0110:  RUN_STATE = 9; break;    // galas ir desine
    case 0b0011:  RUN_STATE = 9; break;    // galas ir kaire
    case 0b1110:  RUN_STATE = 5; break;    // desine
    case 0b1100:  RUN_STATE = 5; break;    // desine ir centras
    case 0b1101:  RUN_STATE = 3; break;    // centras
    case 0b1011:  RUN_STATE = 1; break;    // kaire
    case 0b1001:  RUN_STATE = 1; break;    // kaire ir centras
    case 0b1111:  case 0b0000: case 0b1000:  case 0b0100: case 0b0010: case 0b0001:
                     switch(line_sum) {
                           case 0b1000: RUN_STATE = 6; break;
                           case 0b0100: RUN_STATE = 7; break;
                           case 0b1100: RUN_STATE = 8; break;
                           case 0b0000: RUN_STATE = 0; break;
                      }break;
   }
   if(RUN_STATE != 0){ motor(0,0);  break; }
   if(reikalingas_pultas == true){ STOPAS(); }
   delay(10);
 }
}
void skersas(bool puse, int laikelis){
  unsigned long laikas = millis();
  sukimosi_praejo = laikas;
  
   while(true){
      readSensors();
       if (laikas - sukimosi_praejo >= laikelis) {
          sukimosi_praejo = laikas;
         break;
       }
      if(puse == 0){ motor(200,100);delay(1);}
      else{ motor(100,200);delay(1);}
      
     if(RUN_STATE != 0){ motor(0,0);  break; }
     if(reikalingas_pultas == true){ STOPAS(); }
   }
}
int senas_kampas = -90;
void veliava(int a)
{   
  if(senas_kampas != a){
    a = constrain(a, 0, 180);
    int t = map(a,0,180,450,2000);
     
    digitalWrite(flag_pin, HIGH);
    delayMicroseconds(t);
    digitalWrite(flag_pin, LOW);
  }
}

void taisono_taktika(){
  bool judeti = false;
  int spk = 100;
  int spd = 100;
  while(true){
  unsigned long laikas_3kg = millis();
  
  line_sum = PIND & 0b1100;
  sharp_sum = PINC&0B1111;
  
  if(laikas_3kg - judejimo_praejo >=1000) {
    judejimo_praejo = laikas_3kg;
    
   if(judeti==true){
      spk = 70;
      spd = 20;
     if(naudoti_veliava == 1){ veliava(180); }
    }
    else{
      spk = 20;
      spd = 70;
      if(naudoti_veliava == 1){ veliava(0); }
    }
    judeti= !judeti;
  }

   motor(spk,spd);
   
  switch(sharp_sum) {
    case 0b0111:  RUN_STATE = 9; break;    // galas
    case 0b0101:  RUN_STATE = 3; break;    // galas ir centras
    case 0b0110:  RUN_STATE = 9; break;    // galas ir desine
    case 0b0011:  RUN_STATE = 9; break;    // galas ir kaire
    case 0b1110:  RUN_STATE = 5; break;    // desine
    case 0b1100:  RUN_STATE = 5; break;    // desine ir centras
    case 0b1101:  RUN_STATE = 3; break;    // centras
    case 0b1011:  RUN_STATE = 1; break;    // kaire
    case 0b1001:  RUN_STATE = 1; break;    // kaire ir centras
    case 0b1111:  case 0b0000: case 0b1000:  case 0b0100: case 0b0010: case 0b0001:
                     switch(line_sum) {
                           case 0b1000: RUN_STATE = 6; break;
                           case 0b0100: RUN_STATE = 7; break;
                           case 0b1100: RUN_STATE = 8; break;
                           case 0b0000: RUN_STATE = 0; break;
                      }break;
   }
   if(RUN_STATE != 0){ motor(0,0);  break; }
   if(reikalingas_pultas == true){ STOPAS(); }
   delay(10);
 }
}


