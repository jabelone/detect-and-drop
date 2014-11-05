const boolean startArmed = true;
const boolean runChecks = false;

//Include all of the libraries required for the code
#include <SPI.h>
#include <Pixy.h>
#include <Servo.h>
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_PCD8544.h>
#include <LCD5110_Graph.h>

// Must use these pins for hardware SPI | SCLK - pin 13 on a Uno | DIN - pin 11 on a Uno
//Adafruit_PCD8544 display = Adafruit_PCD8544(DC, CE/CS, RST);
Adafruit_PCD8544 display = Adafruit_PCD8544(5, 4, 3);

//Initialise our objects for the two servos and the pixy
Pixy pixy;
Servo servo1;
Servo servo2;

//Define our constants
const int armTrig = 1800;
const int armIn = 8;
const int lineDelay = 250;  //Delay between each 'corner' on lcd test
const int closed1 = 1;  //Set the closed angle for servo 1
const int closed2 = 20;  //Set the closed angle for servo 2
const int open1 = 160;  //Set the open angle for servo 1
const int open2 = 160;  //Set the open angle for servo 2
const int servoPin1 = 10;  //Set the pins for servo 1
const int servoPin2 = 9;  //Set the pins for servo 2
const int delayCheck = 60;  //About 3 frames
const unsigned long deadTime = 5000;  // 5 secs of dead time after a bomb bay door closes
const unsigned long openTime = 2000;  // 2 secs of time for bomb bay doors to remain open

// define our state variables
int packagesDropped = 0;
int doorState = 0;  // 0 = start, 1 = left door open; 2 = shut; 3 = right door open; 4 = shut; 5 = both doors open; 6 = shut and complete
unsigned long nextAction = 0; // time on the millis() clock that the next timed action can occur; 0 = no next timed action

void setup() 
{ 
  pinMode(2, OUTPUT);
  digitalWrite(2, HIGH);  //Controls the LCD backlight

  Serial.begin(9600);  //Set-up serial at 9600 baud
  Serial.print("Initialising devices...\n");
  display.begin();  //Initialise the LCD
  display.setContrast(55);  //Change this to suit your LCD (usually between 40 and 60)
  display.clearDisplay();  //Clear the LCD display buffer for good practice
  pixy.init();  //Initialise the pixy
  servo1.attach(servoPin1);  //Attach the servo pins to servo object 1
  servo2.attach(servoPin2);  //Attach the servo pins to servo object 2

    if(runChecks) {
    Serial.print("LCD:    OK!\n");
    display.setTextSize(1);
    display.setTextColor(BLACK);
    display.setCursor(0, 0);
    display.println("LCD:     OK!");
    display.display();

    Serial.print("Pixy:    OK!\n");
    display.setTextSize(1);
    display.setTextColor(BLACK);
    display.setCursor(0, 16);
    display.println("Pixy:    OK!");
    display.display();

    Serial.print("Servos:  OK!\n");
    display.setTextSize(1);
    display.setTextColor(BLACK);
    display.setCursor(0, 32);
    display.println("Servos:  OK!");
    display.display();
    delay(2500);
    clearScreen();

    display.setTextSize(1);
    display.setTextColor(BLACK);
    display.setCursor(0, 24);
    display.println("Starting LCD  test...");
    display.display();
    testdrawline();  //Draw test lines (used as screen test)
    delay(500);
    clearScreen();
  }

  //Open both doors
  Serial.print("Opening Doors.\n");

  display.setTextSize(1);
  display.setTextColor(BLACK);
  display.setCursor(0, 0);
  display.println("Opening Doors");
  display.display();
  delay(500);

  both(true, 2000);  //Open both doors
  delay(500);

  display.setTextSize(1);
  display.setTextColor(BLACK);
  display.setCursor(0, 8);
  display.println("Done.");
  display.display();
  delay(500);

  //Close both doors
  Serial.print("Closing Doors.\n");

  display.setTextSize(1);
  display.setTextColor(BLACK);
  display.setCursor(0, 24);
  display.println("Closing Doors");
  display.display();
  delay(500);

  both(false, 2000); //Close both doors
  delay(500);
  display.setTextSize(1);
  display.setTextColor(BLACK);
  display.setCursor(0, 32);
  display.println("Done.");
  display.display();
  delay(500);
  clearScreen();

  Serial.print("Setup code completed! WARNING Detect and drop now active!");
  digitalWrite(2, HIGH);
  display.setTextSize(1);
  display.setTextColor(BLACK);
  display.setCursor(0, 0);
  display.println("Setup code    completed!");
  display.display();
  delay(750);
  clearScreen();
  digitalWrite(2, LOW);
  delay(250);

  digitalWrite(2, HIGH);
  display.setTextSize(1);
  display.setTextColor(BLACK);
  display.setCursor(0, 0);
  display.println("Setup code    completed!");
  display.display();
  delay(750);
  clearScreen();
  digitalWrite(2, LOW);
  delay(250);

  digitalWrite(2, HIGH);
  display.setTextSize(1);
  display.setTextColor(BLACK);
  display.setCursor(0, 0);
  display.println("Setup code    completed!");
  display.display();
  delay(750);
  digitalWrite(2, HIGH);

  clearScreen();
  display.setTextSize(1);
  display.setTextColor(BLACK);
  display.setCursor(0, 0);
  display.println("Version: 1.1");

  display.setCursor(0, 8);
  char stringTemp[32];
  sprintf(stringTemp, "Dropped: %d\n", packagesDropped); 
  display.setTextSize(1);
  display.setTextColor(BLACK);
  display.println(stringTemp);
  display.display();
}

void loop()
{ 

  if(pulseIn(armIn, HIGH, 25000) > armTrig || startArmed) {

    uint16_t blocks;

    if((millis() % 500)) {
      Serial.print("millis(): ");
      Serial.print(millis());
      Serial.print("\t  |  nextAction: ");
      Serial.println(nextAction);
    }

    blocks = pixy.getBlocks();  //Pull new information from the pixy

    if(millis() % 100 == 0) {

      if(nextAction == 0) {
        display.fillRect(0, 16, 84, 48, WHITE);
        display.setTextSize(1);
        display.setTextColor(BLACK);
        display.setCursor(0, 24);
        display.println("Now looking   for outback   joe!");

        if(packagesDropped == 3) {
          display.fillRect(0, 8, 84, 48, WHITE);
          display.setTextSize(1);
          display.setTextColor(BLACK);
          display.setCursor(0, 24);
          display.println("All packages  dropped!      Congrats!");
        }
      }
      char stringTemp[32];
      display.fillRect(0, 8, 84, 16, WHITE);
      display.setCursor(0, 8);
      sprintf(stringTemp, "Dropped: %d\n", packagesDropped); 
      display.setTextSize(1);
      display.setTextColor(BLACK);
      display.println(stringTemp);
      display.display();
    }

    if(blocks && !nextAction)  //If the blocks object exists and we aren't waiting for deadtime
    {    
      if (pixy.blocks[1].signature == 1)  {  //If the largest detected object is from colour signature 1
        delay(delayCheck);  //Wait for about 3 frames (Pixy is 50 FPS)
        if (pixy.blocks[1].signature == 1)  {  //Check again if the largest detected object is from colour signature 1
          Serial.print("Detected an object from signature 1!\n");
          switch(doorState) {
          case 0:
            display.fillRect(0, 16, 84, 48, WHITE);
            display.setTextSize(1);
            display.setTextColor(BLACK);
            display.setCursor(0, 24);
            display.println("Dropping 1st  Package");
            display.display();
            left(true, openTime);
            Serial.print("Dropped package one.  Two remaining.\n");
            packagesDropped++;
            doorState++;
            Serial.print(doorState);
            break;

          case 1:
            //Should never occur
            break;

          case 2:
            display.fillRect(0, 16, 84, 48, WHITE);
            display.setTextSize(1);
            display.setTextColor(BLACK);
            display.setCursor(0, 24);
            display.println("Dropping 2nd  Package");
            display.display();
            right(true, openTime);
            Serial.print("Dropped package two.  One remaining.\n");
            packagesDropped++;
            doorState++;
            Serial.print(doorState);
            break;

          case 3:
            // should never occur
            break;

          case 4:
            display.fillRect(0, 16, 84, 48, WHITE);
            display.setTextSize(1);
            display.setTextColor(BLACK);
            display.setCursor(0, 24);
            display.println("Dropping 3rd  Package");
            display.display();
            both(true, 2000);
            Serial.print("Dropped package three.  All gone!\n");
            packagesDropped++;
            doorState++;
            Serial.print(doorState);
            break;

          case 5:
            // should never occur
            break;

          default:
            // delay(1000);
            // ignore everything...state = 6
            Serial.print(doorState);
            break;
          }
        }
      }
    }

    // deal with timed actions
    if(millis() > nextAction) {
      nextAction = 0;
      switch(doorState) {
      case 1:
        left(false, deadTime); 
        doorState++;
        break;

      case 3:
        right(false, deadTime);
        doorState++;
        break;

      case 5:
        both(false, deadTime);
        doorState++;
        break;

      default:
        // do nothing since you already zeroed nextAction
        break;
      }

    }
  }
  else {
    display.fillRect(0, 8, 84, 48, WHITE);
    display.setTextSize(2);
    display.setTextColor(BLACK);
    display.setCursor(0, 0);
    display.println("       Not    Armed!");
    display.display();
    delay(750);
    display.fillRect(0, 8, 84, 48, WHITE);
    display.display();
    delay(250);
  }
}
