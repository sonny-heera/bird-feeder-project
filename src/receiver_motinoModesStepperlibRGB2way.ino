/**
 * Sandeep Heera
 * receiever_motino.ino
 * This program drives a bipolar stepper motor attached to a Moteino board 
 * from Low Power Lab. The program first sets the stepper motor to the reset
 * position, which corresponds to a magnet placed 
 */
 
#include <Stepper.h>
#include <RFM69.h>         
#include <RFM69_ATC.h>     
#include <SPI.h>          
#include <LowPower.h>
#include <Servo.h>     

#define NETWORKID     100 
#define RECEIVER      1   
#define SENDER        2
#define NODEID        RECEIVER  
#define FREQUENCY     RF69_915MHZ
#define ENCRYPTKEY    "sampleEncryptKey"
#define IS_RFM69HW  
#define ENABLE_ATC
#define ATC_RSSI        -75
#define SERIAL_BAUD   9600
#ifdef ENABLE_ATC
  RFM69_ATC radio;
#else
  RFM69 radio;
#endif

#define TOTAL_STEPS 200 // Full rotation
#define STEPPER_SPEED 2
#define HALL_PIN 8
#define HALL_POWER_PIN 9
#define FLAP_SERVO_PIN 9
#define IN1 4
#define IN2 5
#define IN3 6
#define IN4 7

#define TOGGLE_MODE A0
#define OPEN_FLAP A1 
#define CLOSE_FLAP A2
#define FORWARD 4
#define BACKWARD A3
#define POSITION_0 12
#define POSITION_1 7
#define POSITION_2 8
#define POSITION_3 9

#define SERVO_CLOSED_AMOUNT 10
#define SERVO_OPEN_AMOUNT 80

int mode = 0;

long time;
int trayPosition = 0;
int stepsLeft = 0;

Servo flapServo;
//stepper object
Stepper stepper(TOTAL_STEPS, IN1, IN2, IN3, IN4);

void setup(){
  Serial.begin(SERIAL_BAUD);
  radio.initialize(FREQUENCY,NODEID,NETWORKID);
  radio.setHighPower(); 
  radio.encrypt(ENCRYPTKEY);
  radio.enableAutoPower(ATC_RSSI);
  stepper.setSpeed(STEPPER_SPEED);
  pinMode(HALL_PIN, INPUT);
  pinMode(HALL_POWER_PIN, OUTPUT);
  pinMode(FLAP_SERVO_PIN, OUTPUT);
  flapServo.attach(FLAP_SERVO_PIN);
  flapServo.write(SERVO_CLOSED_AMOUNT);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  //reset_position();
}
 
void loop(){
  int receivedValue = -1;
  if (radio.receiveDone()){
    receivedValue = atoi((char*)radio.DATA);
    Serial.println(receivedValue);
    /*if(receivedValue == TOGGLE_MODE){
      mode++;
      mode %= 2;
    }*/
    char ackBuffer[3];
    sprintf(ackBuffer, "%d", mode);
    //check if sender wanted an ACK
    if (radio.ACKRequested()){
      radio.sendACK(ackBuffer, 3);
    }
  }
    // Manual mode
  switch(receivedValue){
    case TOGGLE_MODE:
        //automaticMode();
        break;
    case OPEN_FLAP:
        //Serial.println("Opening Flap. This functionality has yet to be implemented.");
        openFlap();
        break;
    case CLOSE_FLAP:
        //Serial.println("Closing Flap. This functionality has yet to be implemented.");
        closeFlap();
        break;
    case FORWARD:
        //Serial.println("Moving position forward.");
        moveForward();
        break;
    case BACKWARD:
        //Serial.println("Moving position backward.");
        moveBackward();
        break;
    case POSITION_0:
        //Serial.println("Moving to position 0.");
        moveToPosition(0);
        break;
    case POSITION_1:
        //Serial.println("Moving to position 1.");
        moveToPosition(1);
        break;
    case POSITION_2:
        //Serial.println("Moving to position 2.");
        moveToPosition(2);
        break;
    case POSITION_3:
        //Serial.println("Moving to position 3.");
        moveToPosition(3);
        break;
    case -1:
        //Serial.println("No command received.");
        break;
    default:
        //Serial.println("Error. Unknown command.");
        break;
  }
  radio.receiveDone(); //put radio in RX mode
  LowPower.powerDown(SLEEP_8S, ADC_OFF, BOD_ON); //sleep Moteino in low power mode (to save battery)
}

/*
 * Infinite loop that ends only if the toggle button is pressed. In the loop, the Moteino will be listening for 
 * commands from the Pi.
 */
 void automaticMode(){
    int receivedValue = -1;
    while(receivedValue != TOGGLE_MODE){
        // Send image recognition request
        Serial.println("start");
        while(!Serial.available()){
          // wait for acknowledgement
        }
         //Listen for string from pi
        if(Serial.available()){
          char s = (char)Serial.read();
          //Serial.println(s);
          if(s == '0'){
            // Vulture
            moveToPosition(0);
          }
          else if(s == '1'){
            // Macaw
            moveToPosition(1); 
          }
          else if(s == '2'){
            // HornedBill
            moveToPosition(2);
          }
          else if(s == '3'){
            // Face
            moveToPosition(3);
          }
        }
        if (radio.receiveDone()){
            receivedValue = atoi((char*)radio.DATA);
            if(receivedValue == TOGGLE_MODE){
                mode++;
                mode %= 2;
            }
            char ackBuffer[3];
            sprintf(ackBuffer, "%d", mode);
            //check if sender wanted an ACK
            if (radio.ACKRequested()){
                radio.sendACK(ackBuffer, 3);
            }
        }
        radio.receiveDone(); //put radio in RX mode
     }
     Serial.println("end");
     delay(10);
 }

/*
 * Opens flap
 */
 void openFlap(){
  //Serial.println(millis());
  flapServo.write(SERVO_OPEN_AMOUNT);
  delay(500);
  //Serial.println(millis());
 }

 /*
 * Closes flap
 */
 void closeFlap(){
  //Serial.println("in close");
  flapServo.write(SERVO_CLOSED_AMOUNT);
  delay(500);
 }

/*
 * Moves tray 1 position forward. Clockwise if viewed from above.
 */
 void moveForward(){
  rotate(90);
  trayPosition++;
  trayPosition %= 4;
 }

 /*
 * Moves tray 1 position backward. Counter-clockwise if viewed from above.
 */
 void moveBackward(){
  rotate(-90);
  trayPosition++;
  trayPosition %= 4;
 }

/*
 * Moves tray to desired position. Position 0 is starting position.
 * The layout of the tray positions is as follows:
 *        pos 0
 *   pos 3     pos 1
 *        pos 2
 */
void moveToPosition(int desiredPosition){
  int numTurns = desiredPosition - trayPosition;
  if(numTurns < -2){
    numTurns += 4;
  }
  else if(numTurns > 2){
    numTurns -= 4;
  }
  rotate(numTurns * 90);
  trayPosition = desiredPosition;
}

/**
 * Returns the value of the hall effect sensor. 0 corresponds to
 * the detection of a magnet.
 */
int check_magnet_reading(){
  digitalWrite(HALL_POWER_PIN, HIGH);
  int magnet_reading = digitalRead(HALL_PIN);
  //Serial.println(magnet_reading);
  digitalWrite(HALL_POWER_PIN, LOW);
  
  return magnet_reading;
}

/**
 * Resets the position of the motor. The reset position corresponds to
 * the placement of the magnet.
 */
void reset_position(){ 
  int magnet_reading = check_magnet_reading();
  while(!magnet_reading){
    stepper.step(1);
    delay(1);
    magnet_reading = check_magnet_reading();
  }
  trayPosition = 0;
}

void rotate(int theAngle) {
  int theSteps = map(theAngle, 0, 360, 0, 200);
  stepper.step(theSteps);
  //Serial.println("Rotation has ended.");
}
