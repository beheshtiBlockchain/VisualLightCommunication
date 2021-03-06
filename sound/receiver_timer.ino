void shiftleft(int data[], int datum);
void reinitialize();
void timerISR();
boolean preamble_cmp(int *a, int *b, int len_a, int len_b);
#include <DueTimer.h>
#include <LiquidCrystal.h>
#include <PS2Keyboard.h>
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
int datachain[20];

bool needpreamble = true;
int i = 0;
int sofe2 =0;
int e2 = 1;

void setup() {
  
//  analogValue = analogRead(photoPin);
//  while (analogValue < 60) {
//    analogValue = analogRead(photoPin);
//  }

  Timer1.attachInterrupt(timerISR).setPeriod(125).start();
  Serial.begin(9600);           //  setup serial
  delay(50);
  analogWriteResolution(8);
//  Serial.print(VARIANT_MCK);
}

void loop() {
  
}

void timerISR(){
  val = analogRead(photoPin);  // read the input pin
  num++; // num of analog data till here
  itdata++; // starts from zero when preamble was checked ( once a 5 byte )
//  Serial.println(itdata);
  float voltage = val * 3.3 / 1023.0;
//  lcd.setCursor(13,1);
//  lcd.print(voltage);
//  Serial.write(itoa(val,ret,));
  if(val<minval){minval = val;}// min is equal the least quantity
  if(val>maxval){maxval = val;}// max is equal the greatest quantity active threshold defining (to be defined!)
  if(voltage >0.5){
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
//  lcd.setCursor(8, 1);
//  lcd.print(needpreamble?"true ":"false");
  if( needpreamble ){

  if( preamble_cmp(datachain, preamble, 16, 16) ){
    itdata = 0;
    needpreamble = false;
    Timer1.resetCounterValue();// TC0->TC_CHANNEL[2].TC_CCR = TC_CCR_SWTRG | TC_CCR_CLKEN;
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
        while(i < 8){
          e2 = 1<<(i);
          sofe2 += e2*datachain[ (itdata-8+i)%16 ]; // itdata either 10 or 20
          
          i++;
        }
      
       // lcd.setCursor(n, 0);
        if(sofe2 != 0 ){
          analogWrite(DAC0, sofe2 );
          // we need it not to play random words
        }
        if(itdata % 16 == 0) reinitialize();
        // change check of preamble to every byte
        //reset
     }
     
  if((itdata)%(2*8) == 0 &&!needpreamble){ // needs synchronization after 100 bytes of data
//    Serial.println("");
    needpreamble=true;
    reinitialize();
    //reset
  }
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
      if(mi<6){return true;}else{return false;}
}

void shiftleft(int datum){
  
  for(int i = 1 ; i<16; i++){
    datachain[ i-1 ] = datachain[ i ];
    }
    
  datachain[ 15 ] = datum;
  }
