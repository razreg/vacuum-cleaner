CC = g++
CFLAGS = -std=c++11 -O2 -Wall -pedantic -pthread
CPPOLIST = Main.o House.o Score.o NaiveAlgorithm.o SensorImpl.o Robot.o Simulator.o
HLIST = Direction.h SensorInformation.h AbstractSensor.h AbstractAlgorithm.h Common.h Battery.h Position.h $(CPPOLIST:.o=.h)
OCLEAN = -rm -f *.o *.gch

all: clean simulator
	$(OCLEAN)

clean:
	$(OCLEAN) simulator

simulator: Main.o $(CPPOLIST)
	$(CC) $^ $(CFLAGS) -o $@

$(CPPOLIST): %.o: %.cpp %.h
	$(CC) -c $(CFLAGS) $<