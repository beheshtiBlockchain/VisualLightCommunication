
#include <PS2Keyboard.h>

const int DataPin = 6;
const int IRQpin =  3;

PS2Keyboard keyboard;
bool needpreamble = true;
bool sending = false;
uint32_t num = 0;
int itdata = -1;
int cnt = 200;
int data = 0;
int prmbit = 0 ;
int datait = 0;
char pr;
 
// bits to send using Visual Light Communication
int bits[10] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

// specific data to relatively synchronize transmitter and receiver
int preamble[17] = { 1, 1, 1, 1, 0, 0, 0, 1, 1, 1, 0, 1, 0, 1, 1, 0 };

void reinitialize();
void initiallize_timer();

void setup() {
  pinMode(13, OUTPUT);
  digitalWrite(13, LOW);

  keyboard.begin(DataPin, IRQpin);
  Serial.begin(9600);
  delay(50);

  initiallize_timer();
}

void loop() {
  while(1){
    /// two byte error in the receiver
    if(!sending){// it relates to when it starts(phase noise)
      // dont send an uncompleted data
      if(data == 0) {
        data = analogRead(A0);
        if(data == 0) continue;
//        Serial.println(data);
      }
      
      // what if we need preamble in the middle of transmission
      // there would be no interruption in between transmission
      // transmission should be completed
      // what about receiver
    }
    
    if( data != 0 ){ // dont send NULL
//          Serial.print( data );
//          Serial.print(" - [ ");
//          pr = data;
          reinitialize();
          data = data /4;
        for(int j = 0; data > 0; j++ ){
          bits[j] = data%2;
//          Serial.print(data%2);
//          Serial.print(" ");
          data=data/2;
        }
//        
//          Serial.print(bits[7]);
//          Serial.print(" ");
//        Serial.print("] - ");
//        Serial.println(pr);
        sending = true; // dont read serial until data is sent.
    }
        
  // put your main code here, to run repeatedly:
  }
}
ISR(TIMER1_COMPA_vect){
    
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
            reset_timer();
            }
          }
         
  }
  if(!needpreamble){
    if(sending && datait < 8){
      //Serial.print(bits[datait++]);
        digitalWrite(13, bits[datait++]?HIGH:LOW);
    }
    if(sending && datait >= 8){
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
void initiallize_timer(){
  cli();
//  
//  TCCR1A = 0;// set entire TCCR1A register to 0
//  TCCR1B = 0;// same for TCCR1B
//  TCNT1  = 0;//initialize counter value to 0
//  // set compare match register for 1hz increments
//  // 500ms timer
//  OCR1A = 7812;// = (16*10^6) / (1*1024) - 1 (must be <65536)
//  // turn on CTC mode
//  TCCR1B |= (1 << WGM12);
//  // Set CS10 and CS12 bits for 1024 prescaler
//  TCCR1B |= (1 << CS12) | (1 << CS10);  
//  // enable timer compare interrupt
//  TIMSK1 |= (1 << OCIE1A);
// Timer/Counter 1 initialization
// Clock source: System Clock
// Clock value: 62.500 kHz
// Mode: CTC top=OCR1A
// OC1A output: Disconnected
// OC1B output: Disconnected
// Noise Canceler: Off
// Input Capture on Falling Edge
// Timer Period: 0.5 s
// Timer1 Overflow Interrupt: Off
// Input Capture Interrupt: Off
// Compare A Match Interrupt: On
// Compare B Match Interrupt: Off 20milliseconds 50Hz
TCCR1A=(0<<COM1A1) | (0<<COM1A0) | (0<<COM1B1) | (0<<COM1B0) | (0<<WGM11) | (0<<WGM10);
TCCR1B=(1<<ICNC1) | (0<<ICES1) | (0<<WGM13) | (1<<WGM12) | (0<<CS12) | (0<<CS11) | (1<<CS10);
TCNT1H=0xF8;
TCNT1L=0x30;
ICR1H=0x00;
ICR1L=0x00;
OCR1AH=0x07;
OCR1AL=0xCF;
OCR1BH=0x00;
OCR1BL=0x00;



// Timer(s)/Counter(s) Interrupt(s) initialization
TIMSK1=(1<<OCIE1A);

  sei();
}
void reset_timer(){
unsigned char sreg;
unsigned int i;
/* Save global interrupt flag */
sreg = SREG;
/* Disable interrupts */
cli();
/* Set TCNT1 to i */
TCNT1 = 0;
/* Restore global interrupt flag */
SREG = sreg;
sei();
}
