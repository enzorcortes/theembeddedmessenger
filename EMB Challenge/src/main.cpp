#include <Adafruit_CircuitPlayground.h>
#include <Arduino.h>

// In this project, we are trying to:
// 1. Get input from the built-in acceloremeter which would be
// translated into one of 10 patterns.
//
// 2. Each pattern consists of 3 consecutive motions, and each motion
// starts with left button press and ends with right button press or 3
// seconds after the left button press, whichever comes first.
//
// 2-1. NeoPixel will be turned on when the the accelerometer is in
// action.
//
// 3. Output the pattern to the built-in Neopixel (1 means 1 LED on, 5
// means 5 LEDs on, etc.) the output will shine until the left button is
// pressed again.

// Patterns :
//
// Patterns are consists of 3 consecutive motions, and each motion is
// one of plus minus x, y, or z direction (6 motions in total). For
// simplicity, we will represent each motion as a number from 1 to 6,
// where 1 means plus x, 2 means minus x, 3 means plus y, 4 means minus
// y, 5 means plus z, and 6 means minus z. For example, if the pattern
// is plus x, minus y, plus z, then the pattern is represented as 1,
// 4, 5. 10 of 216 possible sequences are assigned to each of the 10
// patterns. The 10 patterns are as follows (can be changed later):
//
// 1. -y, +z, -y (4, 5, 4)
// 2. +y, +z, +y (3, 5, 3)
// 3. +z, -y, -z (5, 4, 6)
// 4. +z, -y, +z (5, 4, 5)
// 5. +x, -z, -x (1, 6, 2)
// 6. +x, -z, +x (1, 6, 1)
// 7. +y, -z, -y (3, 6, 4)
// 8. +y, -z, +y (3, 6, 3)
// 9. +y, -x, -y (3, 2, 4)
// 10. +y, -x, +y (3, 2, 3)

void ledShow(int num) {
  if (num != 0) {
    for (int i = 0; i < num; i++) {
      CircuitPlayground.strip.setPixelColor(i, 255, 255, 255);
    }
    for (int i = num; i < 10; i++) {
      CircuitPlayground.strip.setPixelColor(i, 0, 0, 0);
    }
    CircuitPlayground.strip.show();
  } else {
    // If the pattern is not one of the 10 patterns, then show red

    for (int i = 0; i < 10; i++) {
      CircuitPlayground.strip.setPixelColor(i, 255, 0, 0);
    }
    CircuitPlayground.strip.show();
  }

  while (!CircuitPlayground.leftButton()) {
    // Do nothing
    delay(1);
  }
  for (int i = 0; i < 10; i++) {
    CircuitPlayground.strip.setPixelColor(i, 0, 0, 0);
  }
  CircuitPlayground.strip.show();
}

void ledCirculate(int startTime) {
  // I really want to make the led circulate, but we cannot make
  // concurrent threads in Arduino. So I use the real time clock to make
  // the led circulate.

  int curr = millis();
  int diff = curr - startTime;
  int num = (diff / 100) % 10;

  // Consecutive 3 LEDs will be on based on the time difference.
  for (int i = 0; i < 10; i++) {
    if (i == num || i == (num + 1) % 10 || i == (num + 2) % 10) {
      CircuitPlayground.strip.setPixelColor(i, 0, 122, 255);
    } else {
      CircuitPlayground.strip.setPixelColor(i, 0, 0, 0);
    }
  }
  CircuitPlayground.strip.show();
}

void ledBlink(int num) {
  // The intention of this blink is to indicate the next motion is
  // first, second or third. When the next motion is second, the led
  // will blink in Red, and when the next motion is third, the led will
  // blink in Green, and when the next motion is first, the led will
  // blink in Blue.

  for (int j = 0; j < 200; j++) {
    for (int i = 0; i < 10; i++) {
      if (num == 1) {
        CircuitPlayground.strip.setPixelColor(i, 0, 0, j);
      } else if (num == 2) {
        CircuitPlayground.strip.setPixelColor(i, 0, j, 0);
      } else {
        CircuitPlayground.strip.setPixelColor(i, j, 0, 0);
      }
    }
    CircuitPlayground.strip.show();
    delay(1);

    if (CircuitPlayground.leftButton()) {
      return;
    }
  }
}

int readMotion() {
  int startTime = millis();
  int motion[7] = {0, 0, 0, 0, 0, 0, 0};

  int flag = 0;

  while (!CircuitPlayground.rightButton() &&
         millis() - startTime < 3000) {
    // Read accelerometer
    ledCirculate(startTime);

    double x = CircuitPlayground.motionX() - 0;
    double y = CircuitPlayground.motionY() - 0;
    double z = CircuitPlayground.motionZ() - 0;

    if (flag > 50) {
      Serial.print("x: ");
      Serial.print(x);
      Serial.print(" y: ");
      Serial.print(y);
      Serial.print(" z: ");
      Serial.println(z);
      flag = 0;
    }
    flag++;


    // For now, we will just detect the direction of the motion for
    // convenience. We will implement the pattern detection later.

    // Find the largest absolute value among x, y, and z.
    double max = abs(x);
    if (abs(y) > max) {
      max = abs(y);
    }
    if (abs(z) > max) {
      max = abs(z);
    }

    // If the largest absolute value is x, then the motion is either
    // plus x or minus x.
    if (max == abs(x)) {
      if (x > 0) {
        // Plus x
        motion[1] += 1;
      } else {
        // Minus x
        motion[2] += 1;
      }
    }

    // If the largest absolute value is y, then the motion is either
    // plus y or minus y.
    if (max == abs(y)) {
      if (y > 0) {
        // Plus y
        motion[3] += 1;
      } else {
        // Minus y
        motion[4] += 1;
      }
    }

    // If the largest absolute value is z, then the motion is either
    // plus z or minus z.
    if (max == abs(z)) {
      if (z > 0) {
        // Plus z
        motion[5] += 1;
      } else {
        // Minus z
        motion[6] += 1;
      }
    }
  }

  // Find the largest number in motion array.
  int max = 0;
  int maxIndex = 0;
  for (int i = 0; i < 7; i++) {
    if (motion[i] > max) {
      max = motion[i];
      maxIndex = i;
    }
  }

  // Return the direction of the motion.
  return maxIndex;
}

void setup() {
  CircuitPlayground.begin();
  Serial.begin(9600);
}

int interpret(int seq[3]) {
  // This function will interpret the sequence of 3 motions and
  // return the pattern number.

  // I know. This is not the best way to do this.
  if (seq[0] == 4 && seq[1] == 5 && seq[2] == 4) {
    return 1;
  } else if (seq[0] == 3 && seq[1] == 5 && seq[2] == 3) {
    return 2;
  } else if (seq[0] == 2 && seq[1] == 3 && seq[2] == 2) {
    return 3;
  } else if (seq[0] == 6 && seq[1] == 2 && seq[2] == 4) {
    return 4;
  } else if (seq[0] == 5 && seq[1] == 5 && seq[2] == 5) {
    return 5;
  } else if (seq[0] == 5 && seq[1] == 2 && seq[2] == 5) {
    return 6;
  } else if (seq[0] == 3 && seq[1] == 2 && seq[2] == 3) {
    return 7;
  } else if (seq[0] == 4 && seq[1] == 2 && seq[2] == 5) {
    return 8;
  } else if (seq[0] == 6 && seq[1] == 2 && seq[2] == 6) {
    return 9;
  } else if (seq[0] == 4 && seq[1] == 1 && seq[2] == 5) {
    return 10;
  } else {
    return 0;
  }
}

void loop() {
  while (!CircuitPlayground.leftButton()) {
    // Do nothing
    ledBlink(1);
  }
  delay(1000);
  int seq[4] = {0, 0, 0, 0};
  // First motion ready to be recorded
  Serial.println("First motion ready to be recorded");
  seq[0] = readMotion();
  Serial.println(seq[0]);

  // wait until the left button is pressed again for the second
  // motion
  while (!CircuitPlayground.leftButton()) {
    // Do nothing
    ledBlink(2);
  }
  delay(1000);

  // Second motion ready to be recorded
  Serial.println("Second motion ready to be recorded");
  seq[1] = readMotion();
  Serial.println(seq[1]);

  // wait until the left button is pressed again for the third
  // motion
  while (!CircuitPlayground.leftButton()) {
    // Do nothing
    ledBlink(3);
  }
  delay(1000);

  // Third motion ready to be recorded
  Serial.println("Third motion ready to be recorded");
  seq[2] = readMotion();
  Serial.println(seq[2]);

  // Interpret the sequence of 3 motions and return the pattern
  // number.

  int pattern = interpret(seq);
  Serial.print("Pattern: ");
  Serial.println(pattern);

  // Show the pattern on the LED
  ledShow(pattern);
}
