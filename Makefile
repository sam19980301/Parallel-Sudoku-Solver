CXXFLAGS=-O3 -std=c++17 -g
ifeq ($(DEBUG),1)
	CXXFLAGS += -D DEBUG
endif

all: sudoku
	
sudoku:
	gcc $(CXXFLAGS) -o sudoku sudoku.cpp

clean:
	rm -rf sudoku