# **Thermoblock**

###### By Ivan Zivkovic

## About the Project

As a chemistry student, I have always strived to learn new skills applicable to scientific experiments. 
This project not only helped me learn how to prepare and eventually make electronic schematics as well as PCB layouts but also improve my programming and 3D modelling skills. 
I would very much so appreciate if you could point out the mistakes I have made because I believe there is no better way to get move forward than to analyze past mistakes, draw conclusions and eventually progress. 
The initial idea behind this project was to monitor how temperature affects the thickness of the polyelectrolyte multilayer (PEM) by heating up the aluminum block placed on an ellipsometer pedestal with a sample on it.
Of course, this is a general-purpose heater. In the future, I would like to upgrade the process by making a 3D filament dehydrator.

Goal was also to develop Thermoblock with readily available components but at the same time keep PCB partially modular. That way, a user can take advantage of utilizing different I2C displays (certain code modifications are required) or different heaters.
This PCB uses following components:

  - 0.91" OLED Display
  - 2x 40W 12V Heaters (the ones most 3D printer hotends use)
  - 2x 100k Ohm NTC Thermistors
  - KY-040 Rotary Encoder Module
  - Arduino Nano (I know this could all be done with the 328P but I don't have SMD soldering station nor knowledge to do that, YET)
  - MOSFET IRL83813
  - 2x 100k Ohm Resistors
  - 10k Ohm resistor
  - 10 uF Capacitor
  - 3 Clamp Terminals
  - 2x 2-pin Male Strip
  - 4-pin Male Strip
  - 5-pin Male Strip
  - 12V 8A Power Supply (I used 350W old PC PSU)

The software uses following libraries:

  - https://github.com/Dlloydev/QuickPID
  - https://github.com/Stutchbury/EncoderButton
  - https://github.com/adafruit/Adafruit_SSD1306
  - https://github.com/adafruit/Adafruit-GFX-Library

