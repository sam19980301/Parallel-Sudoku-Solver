#include "sudoku.h"
#include "stdio.h"

// serialized & non-recursive solver
int solve(Sudoku *sudoku){
    Grid *grid = &sudoku->grid;
    Heap *heap = &sudoku->heap;
    Cell top_cell;
    int row = 0, col = 0, val;
    int find_safe_insertion;
    while (row != N)
    {
        find_safe_insertion = 0;
        if ((*grid)[row][col] == UNASSIGNED){
            for (int v = 1; v <= N; v++){
                if (is_safe(grid, row, col, v)){
                    // find safe insertion
                    Cell cell = {row, col, v};
                    heap_push(heap, &cell, &sudoku->markup, &sudoku->grid);
                    (*grid)[row][col] = v;
                    find_safe_insertion = 1;
                    break;
                }
            }
            while (!find_safe_insertion){
                if (heap->count == 0)
                    return 0;
                // fail to find safe insertion, backtrack
                // try other value for the top cell
                heap_pop(heap, &top_cell, &sudoku->markup, &sudoku->grid);
                row = top_cell.row;
                col = top_cell.col;
                (*grid)[row][col] = UNASSIGNED;
                for (val =  top_cell.val + 1; val <= N; val++)
                {
                    if (is_safe(grid, row, col, val)){
                        (*grid)[row][col] = val;
                        Cell cell = {row, col, val};
                        heap_push(heap, &cell, &sudoku->markup, &sudoku->grid);
                        find_safe_insertion = 1;
                        break;
                    }
                }
            }
        }
        row = row + (col == N-1);
        col = (col + 1)%N;
    }
    return 1;
}