#include "sudoku.h"

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
                    heap_push(heap, {row, col, v});
                    (*grid)[row][col] = v;
                    find_safe_insertion = 1;
                    break;
                }
            }
            while (!find_safe_insertion){
                // fail to find safe insertion, backtrack
                // try other value for the top cell
                top_cell = heap_pop(heap);
                row = top_cell.row;
                col = top_cell.col;
                val = top_cell.val;
                (*grid)[row][col] = UNASSIGNED;
                for (val =  top_cell.val + 1; val <= N; val++)
                {
                    if (is_safe(grid, row, col, val)){
                        (*grid)[row][col] = val;
                        heap_push(heap, {row, col, val});
                        find_safe_insertion = 1;
                        break;
                    }
                }
                if (heap->count == 0)
                    return 0;
            }
        }
        row = row + (col == N-1);
        col = (col + 1)%N;
    }
    return 1;
}