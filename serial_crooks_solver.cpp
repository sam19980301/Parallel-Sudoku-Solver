#include "sudoku.h"

// serialized & non-recursive solver
int solve(Sudoku *sudoku){
    Grid *grid = &sudoku->grid;
    Heap *heap = &sudoku->heap;
    Markup *markup = &sudoku->markup;
    Cell top_cell;
    int row = 0, col = 0, val;
    int find_safe_insertion;

    // perform elimination & lone ranger first
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
    // set_value
    while (row != N)
    {
        find_safe_insertion = 0;
        if ((*grid)[row][col] == UNASSIGNED){
            if ((*markup)[row][col]){
                for (int v = 1; v <= N; v++){
                    if ((*markup)[row][col] & (1 << (v-1))){
                        // find safe insertion
                        Cell cell = {row, col, v};
                        heap_push(heap, &cell, &sudoku->markup, &sudoku->grid);
                        set_value(sudoku, row, col, v);
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
                    val = top_cell.val;
                    (*grid)[row][col] = UNASSIGNED;
                    for (val =  top_cell.val + 1; val <= N; val++)
                    {
                        if ((*markup)[row][col] & (1 << (val-1))){
                            Cell cell = {row, col, val};
                            heap_push(heap, &cell, &sudoku->markup, &sudoku->grid);
                            set_value(sudoku, row, col, val);
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
                            find_safe_insertion = 1;
                            break;
                        }
                    }
                }
            }
            else{
                while (!find_safe_insertion){
                    if (heap->count == 0)
                        return 0;
                    // fail to find safe insertion, backtrack
                    // try other value for the top cell
                    heap_pop(heap, &top_cell, &sudoku->markup, &sudoku->grid);
                    row = top_cell.row;
                    col = top_cell.col;
                    val = top_cell.val;
                    (*grid)[row][col] = UNASSIGNED;
                    for (val =  top_cell.val + 1; val <= N; val++)
                    {
                        if ((*markup)[row][col] & (1 << (val-1))){
                            Cell cell = {row, col, val};
                            heap_push(heap, &cell, &sudoku->markup, &sudoku->grid);
                            set_value(sudoku, row, col, val);
                            find_safe_insertion = 1;
                            break;
                        }
                    }
                }
            }
        }
        row = row + (col == N-1);
        col = (col + 1)%N;
    }
    return 1;
}