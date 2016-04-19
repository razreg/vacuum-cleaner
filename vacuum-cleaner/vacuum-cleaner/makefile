CC = g++
LIBS = -ldl -lboost_thread -lboost_filesystem -lboost_system
CFLAGS = -std=c++11 -O2 -Wall -pedantic -pthread
OLIST = Main.o House.o SensorImpl.o Robot.o Results.o AlgorithmRegistrar.o Simulator.o
ALGOS = Algorithm1.o

all: simulator algorithms

clean:
	-rm -f *.o *.so *.gch simulator

simulator: $(OLIST)
	$(CC) $^ -rdynamic -o $@ $(LIBS)

$(OLIST): %.o: %.cpp
	$(CC) $(CFLAGS) -c $<

algorithms: $(ALGOS) $(ALGOS:.o=.so)

$(ALGOS): %.o: %.cpp
	$(CC) $(CFLAGS) -c $< -mcmodel=large

$(ALGOS:.o=.so): %.so: %.o
	$(CC) -shared -Wl,-soname,$@ -o $@ $<