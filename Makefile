CXX=g++
CXXFLAGS=-O3 -fopenmp -Wall -g -std=c++17

SRC=solve.cpp sudoku.cpp
SRC=solve_parallel.cpp sudoku.cpp

# SRC += __serial_recursive_solver.cpp # Dont use
# SRC += serial_nonrecursive_solver.cpp
# SRC += serial_crooks_solver.cpp

# SRC += parallel_nonrecursive_solver.cpp
SRC += parallel_crooks_solver.cpp

EXE=sudoku

# ifeq ($(PROFILE),1)
# 	CXXFLAGS += -D PROFILE
# endif

all: $(EXE)

$(EXE): $(SRC)
	$(CXX) $^ -o $@ $(CFLAGS)
	
.PHONY: clean

clean:
	rm -rf *.o sudoku

# make clean
# make or make PROFILE=1
# ./sudoku