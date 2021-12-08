# Project Operation Instructions
This project is in C++ and requires a G++ compiler as well as OpenMP and SFML support.

All commands below are run in the main folder. We used Ubuntu 20.04 on Windows WSL and installed XMing for graphical support.

*To Compile the Simulation*
---

make
---
or
---

g++ code/*.h code/*.cpp sfmlDisplay.cpp -lpthread -fopenmp -lsfml-graphics -lsfml-window -lsfml-system -o trafficSim
---

*To Run the Simulation*
Make sure you start XMing first
---

export DISPLAY=:0.0
./trafficSim [-T] [-R] [-P]
---


The Makefile contains the following commands:

# $make all
compiles main.cpp and all headers, executable in a.out

# $make trafficSim
same effect as make all

# $make testing
compiles testing.cpp instead of main.cpp, and all headers. Executable in a.out

# $make clean
removes a.out and all \*.gch files


# Known Issues
Light Controlled - Traffic Controller: Vehicles are in the same physical lane but not in the same "lane" classification. As a result, the lane queues are unaware of other vehicles in the same physical lane, resulting in superpositioning of vehicles on top of each other in the same physical lane. 
