#include <stdlib.h>
#include "sudoku.h"

void sudoku_init(Sudoku *sudoku){
    sudoku->grid = (Grid) malloc(N * N * sizeof(int));
}

void sudoku_reset(Sudoku *sudoku){
    for (int i = 0; i < N; i++)
        for (int j = 0; j < N; j++)
            sudoku->grid[i][j] = UNASSIGNED;
}

void sudoku_free(Sudoku *sudoku){
    free(sudoku->grid);
}

// serialized & recursive solver
int serial_recursive_solver(Grid grid, int row, int col){
    #ifdef DEBUG
        printf("Solving %d\t%d\n", row, col);
        print_sudoku(grid);
    #endif

    // finish iteration
    if (row == N)
        return 1;
    int next_row = row + (col == N-1), next_col = (col + 1)%N;
    // skip if assigned
    if (grid[row][col] != UNASSIGNED)
        return serial_recursive_solver(grid, next_row, next_col);
    // try all possible value
    for (int v = 1; v <= N; ++v){
        // single guess
        if (is_legal(grid, row, col, v)){
            #ifdef DEBUG
                print_sudoku(grid);
            #endif
            grid[row][col] = v;
            if (serial_recursive_solver(grid, next_row, next_col))
                return 1;
        }
        else{
            #ifdef DEBUG
                printf("Invalid insertion %d\t%d\t%d\n", row, col, v);
            #endif
        }
        // undo guess
        grid[row][col] = UNASSIGNED;
    }
    return 0;
}

int solve(Sudoku *sudoku){
    return serial_recursive_solver(sudoku->grid, 0, 0);
}