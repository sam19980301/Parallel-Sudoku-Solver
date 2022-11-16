#include "sudoku.h"

// serialized & recursive solver
int serial_recursive_solver(Grid *grid, int row, int col){
    // finish iteration
    if (row == N)
        return 1;
    int next_row = row + (col == N-1), next_col = (col + 1)%N;
    // skip if assigned
    if ((*grid)[row][col] != UNASSIGNED)
        return serial_recursive_solver(grid, next_row, next_col);
    // try all possible value
    for (int v = 1; v <= N; ++v){
        // single guess
        if (is_safe(grid, row, col, v)){
            #ifdef DEBUG
                show_grid(grid);
            #endif
            (*grid)[row][col] = v;
            if (serial_recursive_solver(grid, next_row, next_col))
                return 1;
        }
        else{
            #ifdef DEBUG
                printf("Invalid insertion %d\t%d\t%d\n", row, col, v);
            #endif
        }
        // undo guess
        (*grid)[row][col] = UNASSIGNED;
    }
    return 0;
}

int solve(Sudoku *sudoku){
    return serial_recursive_solver(&sudoku->grid, 0, 0);
}