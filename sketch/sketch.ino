#include <Keypad.h>
#define RPWM_Output 11 // Arduino PWM output pin 5; connect to IBT-2 pin 1 (RPWM)
#define LPWM_Output 10 // Arduino PWM output pin 6; connect to IBT-2 pin 2 (LPWM)
#define ROWS 4 //four rows
#define COLS 4 //four columns
#define WAIT_TIME 200 // beetween change speed
#define RIGHT 12 //enable right
#define LEFT 13 //enable right

#define START 40 //init speed for motor
#define END 255 //init speed for motor
#define STEP 25 //multipler for keyboard digits
#define MANUALSPEED 5 //for 'A' (plus), 'B' (minus) to current speed
#define MAX_SPEED 255


unsigned long stop_time = 0;

int motor_speed = 0;
boolean forward = true;

boolean want_forward = true;
int want_motor_speed = 0;

unsigned long time = 0;
unsigned long time_to_stop = 0;
unsigned long longclick = 0;

//define the cymbols on the buttons of the keypads
// '0' smooth stop
// '1' - '9' speed 255 - STEP * (10 - i)
// 'A' +5 to current speed
// 'B' -5 to current speed
// 'C' +15 to the stop timer (Press 4 times, stop after 1 min). The countdown starts at the first click
// 'D' reset timer
// '*' forced quick stop. The keyboard will not work until the engine stops.
// '#' smoothly change direction
char hexaKeys[ROWS][COLS] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};
byte rowPins[ROWS] = {9, 8, 7, 6}; //connect to the row pinouts of the keypad
byte colPins[COLS] = {5, 4, 3, 2}; //connect to the column pinouts of the keypad
//initialize an instance of class NewKeypad
Keypad customKeypad = Keypad( makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS);

void setup() {
  pinMode(RPWM_Output, OUTPUT);
  pinMode(LPWM_Output, OUTPUT);
  pinMode(RIGHT, OUTPUT);
  pinMode(LEFT, OUTPUT);
  digitalWrite(LEFT, HIGH);
  digitalWrite(RIGHT, HIGH);
  //Serial.begin(9600);
}



void loop() {
  char customKey = customKeypad.getKey();
  if (customKey) {
    switch(customKey){
      case '#':
          want_forward = !want_forward;
        break;
      case '*':
          while(motor_speed !=0){
            motor_speed--;
            if (motor_speed < START){
              motor_speed=0;
            }
            if(forward){
              analogWrite(LPWM_Output, motor_speed);
              analogWrite(RPWM_Output, 0);
            } else {
              analogWrite(LPWM_Output, 0);
              analogWrite(RPWM_Output, motor_speed);
            }
            delay(35);
          }
          want_motor_speed = 0;
          stop_time = 0;
          time_to_stop = 0;
        break;
      case '0':
            want_motor_speed = 0; 
        break;
      case '1':
            want_motor_speed = START;
          break;
      case '2':
            want_motor_speed = END - STEP * 7;
          break;
      case '3':
            want_motor_speed = END - STEP * 6;
          break;
      case '4':
            want_motor_speed = END - STEP * 5;
          break;
      case '5':
            want_motor_speed = END - STEP * 4;
          break;
      case '6':
            want_motor_speed = END - STEP * 3;
          break;
      case '7':
            want_motor_speed = END - STEP * 2;
          break;
      case '8':
            want_motor_speed = END - STEP * 1;
          break;
      case '9':
            want_motor_speed = END;
          break;
      case 'A':
            want_motor_speed += MANUALSPEED;
            if (want_motor_speed > MAX_SPEED){
              want_motor_speed = MAX_SPEED;
            }
            break;
      case 'B':
            want_motor_speed -= MANUALSPEED;
            if (want_motor_speed < START){
              want_motor_speed = 0; 
            }
            break;
      case 'C':
          stop_time += 15000;
          if (time_to_stop == 0){
            time_to_stop = millis();
          }
        break;
      case 'D':
          stop_time = 0;
          time_to_stop = 0;
        break;
          break;
      default:
        break;
    }
  }

  if (stop_time > 0){
    if(millis() - time_to_stop > stop_time){
      time_to_stop = 0;
      stop_time = 0;
      want_motor_speed = 0;
    }
  }
  
  if (millis() - time > WAIT_TIME){
    time = millis();
    if (want_forward == forward){
      if (want_motor_speed > motor_speed){
        if (motor_speed < START){
          motor_speed = START;
        } else{
          motor_speed++;
        }
      } else {
        if (want_motor_speed < motor_speed){
          if (motor_speed < START){
            motor_speed = 0;
          } else {
            motor_speed--;
          }
        }
      }
    } else {
      if (motor_speed != 0){
        if (motor_speed < START){
          motor_speed = 0;
        } else {
          motor_speed--;
        }
      } else {
        if (want_motor_speed != 0){
          forward = want_forward;
        }
      }
    }
  }
  if(forward){
    analogWrite(LPWM_Output, motor_speed);
    analogWrite(RPWM_Output, 0);
  } else {
    analogWrite(LPWM_Output, 0);
    analogWrite(RPWM_Output, motor_speed);
  }
  //Serial.println(motor_speed);
}
