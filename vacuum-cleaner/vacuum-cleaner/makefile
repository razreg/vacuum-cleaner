# object files
obj = Main.o House.o SensorImpl.o Robot.o Results.o AlgorithmRegistrar.o AlgorithmRegistration.o \
	  Simulator.o Score.o
common_obj = Common.o

# shared object files
so_obj = 305623571_A_.so 305623571_B_.so 305623571_C_.so
so_dep_obj = AstarAlgorithm.o

target = simulator

# flags
SHARED_FLAGS = -O2 -Wall -pthread -std=c++11 -pedantic -g
CC_FLAGS = -c $(SHARED_FLAGS)
SO_FLAGS = $(SHARED_FLAGS) -shared -fPIC -mcmodel=large
LD_FLAGS = -lm -lboost_system -lboost_filesystem -ldl
CC = g++

.DEFAULT_GOAL := $(target)
.PHONY: all clean

all: clean $(target)

score: score_formula.so

$(target): $(obj) $(common_obj) $(so_obj)
	$(CC) -rdynamic -o $@ $(obj) $(common_obj) $(LD_FLAGS)

$(so_obj): %_.so: _%.cpp $(so_dep_obj) $(common_obj)
	$(CC) $(SO_FLAGS) $^ -o $@

$(obj): %.o: %.cpp
	$(CC) $(CC_FLAGS) $< -o $@

$(common_obj): %.o: %.cpp
	$(CC) $(CC_FLAGS) -fPIC $< -o $@

$(so_dep_obj): %.o: %.cpp
	$(CC) $(CC_FLAGS) -fPIC $< -o $@

score_formula.so: score_formula.cpp
	$(CC) $(SO_FLAGS) $^ -o $@

clean:
	rm -f $(obj) $(common_obj) $(so_obj) $(so_dep_obj) $(target)