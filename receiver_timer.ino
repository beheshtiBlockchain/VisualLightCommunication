void shiftleft(int data[], int datum);
void reinitialize();
void initiallize_timer();
boolean preamble_cmp(int *a, int *b, int len_a, int len_b);
#include <LiquidCrystal.h>
int photoPin = A0; // potentiometer wiper (middle terminal) connected to analog pin 3
                    // outside leads to ground and +5V
int val = 0;  // variable to store the value read
int n = 0, m = 0;
uint32_t num =0;
int maxval;
int minval;
int preamble[16] = { 1, 1, 1, 1, 0, 0, 0, 1, 1, 1, 0, 1, 0, 1, 1, 0 };
  
// int nofpass = 0;
int cnt = 500;
int analogValue;
int data = 0;
int itdata=0;
int datachain[16];
bool needpreamble = true;
int i = 0;
int sofe2 =0;
int e2 = 1;
LiquidCrystal lcd(12, 5, 8, 9, 10, 11);
void setup() {
  
  // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);
  
//  analogValue = analogRead(photoPin);
//  while (analogValue < 60) {
//    analogValue = analogRead(photoPin);
//  }
  initiallize_timer();
  Serial.begin(9600);           //  setup serial
  delay(50);
//  Serial.print(VARIANT_MCK);
}

void loop() {
  
}

ISR(TIMER1_COMPA_vect){
  val = analogRead(photoPin);  // read the input pin
  num++; // num of analog data till here
  itdata++; // starts from zero when preamble was checked ( once a 5 byte )
//  Serial.println(itdata);
  float voltage = val * 5.0 / 1023.0;
  lcd.setCursor(13,1);
  lcd.print(voltage);
//  Serial.write(itoa(val,ret,));
  if(val<minval){minval = val;}// min is equal the least quantity
  if(val>maxval){maxval = val;}// max is equal the greatest quantity active threshold defining (to be defined!)
  if(voltage >1.2){
     data = 1;
  } else { data = 0;} // read which data is read
  if(needpreamble){
    shiftleft(data);
//    Serial.print("true");
  }
  else datachain[(itdata-1)%16] = data; // phase locked loop

//  lcd.setCursor(0,0);
//  for(int iterator = 0 ; iterator<16 ; iterator++){// debug (slow motion!)
//   lcd.print( datachain[iterator]);
////   Serial.print(datachain[iterator]);
//  }
  
  
////  Serial.print(data);
  
//  lcd.print("-");
//  
//  lcd.setCursor(n,1);
//  lcd.print(data);
//  lcd.print("-");
//  lcd.print(voltage);
  lcd.setCursor(8, 1);
  lcd.print(needpreamble?"true ":"false");
  if( needpreamble ){

  if( preamble_cmp(datachain, preamble, 16, 16) ){
    itdata = 0;
    needpreamble = false;
    reset_timer();
    reinitialize();//==>no datachain holding after out of phase
    return;
//    lcd.print("true");
//    lcd.print(++nofpass);
//    lcd.print("of");
//    lcd.print(num/16);
  }
  } else if( itdata % 8 == 0 && !needpreamble ){ // is synchronized and was able to recieve data completely
        i = 0;
        sofe2 = 0;
        Serial.print("[ ");
        while(i < 8){
          e2 = 1<<(i);
          sofe2 += e2*datachain[ (itdata-8+i)%16 ]; // itdata either 8 or 16 
          Serial.print(datachain[(itdata-8+i)%16]);
          Serial.print(" ");
          i++;
        }
        Serial.print("] - ");
        char rest = sofe2;
        Serial.println(rest);
        lcd.setCursor(n, 0);
        if(rest != 0 ){
        for(int iso = 0 ; iso < 8; iso++){
        }
          n = (n+1)%16;
          lcd.print(rest); // we need it not to print random words
        }
        if(itdata % 16 == 0) reinitialize();
        // change check of preamble to every byte
        reset_timer();
     }
     
  if((itdata)%(2*8) == 0 &&!needpreamble){ // needs synchronization after 100 bytes of data
//    Serial.println("");
    needpreamble=true;
    reinitialize();
    reset_timer();
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
// Compare B Match Interrupt: Off
TCCR1A=(0<<COM1A1) | (0<<COM1A0) | (0<<COM1B1) | (0<<COM1B0) | (0<<WGM11) | (0<<WGM10);
TCCR1B=(1<<ICNC1) | (0<<ICES1) | (0<<WGM13) | (1<<WGM12) | (0<<CS12) | (1<<CS11) | (1<<CS10);
TCNT1H=0x00;
TCNT1L=0x00;
ICR1H=0x00;
ICR1L=0x00;
OCR1AH=0xC3;
OCR1AL=0x4F;
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
void reinitialize(){
  int iin = 0;
  while( iin<16 ){
    datachain[ iin++ ] = 0;
  }
}
boolean preamble_cmp(int *a, int *b, int len_a, int len_b){
      int ni;
      int mi = 0;
      // if their lengths are different, return false
      if (len_a != len_b) return false;

      // test each element to be the same. if not, return false
      for (ni=0;ni<len_a;ni++) if (a[ni]!=b[ni]) mi++;
      
      //ok, if we have not returned yet, they are equal :)
      if(mi<5){return true;}else{return false;}
}
void shiftleft(int datum){
  
  for(int i = 1 ; i<16; i++){
    datachain[ i-1 ] = datachain[ i ];
    }
    
  datachain[ 15 ] = datum;
  }
