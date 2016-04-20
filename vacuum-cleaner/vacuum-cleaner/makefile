CC = g++
LIBS = -ldl -lboost_thread -lboost_filesystem -lboost_system
CFLAGS = -std=c++11 -O2 -Wall -pedantic -pthread
OLIST = Main.o House.o SensorImpl.o Robot.o Results.o AlgorithmRegistrar.o Simulator.o
ALGOS = _305623571_A.o _305623571_B.o _305623571_C.o
ALGLIBS = 305623571_A_.so 305623571_B_.so 305623571_C_.so

all: simulator algorithms

clean:
	-rm -f *.o *.so *.gch simulator

simulator: $(OLIST)
	$(CC) $^ -rdynamic -o $@ $(LIBS)

$(OLIST): %.o: %.cpp
	$(CC) $(CFLAGS) -c $<

algorithms: ManhattenAlgorithm.o $(ALGOS) $(ALGLIBS)

$(ALGOS): %.o: %.cpp
	$(CC) $(CFLAGS) -c $< -mcmodel=large

ManhattenAlgorithm.o: ManhattenAlgorithm.cpp
	$(CC) $(CFLAGS) -c $< -mcmodel=large

305623571_A_.so: _305623571_A.o
	$(CC) -shared -Wl,-soname,$@ -o $@ $<

305623571_B_.so: _305623571_B.o
	$(CC) -shared -Wl,-soname,$@ -o $@ $< ManhattenAlgorithm.o

305623571_C_.so: _305623571_C.o
	$(CC) -shared -Wl,-soname,$@ -o $@ $< ManhattenAlgorithm.o