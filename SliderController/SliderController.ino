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

#define MAX_SPEED 1500
#define MAX_ACCEL 5000
#define EEPROM_WRITETIME 5
#define CAM_MODEL D60
#define STEPS_MM 20 //20 Steps to get 1mm -> X = 800 >> 800*20=16000 steps target

#include "pins.c"
#include "lcd.c"
#include <AccelStepper.h>
#include <EEPROM.h>
#include <Servo.h> 
#include <Timer.h>
#include <LiquidCrystal.h>
#include <multiCameraIrControl.h> 

String theString = "";
String tempString = "";

Timer tTime;

Canon CAM_MODEL(CAM_PIN);

//initiate bipolar Steppers
AccelStepper xStepper(AccelStepper::DRIVER, X_STEP_PIN, X_DIR_PIN);
AccelStepper nStepper(AccelStepper::DRIVER, N_STEP_PIN, N_DIR_PIN);
AccelStepper gStepper(AccelStepper::DRIVER, G_STEP_PIN, G_DIR_PIN);
//ifdef Stepper
AccelStepper fStepper(AccelStepper::DRIVER, F_STEP_PIN, F_DIR_PIN);
AccelStepper zStepper(AccelStepper::DRIVER, Z_STEP_PIN, Z_DIR_PIN);
//else Servo
Servo fServo;
Servo zServo;


/////////////////////////////////////////////////////////////
// S E T U P
/////////////////////////////////////////////////////////////

void setup() {
  digitalWrite(LED_PIN, HIGH); //light on
  Serial.begin(9600);
  Serial.println("Starting ...");
  initiatePins();
  runHome();
  Serial.println("Done init.");
  digitalWrite(LED_PIN, LOW); //light off
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
      switch (theString.charAt(0)) { // Switch between the Codes: (e.g.: X 100,20,10;) 
      case 'X':
        Serial.println(theString);
        tempString = theString.substring(theString.indexOf(" ")+1, theString.indexOf(","));
        EEPROM.write(0, tempString.toInt()/32); //start Pos in EEPROM
        delay(EEPROM_WRITETIME);
        EEPROM.write(1, tempString.toInt()%32); //start Pos in EEPROM
        delay(EEPROM_WRITETIME);
        tempString = theString.substring(theString.indexOf(",")+1, theString.lastIndexOf(","));
        EEPROM.write(2, tempString.toInt()/32); // End Pos in EEPROM
        delay(EEPROM_WRITETIME);
        EEPROM.write(3, tempString.toInt()%32); //End Pos in EEPROM
        delay(EEPROM_WRITETIME);
        tempString = theString.substring(theString.lastIndexOf(",")+1, theString.indexOf(";"));
        EEPROM.write(4, tempString.toInt()/32); // speed Pos in EEPROM
        delay(EEPROM_WRITETIME);
        EEPROM.write(5, tempString.toInt()%32); //speed Pos in EEPROM
        delay(EEPROM_WRITETIME);
        Serial.print("200 OK go from ");
        Serial.print(EEPROM.read(0)*32 + EEPROM.read(1));
        Serial.print(" to ");
        Serial.print(EEPROM.read(2)*32 + EEPROM.read(3));
        Serial.print(" with ");
        Serial.print(EEPROM.read(4)*32 + EEPROM.read(5));
        Serial.println("/100 mm/s;");
        theString = "";
        tempString = "";
        break;
      case 'G':    
        tempString = theString.substring(theString.indexOf(" ")+1, theString.indexOf(","));
        EEPROM.write(6, tempString.toInt()/32); // Start Pos in EEPROM
        delay(EEPROM_WRITETIME);
        EEPROM.write(7, tempString.toInt()%32); //Start Pos in EEPROM
        delay(EEPROM_WRITETIME);
        tempString = theString.substring(theString.indexOf(",")+1, theString.lastIndexOf(","));
        EEPROM.write(8, tempString.toInt()/32); // End Pos in EEPROM
        delay(EEPROM_WRITETIME);
        EEPROM.write(9, tempString.toInt()%32); // End Pos in EEPROM
        delay(EEPROM_WRITETIME);
        tempString = theString.substring(theString.lastIndexOf(",")+1, theString.indexOf(";"));
        EEPROM.write(10, tempString.toInt()/32); // Speed Pos in EEPROM
        delay(EEPROM_WRITETIME);
        EEPROM.write(11, tempString.toInt()%32); //Speed Pos in EEPROM
        delay(EEPROM_WRITETIME);
        Serial.print("200 OK gier from ");
        Serial.print(EEPROM.read(6)*32 + EEPROM.read(7));
        Serial.print(" to ");
        Serial.print(EEPROM.read(8)*32 + EEPROM.read(9));
        Serial.print(" with ");
        Serial.print(EEPROM.read(10)*32 + EEPROM.read(11));
        Serial.println("/10 deg/s;");
        theString = "";
        tempString = "";
        break;
      case 'N':    
        tempString = theString.substring(theString.indexOf(" ")+1, theString.indexOf(","));
        EEPROM.write(12, tempString.toInt()/32); // Start Pos in EEPROM
        delay(EEPROM_WRITETIME);
        EEPROM.write(13, tempString.toInt()%32); // Start Pos in EEPROM
        delay(EEPROM_WRITETIME);
        tempString = theString.substring(theString.indexOf(",")+1, theString.lastIndexOf(","));
        EEPROM.write(14, tempString.toInt()/32); // End Pos in EEPROM
        delay(EEPROM_WRITETIME);
        EEPROM.write(15, tempString.toInt()%32); // End Pos in EEPROM
        delay(EEPROM_WRITETIME);
        tempString = theString.substring(theString.lastIndexOf(",")+1, theString.indexOf(";"));
        EEPROM.write(16, tempString.toInt()/32); // Speed Pos in EEPROM
        delay(EEPROM_WRITETIME);
        EEPROM.write(17, tempString.toInt()%32); //Speed Pos in EEPROM
        delay(EEPROM_WRITETIME);
        Serial.print("200 OK nick from ");
        Serial.print(EEPROM.read(12)*32 + EEPROM.read(13));
        Serial.print(" to ");
        Serial.print(EEPROM.read(14)*32 + EEPROM.read(15));
        Serial.print(" with ");
        Serial.print(EEPROM.read(16)*32 + EEPROM.read(17));
        Serial.println("/10 deg/s;");
        theString = "";
        tempString = "";
        break;
      case 'F':    
        tempString = theString.substring(theString.indexOf(" ")+1, theString.indexOf(","));
        EEPROM.write(18, tempString.toInt()/32); // Start Pos in EEPROM
        delay(EEPROM_WRITETIME);
        EEPROM.write(19, tempString.toInt()%32); // Start Pos in EEPROM
        delay(EEPROM_WRITETIME);
        tempString = theString.substring(theString.indexOf(",")+1, theString.lastIndexOf(","));
        EEPROM.write(20, tempString.toInt()/32); // End Pos in EEPROM
        delay(EEPROM_WRITETIME);
        EEPROM.write(21, tempString.toInt()%32); // End Pos in EEPROM
        delay(EEPROM_WRITETIME);
        tempString = theString.substring(theString.lastIndexOf(",")+1, theString.indexOf(";"));
        EEPROM.write(22, tempString.toInt()/32); // Speed Pos in EEPROM
        delay(EEPROM_WRITETIME);
        EEPROM.write(23, tempString.toInt()%32); // Speed Pos in EEPROM
        delay(EEPROM_WRITETIME);
        Serial.print("200 OK focus from ");
        Serial.print(EEPROM.read(18)*32 + EEPROM.read(19));
        Serial.print(" to ");
        Serial.print(EEPROM.read(20)*32 + EEPROM.read(21));
        Serial.print(" with ");
        Serial.print(EEPROM.read(22)*32 + EEPROM.read(23));
        Serial.println("/10 deg/s;");
        theString = "";
        tempString = "";
        break;
      case 'Z':    
        tempString = theString.substring(theString.indexOf(" ")+1, theString.indexOf(","));
        EEPROM.write(24, tempString.toInt()/32); // Start Pos in EEPROM
        delay(EEPROM_WRITETIME);
        EEPROM.write(25, tempString.toInt()%32); // Start Pos in EEPROM
        delay(EEPROM_WRITETIME);
        tempString = theString.substring(theString.indexOf(",")+1, theString.lastIndexOf(","));
        EEPROM.write(26, tempString.toInt()/32); // End Pos in EEPROM
        delay(EEPROM_WRITETIME);
        EEPROM.write(27, tempString.toInt()%32); //End Pos in EEPROM
        delay(EEPROM_WRITETIME);
        tempString = theString.substring(theString.lastIndexOf(",")+1, theString.indexOf(";"));
        EEPROM.write(28, tempString.toInt()/32); // Speed Pos in EEPROM
        delay(EEPROM_WRITETIME);
        EEPROM.write(29, tempString.toInt()%32); //Speed Pos in EEPROM
        delay(EEPROM_WRITETIME);
        Serial.print("200 OK zoom from ");
        Serial.print(EEPROM.read(24)*32 + EEPROM.read(25));
        Serial.print(" to ");
        Serial.print(EEPROM.read(26)*32 + EEPROM.read(27));
        Serial.print(" with ");
        Serial.print(EEPROM.read(28)*32 + EEPROM.read(29));
        Serial.println("/10 deg/s;");
        theString = "";
        tempString = "";
        break;     
      case 'S':
        tempString = theString.substring(1,2);
        EEPROM.write(30, tempString.toInt()); // Vidjo (1) or Foto (0)?
        delay(EEPROM_WRITETIME);
        tempString = theString.substring(theString.indexOf(",")+1, theString.lastIndexOf(","));
        EEPROM.write(32, tempString.toInt()/32); // Timer
        delay(EEPROM_WRITETIME);
        EEPROM.write(33, tempString.toInt()%32);
        delay(EEPROM_WRITETIME);
        tempString = theString.substring(theString.lastIndexOf(",")+1, theString.indexOf(";"));
        EEPROM.write(34, tempString.toInt()/32); // Number of Pictures
        delay(EEPROM_WRITETIME);
        EEPROM.write(35, tempString.toInt()%32);
        delay(EEPROM_WRITETIME);
        theString = "";
        tempString = "";
        Serial.println("Starting ...");
        digitalWrite(LED_PIN, HIGH);

        activateDrivers(true);
        Serial.println("Move to #Start ...");
        //run with vMax to Start Pos 
        //Calculate here from input-units to steps -> X >> Convert from mm to Steps
        runStepper((EEPROM.read(0)*32 + EEPROM.read(1)),
        (EEPROM.read(6)*32 + EEPROM.read(7)),
        (EEPROM.read(12)*32 + EEPROM.read(13)),
        (EEPROM.read(18)*32 + EEPROM.read(19)),
        (EEPROM.read(24)*32 + EEPROM.read(25)), true); //run with max speed
        //fServo.write(1);
        //zServo.write(20);
        Serial.println("Wait..."); //Start at the next inetarion
        if (tempString.toInt() == 0){
          CAM_MODEL.shutterDelayed();    //Make Vidjo
          delay(5000);
        }
        delay(1000);
        //run with vDefined to End
        Serial.println("Move to #End ..."); 
        //Calculate here from input-units to steps s.o.
        //End - Start = Way
        runStepper((EEPROM.read(2)*32 + EEPROM.read(3))-(EEPROM.read(0)*32 + EEPROM.read(1)),
        (EEPROM.read(8)*32 + EEPROM.read(9))-(EEPROM.read(6)*32 + EEPROM.read(7)),
        (EEPROM.read(14)*32 + EEPROM.read(15))-(EEPROM.read(12)*32 + EEPROM.read(13)),
        (EEPROM.read(20)*32 + EEPROM.read(21))-(EEPROM.read(18)*32 + EEPROM.read(19)),
        (EEPROM.read(26)*32 + EEPROM.read(27))-(EEPROM.read(24)*32 + EEPROM.read(25)), false); //run the Motor from A to B
        if (tempString.toInt() == 0){
          delay(4000);
          CAM_MODEL.shutterDelayed(); // Stop Vidjo
        }
        runHome();
        Serial.println("Done.");
        activateDrivers(false);
        break;
      default:
        //deactivate Drivers
        activateDrivers(false);
        //Blink
        if (millis() %1000 <500) 
          digitalWrite(13, HIGH);
        else
          digitalWrite(13, LOW);
        theString = "";
      }  
    }
  }
  if (millis() %1000 <500) {
    digitalWrite(13, HIGH);
    //activateDrivers(false);
  } 
  else {
    digitalWrite(13, LOW);
  }

}

/////////////////////////////////////////////////////////////
// W R I T E T O E E P R O M : writes the values to EEPROM
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
// R U N S T E P P E R : moves the head over distance [IMPUT]
/////////////////////////////////////////////////////////////
void runStepper (int iX, int iG, int iN, int iF, int iZ, boolean vMax){
  int timer = EEPROM.read(32)*32 + EEPROM.read(33);
  int counter = EEPROM.read(34)*32 + EEPROM.read(35);
  int foto = EEPROM.read(30);

  Serial.println(iX);
  Serial.println(iG);
  Serial.println(iN);
  Serial.println(iF);
  Serial.println(iZ);

  // Man könnte sich überlegen die Daten anders zu gliedern. Jetz fahrt der waren eine relative position an.
  //Absolut ist aber auch möglich mit MoveTo

  xStepper.setAcceleration(MAX_ACCEL);
  gStepper.setAcceleration(MAX_ACCEL);
  nStepper.setAcceleration(MAX_ACCEL);
  fStepper.setAcceleration(MAX_ACCEL);
  zStepper.setAcceleration(MAX_ACCEL);

  //Move to Position [mm] * Stepindicator
  //xStepper.runToNewPosition(iX*STEPS_MM)
  xStepper.move(iX*STEPS_MM); //MoveTo ??
  gStepper.move(iG*STEPS_MM);
  nStepper.move(iN*STEPS_MM);
  fStepper.move(iF*STEPS_MM);
  zStepper.move(iZ*STEPS_MM);

  if(vMax){  
    xStepper.setMaxSpeed(MAX_SPEED); // with vMax
    gStepper.setMaxSpeed(MAX_SPEED); //setMaxSpeed eigentlich
    nStepper.setMaxSpeed(MAX_SPEED);
    fStepper.setMaxSpeed(MAX_SPEED);
    zStepper.setMaxSpeed(MAX_SPEED);

    counter = 0; //dont shoot while MoveTo StartPos
  }
  else{
    xStepper.setMaxSpeed(EEPROM.read(4)*32 + EEPROM.read(5)); //with defined Speed from EEPROM
    gStepper.setMaxSpeed(EEPROM.read(10)*32 + EEPROM.read(11));
    nStepper.setMaxSpeed(EEPROM.read(16)*32 + EEPROM.read(17));
    fStepper.setMaxSpeed(EEPROM.read(22)*32 + EEPROM.read(23));
    zStepper.setMaxSpeed(EEPROM.read(28)*32 + EEPROM.read(29));
  }
  //Picture Timer
  tTime.every(timer * 1000, callback, counter);

  // What about the 
  while ((!digitalRead(X_MAX_PIN) && !digitalRead(X_MIN_PIN)) 
    && (xStepper.distanceToGo() > 0 || gStepper.distanceToGo() > 0 
    || nStepper.distanceToGo() > 0 || fStepper.distanceToGo() > 0 || zStepper.distanceToGo() > 0)){ 
    /*test abort criteria
    while ((xStepper.distanceToGo() > 0 || gStepper.distanceToGo() > 0 
    || nStepper.distanceToGo() > 0 || fStepper.distanceToGo() > 0 || zStepper.distanceToGo() > 0)){  */
    xStepper.run();
    gStepper.run(); 
    nStepper.run(); 
    fStepper.run(); 
    zStepper.run();

    //timer update
    tTime.update();
  }
  //!!!!!!!!!!! 2DoIs this usefull? test at home -> maybe. turns function from Position based movement, to distance based
  if(vMax){  
    xStepper.setCurrentPosition(0);
    gStepper.setCurrentPosition(0);
    nStepper.setCurrentPosition(0);
    fStepper.setCurrentPosition(0);
    zStepper.setCurrentPosition(0);
  }
}

/////////////////////////////////////////////////////////////
// C A L L B A C K: makes the pic
/////////////////////////////////////////////////////////////
void callback (){
  Serial.println("Make #Shot ...");
  CAM_MODEL.shutterNow();
}

/////////////////////////////////////////////////////////////
// R U N H O M E : moves the head to the homing position
/////////////////////////////////////////////////////////////
void runHome (){
  //Set Direction //test if needed:  digitalWrite(X_DIR_PIN , HIGH);
  Serial.println("Moving #Home ...");

  // Set target Speed
  xStepper.setMaxSpeed(MAX_SPEED); // with vMax
  xStepper.setSpeed(MAX_SPEED);	
  gStepper.setMaxSpeed(MAX_SPEED);
  gStepper.setSpeed(MAX_SPEED);
  nStepper.setMaxSpeed(MAX_SPEED);
  nStepper.setSpeed(MAX_SPEED);
  fStepper.setMaxSpeed(MAX_SPEED);
  fStepper.setSpeed(MAX_SPEED);
  zStepper.setMaxSpeed(MAX_SPEED);
  zStepper.setSpeed(MAX_SPEED);

  //Move
 //TEST: decomment this loop 
  while(!digitalRead(X_MAX_PIN) && !digitalRead(X_MIN_PIN)){
    xStepper.runSpeed();
    gStepper.runSpeed();
    nStepper.runSpeed();
    fStepper.runSpeed();
    zStepper.runSpeed();
  }

  //set Pos to 0
  xStepper.setCurrentPosition(0);
  gStepper.setCurrentPosition(0);
  nStepper.setCurrentPosition(0);
  fStepper.setCurrentPosition(0);
  zStepper.setCurrentPosition(0);

}

/////////////////////////////////////////////////////////////
// S E T S T E P P E R D A T A : Set Acceleration (and Speed)
/////////////////////////////////////////////////////////////
void setStepperData (int iAccel){
  xStepper.setAcceleration(iAccel);
  gStepper.setAcceleration(iAccel);
  nStepper.setAcceleration(iAccel);
  fStepper.setAcceleration(iAccel);
  zStepper.setAcceleration(iAccel);
}

/////////////////////////////////////////////////////////////
// A C T I V A T E D R I V E R S : Set Acceleration and Speed
/////////////////////////////////////////////////////////////
void activateDrivers (boolean state){
  if (state){
    Serial.println("Drivers enabled ...");

    digitalWrite(X_ENABLE_PIN, LOW);  //LOW = an!
    digitalWrite(G_ENABLE_PIN, LOW);
    digitalWrite(N_ENABLE_PIN, LOW);
    digitalWrite(F_ENABLE_PIN, LOW);
    digitalWrite(Z_ENABLE_PIN, LOW);
  }
  else{
    Serial.println("Drivers desabled ...");

    digitalWrite(X_ENABLE_PIN, HIGH);  //HIGH = AUS!
    digitalWrite(G_ENABLE_PIN, HIGH);
    digitalWrite(N_ENABLE_PIN, HIGH);
    digitalWrite(F_ENABLE_PIN, HIGH);
    digitalWrite(Z_ENABLE_PIN, HIGH);
  }
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
  pinMode(X_MAX_PIN, INPUT); // INPUT_PULLUP??
  pinMode(X_MIN_PIN, INPUT); // INPUT_PULLUP??
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
}


