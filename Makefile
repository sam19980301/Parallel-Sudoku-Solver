CXX=gcc
CXXFLAGS=-O3 -Wall -g -std=c++17
# IFLAGS=-include
# LFALGS=-lpthread

SRC=sudoku.cpp solve.cpp serial_recursive_solver.cpp
# SRC=sudoku.cpp solve.cpp serial_nonrecursive_solver.cpp
OBJ=$(SRC:.cpp=.o)
EXE=sudoku

ifeq ($(DEBUG),1)
	CXXFLAGS += -D DEBUG
endif

all: $(EXE)
	
$(EXE): $(OBJ)
	$(CXX) $^ -o $@ $(CFLAGS)

%.o: %.c
	$(CXX) -c $^ -o $@ $(CFLAGS)
	
.PHONY: clean

clean:
	rm -rf *.o sudoku

# make clean
# make or make DEBUG=1
# ./sudoku