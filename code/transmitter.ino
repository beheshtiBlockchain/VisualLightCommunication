#include <DueTimer.h>
#include <PS2Keyboard.h>

const int DataPin = 6;
const int IRQpin =  3;

PS2Keyboard keyboard;
bool needpreamble = true;
bool sending = false;
uint32_t num = 0;
int itdata = -1;
int cnt = 500;
int data = 0;
int prmbit = 0 ;
int datait = 0;
char pr;
 
// bits to send using Visual Light Communication
int bits[10] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

// specific data to relatively synchronize transmitter and receiver
int preamble[17] = { 1, 1, 1, 1, 0, 0, 0, 1, 1, 1, 0, 1, 0, 1, 1, 0 };

void timerISR();
void reinitialize();

void setup() {
  pinMode(13, OUTPUT);
  digitalWrite(13, LOW);

  keyboard.begin(DataPin, IRQpin);
  Serial.begin(9600);
  delay(50);

  Timer1.attachInterrupt(timerISR).setPeriodMilliSeconds(10).start();
}

void loop() {
  while(1){
    /// two byte error in the receiver
    if(keyboard.available() && !sending){// it relates to when it starts(phase noise)
      // dont send an uncompleted data
      if(data == 0) {
        data = keyboard.read();
        if(data == 0) continue;
//        Serial.println(data);
      }
      
      // what if we need preamble in the middle of transmission
      // there would be no interruption in between transmission
      // transmission should be completed
      // what about receiver
    }
    
    if( data != 0 ){ // dont send NULL
          Serial.print( data );
          Serial.print(" - [ ");
          pr = data;
          reinitialize();
        for(int j = 0; data > 0; j++ ){
          bits[j] = data%2;
          Serial.print(data%2);
          Serial.print(" ");
          data=data/2;
        }
        
          Serial.print(bits[7]);
          Serial.print(" ");
        Serial.print("] - ");
        Serial.println(pr);
        sending = true; // dont read serial until data is sent.
    }
        
  // put your main code here, to run repeatedly:
  }
}
void timerISR(){
    
  num++;
  // synchronized iterator for data points
  itdata++;
  //Serial.println(itdata); 
  if(itdata%(2*8) == 0) {
    needpreamble =true; // after 2 bytes of data need synchronization
  }
  if( needpreamble )
          {
          if(sending){
            if(prmbit<17){
              digitalWrite(13, preamble[prmbit]?HIGH:LOW);
              prmbit++;
              num++;
            }
            if(prmbit>=17){
            needpreamble = false;
            itdata = 0;
            datait = 0;
            prmbit = 0;
            }
          }
  }
  if(!needpreamble){
    if(sending && datait < 8){
      //Serial.print(bits[datait++]);
        digitalWrite(13, bits[datait++]?HIGH:LOW);
    }
    if(sending && datait >= 8){
      Serial.println("<==");
      datait = 0; 
  //        Serial.print(num);//synchronization error fix with ISR
      digitalWrite(13,LOW);
      sending = false; // capture data from serial
    }
  }
}

void reinitialize(){
  int iin = 0;
  while( iin<10 ){
    bits[ iin++ ] = 0;
  }
}
