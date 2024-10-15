#include <Stepper.h>
#include <Arduino.h>
#include <ClickButton.h>

const int stepsPerRevolution = 2038;
const int ledPin = 13;
const int buttonPin = 12;
ClickButton button(buttonPin, LOW, CLICKBTN_PULLUP);

Stepper motor1(stepsPerRevolution, A0, A2, A1, A3);
Stepper motor2(stepsPerRevolution, 10, A4, 11, A5);
Stepper motor3(stepsPerRevolution, 2, 4, 3, 5);
Stepper motor4(stepsPerRevolution, 6, 8, 7, 9);

unsigned long startTime;
unsigned long currentTime;
unsigned long ledStopStartTime;
bool forcedStop = false;

void setup() {

    Serial.begin(9600); //burası silinecek
    pinMode(ledPin, OUTPUT);
    //pinMode(buttonPin, INPUT);
    
    motor1.setSpeed(10);
    motor2.setSpeed(10);
    motor3.setSpeed(10);
    motor4.setSpeed(10);
    
    button.debounceTime   = 20;   // Debounce timer in ms
    button.multiclickTime = 250;  // Time limit for multi clicks
    button.longClickTime  = 1000; // time until "held-down clicks" register
    digitalWrite(ledPin, HIGH);
    startTime = millis();
}

int motor1_direction = 0;
int motor2_direction = 0;
int motor3_direction = 0;
int motor4_direction = 0;
int ledMode = 0;
int currrentMode = 0;
int oldMode = 0;
bool isRunning = false;
bool isLedOn = false;

void spinMotor(){

    motor1.step(motor1_direction);
    motor2.step(motor2_direction);
    motor3.step(motor3_direction);
    motor4.step(motor4_direction);
}

void setMode(int mode){
    switch (mode)
    {
    case 0:
        // stop
        isRunning = false;
        motor1_direction = 0;
        motor2_direction = 0;
        motor3_direction = 0;
        motor4_direction = 0;
        break;
    case 1:
        // clockwise
        isRunning = true;
        motor1_direction = 1;
        motor2_direction = 1;
        motor3_direction = 1;
        motor4_direction = 1;
        break;
    case 2:
        // ccwise
        isRunning = true;
        motor1_direction = -1;
        motor2_direction = -1;
        motor3_direction = -1;
        motor4_direction = -1;
        break;
    case 3:
        // 1-3 cwise 2-4 ccwise
        isRunning = true;
        motor1_direction = 1;
        motor2_direction = -1;
        motor3_direction = 1;
        motor4_direction = -1;
        break;
    case 4:
        // 1-3 ccwise 2-4 cwise
        isRunning = true;
        motor1_direction = -1;
        motor2_direction = 1;
        motor3_direction = -1;
        motor4_direction = 1;
        break;
    case 5:
        // 1-2 cwise 3-4 ccwise
        isRunning = true;
        motor1_direction = 1;
        motor2_direction = 1;
        motor3_direction = -1;
        motor4_direction = -1;
        break;
    case 6:
        // 1-2 ccwise 3-4 cwise
        isRunning = true;
        motor1_direction = -1;
        motor2_direction = -1;
        motor3_direction = 1;
        motor4_direction = 1;
        break;
    case 7:
        // 1-4 cwise 2-3 ccwise
        isRunning = true;
        motor1_direction = 1;
        motor2_direction = -1;
        motor3_direction = -1;
        motor4_direction = 1;
        break;
    case 8:
        // 1-4 ccwise 2-3 cwise
        isRunning = true;
        motor1_direction = -1;
        motor2_direction = 1;
        motor3_direction = 1;
        motor4_direction = -1;
        break;

    default:
        isRunning = false;
        motor1_direction = 0;
        motor2_direction = 0;
        motor3_direction = 0;
        motor4_direction = 0;
        break;
    }
}

int checkButton(){
    button.Update();
    
    if (button.clicks == -1 && button.changed == 1){

        Serial.println("Uzun basildi"); // burası silinecek
        return 1;
    }
    else if (button.clicks == 1 && button.changed == 1){

        Serial.println("Kisa basildi"); // burası silinecek
        return 2;
    }
    else {
        return 0;
    }

}

void changeMode(int buttonMode){
    startTime = millis();
    if (buttonMode == 2) {
        if (currrentMode == 8) {
            currrentMode = 1;
        }
        else {
            currrentMode++;
        }
        Serial.println("Kisa basildigi icin mod degisti"); // burası silinecek  
        Serial.println(currrentMode); // burası silinecek
        setMode(currrentMode);
        forcedStop = false;
    }
    else {
        Serial.println("Uzun basildigi icin motor durdu");  // burası silinecek
        setMode(0); // Long press -> Motor Stop
        oldMode = 0;
        forcedStop = true;
    }
}

void ledAction(){
    if (ledMode == 0) {
        digitalWrite(ledPin, LOW);
        ledMode = 1;
    }
    else {
        digitalWrite(ledPin, HIGH);
        ledMode = 0;
    }
}

void ledSet(bool state){
    digitalWrite(ledPin, state);
}

void checkTime(){
    currentTime = millis();

 
    if (currentTime - startTime > 10000 && isRunning && !forcedStop) {
        setMode(0);
        Serial.println("10 Sn Gecti Motor Stop"); // burası silinecek
        oldMode = currrentMode;
        ledStopStartTime = currentTime;
        startTime = currentTime;
    }

    else if (currentTime - startTime > 10000 && !isRunning && !forcedStop) {
        setMode(oldMode);
        Serial.println("10 Sn Gecti Motor Start");  // burası silinecek
        Serial.println(oldMode); // burası silinecek
        currrentMode = oldMode;
        startTime = currentTime;
    }

    if ((1000 < (currentTime - ledStopStartTime)) && isRunning && !forcedStop) {
        ledAction();
        Serial.println("isRunning 1sec blink"); // burası silinecek
        ledStopStartTime = currentTime;
    }

    if ((1000 < (currentTime - ledStopStartTime)) && !isRunning && isLedOn && !forcedStop) {
        ledSet(false);
        Serial.println("!isRunning 3sec off"); // burası silinecek
        ledStopStartTime = currentTime;
        isLedOn = false;
        return;
    }

    if ((3000 < (currentTime - ledStopStartTime)) && !isRunning && !isLedOn && !forcedStop) {
        ledSet(true);
        Serial.println("!isRunning 1sec on"); // burası silinecek
        ledStopStartTime = currentTime;
        isLedOn = true;
        return;
    }
}

void loop() {

    int mode = checkButton();
    if (mode != 0) {changeMode(mode);}
    if (motor1_direction != 0 && motor2_direction != 0 && motor3_direction != 0 && motor4_direction != 0) {
        spinMotor();
    }
    checkTime();
    delayMicroseconds(10);
}