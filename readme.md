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
