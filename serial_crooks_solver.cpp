#include "sudoku.h"

// serialized & non-recursive solver with crooks algorithm
int solve(Sudoku *sudoku){
    Grid *grid = &sudoku->grid;
    Heap *heap = &sudoku->heap;
    Markup *markup = &sudoku->markup;
    Cell top_cell;
    int row = 0, col = 0, val;
    int find_safe_insertion;

    // perform elimination & lone ranger first
    crook_pruning(sudoku);
    // set_value
    while (row != N)
    {
        find_safe_insertion = 0;
        if (grid->grid_val[row][col] == UNASSIGNED){
            if (markup->markup_val[row][col]){
                for (int v = 1; v <= N; v++){
                    if (markup_contain(markup, row, col, v)){
                        // find safe insertion, make a guess
                        Cell cell = {row, col, v};
                        heap_push(heap, &cell, &sudoku->markup, &sudoku->grid);
                        set_value(sudoku, row, col, v);
                        crook_pruning(sudoku);
                        find_safe_insertion = 1;
                        break;
                    }
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
                val = top_cell.val;
                // (*grid)[row][col] = UNASSIGNED;
                unset_value(sudoku, row, col);
                for (val =  top_cell.val + 1; val <= N; val++)
                {
                    if (markup_contain(markup, row, col, val)){
                        Cell cell = {row, col, val};
                        heap_push(heap, &cell, &sudoku->markup, &sudoku->grid);
                        set_value(sudoku, row, col, val);
                        crook_pruning(sudoku);
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

