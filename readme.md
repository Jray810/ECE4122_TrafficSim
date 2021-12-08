# Project Description
This project is a simulation of a four way traffic intersection and a measurement of traffic flow across the intersection. This project was inspired by MIT’s Senseable City Lab - Light Traffic (http://senseable.mit.edu/light-traffic/). The project investigates and compares the traffic flow between existing Stop-Sign infrastructure and fully Autonomous intersection control. Ultimately, this project has the potential to be used as a possible framework for future investigations into autonomous traffic intersections.

A video demonstration of the existing project can be found here: 

# Project Operation Instructions
This project is in C++ and requires a G++ compiler as well as OpenMP and SFML support.

All commands below are run in the main folder. We used Ubuntu 20.04 on Windows WSL and installed XMing for graphical support.

#
To compile the simulation simply do:
```
$ make
```
or
```
$ g++ code/*.h code/*.cpp sfmlDisplay.cpp -lpthread -fopenmp -lsfml-graphics -lsfml-window -lsfml-system -o trafficSim
```

#
To run the simulation, make sure you start XMing first:
```
$ export DISPLAY=:0.0
$ ./trafficSim
```
You can also customize which traffic controller simulation you want to run (Autonomous by default):
```
$ ./trafficSim -A   # For autonomous version
$ ./trafficSim -L   # For traffic light version (Currently unsupported)
$ ./trafficSim -S   # For stop sign version
```
You can also customize the tick speed at which the simulation is running at (100 ms by default). Your input value will be in microseconds and must be the third command line argument (after traffic controller type selection). For example here is an autonomous version with tick speed of 1 second:
```
$ ./trafficSim -A 1000000
```
You can also customize the probability (in percent) at which the simulation spawns a vehicle per tick. Your input value will be a whole number between 1 and 100 inclusive and must be the fourth command line argument. For example here is an autonomous version with tick speed of 1 ms and probability of 50%:
```
$ ./trafficSim -A 1000 50
```

# Makefile Commands:
To compile simulation for SFML display with executable trafficSim, run:
```
$ make all
```
To compile simulation for no display debugging testing, make sure you set DEBUG to 1 in the debugSetup.h file, then compile with:
```
$ make testing
```
To clean up unwanted executables and compilation files:
```
$ make clean
```

# Known Issues
Light Controlled - Traffic Controller: Currently not supported. Pending future development if time allows.

#
*Created by Marcus Chan and Raymond Jia*

*ECE @ Georgia Institute of Technology, Fall 2021*
