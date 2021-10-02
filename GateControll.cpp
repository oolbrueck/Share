#include <printf.h>
#include <RH_ASK.h>
#include <AccelStepper.h>
#ifdef RH_HAVE_HARDWARE_SPI
#include <SPI.h> // Not actually used but needed to compile
#endif

#define isPressed(switch)   (digitalRead(switch) == 0)
#define button 5
#define limSwitchOne 6
#define limSwitchTwo 7
#define pulse 8
#define direction 9
#define enabled 4
RH_ASK driver;
AccelStepper stepper(1, pulse, direction);
long watchDog = 0;
boolean initWatchDog = false;


class State {
  public: static bool lastStateOpen;
  public: static bool lastStateClose;
  public: static bool stoppedWhileMoving;
  public: static void setLastStateOpen() { lastStateOpen = true; }
  public: static void setLastStateClose() { lastStateClose = false; }
  public: static void setStopppedWhileMoving(bool stopped) { stoppedWhileMoving = stopped; }
  public: static bool currentlyClose() { return isPressed(limSwitchOne); }
  public: static bool currentlyOpen() { return isPressed(limSwitchTwo); }
  public: static bool currentlyNeitherOpenNorClose() { return !(currentlyOpen() || currentlyClose());}
  public: static bool lastStateIsOpen() { return lastStateOpen; };
  public: static bool lastStateIsClose() { return lastStateClose; };
  public: static bool stoppedByButtonWhileMoving() { return stoppedWhileMoving; };
};

void setup() {

  pinMode(button, INPUT);
  pinMode(limSwitchOne, INPUT);
  pinMode(limSwitchTwo, INPUT);
  pinMode(pulse, OUTPUT);
  pinMode(direction, OUTPUT);
  pinMode(enabled, OUTPUT);

  #ifdef RH_HAVE_SERIAL
    Serial.begin(9600);   
  #endif
    if (!driver.init())
  #ifdef RH_HAVE_SERIAL
         Serial.println("init failed");
  #else
    ;
  #endif

  stepper.setMaxSpeed(100);
  stepper.setAcceleration(20);
  stepper.setEnablePin(enabled);
}

void loop() {

  if(requestedToMove()) {
    beginToMove();
  }
  if(State::currentlyNeitherOpenNorClose() && initWatchDog) { //start watchdog
    watchDog = millis();
    initWatchDog = true;
  }
  if((millis() - watchDog) > 30000) {                 //evaluate watchdog
      initWatchDog = false;
      watchDog = 0;
      beginToMove();
    }

}

bool requestedToMove() {
  return isPressed(button) || checkForRadioRequest();
}

int beginToMove() {
  long watchDog = millis();

  if(State::currentlyOpen()) {
    stepper.setMoveDirection(1);
  }
  if(State::currentlyClose()) {
    //stepper.setMoveDirection(1);
  }
  if(State::currentlyNeitherOpenNorClose()) {
    if(State::lastStateIsOpen()) {
      //stepper.setMoveDirection(1);
    }
    if(State::lastStateIsClose()) {
      //stepper.setMoveDirection(1);
    }
  }    

  while(!(isPressed(button) || isPressed(limSwitchOne) || isPressed(limSwitchTwo))) {
    stepper.run();
    if((millis() - watchDog) > 20000) {
      break; 
    }
  }
  if(isPressed(limSwitchOne)) {
    State::setLastStateOpen();
  }
  if(isPressed(limSwitchTwo)) {
    //State::lastStateOpen = true;
  }
  
}

bool checkForRadioRequest() {
    uint8_t buf[RH_ASK_MAX_MESSAGE_LEN];
    uint8_t buflen = sizeof(buf);

    if (driver.recv(buf, &buflen)) {
      int i;
      if(buf[0] == 0x68 && buf[1] == 0x65 && buf[2] == 0x6C && buf[3] == 0x6C && buf[4] == 0x6F) {
        switch (buf[5]) {
          case 0x00:
            //log
            return true;
          case 0x01:
            //log
            return true;
          case 0x02:
            //log
            return true;
          case 0x03:
            //log
            return true;
          case 0x04:
            //log
            return true;
          case 0x05:
            //log
            return true;
          case 0x06:
            //log
            return true;
          case 0x07:
            //log
            return true;
          case 0x08:
            //log
            return true;
          case 0x09:
            //log
            return true;
          case 0x0A:
            //log
            return true;
          default:
            Serial.println("unauthorized user");
            return false;
        }
      }
    driver.printBuffer("Got:", buf, buflen);
    return false;
    }
}
