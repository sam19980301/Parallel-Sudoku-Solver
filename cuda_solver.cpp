#include "cuda_solver.h"
#include "sudoku.h"

int solve(Sudoku *sudoku){
    hostFE(&sudoku->grid);
    return 1;
}