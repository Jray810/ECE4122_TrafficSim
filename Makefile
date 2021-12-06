all: trafficSim

trafficSim:
	g++ code/*.h code/main.cpp -lpthread -o trafficSim

testing:
	g++ code/*.h code/testing.cpp -lpthread -o testingSim

clean:
	rm *Sim
	rm code/*.gch