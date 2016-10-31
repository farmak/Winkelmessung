// Mit diesem Code werden die Bewegungsdaten der Sensoren ausgelesen und ausgegeben.
// Zusätzlich kann damit die Verbindung zum Arudino, über die ausgegebene ProductId überprüft werden. 
// Ist diese ungleich null, hat das Auslesen des Registers funktioniert.


// Pinnummern entsprechend der Verbindung zum Arduino anpassen
#define SCLK 22
#define SDIO 24  
#define NRESET 26
#define NCS 28 
#define REG_PRODUCT_ID 0x00
#define REG_REVISION_ID 0x01
#define REG_MOTION 0x02
#define REG_DELTA_X 0x03
#define REG_DELTA_Y 0x04
#define REG_SQUAL 0x05
#define REG_BURST_MODE 0x63
#define REG_PIXEL_GRAB 0x0b


float x = 0; 
float y = 0; 
long xi = 0;
long yi = 0;


byte pullByte() {
  pinMode (SDIO, INPUT);

  delayMicroseconds(100); // tHOLD = 100us min.
  
  byte res = 0;
  for (byte i=128; i >0 ; i >>= 1) {
    digitalWrite (SCLK, LOW);
    res |= i * digitalRead (SDIO);
    //delayMicroseconds(100);
    digitalWrite (SCLK, HIGH);
  }
  return res;
}

void pushByte(byte data){
  pinMode (SDIO, OUTPUT);
  
  delayMicroseconds(100); // tHOLD = 100us min.
  
  for (byte i=128; i >0 ; i >>= 1) {
    digitalWrite (SCLK, LOW);
    digitalWrite (SDIO, (data & i) != 0 ? HIGH : LOW);
    delayMicroseconds(100);
    digitalWrite (SCLK, HIGH);
  }
}

byte readRegister(byte address) {
  address &= 0x7F; // MSB indicates read mode: 0
  
  pushByte(address);
  
  byte data = pullByte();
  
  return data;  
}

void writeRegister(byte address, byte data) {
  address |= 0x80; // MSB indicates write mode: 1
  
  pushByte(address);
  
  delayMicroseconds(100);
  
  pushByte(data);

  delayMicroseconds(100); // tSWW, tSWR = 100us min.
}

void reset() {
  pinMode(SCLK, OUTPUT);
  pinMode(SDIO, INPUT);
  pinMode(NCS, OUTPUT);
  pinMode(NRESET, OUTPUT);
    
  digitalWrite(SCLK, LOW);
  digitalWrite(NCS, LOW);
  digitalWrite(NRESET, HIGH);
  delayMicroseconds(100);
  
  // chip reset
  digitalWrite(NRESET, LOW);
  pushByte(0xfa);
  pushByte(0x5a);
  digitalWrite(NRESET, HIGH);
  
  //writeRegister(0x0d, 0x01); //set Resolution 1000cpi
  
}

void dumpDelta() {
  char motion = readRegister(REG_MOTION); // Freezes DX and DY until they are read or MOTION is read again.
  int dx = readRegister(REG_DELTA_X);
  int dy = readRegister(REG_DELTA_Y);
  byte pixelsum = readRegister(0x09);

  if (dx > 128) {   //Information über die Richtung
    dx = dx - 256;
  }

  if (dy > 128) {
    dy = dy - 256;
  }

   xi += dx;
   yi += dy;
   
  x = xi/19.7; //für 500cpi /19.7, für 1000cpi /39.4
  y = yi/19.7;   
}
  
void setup() {
  Serial.begin(9600);
   
  reset();
  byte productId = readRegister(REG_PRODUCT_ID);
  byte revisionId = readRegister(REG_REVISION_ID);
  int Resolution = readRegister(0x0d);
  if (Resolution = 1){
    Resolution = 1000;
  }
    else{
    Resolution = 500;
  }
  
  Serial.println("Found productId ");
  Serial.print(productId, HEX);
  Serial.print(", rev. ");
  Serial.println(revisionId, HEX);
  Serial.print ("Resolution, ");  
  Serial.print(Resolution, DEC);
  Serial.println("cpi");
}

void loop() {
    dumpDelta();
  Serial.print("x: ");
  Serial.print(x);
  Serial.print(" mm");
  Serial.print("      ");
  Serial.print("y: ");
  Serial.println(y);
  Serial.println(" mm");
  delay(100);
}
