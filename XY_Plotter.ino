//TODO add in idle timer, that'll complain if left idle for too long
#include <Servo.h>
#include <Stepper.h>

// Define step constants
#define FULLSTEP 4
#define HALFSTEP 8

// Define Motor Pins (2 Motors used)
#define stepper_s_pin_1  5     // Blue    - 28BYJ48 pin 1
#define stepper_s_pin_2  6     // Pink    - 28BYJ48 pin 2
#define stepper_s_pin_3  7     // Yellow  - 28BYJ48 pin 3
#define stepper_s_pin_4  8     // Orange  - 28BYJ48 pin 4

#define stepper_t_pin_1  9      // Blue   - 28BYJ48 pin 1
#define stepper_t_pin_2  10     // Pink   - 28BYJ48 pin 2
#define stepper_t_pin_3  11     // Yellow - 28BYJ48 pin 3
#define stepper_t_pin_4  12     // Orange - 28BYJ48 pin 4

Servo pen_servo;
const int SERVO_PIN = 3;
const int PEN_DOWN_POS = 175;
const int PEN_UP_POS = 145;
const int DELAY_BETWEEN_STEPS = 4; //minimum of 4 ms between stepper motor steps

// The sequence 1-3-2-4 is required for proper sequencing of 28BYJ48
Stepper *steppers[2];
Stepper stepper_s(200, stepper_s_pin_1, stepper_s_pin_3, stepper_s_pin_2, stepper_s_pin_4);
Stepper stepper_t(200, stepper_t_pin_1, stepper_t_pin_3, stepper_t_pin_2, stepper_t_pin_4);

long deltas[2]; //array to be used for target movements for each stepper

void penDown() {
  /*
   * Move the pen down, to position PEN_DOWN_POS
   */
  Serial.println("Pen down, LED on");
  pen_servo.write(PEN_DOWN_POS);
  digitalWrite(LED_BUILTIN, HIGH);
}

void penUp() {
  /*
   * Move the pen up, to position PEN_UP_POS
   */
  Serial.println("Pen up, LED off");
  pen_servo.write(PEN_UP_POS);
  digitalWrite(LED_BUILTIN, LOW);
}

int readByte() {
  /*
   * Wait until a byte is available, then read and return it
   */
  while(Serial.available() == 0);
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
}

void setup() {
  /*
   * Start the Serial
   * Configure output pins
   * Initialise the steppers
   * Move the pen up
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
   * Each loop, read the incoming bytes:
   * - pu: Move the pen up
   * - pd: Move the pen down
   * - m SSSS, TTTT: move the S and T steppers by SSSS and TTTT respectively
   * - o: Turn off all the coils for the stepper motors to stop overheating. 
   *      Causes loss of control while power is off 
   */

//  int incomingByte;
//  incomingByte = readByte();
    int incomingByte = readByte();


  switch (incomingByte){
    case 'p':
      Serial.println("Received pen control command");
      incomingByte = readByte();

      if (incomingByte == 'u') {
        penUp();
      } else if (incomingByte == 'd') {
        penDown();
      } else {
        Serial.print("Second byte ('")
        Serial.print(incomingByte)
        Serial.println("') of up/down command is invalid");
      }
    case 'm':
      Serial.println("Received move command");

      deltas[1] = -Serial.parseInt();
      deltas[0] = Serial.parseInt();

      Serial.print("positions: ");
      Serial.print(-deltas[1], DEC);
      Serial.print(" ");
      Serial.println(deltas[0], DEC);

      moveSteppers();
    case 'o':
      // OFF command: Write all the stepper motor pins to LOW, to stop them overheating
      Serial.println("Received turn_off command");
      digitalWrite(stepper_s_pin_1, LOW);
      digitalWrite(stepper_s_pin_2, LOW);
      digitalWrite(stepper_s_pin_3, LOW);
      digitalWrite(stepper_s_pin_4, LOW);

      digitalWrite(stepper_t_pin_1, LOW);
      digitalWrite(stepper_t_pin_2, LOW);
      digitalWrite(stepper_t_pin_3, LOW);
      digitalWrite(stepper_t_pin_4, LOW);
    default:  
      Serial.print("First byte ('")                
      Serial.print(incomingByte)                   
      Serial.println("') of command is invalid");  
  }

//  if (incomingByte == 'p') {
//    Serial.println("Received pen control command");
//    incomingByte = readByte();
//
//    if (incomingByte == 'u') {
//      penUp();
//    } else if (incomingByte == 'd') {
//      penDown();
//    } else {
//      Serial.print("Second byte ('")
//      Serial.print(incomingByte)
//      Serial.println("') of up/down command is invalid");
//    }
//  } else if (incomingByte == 'm') {
//    Serial.print("Received move command\n");
//
//    deltas[1] = -Serial.parseInt();
//    deltas[0] = Serial.parseInt();
//
//    Serial.print("positions: ");
//    Serial.print(-deltas[1], DEC);
//    Serial.print(" ");
//    Serial.print(deltas[0], DEC);
//    Serial.print("\n");
//
//    moveSteppers();
//
//  } else if (incomingByte == 'o'){
//    // OFF command: Write all the stepper motor pins to LOW, to stop them overheating
//    Serial.print("Received turn_off command\n");
//    digitalWrite(stepper_s_pin_1, LOW);
//    digitalWrite(stepper_s_pin_2, LOW);
//    digitalWrite(stepper_s_pin_3, LOW);
//    digitalWrite(stepper_s_pin_4, LOW);
//
//    digitalWrite(stepper_t_pin_1, LOW);
//    digitalWrite(stepper_t_pin_2, LOW);
//    digitalWrite(stepper_t_pin_3, LOW);
//    digitalWrite(stepper_t_pin_4, LOW);
//
//  } else {
//    Serial.print("First byte ('")
//    Serial.print(incomingByte)
//    Serial.print("') of command is invalid\n");
//  }
  readByte(); //ignore the newline character

}
