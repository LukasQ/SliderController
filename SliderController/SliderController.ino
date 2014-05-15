//////////////////////////////////////////////////////////////////////////////
// (c) 2014 by Lukas Merz - merz.lukas@gmail.com
// Controller for an electronic Slider that can move X, Nick, Gier, Focus and Zoom
//
// Used with AccelStepper from Mike McCauley: 
// http://www.schmalzhaus.com/EasyDriver/Examples/EasyDriverExamples.html
// RAMPS Electronics (and therefore the Pinout) http://reprap.org/wiki/RAMPS_1.4
// Open Source Licensing GPL V2
// Thanks to @redreprap: http://forums.reprap.org/read.php?219,99920,165777#msg-165777
// 
// Caution: EEPROM read/writes max 100.000 cycles. Must change it to SD!
// Try to get use of Stepper AND Servo in Code
// Uses atm Canon 60D, here: D60
//
//////////////////////////////////////////////////////////////////////////////

#define MAX_SPEED 100.0
#define EEPROM_WRITETIME 5
#define CAM_MODEL D60

#include "pins.c"
#include "lcd.c"

#include <AccelStepper.h>
#include <EEPROM.h>
#include <Servo.h> 
#include <multiCameraIrControl.h> 


String theString = "";
String tempString = "";

Canon CAM_MODEL(CAM_PIN);

//initiate bipolar Steppers
AccelStepper xStepper(1, X_STEP_PIN, X_DIR_PIN);
AccelStepper nStepper(1, N_STEP_PIN, N_DIR_PIN);
AccelStepper gStepper(1, G_STEP_PIN, G_DIR_PIN);
//ifdef Stepper
AccelStepper fStepper(1, F_STEP_PIN, F_DIR_PIN);
AccelStepper zStepper(1, Z_STEP_PIN, Z_DIR_PIN);
//else Servo
Servo fServo;
Servo zServo;


/////////////////////////////////////////////////////////////
// S E T U P
/////////////////////////////////////////////////////////////

void setup() {
  digitalWrite(LED_PIN, HIGH); //light on
  initiatePins();
  Serial.begin(9600);
  delay(5000);
  //runHome();
  digitalWrite(LED_PIN, LOW); //light off

  //default Acceleration
  xStepper.setAcceleration(100.0);
  gStepper.setAcceleration(100.0);
  nStepper.setAcceleration(100.0);
  fStepper.setAcceleration(100.0);
  zStepper.setAcceleration(100.0);
}

/////////////////////////////////////////////////////////////
// L O O P
/////////////////////////////////////////////////////////////
void loop() {
  // read the serial command if available
  while (Serial.available() > 0) {
    char recieved = Serial.read();
    theString += recieved;
    if (recieved == '\n')
    {
      //hier stimmt was mit dem Speichern nicht. Muss das irgendwie in 2 Byte bekommen
      switch (theString.charAt(0)) { // Switch between the Codes: (e.g.: X 100,20,10;) 
      case 'X':
        Serial.println(theString);
        tempString = theString.substring(theString.indexOf(" ")+1, theString.indexOf(","));
        EEPROM.write(0, tempString.toInt()/8); //start Pos in EEPROM
        delay(EEPROM_WRITETIME);
        EEPROM.write(1, tempString.toInt()%8); //start Pos in EEPROM
        delay(EEPROM_WRITETIME);
        tempString = theString.substring(theString.indexOf(",")+1, theString.lastIndexOf(","));
        EEPROM.write(2, tempString.toInt()/8); // End Pos in EEPROM
        delay(EEPROM_WRITETIME);
        EEPROM.write(3, tempString.toInt()%8); //start Pos in EEPROM
        delay(EEPROM_WRITETIME);
        tempString = theString.substring(theString.lastIndexOf(",")+1, theString.indexOf(";"));
        EEPROM.write(4, tempString.toInt()/8); // End Pos in EEPROM
        delay(EEPROM_WRITETIME);
        EEPROM.write(5, tempString.toInt()%8); //start Pos in EEPROM
        delay(EEPROM_WRITETIME);
        Serial.print("200 OK go from ");
        Serial.print(EEPROM.read(0)*8 + EEPROM.read(1));
        Serial.print(" to ");
        Serial.print(EEPROM.read(2)*8 + EEPROM.read(3));
        Serial.print(" with ");
        Serial.print(EEPROM.read(4)*8 + EEPROM.read(5));
        Serial.println(" mm/s^2;");
        theString = "";
        tempString = "";
        break;
      case 'G':    
        tempString = theString.substring(theString.indexOf(" ")+1, theString.indexOf(","));
        EEPROM.write(6, tempString.toInt()/8); // End Pos in EEPROM
        delay(EEPROM_WRITETIME);
        EEPROM.write(7, tempString.toInt()%8); //start Pos in EEPROM
        delay(EEPROM_WRITETIME);
        tempString = theString.substring(theString.indexOf(",")+1, theString.lastIndexOf(","));
        EEPROM.write(8, tempString.toInt()/8); // End Pos in EEPROM
        delay(EEPROM_WRITETIME);
        EEPROM.write(9, tempString.toInt()%8); //start Pos in EEPROM
        delay(EEPROM_WRITETIME);
        tempString = theString.substring(theString.lastIndexOf(",")+1, theString.indexOf(";"));
        EEPROM.write(10, tempString.toInt()/8); // End Pos in EEPROM
        delay(EEPROM_WRITETIME);
        EEPROM.write(11, tempString.toInt()%8); //start Pos in EEPROM
        delay(EEPROM_WRITETIME);
        Serial.print("200 OK gier from ");
        Serial.print(EEPROM.read(6)*8 + EEPROM.read(7));
        Serial.print(" to ");
        Serial.print(EEPROM.read(8)*8 + EEPROM.read(9));
        Serial.print(" with ");
        Serial.print(EEPROM.read(10)*8 + EEPROM.read(11));
        Serial.println(" mm/s^2;");
        theString = "";
        tempString = "";
        break;
      case 'N':    
        tempString = theString.substring(theString.indexOf(" ")+1, theString.indexOf(","));
        EEPROM.write(12, tempString.toInt()/8); // End Pos in EEPROM
        delay(EEPROM_WRITETIME);
        EEPROM.write(13, tempString.toInt()%8); //start Pos in EEPROM
        delay(EEPROM_WRITETIME);
        tempString = theString.substring(theString.indexOf(",")+1, theString.lastIndexOf(","));
        EEPROM.write(14, tempString.toInt()/8); // End Pos in EEPROM
        delay(EEPROM_WRITETIME);
        EEPROM.write(15, tempString.toInt()%8); //start Pos in EEPROM
        delay(EEPROM_WRITETIME);
        tempString = theString.substring(theString.lastIndexOf(",")+1, theString.indexOf(";"));
        EEPROM.write(16, tempString.toInt()/8); // End Pos in EEPROM
        delay(EEPROM_WRITETIME);
        EEPROM.write(17, tempString.toInt()%8); //start Pos in EEPROM
        delay(EEPROM_WRITETIME);
        Serial.print("200 OK nick from ");
        Serial.print(EEPROM.read(12)*8 + EEPROM.read(13));
        Serial.print(" to ");
        Serial.print(EEPROM.read(14)*8 + EEPROM.read(15));
        Serial.print(" with ");
        Serial.print(EEPROM.read(16)*8 + EEPROM.read(17));
        Serial.println(" mm/s^2;");
        theString = "";
        tempString = "";
        break;
      case 'F':    
        tempString = theString.substring(theString.indexOf(" ")+1, theString.indexOf(","));
        EEPROM.write(18, tempString.toInt()/8); // End Pos in EEPROM
        delay(EEPROM_WRITETIME);
        EEPROM.write(19, tempString.toInt()%8); //start Pos in EEPROM
        delay(EEPROM_WRITETIME);
        tempString = theString.substring(theString.indexOf(",")+1, theString.lastIndexOf(","));
        EEPROM.write(20, tempString.toInt()/8); // End Pos in EEPROM
        delay(EEPROM_WRITETIME);
        EEPROM.write(21, tempString.toInt()%8); //start Pos in EEPROM
        delay(EEPROM_WRITETIME);
        tempString = theString.substring(theString.lastIndexOf(",")+1, theString.indexOf(";"));
        EEPROM.write(22, tempString.toInt()/8); // End Pos in EEPROM
        delay(EEPROM_WRITETIME);
        EEPROM.write(23, tempString.toInt()%8); //start Pos in EEPROM
        delay(EEPROM_WRITETIME);
        Serial.print("200 OK focus from ");
        Serial.print(EEPROM.read(18)*8 + EEPROM.read(19));
        Serial.print(" to ");
        Serial.print(EEPROM.read(20)*8 + EEPROM.read(21));
        Serial.print(" with ");
        Serial.print(EEPROM.read(22)*8 + EEPROM.read(23));
        Serial.println(" mm/s^2;");
        theString = "";
        tempString = "";
        break;
      case 'Z':    
        tempString = theString.substring(theString.indexOf(" ")+1, theString.indexOf(","));
        EEPROM.write(24, tempString.toInt()/8); // End Pos in EEPROM
        delay(EEPROM_WRITETIME);
        EEPROM.write(25, tempString.toInt()%8); //start Pos in EEPROM
        delay(EEPROM_WRITETIME);
        tempString = theString.substring(theString.indexOf(",")+1, theString.lastIndexOf(","));
        EEPROM.write(26, tempString.toInt()/8); // End Pos in EEPROM
        delay(EEPROM_WRITETIME);
        EEPROM.write(27, tempString.toInt()%8); //start Pos in EEPROM
        delay(EEPROM_WRITETIME);
        tempString = theString.substring(theString.lastIndexOf(",")+1, theString.indexOf(";"));
        EEPROM.write(28, tempString.toInt()/8); // End Pos in EEPROM
        delay(EEPROM_WRITETIME);
        EEPROM.write(29, tempString.toInt()%8); //start Pos in EEPROM
        delay(EEPROM_WRITETIME);
        Serial.print("200 OK zoom from ");
        Serial.print(EEPROM.read(24)*8 + EEPROM.read(25));
        Serial.print(" to ");
        Serial.print(EEPROM.read(26)*8 + EEPROM.read(27));
        Serial.print(" with ");
        Serial.print(EEPROM.read(28)*8 + EEPROM.read(29));
        Serial.println(" mm/s^2;");
        theString = "";
        tempString = "";
        break;     
      case 'S':
        //!!!!!!!!!!!!!!!!!!TEST this 
        tempString = theString.substring(1,2);
        EEPROM.write(30, tempString.toInt()); // End Pos in EEPROM
        delay(EEPROM_WRITETIME);
        tempString = theString.substring(theString.indexOf(",")+1, theString.lastIndexOf(","));
        EEPROM.write(32, tempString.toInt()/8); // End Pos in EEPROM
        delay(EEPROM_WRITETIME);
        EEPROM.write(33, tempString.toInt()%8); //start Pos in EEPROM
        delay(EEPROM_WRITETIME);
        tempString = theString.substring(theString.lastIndexOf(",")+1, theString.indexOf(";"));
        EEPROM.write(34, tempString.toInt()/8); // End Pos in EEPROM
        delay(EEPROM_WRITETIME);
        EEPROM.write(35, tempString.toInt()%8); //start Pos in EEPROM
        delay(EEPROM_WRITETIME);
        theString = "";
        tempString = "";
        Serial.println("Starting...");
        digitalWrite(LED_PIN, HIGH);
        Serial.println("Enable Driver..."); 
        digitalWrite(X_ENABLE_PIN    , HIGH);  //Enable Driver
        digitalWrite(G_ENABLE_PIN    , HIGH);
        digitalWrite(N_ENABLE_PIN    , HIGH);
        digitalWrite(F_ENABLE_PIN    , HIGH);
        digitalWrite(Z_ENABLE_PIN    , HIGH);
        Serial.println("Move to start position...");
        //run with vMax to Start Pos
        runStepper((EEPROM.read(0)*8) + EEPROM.read(1),(EEPROM.read(6)*8) + EEPROM.read(7),(EEPROM.read(12)*8) + EEPROM.read(13),(EEPROM.read(18)*8) + EEPROM.read(19),(EEPROM.read(24)*8) + EEPROM.read(25), true); //run with max speed
        //fServo.write(1);
        //zServo.write(20);
        Serial.println("Wait..."); //Start at the next inetarion
        if (tempString.toInt() == 0){
          CAM_MODEL.shutterDelayed();    //Make Vidjo
          delay(5000);
        }
        delay(1000);
        //run with vDefined to End
        Serial.println("Move to end position..."); 
        runStepper((EEPROM.read(2)*8) + EEPROM.read(3),(EEPROM.read(8)*8) + EEPROM.read(9),(EEPROM.read(14)*8) + EEPROM.read(15),(EEPROM.read(20)*8) + EEPROM.read(21),(EEPROM.read(26)*8) + EEPROM.read(27), false);
        if (tempString.toInt() == 0){
          delay(5000);
          CAM_MODEL.shutterDelayed(); // Stop Vidjo
        }
        //runHome();

        //fServo.write(1);
        //zServo.write(20);
        Serial.println("Done.");
        break;
      default:
        digitalWrite(X_ENABLE_PIN    , LOW);  //Low = aus!
        digitalWrite(G_ENABLE_PIN    , LOW);
        digitalWrite(N_ENABLE_PIN    , LOW);
        digitalWrite(F_ENABLE_PIN    , LOW);
        digitalWrite(Z_ENABLE_PIN    , LOW);

        if (millis() %1000 <500) 
          digitalWrite(13, HIGH);
        else
          digitalWrite(13, LOW);
        theString = "";
      }  
    }
  }
  if (millis() %1000 <500) 
    digitalWrite(13, HIGH);
  else
    digitalWrite(13, LOW);
}

/////////////////////////////////////////////////////////////
// W R I T E T O E  E P R O M : moves the head to Position
/////////////////////////////////////////////////////////////
void writeToEeprom (char cIndex, int iStart, int iEnd, int iSpeed){
  switch (cIndex){
  case 'X':
    break;
  case 'G':
    break;
  case 'N':
    break;
  case 'F':
    break;
  case 'Z':
    break;
  default:
    break;
  }
}


/////////////////////////////////////////////////////////////
// R U N S T E P P E R : moves the head to Position
/////////////////////////////////////////////////////////////
void runStepper (int iX, int iG, int iN, int iF, int iZ, boolean vMax){
  int timer = EEPROM.read(32)*8 + EEPROM.read(33);
  int counter = EEPROM.read(34)*8 + EEPROM.read(35);
  int foto = EEPROM.read(30);

  if(vMax){  
    xStepper.setMaxSpeed(MAX_SPEED); // with vMax
    gStepper.setMaxSpeed(MAX_SPEED);
    nStepper.setMaxSpeed(MAX_SPEED);
    fStepper.setMaxSpeed(MAX_SPEED);
    zStepper.setMaxSpeed(MAX_SPEED);
    counter = 0;
  }
  else{
    xStepper.setMaxSpeed(EEPROM.read(4)*8 + EEPROM.read(5)); //with defined Speed from EEPROM
    gStepper.setMaxSpeed(EEPROM.read(10)*8 + EEPROM.read(11));
    nStepper.setMaxSpeed(EEPROM.read(16)*8 + EEPROM.read(17));
    fStepper.setMaxSpeed(EEPROM.read(22)*8 + EEPROM.read(23));
    zStepper.setMaxSpeed(EEPROM.read(28)*8 + EEPROM.read(29));
  }

  xStepper.move(iX);// .moveTo?
  gStepper.move(iG);
  nStepper.move(iN);
  fStepper.move(iF);
  zStepper.move(iZ);

  // !!!!!!!!!!!!!!!!!!2Do: test Abort criteria
  //while (!digitalRead(X_MAX_PIN) && !digitalRead(X_MAX_PIN) && xStepper.distanceToGo() >= 0 && gStepper.distanceToGo() >= 0 && nStepper.distanceToGo() >= 0 && fStepper.distanceToGo() >= 0 && zStepper.distanceToGo() >= 0){ 
  while (xStepper.distanceToGo() > 0 || gStepper.distanceToGo() > 0 || nStepper.distanceToGo() > 0 || fStepper.distanceToGo() > 0 || zStepper.distanceToGo() > 0){ 
    if (!digitalRead(X_MAX_PIN) || !digitalRead(X_MAX_PIN)){ //As long no endstop is pressed
      xStepper.stop(); 
      gStepper.stop(); 
      nStepper.stop(); 
      fStepper.stop(); 
      zStepper.stop(); 
      break;
    }

    xStepper.run(); 
    gStepper.run(); 
    nStepper.run(); 
    fStepper.run(); 
    zStepper.run(); 

    Serial.print("X: ");
    Serial.println(xStepper.distanceToGo());
    Serial.print("G: ");
    Serial.println(gStepper.distanceToGo());
    // TEST this as well 
    if ((foto == 0 && millis() % 1000 <= timer) || counter != 0)
      CAM_MODEL.shutterNow();
    counter--;
  }
}

/////////////////////////////////////////////////////////////
// R U N H O M E : moves the head to the homing position
/////////////////////////////////////////////////////////////
void runHome (){
  /*
// Set target Position
   xStepper.setMaxSpeed(MAX_SPEED); // with vMax
   gStepper.setMaxSpeed(MAX_SPEED);
   nStepper.setMaxSpeed(MAX_SPEED);
   fStepper.setMaxSpeed(MAX_SPEED);
   zStepper.setMaxSpeed(MAX_SPEED);
   
   xStepper.move(-100);// .moveTo?
   gStepper.move(-100);
   nStepper.move(-100);
   fStepper.move(-100);
   zStepper.move(-100);
   
   while(!digitalRead(X_MAX_PIN) || !digitalRead(X_MAX_PIN)){
   xStepper.run(); 
   gStepper.run(); 
   nStepper.run(); 
   fStepper.run(); 
   zStepper.run(); 
   }*/

  digitalWrite(X_DIR_PIN , HIGH);
  digitalWrite(G_DIR_PIN , HIGH);
  digitalWrite(N_DIR_PIN , HIGH);


  while(!digitalRead(X_MIN_PIN)){
    //accelStepper.run();
    digitalWrite(X_STEP_PIN , HIGH);
    delay(1);
    digitalWrite(X_STEP_PIN , LOW);
  }
  while(!digitalRead(G_MIN_PIN)){
    //accelStepper.run();
    digitalWrite(G_STEP_PIN, HIGH);
    delay(1);
    digitalWrite(G_STEP_PIN , LOW);
  }
  while(!digitalRead(N_MIN_PIN)){
    //accelStepper.run();
    digitalWrite(N_STEP_PIN, HIGH);
    delay(1);
    digitalWrite(N_STEP_PIN, LOW);
  }
}

/////////////////////////////////////////////////////////////
// S E T S T E P P E R D A T A : Set Acceleration and Speed
/////////////////////////////////////////////////////////////
void setStepperData (){
  xStepper.setAcceleration(100.0);
  gStepper.setAcceleration(100.0);
  nStepper.setAcceleration(100.0);
  fStepper.setAcceleration(100.0);
  zStepper.setAcceleration(100.0);
}

/////////////////////////////////////////////////////////////
// I N I T I A T E P I N S : Setup for I/O Pins
/////////////////////////////////////////////////////////////
void initiatePins (){
  pinMode(FAN_PIN , OUTPUT);
  //pinMode(HEATER_0_PIN , OUTPUT);
  //pinMode(HEATER_1_PIN , OUTPUT);
  pinMode(LED_PIN  , OUTPUT);

  //Settings X-Axis
  pinMode(X_STEP_PIN  , OUTPUT);
  pinMode(X_DIR_PIN    , OUTPUT);
  pinMode(X_ENABLE_PIN    , OUTPUT);
  pinMode(X_MAX_PIN, INPUT);
  pinMode(X_MIN_PIN, INPUT);
  digitalWrite(X_MAX_PIN, HIGH); //Internal PullUp
  digitalWrite(X_MIN_PIN, HIGH); //Internal PullUp

  //Settings Gier-Axis
  pinMode(G_STEP_PIN  , OUTPUT);
  pinMode(G_DIR_PIN    , OUTPUT);
  pinMode(G_ENABLE_PIN    , OUTPUT);
  //OPTO Endstop / ENCODER?

  //Settings Nick-Axis
  pinMode(N_STEP_PIN  , OUTPUT);
  pinMode(N_DIR_PIN    , OUTPUT);
  pinMode(N_ENABLE_PIN    , OUTPUT);
  //OPTO Endstop / ENCODER?

  //Settings Focus-Axis
  pinMode(F_STEP_PIN  , OUTPUT);
  pinMode(F_DIR_PIN    , OUTPUT);
  pinMode(F_ENABLE_PIN    , OUTPUT);
  // SERVO -> encoder enthalten

  //Settings Zoom-Axis
  pinMode(Z_STEP_PIN  , OUTPUT);
  pinMode(Z_DIR_PIN    , OUTPUT);
  pinMode(Z_ENABLE_PIN    , OUTPUT);
  // SERVO -> encoder enthalten

  fServo.attach(F_SERVO_PIN);  // attaches the servo on pin 9 to the servo object
  zServo.attach(Z_SERVO_PIN);  // attaches the servo on pin 9 to the servo object 

  //maybe here enable the drivers
}
