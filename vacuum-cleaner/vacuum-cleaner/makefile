CC = g++
CFLAGS = -std=c++11 -O2 -Wall -pedantic -pthread -lboost_thread -lboost_filesystem -lboost_system
OLIST = Main.o House.o Score.o NaiveAlgorithm.o SensorImpl.o Robot.o Simulator.o Algorithm1.o Algorithm2.o Algorithm3.o
OCLEAN = -rm -f *.o *.gch

all: clean simulator
	$(OCLEAN)

clean:
	$(OCLEAN) simulator

simulator: $(OLIST)
	$(CC) $^ $(CFLAGS) -o $@

$(OLIST): %.o: %.cpp %.h
	$(CC) -c $(CFLAGS) $<