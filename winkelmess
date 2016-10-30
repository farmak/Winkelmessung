//Mit diesem Programm können die gemessenen Winkel der Sensoren einzeln ausgegeben werden oder der resultierende (der größere der beiden) Winkel.
//Über die Serielle Datenverbindung zu Matlab können diese Daten parallel geplottet werden.

// Pinnummern entsprechend der Verbindung zum Arduino anpassen
#define SCLK1 47
#define SDIO1 49  
#define NRESET1 51
#define NCS1 53 
#define SCLK2 22
#define SDIO2 24 
#define NRESET2 26
#define NCS2 28 

//Definieren der Register der Sensoren
#define REG_PRODUCT_ID 0x00
#define REG_REVISION_ID 0x01
#define REG_MOTION 0x02
#define REG_DELTA_X 0x03
#define REG_DELTA_Y 0x04
#define REG_SQUAL 0x05
#define REG_PIXEL_SUM 0x09


 
//________________________________________________________________________________________________________________
//  globale Variablen
//________________________________________________________________________________________________________________

//Sensordaten
byte motion1; 
byte motion2;
char dx1;
char dy1; 
char dx2; 
char dy2;
byte pixelsum1;

//Bewegungsdaten
float x1 = 0; 
float y1 = 0; 
float x2 = 0;
float y2 = 0;
float xi1 = 0;
float yi1 = 0;
float xi2 = 0;
float yi2 = 0;
float phi1;
float phi2;
float phi;
float U = 268.48; //Länge der Kreisbahn in cm 

//Merker
byte n;
byte i;
byte m;
byte r;

//Korrekturfaktoren
float k1 = 1;
float k2 = 1;

//Zeitmarke
unsigned long t0;

//_____________________________________________________________________________________________________________
//  Funktionen
//_____________________________________________________________________________________________________________

//Byte aus der Datenleitung auslesen
byte pullByte1() {
  pinMode (SDIO1, INPUT);

  delayMicroseconds(100); // tHOLD = 100us min.
  
  byte res = 0;
  for (byte i=128; i >0 ; i >>= 1) {
    digitalWrite (SCLK1, LOW);
    res |= i * digitalRead (SDIO1);
    delayMicroseconds(100);
    digitalWrite (SCLK1, HIGH);
  }

  return res;
}

byte pullByte2() {
  pinMode (SDIO2, INPUT);

  delayMicroseconds(100); // tHOLD = 100us min.
  
  byte res = 0;
  for (byte i=128; i >0 ; i >>= 1) {
    digitalWrite (SCLK2, LOW);
    res |= i * digitalRead (SDIO2);
    delayMicroseconds(100);
    digitalWrite (SCLK2, HIGH);
  }

  return res;
}

//Adresse des Registers übermitteln, das ausgelesen werden soll
void pushAddress1(byte address){
  
  address &= 0x7F;    //MSB = 0 signalisiert, dass Register gelesen werden soll
  
  pinMode (SDIO1, OUTPUT);
  
  for (byte i=128; i >0 ; i >>= 1) {
    digitalWrite (SCLK1, LOW);
    digitalWrite (SDIO1, (address & i) != 0 ? HIGH : LOW);
    delayMicroseconds(100);
    digitalWrite (SCLK1, HIGH);
  }
}

void pushAddress2(byte address){
  address &= 0x7F;
  
  pinMode (SDIO2, OUTPUT);
  
  for (byte i=128; i >0 ; i >>= 1) {
    digitalWrite (SCLK2, LOW);
    digitalWrite (SDIO2, (address & i) != 0 ? HIGH : LOW);
    delayMicroseconds(100);
    digitalWrite (SCLK2, HIGH);
  }
}

//Beliebiges Byte übermitteln
void pushByte1(byte data){
  pinMode (SDIO1, OUTPUT);
  
  delayMicroseconds(100); // tHOLD = 100us min.
  
  for (byte i=128; i >0 ; i >>= 1) {
    digitalWrite (SCLK1, LOW);
    digitalWrite (SDIO1, (data & i) != 0 ? HIGH : LOW);
    delayMicroseconds(100);
    digitalWrite (SCLK1, HIGH); 
  }
}

void pushByte2(byte data){
  pinMode (SDIO2, OUTPUT);
  
  delayMicroseconds(100); // tHOLD = 100us min.
  
  for (byte i=128; i >0 ; i >>= 1) {
    digitalWrite (SCLK2, LOW);
    digitalWrite (SDIO2, (data & i) != 0 ? HIGH : LOW);
    delayMicroseconds(100);
    digitalWrite (SCLK2, HIGH);
  }
}


//Byte auslesen
//byte readRegister1(byte address) {
//  address &= 0x7F; 
//  
//  pushByte1(address1);
//  delayMicroseconds(4);
//  byte data = pullByte1();
//  delayMicroseconds(1);
//  
//  return data;  
//}


//Byte in Register schreiben
void writeRegister1(byte address, byte data) {
  address1 |= 0x80; //MSB = 1 signalisiert, dass Register beschrieben werden soll
  
  pushByte1(address);
  
  delayMicroseconds(100);
  
  pushByte1(data);

delayMicroseconds(100); // tSWW, tSWR = 1
}

void writeRegister2(byte address, byte data) {
  address |= 0x80; 
  
  pushByte2(address);
  
  delayMicroseconds(100);
  
  pushByte2(data);

delayMicroseconds(100); 
}

//Sensoren zurücksetzen (immer nach dem Einschalten oder dem Ändern der Auflösung zurücksetzen)
void reset() {
  pinMode(SCLK1, OUTPUT);
  pinMode(SDIO1, INPUT);
  pinMode(NCS1, OUTPUT);
  pinMode(NRESET1, OUTPUT);
  pinMode(SCLK2, OUTPUT);
  pinMode(SDIO2, INPUT);
  pinMode(NCS2, OUTPUT);
  pinMode(NRESET2, OUTPUT);
    
  digitalWrite(SCLK1, LOW);
  digitalWrite(NCS1, LOW);
  digitalWrite(NRESET1, HIGH);
  digitalWrite(SCLK2, LOW);
  digitalWrite(NCS2, LOW);
  digitalWrite(NRESET2, HIGH);
  delayMicroseconds(100);
  
 
  digitalWrite(NRESET1, LOW);
  pushByte1(0xfa);
  pushByte1(0x5a);
  digitalWrite(NRESET1, HIGH);

  digitalWrite(NRESET2, LOW);
  pushByte2(0xfa);
  pushByte2(0x5a);
  digitalWrite(NRESET2, HIGH);

  
// Entferne Kommentar um die Auflösung auf 1000cpi zu setzen; verändere zusätzlich Umrechnungsfaktor in dumpDelta()
//  writeRegister1(0x0d, 0x01); // Sensor1
//  writeRegister2(0x0d, 0x01); // Sensor2
}
   

//Lese Daten aus Sensoren
void dumpDelta() {

  pushAddress1(REG_MOTION);
  pushAddress2(REG_MOTION);
  delayMicroseconds(4); //t_SRAD SPI Read Address Data Delay, Verzögerung für die Übergabe der Datenleitung
  motion1 = pullByte1();
  motion2 = pullByte2(); 
  delayMicroseconds(1); //t_SRR Verzögerung zwischen zwei Reads
  pushAddress1(REG_DELTA_X);
  pushAddress2(REG_DELTA_X);
  delayMicroseconds(4);
  dx1 = pullByte1();
  dx2 = pullByte2();
  delayMicroseconds(1); 
  pushAddress1(REG_DELTA_Y);
  pushAddress2(REG_DELTA_Y);
  delayMicroseconds(4); 
  dy1 = pullByte1();
  dy2 = pullByte2();
  delayMicroseconds(1); 
  pushAddress1(REG_PIXEL_SUM);
  delayMicroseconds(4);
//  pixelsum1 = readRegister1(REG_PIXEL_SUM);
  
  xi1 += dx1; 
  yi1 += dy1;

  xi2 += dx2;
  yi2 += dy2;
  
  x1 = xi1/197; // xi1/197 für 500cpi, xi1/394 für 1000cpi; analog für y1 etc.
  y1 = yi1/197; 

  x2 = xi2/197; 
  y2 = yi2/197; 
  

//Wenn die Nullmarkierung erreicht ist, Korrekturfaktor berechnen und Winkel null setzen
  if (pixelsum1 > 140 && n == 0) {  //Grenzwert von pixelsum1 variiert wenn Fahrzeug auf der Drehbühne steht, ohne Fahrzeug 140
                                                                                              
    k1 = k1*(2-(phi1/360));
    k2 = k2*(2-(phi2/360)); 

    //Unrealistische Korrekturfaktoren ausschließen, z. B. im Falle des Nicht-Erkennens der Nullmarkierung; Grenzwerte sind Erfahrungswerte
    if (k2 < 0.8 | k2 > 1.2 | k1 < 0.8 | k1 > 1.2){
      k1 = 1;
      k2 = 1;
    }

    x1 = 0; 
    y1 = 0;
    x2 = 0;
    y2 = 0;
    
    xi1 = 0;
    yi1 = 0;
    xi2 = 0;
    yi2 = 0;

    n = 1; //Merker wird beim ersten Erkennen der Nullmarkierung gesetzt, damit weiter gemessen wird, während der Sensor über der Nullmarkierung liegt.
  }

  if (pixelsum1 < 140) {
    n = 0; 
  }

  phi1 = -(360*y1*k1)/U; //Alternativ (360*y2*(dy2/dx2))/(U*sqrt(((dy2/dx2)^2) + 1));  
  phi2 = -(360*y2*k2)/U;  

 //Der Wert des Sensors mit dem größeren Winkel, wird als Ist-Wert genommen 
 if (phi1 > phi2) {
  phi = phi1;
 }
 else {
  phi = phi2;
 }
}

//_____________________________________________________________________________________________________________
//  Setup
//_____________________________________________________________________________________________________________

void setup() {
  
  
  //Starte serielle Datenverbindung  
  Serial.begin(250000);
  
  //Maussensoren initialisieren
  reset();
  
  delay(1000); 
}            

//_____________________________________________________________________________________________________________                
//LOOP
//_____________________________________________________________________________________________________________

void loop() {
  
  // Warte auf Startsignal, damit Matlab die Daten in richtiger Reihenfolge aufnimmt 
  while (Serial.available() == 0); 
    
  //Startzeitpunkt festhalten
  if (i == 0){
    i = 1;
    t0 = millis();
  }
  
  dumpDelta();

  //Die Reihenfolge der Daten muss mit der Reihenfolge der Auswertung in Matlab übereinstimmen
  unsigned long t1 = millis();
  Serial.println(phi1);
  Serial.println(phi2);
  Serial.println(t1 - t0);    
}
