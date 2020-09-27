/*
 * Kitchen timer using a Feather nRF52840 Express
 * 
 * Author: Philipp Alder
 * Create Time: 26 September, 2020
 * 
 */

#include "TM1637.h"
#define CLK 9 //pin for Grove 4-Digit display (Grove D4)
#define DIO 10 
#define BTN 5 //pin for button (Grove D2)
#define BZR 6 //pin for buzzer (Pin 6)
#define ROT A2 //pin for rotary angle sensor (Grove A2)


TM1637 tm1637(CLK,DIO);

int rotaryValue;
int b;
int timerSetting = 0;
int state = 0;

void setup() {
  tm1637.init();
  tm1637.set(BRIGHT_TYPICAL); //BRIGHT_TYPICAL = 2,BRIGHT_DARKEST = 0,BRIGHTEST = 7;
  pinMode(BTN, INPUT);
  pinMode(BZR, OUTPUT);
}

// Function that handles the display of the time. If less than 60 seconds are displayed then the colon separator is 
//deactivated
void displayTime(int sec) {
  tm1637.point(false); //has to be before clearDisplay()...no idea why
  tm1637.clearDisplay();

  tm1637.display(3,sec % 10); //least significant second digit
  if(sec>9) {
    tm1637.display(2,sec/10 % 6); //most significant second digit
  }
  if(sec>59) {
    tm1637.point(true);
    tm1637.display(1,sec/60 % 10);  //least significant minute digit
  }
  if(sec>599) {
    tm1637.display(0,sec/600 % 10); //most significant minute digit
  }
}


void loop() {
  b = digitalRead(BTN); //update the current values for the button and the rotary dial
  rotaryValue = analogRead(ROT);  
  
  if(state == 0 && b == LOW ) { //State 0; "Setting the timer"
    if(abs(rotaryValue-timerSetting)>4) //to prevent the value from stuttering 
    {
      timerSetting = rotaryValue;  //rotary values are between 0 and 935
      displayTime(timerSetting);
    } 
  }
  else if(state == 0 && b == HIGH) { 
    state = 1;
  }
  else if(state == 1 && b ==LOW) { //State 1; "transition state"
    state = 2;
  }
  else if(state == 2 && b == LOW) { //State 2; "timer is counting down"
    timerSetting--;
    if(timerSetting == 0) {
      state = 4;
    }
    displayTime(timerSetting);
    delay(1000);
  }
  else if(state == 2 && b == HIGH) { 
    state = 3;
  }
   else if(state == 3 && b == LOW) { //State 3; "transition state and turning buzzer off"
    digitalWrite(BZR, LOW); //turn buzzer off
    state = 0;
  }
  else if(state == 4 && b == LOW) { //State 4; "alarm is going off"
    digitalWrite(BZR, HIGH);
  }
  else if(state == 4 && b == HIGH) {
    state = 3;
  }
}
