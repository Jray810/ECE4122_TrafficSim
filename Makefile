all: trafficSim

trafficSim:
	g++ code/*.h code/*.cpp sfmlDisplay.cpp -lpthread -fopenmp -lsfml-graphics -lsfml-window -lsfml-system -o trafficSim

sfml:
	export DISPLAY=:0.0
	g++ code/*.h code/*.cpp sfmlDisplay.cpp -lpthread -fopenmp -lsfml-graphics -lsfml-window -lsfml-system -o trafficSim

testing:
	g++ code/*.h code/*.cpp testing.cpp -lpthread -fopenmp -o testingSim

clean:
	rm *Sim