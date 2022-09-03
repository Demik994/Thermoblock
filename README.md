# **Thermoblock**

###### By Ivan Zivkovic

## About the Project

As a chemistry student, I've strived to learn new skills applicable to scientific experiments during my college years.
This project helped me learn how to make electronic schematics and PCB layouts, but also helped me improve my programming and 3D modeling skills, hence please keep in mind that mentioned skills are not my primary expertise.
I would be very glad if You would point out mistakes I've done because there is no better way to learn something.
The initial idea behind this project was to monitor how temperature affects the thickness of a polyelectrolyte multilayer (PEM) by heating the aluminium block with a sample on it, which is placed on an ellipsometer pedestal.
Of course, this is a general-purpose heater. One of my plans for the future includes making a 3D filament dehydrator.

I wanted to develop Thermoblock with readily available components but keep PCB partially modular so a user can use different I2C displays (certain modifications in code are required) or different heaters.
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

