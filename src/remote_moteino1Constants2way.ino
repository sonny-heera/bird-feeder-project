/**
 * Sandeep Heera
 * remote_moteino.ino
 * This program sends a message to the receiver moteino that is hooked up to a
 * stepper motor. The message indicates whether to turn the specified step or
 * to reset the motor position.
 */
#include <RFM69.h>         
#include <RFM69_ATC.h>     
#include <SPI.h>          
#include <LowPower.h>     

#define NETWORKID     100 
#define RECEIVER      1   
#define SENDER        2
#define NODEID        SENDER 
#define FREQUENCY     RF69_915MHZ
#define ENCRYPTKEY    "sampleEncryptKey"
#define IS_RFM69HW  
#define ENABLE_ATC
#define ATC_RSSI        -75
#define SERIAL_BAUD   115200
#ifdef ENABLE_ATC
  RFM69_ATC radio;
#else
  RFM69 radio;
#endif

// Buttons listed based on priority
#define TOGGLE_MODE A0
#define OPEN_FLAP A1 
#define CLOSE_FLAP A2
#define FORWARD 4
#define BACKWARD A3
#define POSITION_0 12
#define POSITION_1 7
#define POSITION_2 8
#define POSITION_3 9
#define RED_PIN 3
#define BLUE_PIN 5
#define GREEN_PIN 6

int mode = 0;

void setup() {
  Serial.begin(SERIAL_BAUD);
  radio.initialize(FREQUENCY,NODEID,NETWORKID);
  radio.setHighPower(); //only for RFM69HW!
  radio.encrypt(ENCRYPTKEY);
  radio.enableAutoPower(ATC_RSSI);
  pinMode(POSITION_0, INPUT_PULLUP);
  pinMode(POSITION_1, INPUT_PULLUP);
  pinMode(POSITION_2, INPUT_PULLUP);
  pinMode(POSITION_3, INPUT_PULLUP);
  pinMode(TOGGLE_MODE, INPUT_PULLUP);
  pinMode(FORWARD, INPUT_PULLUP);
  pinMode(BACKWARD, INPUT_PULLUP);
  pinMode(OPEN_FLAP, INPUT_PULLUP);
  pinMode(CLOSE_FLAP, INPUT_PULLUP);
  pinMode(RED_PIN, OUTPUT);
  pinMode(GREEN_PIN, OUTPUT);
  pinMode(BLUE_PIN, OUTPUT);
  setColor(0, 255, 0);
}

void loop() {
  int buttonPressed = readButtonPress();
  char stringToSend[4];
  sprintf(stringToSend, "%d", buttonPressed);
  if(buttonPressed != -1){
    Serial.println(stringToSend);
    if (radio.sendWithRetry(RECEIVER, stringToSend, 4)){
        if(mode == 0 || buttonPressed == TOGGLE_MODE){
          setColor(0, 0, 0);
        }
        delay(500);
        mode = atoi((char*)radio.DATA);
    }
    //delay(1000);// Wait for 1.5 seconds before another command can be sent.
  }
  if(mode == 0){
    setColor(0, 255, 0);
  }
  else if(mode == 1){
    setColor(255, 0, 0);
  }
  LowPower.powerDown(SLEEP_120MS, ADC_OFF, BOD_ON); //sleep Moteino in low power mode (to save battery)
}

void setColor(int red, int green, int blue)
{
  analogWrite(RED_PIN, red);
  analogWrite(GREEN_PIN, green);
  analogWrite(BLUE_PIN, blue);  
}

/*
 * Read the buttons in sequential order from the ordering in the define statements. If the button is detected to be pushed, return
 * the pin number associated with reading.
 */
int readButtonPress(){
  if(!digitalRead(TOGGLE_MODE)){
    return TOGGLE_MODE;
  }
  else if(!digitalRead(OPEN_FLAP)){
    return OPEN_FLAP;
  }
  else if(!digitalRead(CLOSE_FLAP)){
    return CLOSE_FLAP;
  }
  else if(!digitalRead(FORWARD)){
    return FORWARD;
  }
  else if(!digitalRead(BACKWARD)){
    return BACKWARD;
  }
  else if(!digitalRead(POSITION_0)){
    return POSITION_0;
  }
  else if(!digitalRead(POSITION_1)){
    return POSITION_1;
  }
  else if(!digitalRead(POSITION_2)){
    return POSITION_2;
  }
  else if(!digitalRead(POSITION_3)){
    return POSITION_3;
  }
  else{
    return -1;
  }
}

/*
 * Translate the button number to the string associated with that button.
 */
/*const char* translateButton(int button){
  switch(button){
    case TOGGLE_MODE:
      Serial.println("Toggle switch");
      return "Toggle switch";
      break;
    case OPEN_FLAP:
      Serial.println("Open flap");
      return "Open flap";
      break;
    case CLOSE_FLAP:
      Serial.println("Close flap");
      return "Close flap";
      break;
    case FORWARD:
      Serial.println("Forward");
      return "Forward";
      break;
    case BACKWARD:
      Serial.println("Backward");
      return "Backward";
      break;
    case POSITION_0:
      Serial.println("Position 0");
      return "Position 0";
      break;
    case POSITION_1:
      Serial.println("Position 1");
      return "Position 1";
      break;
    case POSITION_2:
      Serial.println("Position 2");
      return "Position 2";
      break;
    case POSITION_3:
      Serial.println("Position 3");
      return "Position 3";
      break;
    case -1:
      Serial.println("No button press detected");
      return "No button press detected";
      break;
    default:
      Serial.println("Error");
      return "Error";
      break;
  }
}*/

/*void readPins(){
    Serial.print(digitalRead(POSITION_0));
    Serial.print(", ");
    Serial.print(digitalRead(POSITION_1));
    Serial.print(", ");
    Serial.print(digitalRead(POSITION_2));
    Serial.print(", ");
    Serial.print(digitalRead(POSITION_3));
    Serial.print(", ");
    Serial.print(digitalRead(TOGGLE_MODE));
    Serial.print(", ");
    Serial.print(digitalRead(FORWARD));
    Serial.print(", ");
    Serial.print(digitalRead(BACKWARD));
    Serial.print(", ");
    Serial.print(digitalRead(OPEN_FLAP));
    Serial.print(", ");
    Serial.println(digitalRead(CLOSE_FLAP));
    }*/


    /*switch(buttonPressed){
      case TOGGLE_MODE:
        Serial.println("Toggle button pressed");
        if (radio.sendWithRetry(RECEIVER, "7", 2)){ 
          Serial.println("7");
          digitalWrite(LED_PIN, LOW);
        }
        break;
      case OPEN_FLAP:
        if (radio.sendWithRetry(RECEIVER, "10", 2)){ 
          Serial.println("10");
          digitalWrite(LED_PIN, LOW);
        }
        break;
      case CLOSE_FLAP:
        if (radio.sendWithRetry(RECEIVER, "11", 2)){
          Serial.println("11"); 
          digitalWrite(LED_PIN, LOW);
        }
        break;
      case FORWARD:
        if (radio.sendWithRetry(RECEIVER, "8", 2)){ 
          Serial.println("8");
          digitalWrite(LED_PIN, LOW);
        }
        break;
      case BACKWARD:
        if (radio.sendWithRetry(RECEIVER, "9", 2)){
          Serial.println("9"); 
          digitalWrite(LED_PIN, LOW);
        }
        break;
      case POSITION_0:
        Serial.println("Position 0");
        if (radio.sendWithRetry(RECEIVER, "on", 2)){ 
          Serial.println("3");
          digitalWrite(LED_PIN, LOW);
        }
        break;
      case POSITION_1:
        if (radio.sendWithRetry(RECEIVER, "4", 2)){ 
          Serial.println("4");
          digitalWrite(LED_PIN, LOW);
        }
        break;
      case POSITION_2:
        if (radio.sendWithRetry(RECEIVER, "5", 2)){ 
          Serial.println("5");
          digitalWrite(LED_PIN, LOW);
        }
        break;
      case POSITION_3:
        if (radio.sendWithRetry(RECEIVER, "6", 2)){ 
          Serial.println("6");
          digitalWrite(LED_PIN, LOW);
        }
        break;
      default:
        Serial.println("Error");
        digitalWrite(LED_PIN, LOW);
    }*/
