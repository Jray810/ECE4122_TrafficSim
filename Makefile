all: trafficSim

trafficSim:
	g++ *.h main.cpp -lpthread

testing:
	g++ *.h testing.cpp -lpthread

clean:
	rm a.out
	rm *.gch