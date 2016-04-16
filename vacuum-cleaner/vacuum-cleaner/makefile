CC = g++
CFLAGS = -std=c++11 -O2 -Wall -pedantic -pthread -lboost_thread -lboost_filesystem -lboost_system
OLIST = Main.o House.o Score.o NaiveAlgorithm.o SensorImpl.o Robot.o Simulator.o
OCLEAN = -rm -f *.o *.gch
OLIST = Main.o House.o Score.o NaiveAlgorithm.o SensorImpl.o Robot.o Results.o Simulator.o


all: clean simulator
	$(OCLEAN)
all: simulator

clean:
	$(OCLEAN) simulator
	-rm -f *.o *.gch simulator

simulator: $(OLIST)
	$(CC) $^ $(CFLAGS) -o $@

$(OLIST): %.o: %.cpp %.h
	$(CC) -c $(CFLAGS) $<