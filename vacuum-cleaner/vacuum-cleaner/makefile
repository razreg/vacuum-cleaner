CC = g++
LIBS = -ldl -lboost_thread -lboost_filesystem -lboost_system
CFLAGS = -std=c++11 -O2 -Wall -pedantic -pthread # remove -g

all: simulator algorithms

clean:
	-rm -f *.o *.so *.gch simulator

############################ Simulation ############################

OLIST = Main.o House.o SensorImpl.o Robot.o Results.o AlgorithmRegistrar.o Simulator.o

simulator: Common.o $(OLIST)
	$(CC) $^ -rdynamic -o $@ $(LIBS)

$(OLIST): %.o: %.cpp
	$(CC) $(CFLAGS) -c $<

############################ Algorithms ############################

ALGOS = AlgorithmRegistration.o AstarAlgorithm.o _305623571_A.o _305623571_B.o _305623571_C.o
ALGLIBS = AstarAlgorithm.so 305623571_A_.so 305623571_B_.so 305623571_C_.so
MISC = ManhattenAlgorithm.o

algorithms: Common.o algorithms_after_simulator

algorithms_after_simulator: $(MISC) $(ALGOS) $(ALGLIBS)

$(MISC) $(ALGOS): %.o: %.cpp
	$(CC) $(CFLAGS) -c $< -mcmodel=large

AstarAlgorithm.so: AstarAlgorithm.o
	$(CC) -shared -Wl,-soname,$@ -o $@ $< Common.o AlgorithmRegistration.o

305623571_A_.so: _305623571_A.o
	$(CC) -shared -Wl,-soname,$@ -o $@ $< Common.o AlgorithmRegistration.o

305623571_B_.so: _305623571_B.o
	$(CC) -shared -Wl,-soname,$@ -o $@ $< Common.o AlgorithmRegistration.o $(MISC)

305623571_C_.so: _305623571_C.o
	$(CC) -shared -Wl,-soname,$@ -o $@ $< Common.o AlgorithmRegistration.o $(MISC)

############################ ScoreFormula ##########################

score_formula.so: score_formula.cpp
	$(CC) $(CFLAGS) -shared -fPIC -o score_formula.so $<

############################### Common #############################

Common.o: Common.cpp
	$(CC) $(CFLAGS) -c $< -mcmodel=large