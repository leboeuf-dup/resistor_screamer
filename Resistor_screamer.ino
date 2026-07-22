// Program that determines a resistor value from the voltage produced by a voltage divider.
//
// The program will produce an output intended to cause a buzzer to beep 
//   and comunicate the resistor value to an operator.

#include <LinkedList.h>

// Some global constants
const float RESISTOR_1 = 10000;          // Fixed resistor in the voltage divider (this resistor connects to supply)
const int SUPPLY_VOLT = 5;              // Voltage devider V_DD
const int BAUD_RATE = 9600;             // Baud Rate
const int analogPin = A0;               // adcValue input pin
const int UPDATE_INTERVAL_MSEC = 1000;  // Loop delay.
const int COUNT_SPEED = 100;            // Depay between each output when counting up
const int BEEP_DURATION = 50;           // How long a beep lasts
const int DOT_TIME = 200;               // Duration of a dot in ms
const int DASH_TIME = 600;              // Duration of a dash (3 x dot)
const int DOT_DASH_SPACE = 200;         // Space between dots/dashes
const int DIGIT_SPACE = 600;            // Space between digits
const int OUTPUT_PIN = 7;

// Defines the way in which the resistor value is comunicated:
// { 1: Morse code (DEFAULT)  }
// { 2: Binary                }
// { 3: Counting total        }
const int OUTPUT_MODE = 1;

const char* morseDigits[] = {
  "-----", // 0
  ".----", // 1
  "..---", // 2
  "...--", // 3
  "....-", // 4
  ".....", // 5
  "-....", // 6
  "--...", // 7
  "---..", // 8
  "----."  // 9
};

void setup() {
  pinMode(OUTPUT_PIN, OUTPUT);
  Serial.begin(BAUD_RATE);
}

void loop() {
  delay(UPDATE_INTERVAL_MSEC);                //Wait (UPDATE_INTERVAL_MSEC) milisecs before starting the measuring
  int adcValue = analogRead(analogPin);
  float input_v = (adcValue / 1023.0) * SUPPLY_VOLT;
  int resistor_2 = RESISTOR_1 * (input_v / (SUPPLY_VOLT - input_v));  // Resistor we're trying to measure (is connected to ground)
  printCalculations(adcValue, input_v, resistor_2);

  runBeep(resistor_2);

  delay(UPDATE_INTERVAL_MSEC);                  // Wait (UPDATE_INTERVAL_MSEC) milisecs before loop restarts
}

void runBeep(int resistorVal) {
  if (OUTPUT_MODE == 1) {
    beepMorse(resistorVal);
  } else if (OUTPUT_MODE == 2) {
    beepBinary(resistorVal);
  } else if (OUTPUT_MODE == 3) {
    beepCount(resistorVal);
  }
}

void beepMorse(int resistorVal) {
  String numString = String(resistorVal);
  for (int i = 0; i < numString.length(); i++) {
    int digit = numString.charAt(i) - '0';
    const char* code = morseDigits[digit];
    Serial.print(code);
    Serial.print(" ");
    for (int j = 0; code[j] != '\0'; j++) {
      if (code[j] == '.') {
        beep(DOT_TIME);
      } else if (code[j] == '-') {
        beep(DASH_TIME);
      }
      delay(DOT_DASH_SPACE);
    }
    delay(DIGIT_SPACE);
  }
}

void beepBinary(int resistorVal) {
  // makes a buzzer signal a binary code
  LinkedList<int> binList;
  while(resistorVal > 0) {
    int binaryTerm = resistorVal % 2;
    binList.add(binaryTerm);
    resistorVal /= 2;
  }
  int size = binList.size();            // Establishes the size of the list before changing it's size via pops
  for (int i = 0; i < size ; i++) {
    int variable = binList.pop();
    if (variable == 0) {                // Beep TWICE for a ZERO
      beep(100);
      delay(50);
      beep(100);
    } else {                             // Beep ONCE for ONE
      beep(200);
    }
    Serial.print(variable);
    delay(500);
  }
}

void beepCount(int resistorVal) {
  // makes the buzzer buzz for a total number of times
  for (int i=1; i <= resistorVal; i++) {
    //Serial.print(i);                       // Count up on the serial monitor

    digitalWrite(OUTPUT_PIN, HIGH);          // turn the LED on (HIGH is the voltage level)
    delay(COUNT_SPEED);                      // wait for a second
    digitalWrite(OUTPUT_PIN, LOW);           // turn the LED off by making the voltage LOW
    delay(COUNT_SPEED);
    Serial.print("*");
  }
}

void beep(int duration) {
  digitalWrite(OUTPUT_PIN, HIGH);
  delay(duration);
  digitalWrite(OUTPUT_PIN, LOW);
}

void printCalculations(int adcValue,float input_v, int resistor_2) {
  // Prints all calculations to serial  monitor for 
  Serial.print("adcValue: ");
  Serial.println(adcValue);
  Serial.print("input_v: ");
  Serial.println(input_v);
  Serial.print("resistor_2: ");
  Serial.println(resistor_2);
}