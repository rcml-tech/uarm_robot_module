#include <EEPROM.h>
#include <UF_uArm.h>

int  heightTemp  = 0, stretchTemp = 0, rotationTemp = 0, handRotTemp = 0;
char stateMachine = 0, counter = 0;
char dataBuf[8] = {0};
byte rxBuf = 0;

UF_uArm uarm;           // initialize the uArm library 

void setup(void) {
  Serial.begin(9600);
  uarm.init();          // initialize the uArm position
  uarm.setServoSpeed(SERVO_R, 50);  // 0=full speed, 1-255 slower to faster
  uarm.setServoSpeed(SERVO_L, 50);  // 0=full speed, 1-255 slower to faster
  uarm.setServoSpeed(SERVO_ROT, 50); // 0=full speed, 1-255 slower to faster
}

void loop(void) {
  uarm.calibration();   // if corrected, you could remove it, no harm though
  if (Serial.available()) {
    rxBuf = Serial.read();
    Serial.print("rxBuf = " + String(rxBuf) + "\n");
    
    if (stateMachine == 0) {
      stateMachine = rxBuf == 0xFF ? 1 : 0;
    } else if (stateMachine == 1) {
      if (rxBuf == 0xEE) {
        stateMachine = 2;
      } else if (rxBuf == 0xDD) {
        stateMachine = 3;
      } else {
        stateMachine = 0;
      }
    } else if (stateMachine == 2) {
      dataBuf[counter++] = rxBuf;
      if(counter > 7) {
        stateMachine = 0;
        counter=0;
        
        *((char *)(&stretchTemp )  )  = dataBuf[1]; 
        *((char *)(&stretchTemp )+1)  = dataBuf[0]; 
        *((char *)(&heightTemp  )  )  = dataBuf[3]; 
        *((char *)(&heightTemp  )+1)  = dataBuf[2]; 
        *((char *)(&rotationTemp)  )  = dataBuf[5];
        *((char *)(&rotationTemp)+1)  = dataBuf[4]; 
        *((char *)(&handRotTemp )  )  = dataBuf[7]; 
        *((char *)(&handRotTemp )+1)  = dataBuf[6]; 
        
        Serial.print("stretch = " + String(stretchTemp) + "; height = " + String(heightTemp) + "; rotation = " + String(rotationTemp) + "; handRot = " + String(handRotTemp) + ";\n");
        
        uarm.setPosition(stretchTemp, heightTemp, rotationTemp, handRotTemp);
      }
    } else if (stateMachine == 3) {
      stateMachine = 0;
      if (rxBuf) {
        uarm.gripperCatch();
        Serial.print("gripperCatch\n");
      } else {
        uarm.gripperRelease();
        Serial.print("gripperRelease\n");
      }
    }
  }
  uarm.gripperDetach();
}
