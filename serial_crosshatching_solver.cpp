#include "sudoku.h"

// serialized & non-recursive solver with crooks algorithm
int solve(Sudoku *sudoku){
    Grid *grid = &sudoku->grid;
    Heap *heap = &sudoku->heap;
    Markup *markup = &sudoku->markup;
    Cell min_branch_cell = {0 , 0, 0}, top_cell;
    int row = 0, col = 0, val;
    int find_safe_insertion;

    // // perform elimination & lone ranger first
    // show_grid(grid);
    // show_heap(heap);

    crook_pruning(sudoku);
    // set_value
    while (grid->unknown)
    {
        // printf("Unknown: %d\n",grid->unknown);
        // show_grid(grid);
        // show_heap(heap);

        // get cell with minimum branch
        find_safe_insertion = get_minimum_branch_cell(markup, &min_branch_cell);
        row = min_branch_cell.row;
        col = min_branch_cell.col;

        for (int v = 1; v <= N; v++){
            if (markup_contain(markup, row, col, v)){
                // find safe insertion, make a guess
                Cell cell = {row, col, v};
                heap_push(heap, &cell, &sudoku->markup, &sudoku->grid);
                set_value(sudoku, row, col, v);
                crook_pruning(sudoku);
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
            val = top_cell.val;
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
    return 1;
}

