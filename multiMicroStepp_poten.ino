#include <AccelStepper.h>
#include <MultiStepper.h>
#include <Servo.h>

#define NUM_MOTORS 5

struct Motor
{  
    unsigned short stepPin;
    unsigned short dirPin;
    int poten;
    int microstep;
    int maxPos;
    int minPos;
    unsigned int velocity;
    unsigned int accumulator;
    int value;
    int newValue;
};

/* GLOBAL VARIABLES */
MultiStepper steppers;
long positions[] = {0, 0, 0, 0, 0};
int counter = 0;
Servo myservo;

/* Define motors */
/* MAX_POS & MIN_POS refer to fullstep */
/* VELOCITY fullstep per second */
/* {STEP_PIN, DIR_PIN, POTEN, MICROSTEPS, MAX_POS, MIN_POS, VELOCITY, ACCUMULATOR, VALUE, NEW_VALUE} */
Motor motors[NUM_MOTORS] = {
    /*elbow driver X*/ {54, 55, A3, 8, 1100, -1100, 1000, 0, 0, 0}, 
    /*gripper driver Y*/ {60, 61, A4, 8, 200, -200, 300, 0, 0, 0},
    /*waist driver Z*/ {46, 48, A5, 8, 300, -300, 300, 0, 0, 0},
    /*wrist driver E0*/ {26, 28, A9, 16, 75, -75, 300, 0, 0, 0},
    /*shoulder driver E1*/ {36, 34, A10, 16, 165, 0, 300, 0, 0, 0}
};

void readAccumulator() {
    for(int i = 0; i < NUM_MOTORS; i++)
    {
        motors[i].accumulator += analogRead(motors[i].poten);
    }
}

void setPositions() {
    for(int i = 0; i < NUM_MOTORS; i++)
    {
      motors[i].newValue = map(motors[i].accumulator / 60, 0, 1023, motors[i].minPos * motors[i].microstep, motors[i].maxPos * motors[i].microstep);

      if ((motors[i].newValue < (motors[i].value - motors[i].microstep)) || ((motors[i].value + motors[i].microstep) < motors[i].newValue))
      {
        motors[i].value = motors[i].newValue;
        positions[i] = motors[i].value;
        steppers.moveTo(positions);
      }
      motors[i].accumulator = 0;
    }
}

/* INITIALIZATION */
void setup() {
    Serial.begin(9600);
    myservo.attach(4);
    
    AccelStepper accelStepper[NUM_MOTORS];
    for(int i = 0; i < NUM_MOTORS; i++)
    {
        accelStepper[i] = AccelStepper(AccelStepper::DRIVER, motors[i].stepPin, motors[i].dirPin);
        accelStepper[i].setMaxSpeed(motors[i].velocity);
        steppers.addStepper(accelStepper[i]);
    }
}

/* MAIN LOOP */
void loop() {
    readAccumulator();
    //TO-DO implement button for servo
    counter++;
    if (counter >= 60 ) {
        setPositions();
        counter = 0;
    }
    steppers.run();
}
