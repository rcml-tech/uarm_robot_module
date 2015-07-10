#include <UF_uArm_Metal.h>
#include <VarSpeedServo.h>
#include <EEPROM.h>

<<<<<<< HEAD
VarSpeedServo servoL;
VarSpeedServo servoR;
VarSpeedServo servoRot;
VarSpeedServo servoHandRot;
VarSpeedServo servoHand;
=======
int  heightTemp  = 0, stretchTemp = 0, rotationTemp = 0, handRotTemp = 0;
char stateMachine = 0, counter = 0;
char dataBuf[8] = {0};
byte rxBuf = 0;
>>>>>>> bea2b19c183c0fccc0dbb2e7aeadd60bfa027926

UF_uArm uarm;

String msg;

void setup() {
  Serial.begin(9600);
<<<<<<< HEAD
  pinMode(BTN_D4,   INPUT);  digitalWrite(BTN_D4,   HIGH);
  pinMode(BTN_D7,   INPUT);  digitalWrite(BTN_D7,   HIGH);
  pinMode(BUZZER,   OUTPUT); digitalWrite(BUZZER,   LOW);
  pinMode(PUMP_EN,  OUTPUT); digitalWrite(PUMP_EN,  LOW);
  pinMode(VALVE_EN, OUTPUT); digitalWrite(VALVE_EN, LOW);
  servoL.attach(SERVO_L, D090M_SERVO_MIN_PUL, D090M_SERVO_MAX_PUL);
  servoR.attach(SERVO_R, D090M_SERVO_MIN_PUL, D090M_SERVO_MAX_PUL);
  servoRot.attach(SERVO_ROT, D090M_SERVO_MIN_PUL, D090M_SERVO_MAX_PUL);
  servoHand.attach(SERVO_HAND, D009A_SERVO_MIN_PUL, D009A_SERVO_MAX_PUL);
  servoHandRot.attach(SERVO_HAND_ROT, D009A_SERVO_MIN_PUL, D009A_SERVO_MAX_PUL);
  servoHand.write(HAND_ANGLE_OPEN, 0, true);
  servoHand.detach();
}

void loop()
{
  
  while(Serial.available() > 0)
  { 
    char now = Serial.read();
    if(now == ';')
    {
      switch(msg.charAt(0))
      {
        case 'T':
          uarm.alert(1, 1000, 0);
          while(1)
          {
            if(!digitalRead(BTN_D4))
            {
              uarm.alert(2, 250, 250);
              delay(3000);
              
              if(servoL.attached() && servoR.attached() && servoRot.attached() && servoHandRot.attached())
              {
                servoL.detach();
                servoR.detach();
                servoRot.detach();
                servoHandRot.detach();
              }
              else
              {
                servoL.attach(SERVO_L, D090M_SERVO_MIN_PUL, D090M_SERVO_MAX_PUL);
                servoR.attach(SERVO_R, D090M_SERVO_MIN_PUL, D090M_SERVO_MAX_PUL);
                servoRot.attach(SERVO_ROT, D090M_SERVO_MIN_PUL, D090M_SERVO_MAX_PUL);
                servoHand.attach(SERVO_HAND, D009A_SERVO_MIN_PUL, D009A_SERVO_MAX_PUL);
                servoHandRot.attach(SERVO_HAND_ROT, D009A_SERVO_MIN_PUL, D009A_SERVO_MAX_PUL);
                servoHand.write(HAND_ANGLE_OPEN, 0, true);
                servoHand.detach();
                break;
              }
            }

            if(!digitalRead(BTN_D7))
            {
              delay(500);
              Serial.print("angle for ROTATION - ");
              Serial.println(
                              map(
                                constrain(uarm.readAngle(SERVO_ROT), SERVO_MIN, SERVO_MAX),
                                SERVO_MIN,
                                SERVO_MAX,
                                D090M_SERVO_MIN_PUL, 
                                D090M_SERVO_MAX_PUL
                                )
                            );
              Serial.print("angle for L - ");
              Serial.println(
                              map(
                                constrain(uarm.readAngle(SERVO_L), SERVO_MIN, SERVO_MAX),
                                SERVO_MIN,
                                SERVO_MAX,
                                D090M_SERVO_MIN_PUL,
                                D090M_SERVO_MAX_PUL
                                )
                            );
              Serial.print("angle for R - ");
              Serial.println(
                              map(
                                constrain(uarm.readAngle(SERVO_R), SERVO_MIN, SERVO_MAX),
                                SERVO_MIN,
                                SERVO_MAX,
                                D090M_SERVO_MIN_PUL,
                                D090M_SERVO_MAX_PUL
                                )
                            );
              Serial.print("angle for HAND ROTATION - ");
              Serial.println(
                              map(
                                constrain(uarm.readAngle(SERVO_HAND_ROT), SERVO_MIN, SERVO_MAX),
                                SERVO_MIN,
                                SERVO_MAX,
                                D009A_SERVO_MIN_PUL,
                                D009A_SERVO_MAX_PUL
                                )
                            );
              Serial.println("");
            }
    
//            if(!digitalRead(BTN_D7))
//            {
//              delay(500);
//              Serial.print("angle for ROTATION - ");
//              Serial.println(uarm.readAngle(SERVO_ROT), DEC);
//              Serial.print("angle for L - ");
//              Serial.println(uarm.readAngle(SERVO_L), DEC);
//              Serial.print("angle for R - ");
//              Serial.println(uarm.readAngle(SERVO_R), DEC);
//              Serial.print("angle for HAND ROTATION - ");
//              Serial.println(uarm.readAngle(SERVO_HAND_ROT), DEC);
//              Serial.println("");
//            }
              
          }
        break;
        case 'G':
          if(msg.charAt(1) == '1')
          {
            uarm.gripperCatch();
          }
          else
          {
            uarm.gripperRelease();
            uarm.gripperDirectDetach();
          }
        break;
        case 'M':
          byte count = 0;
          String note = "";
          unsigned int mass[8];
          int MSGlength = msg.length() - 1;
          for(int i = 1; i <= MSGlength; i++)
          {
            if( (msg.charAt(i) >= '0') && (msg.charAt(i) <= '9') )
            {
              note += msg.charAt(i);
            }
            if( (msg.charAt(i) == ',') || (i == MSGlength) )
            {
              mass[count] = note.toInt();
              count++;
              note = "";
            }
          }

          for(byte a = 0; a < 8; a++)
          {
            Serial.print(mass[a]);
            Serial.print(" ");
          }
          Serial.println("");
          
          servoRot.write(mass[0],mass[1]);
          servoL.write(mass[2],mass[3]);
          servoR.write(mass[4],mass[5]);
          servoHandRot.write(mass[6],mass[7]);
        break;
=======
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
>>>>>>> bea2b19c183c0fccc0dbb2e7aeadd60bfa027926
      }
      msg = "";
    }
    else
    {
      msg += now;
    }
    
  }
<<<<<<< HEAD
  
=======
  uarm.gripperDetach();
>>>>>>> bea2b19c183c0fccc0dbb2e7aeadd60bfa027926
}
