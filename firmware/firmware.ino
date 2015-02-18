#include <EEPROM.h>
#include <UF_uArm.h>

byte  heightTemp  = 0, stretchTemp = 0, rotationTemp = 0, handRotTemp = 0;
char stateMachine = 0, counter = 0;
byte dataBuf[5] = {0};
byte rxBuf = 0;

UF_uArm uarm;           // initialize the uArm library 

void setup(void) {
  Serial.begin(9600);
  uarm.init();          // initialize the uArm position
  uarm.setServoSpeed(SERVO_R, 50);  // 0=full speed, 1-255 slower to faster
  uarm.setServoSpeed(SERVO_L, 50);  // 0=full speed, 1-255 slower to faster
  uarm.setServoSpeed(SERVO_ROT, 50); // 0=full speed, 1-255 slower to faster
  delay(500);
}

void loop(void) {
  uarm.calibration();   // if corrected, you could remove it, no harm though
  if (Serial.available()) {
    rxBuf = Serial.read(); 
    if (stateMachine == 0) {
      stateMachine = rxBuf == 0xFF ? 1 : 0;
    } else if (stateMachine == 1) {
      stateMachine = rxBuf == 0xEE ? 2 : 0;
    } else if (stateMachine == 1) {
      stateMachine = rxBuf == 0xDD ? 3 : 0;
    } else if (stateMachine == 2) {
      dataBuf[counter++] = rxBuf;
      if(counter > 4) {
        stateMachine = 0;
        counter=0;
        
        stretchTemp = dataBuf[0];
        heightTemp = dataBuf[1];
        rotationTemp = dataBuf[2];
        handRotTemp = dataBuf[3];
        
        uarm.setPosition(stretchTemp, heightTemp, rotationTemp, handRotTemp);
      }
    } else if (stateMachine == 3) {
      stateMachine = 0;
      if (rxBuf) {
        uarm.gripperCatch();
      } else {
        uarm.gripperRelease();
      }
    }
  }
}
