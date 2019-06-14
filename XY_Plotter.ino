#include <AccelStepper.h>
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
const int PEN_DOWN_POS = 155;
const int PEN_UP_POS = 145;
//const int stepsPerRevolution = 2048;  // change this to fit the number of steps per revolution
int stepCount = 0;         // number of steps the motor has taken

int pos = 0;    // variable to store the servo position

String inputString = "";         // a String to hold incoming data
bool stringComplete = false;  // whether the string is complete

// Define two motor objects
// The sequence 1-3-2-4 is required for proper sequencing of 28BYJ48
AccelStepper stepper1(HALFSTEP, motorPin1, motorPin3, motorPin2, motorPin4);
AccelStepper stepper2(FULLSTEP, motorPin5, motorPin7, motorPin6, motorPin8);

//Stepper stepper1(stepsPerRevolution, 9, 11, 10, 12);
//Stepper stepper2(stepsPerRevolution, 5, 7, 6, 8);

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

/*
  SerialEvent occurs whenever a new data comes in the hardware serial RX. This
  routine is run between each time loop() runs, so using delay inside loop can
  delay response. Multiple bytes of data may be available.
*/
void serialEvent() {
  while (Serial.available()) {
    char inChar = (char)Serial.read();    // get the new byte:
    inputString += inChar;    // add it to the inputString:
    if (inChar == '\n') {    // if the incoming character is a newline,
      stringComplete = true; // set a flag so the main loop can do something about it:
    }
  }
}

void setup() {
  Serial.begin(9600);
  pinMode(LED_BUILTIN, OUTPUT);
  myservo.attach(SERVO_PIN);
  inputString.reserve(200);

  // 1 revolution Motor 1 CW
  stepper1.setMaxSpeed(1000.0);
  stepper1.setAcceleration(50.0);
  stepper1.setSpeed(300);
  stepper1.moveTo(2048);

  // 1 revolution Motor 2 CCW
  stepper2.setMaxSpeed(1000.0);
  stepper2.setAcceleration(50.0);
  stepper2.setSpeed(300);
  stepper2.moveTo(-2048);


}

void loop() {
  if (stringComplete) {
    if (inputString == "up\n") {
      penUp();
    } else if (inputString == "down\n") {
      penDown();
    } else {
      myservo.write(inputString.toInt());
      Serial.println(inputString);
    }
    // clear the string:
    inputString = "";
    stringComplete = false;
  }

//  for (pos = 0; pos <= 180; pos += 1) { // goes from 0 degrees to 180 degrees
//    // in steps of 1 degree
//    myservo.write(pos);              // tell servo to go to position in variable 'pos'
//    delay(10);                       // waits 15ms for the servo to reach the position
//  }
//  for (pos = 180; pos >= 0; pos -= 1) { // goes from 180 degrees to 0 degrees
//    myservo.write(pos);              // tell servo to go to position in variable 'pos'
//    delay(10);                       // waits 15ms for the servo to reach the position
//  }


    //Change direction at the limits
    if (stepper1.distanceToGo() == 0)
      stepper1.moveTo(-stepper1.currentPosition());
    if (stepper2.distanceToGo() == 0)
      stepper2.moveTo(-stepper2.currentPosition());
  
    stepper1.run();
    stepper2.run();

}
