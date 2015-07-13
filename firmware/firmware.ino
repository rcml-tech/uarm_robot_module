#include <UF_uArm_Metal.h>
#include <VarSpeedServo.h>
#include <EEPROM.h>

VarSpeedServo servoL;
VarSpeedServo servoR;
VarSpeedServo servoRot;
VarSpeedServo servoHandRot;
VarSpeedServo servoHand;

UF_uArm uarm;

String msg;

void setup() {
  Serial.begin(9600);
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

void readServo(int servo_pin, int min_pul, int max_pul) {
  Serial.println(
    map(
      constrain(uarm.readAngle(servo_pin), SERVO_MIN, SERVO_MAX),
      SERVO_MIN,
      SERVO_MAX,
      min_pul,
      max_pul
    )
  );
}

void loop() {
  while (Serial.available()) { 
    char now = Serial.read();
    if(now != ';') {
      msg += now;
    } else {
      switch (msg[0]) {
        case 'T': {
          uarm.alert(1, 1000, 0);
          bool isAttached = true;
          while (1) {
            if (!digitalRead(BTN_D4)) {
              uarm.alert(2, 250, 250);
              delay(3000);
              
              if (isAttached) {
                isAttached = false;
                servoL.detach();
                servoR.detach();
                servoRot.detach();
                servoHandRot.detach();
              } else {
                Serial.print(";");
                servoL.attach(SERVO_L, D090M_SERVO_MIN_PUL, D090M_SERVO_MAX_PUL);
                servoR.attach(SERVO_R, D090M_SERVO_MIN_PUL, D090M_SERVO_MAX_PUL);
                servoRot.attach(SERVO_ROT, D090M_SERVO_MIN_PUL, D090M_SERVO_MAX_PUL);
                servoHandRot.attach(SERVO_HAND_ROT, D009A_SERVO_MIN_PUL, D009A_SERVO_MAX_PUL);
                break;
              }
            }

            if (!isAttached) {
              if (!digitalRead(BTN_D7)) {
                delay(500);
                Serial.print("SERVO_ROT - ");
                readServo(SERVO_ROT, D090M_SERVO_MIN_PUL, D090M_SERVO_MAX_PUL);
                
                Serial.print("SERVO_LEFT - ");
                readServo(SERVO_L, D090M_SERVO_MIN_PUL, D090M_SERVO_MAX_PUL);
              
                Serial.print("SERVO_RIGHT - ");
                readServo(SERVO_R, D090M_SERVO_MIN_PUL, D090M_SERVO_MAX_PUL);

                Serial.print("SERVO_HAND_ROT - ");
                readServo(SERVO_HAND_ROT, D009A_SERVO_MIN_PUL, D009A_SERVO_MAX_PUL);
                Serial.println("");
                Serial.print("|");
              }
            }
          }
          break;
        }
        case 'G': {
          if (msg[1] == '1') {
            uarm.gripperCatch();
          } else {
            uarm.gripperRelease();
            uarm.gripperDirectDetach();
          }
          break;
        }
        case 'M': {
          byte count = 0;
          String note = "";
          unsigned int mass[8];
          
          int MSGlength = msg.length() - 1;
          for (int i = 1; i <= MSGlength; i++) {
            if ((msg[i] >= '0') && (msg[i] <= '9')) {
              note += msg[i];
            }
            if ((msg[i] == ',') || (i == MSGlength)) {
              mass[count] = note.toInt();
              count++;
              note = "";
            }
          }

          servoRot.write(mass[0], mass[1]);
          servoL.write(mass[2], mass[3]);
          servoR.write(mass[4], mass[5]);
          servoHandRot.write(mass[6], mass[7]);
          break;
        }
      }
      msg = "";
    }
  }
}
