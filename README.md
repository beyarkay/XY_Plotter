# XY Plotter - AKA PlotBot
An Arduino-based robotics project. PlotBot is anchored at the top corners of a drawing area, and then is able to move freely within the space, drawing as its silicon heart desires. 
[Youtube Video](https://youtu.be/_lssg5iOdFE) of PlotBot sketching sin(x).


![image1](readme_files/sin(x)-1.png)

![image1](readme_files/sin(x)-2.png)

![image1](readme_files/sin(x)-3.png)

![image1](readme_files/sin(x)-4.png)


## Circuit Diagram

![Circuit Diagam](readme_files/circuit_diagram.png)

## Bill of Materials
* 2x Stepper motors 
* 2x Stepper motor drivers
* 1x servo with a drawing pen attached
* Power supply
* 1x Arduino Uno
* Various Dupont cables


## Features
* Can move in its drawing space when given x,y co-ordinates, despite only being able to change its position based on the distance it is from each corner
* Python wrapper which communicates through the serial port with the Arduino
* Intelligent serial port detection
