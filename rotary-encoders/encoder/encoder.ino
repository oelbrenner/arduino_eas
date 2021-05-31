// Connections and Layout:
// Connect the common pin on each encoder to gnd.
// Connect a 10k resistor between +5v and each pin on the board you are going to connect to an encoder.
// This pulls the pins high to avoid fluttering mis-reads.

// Board setup:
// analog pins: a0 - a15
// digital pins: d0-d54
// see README for pin restrictions

// Rotary encoder library
#include <Rotary.h>
// set up each encoder's pins and start
// Encoder pins
// {LF, RF, LR, RR}
// {30, 32, 34, 36} <-- A pins
// {31, 33, 35, 37} <-- B pins
Rotary eLF = Rotary(30, 31);
Rotary eRF = Rotary(32, 33);
Rotary eLR = Rotary(34, 35);
Rotary eRR = Rotary(36, 37);

// Screen setup
#include <SPI.h>
#include "Adafruit_GFX.h"
#include "Adafruit_HX8357.h"
  // above libs assume the below pin assignments:
  // CLK connects to SPI clock. On Mega's, its Digital 52
  // MISO connects to SPI MISO. On Mega's, its Digital 50
  // MOSI connects to SPI MOSI. On Mega's, its Digital 51
  //
  // These are 'flexible' lines that can be changed
  // CS connects to our SPI Chip Select pin. We'll be using Digital 10
  // D/C connects to our SPI data/command select pin. We'll be using Digital 9
#define TFT_CS 10
#define TFT_DC 9
#define TFT_RST 8 // RST can be set to -1 if you tie it to Arduino's reset
// Use hardware SPI (on Uno, #13, #12, #11) and the above for CS/DC
Adafruit_HX8357 tft = Adafruit_HX8357(TFT_CS, TFT_DC, TFT_RST);
// Assign human-readable names to some common 16-bit color values:
#define	BLACK   0x0000
#define	BLUE    0x001F
#define	RED     0xF800
#define	GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF
#define ORANGE  0xFD20
// Global vars

byte solenoidActivity_State = LOW;
// how often to check height sensor 
const int heightSensorCheckInterval = 1000;
// stores the value of millis() in each iteration of loop()
unsigned long currentMillis = 0;
// time since encoder was checked
unsigned long heightSensorPreviousCheck = 0;

// height sensor values
int lfSensor = 0;
int rfSensor = 0;
int lrSensor = 0;
int rrSensor = 0;

//current wheel
const int current_wheel[4] = {0, 1, 2, 3};

// height sensor pins
const int lfSensorPin = 6;
const int rfSensorPin = 7;
const int lrSensorPin = 8;
const int rrSensorPin = 9;

// height selection encoder value defaults
int lfSelection = 25;
int rfSelection = 25;
int lrSelection = 25;
int rrSelection = 25;

// valve block controls
const int lfValvePin = 10;
const int rfValvePin = 11;
const int lrValvePin = 12;
const int rrValvePin = 13;
const int DiaphramValvePin = 14;
const int OutletValvePin   = 15;
const int InletValvePin    = 16;

// individual wheel dwell time
int singleDwell = 200;
// all wheels dwell time
int allDwell = 300;

void setup () {
  Serial.begin(9600);
  // refactor this to blink LED for boot sequence
  Serial.print("Booting..");
  delay(20);
  Serial.println("Done");
  // start up the TFT screen
  tft.begin();
  // rotation 1 is landscape with SPI pins on right side
  tft.setRotation(1);
  blackScreen();
  //splashScreen();
  //blackScreen();

  // set height selection boxes
  heightSelectBoxes(WHITE);
  // set up the color bars
  Serial.println("building color bars");
  colorSegments();

  // startup encoders
  Serial.print("Initializing encoder: LF");
  eLF.begin(false, true);
  Serial.println(" Done");

  Serial.print("Initializing encoder: RF");
  eRF.begin(false, true);
  Serial.println(" Done");

  Serial.print("Initializing encoder: LR");
  eLR.begin(false, true);
  Serial.println(" Done");

  Serial.print("Initializing encoder: RR");
  eRR.begin(false, true);
  Serial.println(" Done");
  
}

void loop() {
  //Serial.println("read height sensors");
  read_height_sensors();
  //Serial.println("---------");
  check_encoders();
  //Serial.println("Loop complete.");
}

// fill screen with black
unsigned long blackScreen() {
  tft.fillScreen(HX8357_BLACK);
}

unsigned long colorSegments() {
  // position: left-right,up-down,width,height,color
  // color segments: red, yellow, green orange
  // indicator: black outline: 0-113,  white fill: 1-114
  LFcolorSegment();
  RFcolorSegment();
  LRcolorSegment();
  RRcolorSegment();
}

// Left Front
void LFcolorSegment() {
  tft.fillRect(0,  0, 21, 10, RED);
  tft.fillRect(0, 10, 21, 30, YELLOW);
  tft.fillRect(0, 40, 21, 50, GREEN);
  tft.fillRect(0, 90, 21, 30, ORANGE);
  tft.fillRect(21, 1, 10, 118, BLACK); // black out area under indicator
}
int LFindicator(int lfSensor) {
  // indicator top:0,1  bottom 114,115
  LFcolorSegment();
  tft.drawRect(9,  lfSensor,      22, 8, BLACK);
  tft.fillRect(10, (lfSensor +1), 21, 6, WHITE);
}

// Right Front
void RFcolorSegment() {
  tft.fillRect(459,  0, 21, 10, RED); // RF
  tft.fillRect(459, 10, 21, 30, YELLOW); // RF
  tft.fillRect(459, 40, 21, 50, GREEN); // RF
  tft.fillRect(459, 90, 21, 30, ORANGE); // RF
  tft.fillRect(447,  1, 12, 118, BLACK); // black out area under indicator
}
int RFindicator(int rfSensor) {
  // indicator top:200,201  bottom 314,315
  RFcolorSegment();
  tft.drawRect(448, rfSensor,      22, 8, BLACK);
  tft.fillRect(447, (rfSensor +1), 21, 6, WHITE);
}

// Left Rear
void LRcolorSegment() {
  tft.fillRect(0,  200, 21, 10, RED); // LR
  tft.fillRect(0,  210, 21, 30, YELLOW); // LR
  tft.fillRect(0,  240, 21, 50, GREEN); // LR
  tft.fillRect(0,  290, 21, 30, ORANGE); // LR
  tft.fillRect(21, 201, 10, 118, BLACK); // black out area under indicator
}
int LRindicator(int lrSensor) {
  // indicator top:200,201  bottom 314,315
  LRcolorSegment();
  tft.drawRect(9,  lrSensor,      22, 8, BLACK);
  tft.fillRect(10, (lrSensor +1), 21, 6, WHITE);
}

void RRcolorSegment() {
  // Right Rear
  tft.fillRect(459, 200, 21, 10, RED); // RR
  tft.fillRect(459, 210, 21, 30, YELLOW); // RR
  tft.fillRect(459, 240, 21, 50, GREEN); // RR
  tft.fillRect(459, 290, 21, 30, ORANGE); // RR
  tft.fillRect(447, 201, 12, 118, BLACK); // black out area under indicator
}
int RRindicator(int rrSensor) {
  // indicator top:200,201  bottom 314,315
  RRcolorSegment();
  tft.drawRect(448, rrSensor,      22, 8, BLACK);
  tft.fillRect(447, (rrSensor +1), 21, 6, WHITE);
}

int read_height_sensors() {

  if (millis() - heightSensorPreviousCheck >= heightSensorCheckInterval) {
    Serial.println("times up! checking height sensors");
    heightSensorPreviousCheck = millis();

    // analog read the height sensors then map to the range of the display
    // keep this short of the box size on both ends
    lfSensor = map(analogRead(lfSensorPin), 0, 1023, 111, 1);
    LFindicator(lfSensor);

    rfSensor = map(analogRead(lfSensorPin), 0, 1023, 111, 1);
    RFindicator(rfSensor);

    lrSensor = map(analogRead(lfSensorPin), 0, 1023, 311, 201);
    LRindicator(lrSensor);

    rrSensor = map(analogRead(lfSensorPin), 0, 1023, 311, 201);
    RRindicator(rrSensor);

    // map: (pot value low, pot value high, range start, range end)
    //current_wheel = "lf"
    //update_height(lfsensor);
    return lfSensor;
  }
  else {
    //Serial.print("NOT checking encoders");
    //Serial.println();
  }

}

// startup message
unsigned long splashScreen() {
  unsigned long start = micros();
  tft.setCursor(0, 0);
  tft.setTextColor(ORANGE);    tft.setTextSize(3);
  tft.println("OMFG industries");
  tft.println();
  tft.setTextColor(HX8357_GREEN);
  tft.setTextSize(5);
  tft.println("Range Rover P38");
  tft.println();
  tft.setTextColor(HX8357_RED);    tft.setTextSize(3);
  tft.println("Suspension Controller");
  tft.println();
  tft.setTextSize(1);
  tft.setTextColor(HX8357_WHITE);
  tft.println(F("the software is provided as is, without warranty of any kind, express or implied, including but not limited to the warranties of merchantability, fitness for a particular purpose and noninfringement. in no event shall the authors or copyright holders be liable for any claim, damages or other liability, whether in an action of contract, tort or otherwise, arising from, out of or in connection with the software or the use or other dealings in the software."));
  delay(1500);
  return micros() - start;
}

unsigned long heightSelectBoxes(uint16_t color) {
  // text rectangle
  tft.drawRect(20, 0, 100, 120, color); // left front
  tft.drawRect(360, 0, 100, 120, color); // right front
  tft.drawRect(20, 200, 100, 120, color); // left rear
  tft.drawRect(360, 200, 100, 120, color); // right rear
  // fill the background
  tft.fillRect(24, 74, 92, 42, BLACK); // LF background
  tft.fillRect(364, 74, 92, 42, BLACK);// RF background
  tft.fillRect(24, 274, 92, 42, BLACK);// LR background
  tft.fillRect(364, 274, 92, 42, BLACK);// RR background
  // put values in box
  tft.setTextColor(WHITE, BLACK);
  tft.setTextSize(5);
  tft.setCursor(36, 74);
  tft.println("LF");
  tft.setCursor(376, 74);
  tft.println("RF");
  tft.setCursor(36, 274);
  tft.println("LR");
  tft.setCursor(376, 274);
  tft.println("RR");
}


void check_encoders() {
  unsigned char eLFstatus = eLF.process();
  // debug state transitions
  //Serial.print(digitalRead(30));
  //Serial.print(" ");
  //Serial.println(digitalRead(31));
  if (eLFstatus) {
    Serial.print("encoder: LF");
    //Serial.println(eLFstatus == DIR_CW ? " Down v" : "Up   ^");
    (eLFstatus == DIR_CW ? decrement_encoder(0) : increment_encoder(0));
  }

  unsigned char eRFstatus = eRF.process();
  if (eRFstatus) {
    Serial.print("encoder: RF");
    Serial.println(eRFstatus == DIR_CW ? " Down v" : "Up   ^");
  }

  unsigned char eLRstatus = eLR.process();
  if (eLRstatus) {
    Serial.print("encoder: LR");
    Serial.println(eLRstatus == DIR_CW ? " Down v" : "Up   ^");
  }

  unsigned char eRRstatus = eRR.process();
  if (eRRstatus) {
    Serial.print("encoder: RR");
    Serial.println(eRRstatus == DIR_CW ? " Down v" : "Up   ^");
  }
}

void increment_encoder (int encoderNum) {
  switch(encoderNum) {
    case 0:
      lfSelection = --lfSelection;
      tft.fillRect(24, 74, 92, 42, BLACK); // LF background
      tft.setCursor(34, 74);
      tft.println(lfSelection);
      Serial.println(lfSelection);
    break;
  }

}
void decrement_encoder (int encoderNum) {
  switch(encoderNum) {
    case 0:
      lfSelection = ++lfSelection;
      tft.fillRect(24, 74, 92, 42, BLACK); // LF background
      tft.setCursor(36, 74);
      tft.println(lfSelection);
    break;
  }

}

void up_arrow() {
  //fillTriangle(int16_t x1, int16_t y1, int16_t x2, int16_t y2, int16_t x3, int16_t y3,// uint16_t t)
  //tft.fillTriangle(x1,y1,x2,y2,x3,y3,t);
}

void down_arrow() {
  //fillTriangle(int16_t x1, int16_t y1, int16_t x2, int16_t y2, int16_t x3, int16_t y3,// uint16_t t)
  //tft.fillTriangle(x1,y1,x2,y2,x3,y3,t);
}


void raise_wheel(int wheelID) {
  Serial.print("Raising wheel: ");
  Serial.print(wheelID);
  Serial.println();
  Serial.print("opening inlet valve");
  digitalWrite(InletValvePin, HIGH);
  Serial.print("opening wheel valve");
  digitalWrite(wheelID, HIGH);
  delay(singleDwell);
  digitalWrite(InletValvePin, LOW);
  digitalWrite(wheelID, LOW);
}

void raise_all() {
  Serial.print("Raise all wheels");
  Serial.println();
  Serial.print("opening inlet valve");
  digitalWrite(InletValvePin, HIGH);
  Serial.print("opening wheel valves");
  digitalWrite(lfValvePin, HIGH);
  digitalWrite(rfValvePin, HIGH);
  digitalWrite(lrValvePin, HIGH);
  digitalWrite(rrValvePin, HIGH);
  delay(allDwell);
  digitalWrite(lfValvePin, LOW);
  digitalWrite(rfValvePin, LOW);
  digitalWrite(lrValvePin, LOW);
  digitalWrite(rrValvePin, LOW);
  digitalWrite(InletValvePin, LOW);
}

void lower_wheel(int wheelID) {
  Serial.print("Lowering wheel: ");
  Serial.print(wheelID);
  Serial.println();
  Serial.print("opening outlet valve");
  digitalWrite(OutletValvePin, HIGH);
  Serial.print("opening wheel valve");
  digitalWrite(wheelID, HIGH);
  delay(singleDwell);
  digitalWrite(OutletValvePin, LOW);
  digitalWrite(wheelID, LOW);
}

void lower_all() {
  Serial.print("Lower all wheels");
  Serial.println();
  Serial.print("opening outlet valve");
  digitalWrite(OutletValvePin, HIGH);
  Serial.print("opening wheel valves");
  digitalWrite(lfValvePin, HIGH);
  digitalWrite(rfValvePin, HIGH);
  digitalWrite(lrValvePin, HIGH);
  digitalWrite(rrValvePin, HIGH);
  delay(allDwell);
  digitalWrite(rrValvePin, LOW);
  digitalWrite(lrValvePin, LOW);
  digitalWrite(rfValvePin, LOW);
  digitalWrite(lfValvePin, LOW);
  digitalWrite(OutletValvePin, LOW);
}
