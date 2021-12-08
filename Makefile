all: trafficSim

trafficSim:
	g++ code/*.h code/*.cpp sfmlDisplay.cpp -lpthread -fopenmp -lsfml-graphics -lsfml-window -lsfml-system -o trafficSim
	./trafficSim

sfml:
	export DISPLAY=:0.0
	g++ code/*.h code/*.cpp sfmlDisplay.cpp -lpthread -fopenmp -lsfml-graphics -lsfml-window -lsfml-system -o sfmlSim
	# ./sfmlSim -S

testing:
	g++ code/*.h code/*.cpp testing.cpp -lpthread -fopenmp -o testingSim
	# ./testingSim -S

clean:
	rm code/*.gch
	rm *Sim