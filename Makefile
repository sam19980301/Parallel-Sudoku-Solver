CXX=gcc
CXXFLAGS=-O3 -Wall -g -std=c++17

SRC=solve.cpp sudoku.cpp

SRC += serial_recursive_solver.cpp
# SRC += serial_nonrecursive_solver.cpp
# SRC += serial_crooks_solver.cpp
# SRC += parallel_crooks_solver.cpp

EXE=sudoku

ifeq ($(DEBUG),1)
	CXXFLAGS += -D DEBUG
endif

all: $(EXE)

$(EXE): $(SRC)
	$(CXX) $^ -o $@ $(CFLAGS)
	
.PHONY: clean

clean:
	rm -rf *.o sudoku

# make clean
# make or make DEBUG=1
# ./sudoku