// encoder provides 20000 pulses per cycle;
// Diameter of ring mounted on encoder - 160 mm.  / 80 mm radius. 
// 2*pi*R - lenght of circle   actual lenght of circle = 502,4 mm (might we use 0.8 meter?) 
// per pulse = 25 um. 
const int32_t um_per_pulse = 25; 
const int32_t ms_vcom_update_rate = 1000; //one times a second

// pads at PCB - channel A - DIO8, channel B - DIO9, reference channel - DIO10; 
const byte channelA = 8;
const byte channelB = 9;
const byte channelSync = 10;
const byte CW  = 0x00;
const byte CCW = 0x01;
byte Rotary_dir = CW;
byte Current_Direction = CW;
int64_t pulse_count =0;
int32_t spins_count =0; 
int32_t pulses_per_circle = 0; 
int32_t saved_number = 0;
int64_t mm_overall_distance = 0; 

char inputString[100];         // a String to hold incoming data
int32_t string_pointer =0; 
boolean stringComplete = false;  // whether the string is complete

const char RESET[] = "RESET";
const char REVERSE[] = "REVERSE";

void setup() {
  // put your setup code here, to run once:
  pinMode(channelA, INPUT);
  pinMode(channelB, INPUT);
  pinMode(channelSync, INPUT);
  attachInterrupt(digitalPinToInterrupt( channelA),chA_handler, CHANGE);
  //attachInterrupt(digitalPinToInterrupt( channelB),chB_handler, CHANGE); // for instance it's enough to have 1 channel and use second only as a reference 
  attachInterrupt(digitalPinToInterrupt( channelSync),chRef_handler, CHANGE);
  Serial.begin(115200);
}
void ResetValues(void){
     pulse_count =0;
     spins_count =0; 
     mm_overall_distance = 0;   
}
void loop() {
  // put your main code here, to run repeatedly:

  if(stringComplete){
    Serial.write(inputString);
    if(memcmp(inputString, RESET, sizeof(RESET)-1)==0){
        ResetValues();
        Serial.write("Reseting values\n");
    }
    else if(memcmp(inputString, REVERSE, sizeof(REVERSE)-1)==0){
        ResetValues();
        Current_Direction ^= 0x01;
        Serial.write("Reversing direction, values reserted\n");
    }
    string_pointer =0;
    stringComplete = false; 
  }
  if(Rotary_dir==CW){
    Serial.write("Direct\r\n");
  }
  else{
    Serial.write("Reverse\r\n");    
  }
  Serial.write("Number of turns: ");
  char count[20];  // 19 symbos == 2^63, 1 symbol for sign
  itoa(spins_count, count, 10);
  Serial.write(count);
  Serial.write("\n");
  Serial.write("Number of pulses overall: ");
  itoa(pulse_count, count, 10);  
  Serial.write(count);
  Serial.write("\n");
  Serial.write("Overall distance (mm): ");
  mm_overall_distance = (pulse_count*um_per_pulse)/1000;
  itoa(mm_overall_distance, count, 10);  
  Serial.write(count);
  Serial.write("\n");  
  delay(ms_vcom_update_rate); // heating core... 
}
void chA_handler(){
  byte val=0;
  if(digitalRead(channelA)==HIGH){
    val|= 0x01;
  }
  if(digitalRead(channelB)==HIGH){
    val|= 0x02;
  }
  if( (val==0x01) || (val == 0x02)){
    Rotary_dir = CW^Current_Direction;
  }
  else{
    Rotary_dir = CCW^Current_Direction;
  }
  if((digitalRead(channelA)==HIGH)){
    if(Rotary_dir == CW)  pulse_count ++;
    else pulse_count--;
  }
}

// might improve precision. Out of use for now. 
void chB_handler(){
  if(digitalRead(channelB)==HIGH){
   // Serial.write("CH_B up\r\n");
  }
  else{
   // Serial.write("CH_B down\r\n");
  }
}


void chRef_handler(){
  if(digitalRead(channelSync)==HIGH){
    if(Rotary_dir == CW){
      spins_count++;
      // for debug purpose - check how many pulses per cycle (only CW direction) 
      //pulses_per_circle = pulse_count - saved_number;
      //saved_number = pulse_count;
    }
    else if(Rotary_dir == CCW) {
      spins_count --; 
    }
  }
}
// simple command iterface 
//
//char inputString[100];         // a String to hold incoming data
//int32_t string_pointer =0; 
void serialEvent() {
  while (Serial.available()) {
    // get the new byte:
    // add it to the inputString:
    inputString[string_pointer] = (char)Serial.read();
    if (inputString[string_pointer] == '\n') {
      stringComplete = true;
    }
    string_pointer++;
  }
}
