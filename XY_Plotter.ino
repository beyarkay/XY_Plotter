#include <Servo.h>
#include <Stepper.h>


// Possibly a good explanation of the AccelStepper Library: https://42bots.com/tutorials/28byj-48-stepper-motor-with-uln2003-driver-and-arduino-uno/
// Define step constants. Only used for AccelSteppers
#define FULLSTEP 4  // Used for running AccelSteppers in half steps
#define HALFSTEP 8  // Used for running AccelSteppers in half steps

// Define stepper pins (2 steppers used, named S and T)
#define stepper_s_pin_1  8
#define stepper_s_pin_2  7
#define stepper_s_pin_3  6
#define stepper_s_pin_4  5

#define stepper_t_pin_1  12
#define stepper_t_pin_2  11
#define stepper_t_pin_3  10
#define stepper_t_pin_4  9

Servo pen_servo;
const int SERVO_PIN = 3;
const int PEN_DOWN_POS = 175;
const int PEN_UP_POS = 145;
const int DELAY_BETWEEN_STEPS = 2; //minimum of 2 ms between stepper motor steps
// Gearing ratio inside the 28BYJ-48 stepper is 63.68395:1, resulting in a funky steps per revolution
const int STEPS_PER_REVOLUTION = 4076;


// The sequence 1-3-2-4 is required for proper sequencing of 28BYJ-48 stepper motors
Stepper *steppers[2];
Stepper stepper_s(STEPS_PER_REVOLUTION, stepper_s_pin_1, stepper_s_pin_3, stepper_s_pin_2, stepper_s_pin_4);
Stepper stepper_t(STEPS_PER_REVOLUTION, stepper_t_pin_1, stepper_t_pin_3, stepper_t_pin_2, stepper_t_pin_4);


long last_active_at = millis();
long deltas[2]; //array to be used for target movements for each stepper

void penDown() {
  /*
     Move the pen down, to position PEN_DOWN_POS
  */
  Serial.println("Pen down, (and LED on)");
  pen_servo.write(PEN_DOWN_POS);
  digitalWrite(LED_BUILTIN, HIGH);
  last_active_at = millis();
}

void penUp() {
  /*
     Move the pen up, to position PEN_UP_POS
  */
  Serial.println("Pen up, LED off");
  pen_servo.write(PEN_UP_POS);
  digitalWrite(LED_BUILTIN, LOW);
  last_active_at = millis();
}

void turnOff() {
  digitalWrite(stepper_s_pin_1, LOW);
  digitalWrite(stepper_s_pin_2, LOW);
  digitalWrite(stepper_s_pin_3, LOW);
  digitalWrite(stepper_s_pin_4, LOW);

  digitalWrite(stepper_t_pin_1, LOW);
  digitalWrite(stepper_t_pin_2, LOW);
  digitalWrite(stepper_t_pin_3, LOW);
  digitalWrite(stepper_t_pin_4, LOW);
  last_active_at = millis();
}

int readByte() {
  /*
     Wait until a byte is available, then read and return it
  */
  while (Serial.available() == 0);
  return Serial.read();
}

void moveSteppers() {
  // Sorry stu, I renamed small and big to small and large so they'd be the same length...
  int index_of_small, index_of_large;


  // Figure out the stepper with more distance to go, so that it moves faster
  // This allows them to arrive at their destinations at the same time
  if (abs(deltas[0]) < abs(deltas[1])) {
    index_of_small = 0;
    index_of_large = 1;
  } else {
    index_of_small = 1;
    index_of_large = 0;
  }
  // TODO what happens if deltas[index_of_large] == 0?
  // ratio * further_distance = shorter_distance
  float ratio = 0;
  if (deltas[index_of_large] != 0) {
    ratio = (float) abs(deltas[index_of_small]) / (float) abs(deltas[index_of_large]);
  }


  float index_of_smallExpectedStepsComplete = 0.0;
  int stepsComplete[2] = {}; // initialise index_of_small and index_of_small to 0

  while (abs(stepsComplete[index_of_large]) < abs(deltas[index_of_large])) {

    //Do one step for the larger stepper
    if (deltas[index_of_large] > 0) {
      stepsComplete[index_of_large]++;
      (*steppers[index_of_large]).step(1);
    } else {
      stepsComplete[index_of_large]--;
      (*steppers[index_of_large]).step(-1);
    }

    //Step index_of_smaller one
    if (deltas[index_of_small] > 0) {
      index_of_smallExpectedStepsComplete += ratio;
      // Make the shorter distance stepper do a step if total of previous fractional steps add to be a whole number
      if ((int) index_of_smallExpectedStepsComplete > stepsComplete[index_of_small]) {
        stepsComplete[index_of_small]++;
        (*steppers[index_of_small]).step(1);
      }
    } else {
      index_of_smallExpectedStepsComplete -= ratio;
      // Make the shorter distance stepper do a step if total of previous fractional steps add to be a whole number
      if ((int) index_of_smallExpectedStepsComplete < stepsComplete[index_of_small]) {
        stepsComplete[index_of_small]--;
        (*steppers[index_of_small]).step(-1);
      }
    }
    // Wait for the steppers to complete their steps
    delay(DELAY_BETWEEN_STEPS);
  }

  //Get the shorter stepper to finish any remaining steps
  while (abs(stepsComplete[index_of_small]) < abs(deltas[index_of_small])) {
    if (deltas[index_of_small] > 0) {
      (*steppers[index_of_small]).step(1);
      stepsComplete[index_of_small]++;
    } else {
      (*steppers[index_of_small]).step(-1);
      stepsComplete[index_of_small]--;
    }

    delay(DELAY_BETWEEN_STEPS);
  }
  last_active_at = millis();
}

void setup() {
  /*
     Start the Serial
     Configure output pins
     Initialise the steppers
     Move the pen up
  */
  Serial.begin(9600);
  pinMode(LED_BUILTIN, OUTPUT);
  pen_servo.attach(SERVO_PIN);

  steppers[0] = &stepper_s;
  steppers[1] = &stepper_t;

  penUp();
}

void loop() {
  /*
     Each loop, read the incoming bytes:
     - pu: Move the pen up
     - pd: Move the pen down
     - m SSSS, TTTT: move the S and T steppers by SSSS and TTTT respectively
     - o: Turn off all the coils for the stepper motors to stop overheating.
          Causes loss of control while power is off
  */

  if (millis() - last_active_at > 30000) {
    turnOff();
  }

  int incomingByte = readByte();

  switch (incomingByte) {
    case 'p':
      Serial.print("Pen control command: ");
      incomingByte = readByte();

      if (incomingByte == 'u') {
        penUp();
      } else if (incomingByte == 'd') {
        penDown();
      } else {
        Serial.print("Second byte ('");
        Serial.write(incomingByte); // Use write for sending single bytes
        Serial.println("') of up/down command is invalid");
      }
      break;
    case 'm':
      Serial.print("Received move command: ");

      deltas[0] = Serial.parseInt();
      deltas[1] = Serial.parseInt();

      Serial.print("s, t = ");
      Serial.print(deltas[0], DEC);
      Serial.print(", ");
      Serial.println(deltas[1], DEC);

      moveSteppers();
      break;
    case 'o':
      // OFF command: Write all the stepper motor pins to LOW, to stop them overheating
      Serial.println("Received turn_off command");
      turnOff();
      break;
    default:
      Serial.print("First byte ('");
      Serial.write(incomingByte); // Use write for sending single bytes
      Serial.println("') of command is invalid");
      break;
  }
  readByte(); //ignore the newline character

}
