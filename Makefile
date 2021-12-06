all: trafficSim

trafficSim:
	g++ code/*.h code/main.cpp -lpthread -o trafficSim
	./trafficSim

sfml:
	export DISPLAY=:0.0
	g++ code/*.h code/sfmlDisplay.cpp -lpthread -lsfml-graphics -lsfml-window -lsfml-system -o sfmlSim
	./sfmlSim

testing:
	g++ code/*.h code/testing.cpp -lpthread -o testingSim
	./testingSim

clean:
	rm code/*.gch
	rm *Sim