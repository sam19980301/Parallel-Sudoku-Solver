#include "sudoku.h"
#include "omp.h"

void crook_pruning(Sudoku *sudoku);

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

void crook_pruning(Sudoku *sudoku){
    int changed, e_changed, l_changed;
    do
    {
        changed = 0;
        do
        {
            e_changed = elimination(sudoku);
            changed += e_changed;
        } while (e_changed);
        do
        {
            l_changed = lone_ranger(sudoku);
            changed += l_changed;
        } while (l_changed);
    } while (changed);
}

int solve(Sudoku *sudoku){
    crook_pruning(sudoku);
    return serial_recursive_solver(&sudoku->grid, 0, 0);
}