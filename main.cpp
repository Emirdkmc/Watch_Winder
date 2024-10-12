#include <Stepper.h>
#include <Arduino.h>

const int stepsPerRevolution = 2038;

const int stepsPerRevolution = 2038;
const int ledPin = 12;
const int buttonPin = 13;

Stepper motor1(stepsPerRevolution, A0, A2, A1, A3);
Stepper motor2(stepsPerRevolution, 10, A4, 11, A5);
Stepper motor3(stepsPerRevolution, 2, 4, 3, 5);
Stepper motor4(stepsPerRevolution, 6, 8, 7, 9);

unsigned long lastDebounceTime = 0;
unsigned long debounceDelay = 50;
unsigned long lastButtonPressTime = 0;
unsigned long operationStartTime = 0;
bool isRunning = true;
int buttonState;
int lastButtonState = HIGH;
int buttonCount = 0;

void setup() {

    pinMode(ledPin, OUTPUT);
    pinMode(buttonPin, INPUT);
    
    motor1.setSpeed(10);
    motor2.setSpeed(10);
    motor3.setSpeed(10);
    motor4.setSpeed(10);
    
    digitalWrite(ledPin, HIGH);
    operationStartTime = millis();
}

void allcw() {
  for (int i = 0; i < stepsPerRevolution; i++) {
      motor1.step(1);
      motor2.step(1);
      motor3.step(1);
      motor4.step(1);
      delayMicroseconds(1000);
  }
}

void allccw() {
    for (int i = 0; i < stepsPerRevolution; i++) {
        motor1.step(-1);
        motor2.step(-1);
        motor3.step(-1);
        motor4.step(-1);
        delayMicroseconds(1000);
        if (buttonCount == 1) {
            break;
        }
    }
}

void allstop() {
    motor1.step(0);
    motor2.step(0);
    motor3.step(0);
    motor4.step(0);
}

void oneandthreeccw() {
    motor1.step(1);
    motor3.step(1);
    motor2.step(-1);
    motor4.step(-1);
}

void oneandthreecw() {
    motor1.step(-1);
    motor3.step(-1);
    motor2.step(1);
    motor4.step(1);    
}

void oneandtwoccw() {
    motor1.step(-1);
    motor2.step(-1);
    motor3.step(1);
    motor4.step(1);
}

void oneandtwocw() {
    motor1.step(-1);
    motor2.step(-1);
    motor3.step(1);
    motor4.step(1);
}  

void oneandfourccw() {
    motor1.step(-1);
    motor4.step(-1);
    motor2.step(1);
    motor3.step(1);
}

void oneandfourcw() {
    motor1.step(1);
    motor4.step(1);
    motor2.step(-1);
    motor3.step(-1);
}

void loop() {

    button1.Update();

    if (buttonCount == 0) {
        allcw();

    } else if (buttonCount == 1) {
        allccw();

    } else if (buttonCount == 2) {
        oneandthreeccw();

    } else if (buttonCount == 3) {
        oneandthreecw();

    } else if (buttonCount == 4) {
        oneandtwoccw();

    } else if (buttonCount == 5) {
        oneandtwocw();

    } else if (buttonCount == 6) {
        oneandfourccw();

    } else if (buttonCount == 7) {
        oneandfourcw();

    } else if (buttonCount == 8) {
        allstop();

    }

}

void loop() {
    unsigned long currentMillis = millis();
    int reading = digitalRead(buttonPin);

    if (reading != lastButtonState) {
        lastDebounceTime = currentMillis;
    }

    if ((currentMillis - lastDebounceTime) > debounceDelay) {
        if (reading != buttonState) {
            buttonState = reading;
            
            if (buttonState == LOW) {
                lastButtonPressTime = currentMillis;
            } else {
                if ((currentMillis - lastButtonPressTime) >= 1000) {
                    isRunning = !isRunning;
                    digitalWrite(ledPin, isRunning ? HIGH : LOW);
                } else {
                    buttonCount++;
                    if (buttonCount > 8) buttonCount = 0;
                }
            }
        }
    }

    lastButtonState = reading;

    if (isRunning) {
        if ((currentMillis - operationStartTime) < 3000000) {  // 50 dakika çalış
            switch (buttonCount) {
                case 0: allcw(); break;
                case 1: allccw(); break;
                case 2: oneandthreeccw(); break;
                case 3: oneandthreecw(); break;
                case 4: oneandtwoccw(); break;
                case 5: oneandtwocw(); break;
                case 6: oneandfourccw(); break;
                case 7: oneandfourcw(); break;
                case 8: allstop(); break;
            }
        } else if ((currentMillis - operationStartTime) >= 6000000) {
            // 50 dakika dinlenme süresinden sonra sıfırla
            operationStartTime = currentMillis;
        }
    } else {
        allstop();  // Sistem kapalıyken motorlar duracak
    }
}