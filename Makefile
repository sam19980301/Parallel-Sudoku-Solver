CXX=g++
CXXFLAGS=-O3 -Wall -g -std=c++17 -fopenmp

# Serial or Parallel
ifeq ($(SERIAL),1)
	SRC=solve.cpp
endif

ifeq ($(PARALLEL),1)
	SRC=solve_parallel.cpp
endif

# Brute force, Crooks, or Cross hatching
ifeq ($(BRUTE_FORCE),1)
	SRC += serial_nonrecursive_solver.cpp parallel_nonrecursive_solver.cpp
endif

ifeq ($(CROOKS),1)
	SRC += serial_crooks_solver.cpp parallel_crooks_solver.cpp
endif

ifeq ($(CROSS_HATCHING),1)
	SRC += serial_crosshatching_solver.cpp parallel_crosshatching_solver.cpp
endif

SRC += sudoku.cpp
EXE=sudoku

all: $(EXE)

$(EXE): $(SRC)
	$(CXX) $^ -o $@ $(CXXFLAGS)
	
.PHONY: clean

clean:
	rm -rf *.o sudoku