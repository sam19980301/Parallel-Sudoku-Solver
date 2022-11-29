CXX=g++
CXXFLAGS=-O3 -fopenmp -Wall -g -std=c++17 

SRC=solve.cpp sudoku.cpp generator.cpp

# SRC += serial_recursive_solver.cpp
# SRC += serial_nonrecursive_solver.cpp
SRC += serial_crooks_solver.cpp
# SRC += parallel_crooks_solver.cpp

EXE=sudoku

ifeq ($(DEBUG),1)
	CXXFLAGS += -D DEBUG
endif

all: $(EXE)

$(EXE): $(SRC)
	$(CXX) $^ -o $@ $(CFLAGS) -fopenmp
	
.PHONY: clean

clean:
	rm -rf *.o sudoku

# make clean
# make or make DEBUG=1
# ./sudoku