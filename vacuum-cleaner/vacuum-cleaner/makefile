CXX = g++
LOADLIBES = -lm
CXXFLAGS = -Wall -pedantic -o2
OLIST = Direction.o SensorInformation.o AbstractSensor.o AbstractAlgorithm.o Common.o House.o \
		Battery.o Score.o NaiveAlgorithm.o SensorImpl.o Robot.o 

all: simulator

clean: 
	rm *.o simulator

simulator: $(OLIST) Simulator.o
	($CXX) -o simulator $(OLIST) Simulator.o $(CFLAGS)

%.o: %.cpp %.h
	$(CXX) -c $(CXXFLAGS) $<
	
Simulator.o: $(OLIST:.o=.h) $(OLIST:.o=.cpp) Simulator.h Simulator.cpp
	$(CXX) $(CXXFLAGS) Simulator.cpp