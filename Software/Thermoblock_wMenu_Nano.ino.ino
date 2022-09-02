#include <QuickPID.h>
#include <SPI.h>
#include <Wire.h>
#include <EncoderButton.h>
#include <Wire.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_GFX.h>
#include <math.h>

// pins
#define thermPin1 A6
#define thermPin2 A7
#define encoderPin1 2
#define encoderPin2 3
#define switchPin 4
#define heaterPin 5
#define OLED_RESET     -1

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels


#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// constants
int avgSize = 10; // averaging size

float R1 = 100000; // NTC and voltage divider resistance in Ohms
float c1 = 1.009249522e-03, c2 = 2.378405444e-04, c3 = 2.019202697e-07; //NTC thermistor coefficients

const unsigned long windowSize = 2000; //PID time period
const byte debounce = 50; //PID debounce
float Input, Output, Setpoint = 0, Kp = 20, Ki = 10, Kd = 45.0; //PID coefficients

float avgT; //average temperature from both thermistors
float avgT1; //average temperature from thermistor #1
float avgT2; //average temperature from thermistor #2

int initialT; //initial temperature
int setpointT; //temperature set by user

byte clickState; //detecting encoder click
bool doubleClickState = false; //detecting encoder double click
byte menuCount = 0; //menu manager
byte dir = 0; //encoder direction
bool runState = false; //is heater running or not

int Vo; //thermistor voltage
float logR2, R2; //thermistor resistance and logarithm

// status
unsigned long windowStartTime, nextSwitchTime;
boolean relayStatus = false;

QuickPID myPID(&Input, &Output, &Setpoint, Kp, Ki, Kd,
               myPID.pMode::pOnError,
               myPID.dMode::dOnMeas,
               myPID.iAwMode::iAwClamp,
               myPID.Action::direct);

/**
 * Instatiate an EncoderButton.
 * For Arduino Uno, the hardware interrupts are pins 2 & 3
 * Encoder+button:
 * EncoderButton(byte encoderPin1, byte encoderPin2, byte switchPin);
 * Encoder only:
 * EncoderButton(byte encoderPin1, byte encoderPin2);
 * Button only:
 * EncoderButton(byte switchPin);
 */
EncoderButton eb1(encoderPin1, encoderPin2, switchPin);
volatile unsigned int encoder0Pos = 0;

/**
 * A function to handle the 'clicked' event
 * Can be called anything but requires EncoderButton& 
 * as its only parameter.
 */
void onEb1Clicked(EncoderButton& eb) {
  
  if (eb.clickCount() == 1) {
    clickState = LOW;
    //eb.position() == 0;
    //encoder0Pos = 0;
  }
  if (eb.clickCount() == 2) {
    doubleClickState = true;
    
  }
  
}

/**
 * A function to handle the 'encoder' event
 */
void onEb1Encoder(EncoderButton& eb) {
  
  if (eb.increment() > 0) {
    encoder0Pos += 1;
  }
  if (eb.increment() < 0) {
    encoder0Pos -= 1;
  }

  
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  delay(500);
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }

  // Show initial display buffer contents on the screen --
  // the library initializes this with an Adafruit splash screen.
  display.display();
  delay(1000); // Pause for 1 second

  // Clear the buffer
  display.clearDisplay();
  
  pinMode(thermPin1, INPUT);
  pinMode(thermPin2, INPUT);
  pinMode(heaterPin, OUTPUT);
  

  analogReference(EXTERNAL); //AREF pin is connected to Arduino 3.3V output pin for a more stable signal

  
  myPID.SetOutputLimits(0, windowSize);
  myPID.SetSampleTimeUs(windowSize * 1000);
  myPID.SetMode(myPID.Control::automatic);

  //Link the event(s) to your function
  eb1.setClickHandler(onEb1Clicked);
  eb1.setEncoderHandler(onEb1Encoder);
  
}

void loop() {
  // You must call update() for every defined EncoderButton.
  // This will update the state of the encoder & button and 
  // fire the appropriate events.
  clickState = HIGH;
  eb1.update();
  menuCheck();
  staticMenu(tempRead(1), tempRead(2), tempRead(3), setpointT);
  Setpoint = setpointT;
  heaterControl();     
  display.clearDisplay();
}

float tempRead(int sensorValue) {
  float T_sum = 0.0; //sum of temperatures for averaging
  
  if (sensorValue == 1) {
    for (int i=0 ; i<avgSize ; i++){
      
      Vo = analogRead(thermPin1);
      R2 = R1/10 * (1023.0 / (float)Vo - 1.0);
      logR2 = log(R2);
      T_sum += ((1.0 / (c1 + c2*logR2 + c3*logR2*logR2*logR2))-273);
      
      
        
    }
    avgT = T_sum/float(avgSize);
  }

  if (sensorValue == 2) {
    for (int i=0 ; i<avgSize ; i++){
              
      Vo = analogRead(thermPin2);
      R2 = R1/10 * (1023.0 / (float)Vo - 1.0);
      logR2 = log(R2);
      T_sum += ((1.0 / (c1 + c2*logR2 + c3*logR2*logR2*logR2))-273);
      
        
    }
    avgT = T_sum/float(avgSize);
  }

  if (sensorValue == 3) {
    for (int i=0 ; i<avgSize ; i++){
      
      Vo = analogRead(thermPin1);
      R2 = R1/10 * (1023.0 / (float)Vo - 1.0);
      logR2 = log(R2);
      T_sum += ((1.0 / (c1 + c2*logR2 + c3*logR2*logR2*logR2))-273);
      
      Vo = analogRead(thermPin2);
      R2 = R1/10 * (1023.0 / (float)Vo - 1.0);
      logR2 = log(R2);
      T_sum += ((1.0 / (c1 + c2*logR2 + c3*logR2*logR2*logR2))-273);
      
        
    }
    avgT = T_sum/(2*float(avgSize));
  }
 
  return avgT; //returns average temperature for called sensor(s)
}

void staticMenu(float avgT1, float avgT2, float avgT, int setpoint) {
 
    
  //---------------------------------
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(1, 0);
  display.print("T1:");
  display.println(round(avgT1));
 
  display.setCursor(46, 0);
  display.print("T2:");
  display.println(round(avgT2));
  
  display.setCursor(91, 0);
  display.print("Ta:");
  display.println(round(avgT));
  
  
  
  display.setTextSize(1);
  display.setCursor(10, 10);
  display.println("Setpoint:");
  display.setCursor(65, 10);
  display.println(setpoint);

  

  display.setCursor(10, 20);
  display.println("Start:");
  display.setCursor(45, 20);
  if (doubleClickState == true && menuCount == 1) { //if double clicked and cursor set on "Start:" changes heater state
    runState = !runState;   
  }

  if (doubleClickState == true && menuCount == 0) { //if double clicked and cursor set on "Setpoint:" resets setpoint temperature
    setpointT = 0;  
  }


  if (runState == true) {
    display.println("Run!");
  }  
          
  if (runState == false) {
    display.println("Idle");
  }
  doubleClickState = false; 

  display.setCursor(2, (menuCount * 10) + 10); //changing cursor position
  display.println(">");

  display.display();
}

void heaterControl() {
  if (runState == true) {
    unsigned long msNow = millis();
    Input = tempRead(3); // average temperature from both sensors
    if (Setpoint < avgT) {
      Setpoint -= 10; //temperature setpoint correction because of overshoot, could be different value, not from original QuickPID
      if (myPID.Compute()) windowStartTime = msNow;

    if (!relayStatus && Output > (msNow - windowStartTime)) {
      if (msNow > nextSwitchTime) {
        nextSwitchTime = msNow + debounce;
        relayStatus = true;
        digitalWrite(heaterPin, HIGH);
        
      }
    } else if (relayStatus && Output < (msNow - windowStartTime)) {
      if (msNow > nextSwitchTime) {
        nextSwitchTime = msNow + debounce;
        relayStatus = false;
        digitalWrite(heaterPin, LOW);
      }
    }
    Setpoint += 10;
    }
    if (Setpoint >= avgT) {
      Setpoint += 10; //temperature setpoint correction because of overshoot, could be different value
      if (myPID.Compute()) windowStartTime = msNow;

    if (!relayStatus && Output > (msNow - windowStartTime)) {
      if (msNow > nextSwitchTime) {
        nextSwitchTime = msNow + debounce;
        relayStatus = true;
        digitalWrite(heaterPin, HIGH);
        
      }
    } else if (relayStatus && Output < (msNow - windowStartTime)) {
      if (msNow > nextSwitchTime) {
        nextSwitchTime = msNow + debounce;
        relayStatus = false;
        digitalWrite(heaterPin, LOW);
      }
    }
    Setpoint -= 10; //temperature setpoint correction because of overshoot, could be different value
    }    
  }    
}
void menuCheck() {
  if (clickState == LOW && menuCount < 2) {
    menuCount++;
  }

  if (clickState == LOW && menuCount >= 2) { //resets cursor to first option
    menuCount = 0;
    encoder0Pos = 0;
  }

  if (menuCount == 0) {
    initialT = encoder0Pos;
    encoder0Pos = 0;
    setpointT += initialT;
    setpointT = constrain(setpointT, 0, 250);    
  }  
}

