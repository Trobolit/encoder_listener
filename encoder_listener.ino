
const byte interruptPinLs = 3; //speed
const byte interruptPinLd = 4; //direction
const byte interruptPinRs = 5; //speed
const byte interruptPinRd = 6; //direction
volatile long L_count = 0;
volatile byte L_dir = 0;
volatile long R_count = 0;
volatile byte R_dir = 0;
long Lc=0;
byte Ld=0;
long Rc=0;
byte Rd=0;

const int MESSAGE_LENGTH = 12; // including SOM and EOMs
const int EMPTY_MESSAGE = 0;

// pointer to memory of long so we can send the four bytes individually over serial.
byte *L_cPtr = (byte*) &Lc;
byte *R_cPtr = (byte*) &Rc;

const long timeThresh = 50; //ms
long lastTime = 0;

void setup() {
  pinMode(interruptPinLs, INPUT_PULLUP);
  pinMode(interruptPinRs, INPUT_PULLUP);
  pinMode(interruptPinLd, INPUT_PULLUP);
  pinMode(interruptPinRd, INPUT_PULLUP);
  Serial.begin(115200);
  lastTime = millis();
  attachInterrupt(digitalPinToInterrupt(interruptPinLs), myInterruptL1, RISING);
  attachInterrupt(digitalPinToInterrupt(interruptPinRs), myInterruptR1, RISING);
}

// Current configuration is right withing about 0.1 percent of actual frequency.
void loop() {
  if(millis() - lastTime >= timeThresh) {

    // important to do this, otherwise prints may affect the count from interrupts epending on the number in sequence it is printed. Serial print is slow.
    lastTime = millis(); //lastTime+timeThresh; // Current version protects us from potential overflow, but might increase the error slightly (about .05 percent...)
    Lc = L_count;
    Ld = L_dir;
    Rc = R_count;
    Rd = R_dir;
    L_count=0;
    L_dir=0;
    R_count=0;
    R_dir=0;

    Serial.write(250);
    Serial.write(Ld);
    for(byte i=3;i<4;i--){ //underflows
      Serial.write(*(L_cPtr+i));
    }
    Serial.write(Rd);
    for(byte i=3;i<4;i--){ //underflows
      Serial.write(*(R_cPtr+i));
    }
    Serial.write(251);

    // now print zeros, longer than message to make sure reciever cannot get garbage data.
    for(int i=0;i<MESSAGE_LENGTH+1;i++)
      Serial.write(EMPTY_MESSAGE);
 
  }
}

void myInterruptL1() {
    L_count+=1;
    L_dir = digitalRead(interruptPinLd); // this checks the phase signal for spinning direction.
                                          // Probably not the most elegant solution, but it needs to 
                                          // be done as the count signal rises.
}
void myInterruptR1() {
    R_count+=1;
    R_dir = !digitalRead(interruptPinRd);
}
