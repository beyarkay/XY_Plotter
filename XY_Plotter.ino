#include <AccelStepper.h>
#include <MultiStepper.h>
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
const int PEN_DOWN_POS = 145;
const int PEN_UP_POS = 155;

// Define two motor objects
// The sequence 1-3-2-4 is required for proper sequencing of 28BYJ48
AccelStepper stepper1(HALFSTEP, motorPin1, motorPin3, motorPin2, motorPin4);
AccelStepper stepper2(FULLSTEP, motorPin5, motorPin7, motorPin6, motorPin8);

MultiStepper multiStepper;

long positions[2]; //array to be used for target positions to be passed

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

void setup() {
  Serial.begin(9600);
  pinMode(LED_BUILTIN, OUTPUT);
  myservo.attach(SERVO_PIN);
  penUp();

  stepper1.setMaxSpeed(1000.0);
  stepper2.setMaxSpeed(1000.0);
  multiStepper.addStepper(stepper1);
  multiStepper.addStepper(stepper2);
}

void loop() {

  int incomingByte;
  incomingByte = readByte();

  if (incomingByte == 'p') {
    Serial.print("received pen control command\n");
    
    incomingByte = readByte();
    if (incomingByte == 'u') {
      penUp();
    } else if (incomingByte == 'd') {
      penDown();
    } else {
      Serial.print("second byte of up/down command is invalid\n");
    }
  } else if (incomingByte == 'm') {
    Serial.print("received move command\n");
    
    positions[0] = (readByte() - 'a') * 100;
    positions[1] = (readByte() - 'a') * 100;

    Serial.print("positions: ");
    Serial.print(positions[0], DEC);
    Serial.print(" ");
    Serial.print(positions[1], DEC);
    Serial.print("\n");

//    multiStepper.moveTo(positions);
//    multiStepper.runSpeedToPosition();
    stepper1.moveTo(positions[0]);
    stepper2.moveTo(positions[1]);
    stepper1.runToPosition();
    stepper2.runToPosition();
  } else if (incomingByte == 's') {
    Serial.print("received set position command\n");

    stepper1.setCurrentPosition(readByte());
    stepper2.setCurrentPosition(readByte());
  } else {
    Serial.print("first byte of command is invalid\n");
  }
  readByte(); //ignore newline

}
