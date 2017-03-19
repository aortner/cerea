// Arduino Cerea Teilbreitenschaltung
// (C) 2016 by Andreas Ortner - office@gh-ortner.com
// Weitergabe an Dritte nur unter Erlaubniss von Andreas Ortner
// Version 2 mit Spritze overdrive
//
// 19.3.2017
// erweiter um Tasteneingabe für Lenkungsaktivierung ("Auto")
// (C) 2017 by Klaus Sieverding - Klaus.Sieverding@web.de

String cmd;
char nextChar;
String teilbreite;
int anztb ;
int i;
int sensorVal;
int lsensorVal;
// - 19.3.2017
int sensorAuto;
int aDos[11]={0,0,0,0,0,0,0,0,0,0,0}; // Array für Befehlübergabe

void setup() {
  Serial.begin(9600);
  pinMode(2, OUTPUT);   //Teilbreite 1 Pin 2
  pinMode(3, OUTPUT);
  pinMode(4, OUTPUT);
  pinMode(5, OUTPUT);
  pinMode(6, OUTPUT);
  pinMode(7, OUTPUT);
  pinMode(8, OUTPUT);  // Teilbreite 7 Pin 8
  pinMode(9, OUTPUT);  // Pin 9  Hydraulik Befehl
  pinMode(10, INPUT_PULLUP);   // Marcar ein oder aus
// - 19.3.2017
  pinMode(11, INPUT_PULLUP);   // Auto ein oder aus

  digitalWrite(2, HIGH);  // alle Ports of 0 = HIGH Setzten
  digitalWrite(3, HIGH);
  digitalWrite(4, HIGH);
  digitalWrite(5, HIGH);
  digitalWrite(6, HIGH);
  digitalWrite(7, HIGH);
  digitalWrite(8, HIGH);
  digitalWrite(9, HIGH);
}

void loop()
{
  cmd = "";   // String leeren
  ser();      // Commando einleisen
  
  if (cmd.startsWith("@CEREA;")) {    // Wenn Cerea entdeckt wird mit ausführung starten
    cerea();
  }
  if (cmd.startsWith("@HIDRAU")) {    // Wenn Hidrau entdeckt wird mit ausführung starten
    hidrau();                         // evt. hier entferenen um Hyraulik nicht zu nutzten
  }



sensorVal = digitalRead(10);   // pin 10 Abfragen "Marca" 
// - 19.3.2017
sensorAuto = digitalRead(11);       // pin 11 Abfragen "Auto"

/* alte Version 19.3.2017
 if (sensorVal == HIGH ) {
    Serial.print ("@SDOSE;1;0;0;0;0;0;0;0;0;0;0;END");    //wenn Kontakt frei Marc ein
    Serial.print ('\r');
    Serial.print ('\n');
 }
 else if (sensorVal == LOW )
 {
    Serial.print ("@SDOSE;0;0;0;0;0;0;0;0;0;0;0;END");   //wenn Kontakt auf GND Marc aus
    Serial.print ('\r');
    Serial.print ('\n');
 }
*/

// - 19.3.2017
 if (sensorVal == HIGH ) {
    aDos[0]=1; // wenn Kontakt frei Marca ein
 }
 else if (sensorVal == LOW )
 {
    aDos[0]=0; // wenn Kontakt auf GND Marca ausn
 }

 
 if (sensorAuto == HIGH ) {
    aDos[5]=1; // wenn Kontakt frei Auto ein
 }
 else if (sensorAuto == LOW )
 {
    aDos[5]=0; // wenn Kontakt auf GND Auto ausn
 }

// gibt Befehl an Cerea
Serial.print ("@SDOSE;aDos[0];aDos[1];aDos[2];aDos[3];aDos[4];aDos[5];aDos[6];aDos[7];aDos[8];aDos[9];aDos[10];END");    
Serial.print ('\r');
Serial.print ('\n');
  
  
}



void cerea()   //String auswerten
{
   
  cmd.remove(0, 7); // @Cerea; entferenen

  int first = cmd.indexOf(';');   // erstem ; suchen
  int second = cmd.indexOf(';', first + 1 );  // zweiten ; suchen
  cmd.remove(0, second + 1); //geschwindigkeit und minus 1 entferene

  int ende = cmd.indexOf('END'); //sucht nach "end"
  anztb = (ende - 2) / 2; //anzahl teilbreiten ermitteln

  teilbreite = cmd.substring(0, ende - 3); //teilbreiten in einen neuen string

 

  int b = 1;
  for (i = 0; i < anztb * 2 ; i = i + 2) { //teilbreiten schalten
    b = b + 1;
    if (teilbreite.substring(i, i + 1) == "1")  digitalWrite(b, LOW);       //Teilbreite einschalten
    if (teilbreite.substring(i, i + 1) == "0" ) digitalWrite(b, HIGH);      //Teilbreite ausschalten
  }

  for (i = anztb + 1 ; i < 8; i++) { // restlichen teilbreiten aus
    digitalWrite(i + 1, HIGH);
  }
}


void hidrau()
{
  int ende = cmd.indexOf('EN'); //sucht nach "end"
  cmd.remove(0, ende - 3); // rest entfernen
  
  
  if (cmd.substring(0, 1) == "1") {
    digitalWrite(9, LOW);         //hyro ein  --> hier low mit high ersetzten falls hydro nicht gewünscht wird
  }
  if (cmd.substring(0, 1) == "0") {
    digitalWrite(9, HIGH);       //hyro aus
  }
}


void ser()   // Serielle Schnittstelle bis Return lesen
{
  do {
    if (Serial.available() > 0)
    {
      nextChar = Serial.read();
      if (nextChar >= 32) {
        cmd += nextChar;
      }
    }
  } while (nextChar != 10);  //solange lesen bis zeilenende

}


