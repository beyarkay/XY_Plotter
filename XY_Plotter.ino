#include <Servo.h>
#include <Stepper.h>

// Define step constants
#define FULLSTEP 4
#define HALFSTEP 8

// Define Motor Pins (2 Motors used)
#define motorPin1  5     // Blue   - 28BYJ48 pin 1
#define motorPin2  6     // Pink   - 28BYJ48 pin 2
#define motorPin3  7    // Yellow - 28BYJ48 pin 3
#define motorPin4  8    // Orange - 28BYJ48 pin 4

#define motorPin5  9     // Blue   - 28BYJ48 pin 1
#define motorPin6  10     // Pink   - 28BYJ48 pin 2
#define motorPin7  11     // Yellow - 28BYJ48 pin 3
#define motorPin8  12     // Orange - 28BYJ48 pin 4

Servo myservo;
const int SERVO_PIN = 3;
const int PEN_DOWN_POS = 175;
const int PEN_UP_POS = 145;
const int DELAY_BETWEEN_STEPS = 4; //minimum of 4 ms between stepper motor steps

// The sequence 1-3-2-4 is required for proper sequencing of 28BYJ48
Stepper *steppers[2];
Stepper stepper1(200, motorPin1, motorPin3, motorPin2, motorPin4);
Stepper stepper2(200, motorPin5, motorPin7, motorPin6, motorPin8);

long deltas[2]; //array to be used for target movements for each stepper

void penDown() {
  Serial.println("Pen down");
  myservo.write(PEN_DOWN_POS);
  digitalWrite(LED_BUILTIN, HIGH);
}

void penUp() {
  Serial.println("Pen up");
  myservo.write(PEN_UP_POS);
  digitalWrite(LED_BUILTIN, LOW);

}

int readByte() {
  while(Serial.available() == 0); //Wait for byte
  return Serial.read();
}

void moveSteppers() {
  
  int small, big;
  if (abs(deltas[0]) < abs(deltas[1])) {
    small = 0;
    big = 1;
  } else {
    small = 1;
    big = 0;
  }

  float ratio = 0;
  
  if (deltas[big] != 0) {
    ratio = (float)abs(deltas[small]) / (float)abs(deltas[big]);
  }
  
  float smallExpectedStepsComplete = 0.0;
  int stepsComplete[2];
  stepsComplete[small] = 0;
  stepsComplete[big] = 0;
  while (abs(stepsComplete[big]) < abs(deltas[big])) {

    //Step bigger one
    if (deltas[big] > 0) {
      stepsComplete[big]++;
      (*steppers[big]).step(1);
    } else {
      stepsComplete[big]--;
      (*steppers[big]).step(-1);
    }

    //Step smaller one
    if (deltas[small] > 0) {
      smallExpectedStepsComplete += ratio;
      if ((int)smallExpectedStepsComplete > stepsComplete[small]) {
        stepsComplete[small]++;
        (*steppers[small]).step(1);
      }
    } else {
      smallExpectedStepsComplete -= ratio;
      if ((int)smallExpectedStepsComplete < stepsComplete[small]) {
        stepsComplete[small]--;
        (*steppers[small]).step(-1);
      }
    }
    
    delay(DELAY_BETWEEN_STEPS);
  }

  //Finish any remaining steps
  while (abs(stepsComplete[small]) < abs(deltas[small])) {
    if (deltas[small] > 0) {
      (*steppers[small]).step(1);
      stepsComplete[small]++;
    } else {
      (*steppers[small]).step(-1);
      stepsComplete[small]--;
    }

    delay(DELAY_BETWEEN_STEPS);
  }
}

void setup() {
  Serial.begin(9600);
  pinMode(LED_BUILTIN, OUTPUT);
  myservo.attach(SERVO_PIN);

  steppers[0] = &stepper1;
  steppers[1] = &stepper2;
  
  penUp();
}

void loop() {

  int incomingByte;
  incomingByte = readByte();

  if (incomingByte == 'p') {
    Serial.print("Received pen control command\n");
    incomingByte = readByte();

    if (incomingByte == 'u') {
      penUp();
    } else if (incomingByte == 'd') {
      penDown();
    } else {
      Serial.print("Second byte ('")
      Serial.print(incomingByte)
      Serial.print("') of up/down command is invalid\n");
    }
  } else if (incomingByte == 'm') {
    Serial.print("Received move command\n");

    deltas[1] = -Serial.parseInt();
    deltas[0] = Serial.parseInt();

    Serial.print("positions: ");
    Serial.print(-deltas[1], DEC);
    Serial.print(" ");
    Serial.print(deltas[0], DEC);
    Serial.print("\n");

    moveSteppers();
    
  } else if (incomingByte == 'o'){
    Serial.print("Received turn_off command\n");
    digitalWrite(motorPin1, LOW);
    digitalWrite(motorPin2, LOW);
    digitalWrite(motorPin3, LOW);
    digitalWrite(motorPin4, LOW);

    digitalWrite(motorPin5, LOW);
    digitalWrite(motorPin6, LOW);
    digitalWrite(motorPin7, LOW);
    digitalWrite(motorPin8, LOW);
    
  } else {
    Serial.print("First byte ('")
    Serial.print(incomingByte)
    Serial.print("') of command is invalid\n");
  }
  readByte(); //ignore newline

}
