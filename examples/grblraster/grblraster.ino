/*
  Thanks for supporting Open-Hard/Soft-ware and thanks
  for all of the contributors to this project.

  For extra info on GRBL please have a look at my blog : 
	http://blog.protoneer.co.nz/tag/grbl/

  Grbl is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  Grbl is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.
  http://www.gnu.org/licenses/
*/

/*
	Supported hardware:
		Arduino Duemilanove
		Arduino Uno
		Arduino Mega 2560 (Limited Testing)

*/
#include <grblmain.h>
#include <serial.h>
#include <print.h>

#define SERIAL_NO_DATA 0xff
#ifndef LINE_BUFFER_SIZE
  #define LINE_BUFFER_SIZE 100
#endif
static char line[LINE_BUFFER_SIZE];
static uint8_t char_counter;
static uint8_t char_counter2;
static char line2[LINE_BUFFER_SIZE];

void setup(){
  pinMode(7,OUTPUT);
digitalWrite(7,HIGH);
pinMode(13,OUTPUT);
digitalWrite(13,LOW);
  serial_init();
char_counter = 0;
char_counter2 = 0;
}

void loop(){
  //printString("V/R: \n");
  char* t = ReadSerial();
  if(strcmp(t,"vector") == 0)
  {
    printString("Vector\n");
    startGrbl();
    
  
  }else if(strcmp(t,"raster") == 0)
  {
   printString("Raster\n");
    Raster();
  
  }


}



  //*******************************SET LASING PARAMETERS*****************
//Set LASER POWER -- SET POWER-PERCENTAGE THEN CONVERT TO "ON" for "LASER ON" function using analogWrite (PWM)/// 
int laserPercentage = 80; // enter Power percentage 0-100%
const unsigned int longdelay = 1500; //Set burn time depending on the target material
const unsigned int shortdelay = 1500; //go as low as your motors will allow without locking up/making bad noise. Should sound nice and smooth. If image is skewed or worse, make larger b/c your motors are skipping steps
const float stepsPerPixel = 10;  // ALSO KNOWN AS SCALE FACTOR*****************

//*********************************END SET LASER PARAMETERS******************

// Depending on you setup 
//For me: (200 steps /rot)*(18 rot / inch)*(1 inch / 25.4 mm) -> .007 mmPerStep
const float mmPerStep = 1/(200*18/25.4); 
const float mmPerPixel = mmPerStep * stepsPerPixel;// NOT yet used

const byte ledPin =  13;      // the number of the LED pin
boolean ledState = LOW; //to toggle our LED
const byte OFF = 0;
const byte ON = map(laserPercentage, 0, 100, 0, 255);// IF Laser pin is PWM then then convert for analogWrite PWM

const byte XmotorDIR = 24;
const byte XmotorPULSE = 22;
const byte YmotorDIR = 25;
const byte YmotorPULSE = 23;
const byte LASER = 6; //PWM on Arduino Uno ************************************
byte xsleep = 7;//SLEEP HIGH TO ENABLE MOTORS
//byte ysleep = 9;//SLEEP HIGH TO ENABLE MOTORS

// Variables that will change:
byte a = 0;
byte initialmode = 0;
byte lasermode = 0;
int byteCounter=0; //counting each instruction byte (also tells if data was received)
int setCounter=0; //counting the instruction blocks'
int linelength=0;
long xpositioncount = 0;
long ypositioncount = 0;

char* ReadSerial()
{
  uint8_t d;
  bool finished = false;
  //clear
  for(int i=0;i<LINE_BUFFER_SIZE;i++)
  {
    line2[i] = 0; 
  }
while(!finished)
{
  
  while((d = serial_read()) != SERIAL_NO_DATA && !finished)
  {
    if ((d == '\n') || (d == '\r')) {
      if (char_counter2 > 0) {
        line2[char_counter2] = 0;
        //char_counter2 = 0;
        finished = true;
       
      }
      char_counter2 = 0;
    }else
    {
     line2[char_counter2++] = d; 
    }
  }
}
return line2;
}
void Raster()
{


  // set the digital pin as output:
  pinMode(ledPin, OUTPUT);
  pinMode(LASER, OUTPUT);
  pinMode(xsleep, OUTPUT);
  //pinMode(ysleep, OUTPUT);
  pinMode(YmotorPULSE, OUTPUT);
  pinMode(YmotorDIR, OUTPUT);
  pinMode(XmotorPULSE, OUTPUT);
  pinMode(XmotorDIR, OUTPUT);
  
  digitalWrite(xsleep, HIGH); //Set motors to sleep Mode until Connection with Processing is established
  //digitalWrite(ysleep, HIGH);
  
  pinMode(ledPin, OUTPUT); // Set pin as OUTPUT
  
  printString("Establishing contact..\n");
  
  uint8_t c;

  printString("A\n");
  delay(300);
  digitalWrite(LASER,0);
  
  digitalWrite(xsleep, LOW); //Enable motors
  
  while(serial_read() == SERIAL_NO_DATA)
    {
    }
  
  delay(2000);
    //READY...OK
  printString("Arduino is Ready for ");
  printString("linelength \n");//Send READY signal to processing to get next instruction block
  printString("1\n"); //Printing takes up precious milliseconds!
  byteCounter=0;
  boolean vector = false;
  while(!vector)
  {
    byte byteRead;
    char* k;
    char* m;
    //if(serial_read() != SERIAL_NO_DATA)
    //{
    if(byteCounter == 0){ 
      //linelength = 10;
      //k = ReadSerial();
      //linelength = k[0];
      linelength = atoi(ReadSerial());
      linelength = linelength - 10;
      //linelength = linelength - 10;
      //printString(k);
      //printInteger(linelength);
      //printString();
      //printString("L\n");
      printString("2\n");
      m = ReadSerial();
 // while(e = serial_read() == SERIAL_NO_DATA)
   // {
   // }
    }
    
      while(byteCounter < linelength){

        
        byteRead = (byte) m[byteCounter];

        
        if (byteRead != 'r'){
        byteRead=byteRead-'0';
        //byteCounter=byteCounter-'0';
      }
        
        //printString("bara\n");
        if((byteRead < 10) || (byteRead == 'r')){   // RANDOMLY GETING BYTES "207"??? SKIP THOSE *******But don't ignore the reset command!
        byteCounter++;
            //printString("7o7o\n");
            //Move motors
    if(byteRead==1){
      //Move right FAST
      fastright();    
 //printString("done1\n");     
    }
    if(byteRead==2){
      //Move right SLOW
      slowright();
    }
    if(byteRead==3){
      //Move left FAST
      fastleft();         
    }
    if(byteRead==4){
      //Move left SLOW
      slowleft();         
    }
    if(byteRead==5){
      //Move up FAST
      fastup();
    }
    if(byteRead==6){
      //Move up SLOW
      slowup();
    }
    if(byteRead==7){
      //Move down FAST
      fastdown();
      //printString("done\n");
    }
    if(byteRead==8){
      //Move down SLOW
      slowdown();
      //printString("done\n");
    }
    if(byteRead==9){
      analogWrite (LASER, ON);
      //printString("done\n");
    }
    if(byteRead==0){
      analogWrite (LASER, OFF);
      //printString("done\n");
    }
    if (byteRead=='r'){
      //reset position
      xresetposition();
      yresetposition();
      delay(1000);
      digitalWrite(xsleep, HIGH);
      vector = true;
       }
      }
     }
    //}
    
   //printString("tttt\n");
 if(byteCounter != 0){//ONLY SEND READY SIGNAL IF YOU'VE RECEIVED DATA SINCE THE LAST TIME
       setCounter++;
     if(!vector)
     {
      printString("1\n");
       
     }
     byteCounter=0;
 }

  }
  
  
}
//************************************************************************************************************
// Main Motor functions
//************************************************************************************************************

void fastright()
{digitalWrite (XmotorDIR, ON);
  for (a=0; a<stepsPerPixel; a++)
  {
    digitalWrite (XmotorPULSE, ON);
    digitalWrite (XmotorPULSE, OFF);
    delayMicroseconds(shortdelay);
    }
  xpositioncount++;
}

void slowright()
{digitalWrite (XmotorDIR, ON);
  for (a=0; a<stepsPerPixel; a++)
  {
    
    digitalWrite (XmotorPULSE, ON);
    //digitalWrite (ledPin, ON);
    digitalWrite (XmotorPULSE, OFF);
    delayMicroseconds(longdelay);
    //digitalWrite (ledPin, OFF);
  }
  xpositioncount++;
}

void fastleft()
{digitalWrite (XmotorDIR, OFF);
  for (a=0; a<stepsPerPixel; a++)
  {
    
    digitalWrite (XmotorPULSE, ON);
    //digitalWrite (ledPin, ON);
    digitalWrite (XmotorPULSE, OFF);
    delayMicroseconds(shortdelay);
    //digitalWrite (ledPin, OFF);
  }
  xpositioncount--;
}

void slowleft()
{digitalWrite (XmotorDIR, OFF);
  for (a=0; a<stepsPerPixel; a++)
  {
    
    digitalWrite (XmotorPULSE, ON);
    //digitalWrite (ledPin, ON);
    digitalWrite (XmotorPULSE, OFF);
    delayMicroseconds(longdelay);
    //digitalWrite (ledPin, OFF);
  }
  xpositioncount--;
}

void fastup()
{digitalWrite (YmotorDIR, ON);
  for (a=0; a<stepsPerPixel; a++)
  {
    
    digitalWrite (YmotorPULSE, ON);
    //digitalWrite (ledPin, ON);
    digitalWrite (YmotorPULSE, OFF);
    delayMicroseconds (shortdelay);
    //digitalWrite (ledPin, OFF);
  }
  ypositioncount++;
}

void slowup()
{digitalWrite (YmotorDIR, ON);
  for (a=0; a<stepsPerPixel; a++)
  {
    
    digitalWrite (YmotorPULSE, ON);
    //digitalWrite (ledPin, ON);
    digitalWrite (YmotorPULSE, OFF);
    delayMicroseconds (longdelay);
    //digitalWrite (ledPin, OFF);
  }
  ypositioncount++;
}

void fastdown()
{digitalWrite (YmotorDIR, OFF);
  for (a=0; a<stepsPerPixel; a++)
  {
    
    digitalWrite (YmotorPULSE, ON);
    //digitalWrite (ledPin, ON);
    digitalWrite (YmotorPULSE, OFF);
    delayMicroseconds (shortdelay);
    //digitalWrite (ledPin, OFF);
  }
  ypositioncount--;
}

void slowdown()
{digitalWrite (YmotorDIR, OFF);
  for (a=0; a<stepsPerPixel; a++)
  {
    
    digitalWrite (YmotorPULSE, ON);
    //digitalWrite (ledPin, ON);
    digitalWrite (YmotorPULSE, OFF);
    delayMicroseconds (longdelay);
    //digitalWrite (ledPin, OFF);
  }
  ypositioncount--;
}

void xresetposition()
{
  while (xpositioncount!=0){
    if (xpositioncount > 0){
      fastleft();
    }
    if (xpositioncount < 0){
      fastright();
    }
  }
}


void yresetposition()
{
  while (ypositioncount!=0){
    if (ypositioncount > 0){
      fastdown();
    }
    if (ypositioncount < 0){
      fastup();
    }
  }
}
