#include "sudoku.h"

void crook_pruning(Sudoku *sudoku);

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
        if ((*grid)[row][col] == UNASSIGNED){
            if ((*markup)[row][col]) {
                for (int v = 1; v <= N; v++){
                    if ((*markup)[row][col] & (1 << (v-1))){
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
                (*grid)[row][col] = UNASSIGNED;
                for (val =  top_cell.val + 1; val <= N; val++)
                {
                    if ((*markup)[row][col] & (1 << (val-1))){
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

int elimination(Sudoku *sudoku){
    Grid *grid = &sudoku->grid;
    Markup *markup = &sudoku->markup;
    int changed = 0;
    // #pragma omp parallel for
    for (int i = 0; i < N; i++)
    {
        // #pragma omp parallel for
        for (int j = 0; j < N; j++)
        {
            int cnt = 0, ind = 0;
            for (int k = 0; k < N; k++)
            {
                if (((*markup)[i][j] >> k) & 1){
                    cnt++;
                    ind = k;
                }
            }
            if (cnt == 1){
                set_value(sudoku, i, j, ind + 1);
                changed = 1;
            }
        }
    }
    return changed;
}

int lone_ranger(Sudoku *sudoku){
    Grid *grid = &sudoku->grid;
    Markup *markup = &sudoku->markup;
    int checked = 0;

    // check row
    // #pragma omp parallel for
    for (int i = 0; i < N; i++)
    {
        // #pragma omp parallel for
        for (int k = 1; k <= N; k++)
        {
            int cnt = 0, col = -1;
            for (int j = 0; j < N; j++)
            {
                if (markup_contain(markup, i, j, k)){
                    cnt++;
                    col = j;
                }
            }
            if (cnt == 1){
                set_value(sudoku, i, col, k);
                checked = 1; // return 1;
            }
        }
    }
    // check column
    // #pragma omp parallel for
    for (int j = 0; j < N; j++)
    {
        // #pragma omp parallel for
        for (int k = 0; k < N; k++)
        {
            int cnt = 0, row = -1;
            for (int i = 0; i < N; i++)
            {
                if (markup_contain(markup, i, j, k)){
                    cnt++;
                    row = i;
                }
            }
            if (cnt == 1){
                set_value(sudoku, row, j, k);
                checked = 1; // return 1;
            }
        }
    }

    // check sub-grid
    // #pragma omp parallel for
    for (int i = 0; i < N; i+=SUB_N)
    {
        // #pragma omp parallel for
        for (int j = 0; j < N; j+=SUB_N)
        {
            // #pragma omp parallel for
            for (int k = 1; k <= N; k++)
            {
                int cnt = 0, row = -1, col = -1;
                for (int ii = 0; ii < SUB_N; ii++)
                {
                    for (int jj = 0; jj < SUB_N; jj++){
                        if (markup_contain(markup ,i+ii, j+jj, k)){
                            cnt++;
                            row = i + ii;
                            col = j + jj;
                        }
                    }
                }
                if (cnt == 1){
                    set_value(sudoku, row, col, k);
                    checked = 1; // return 1;
                }
            }
        }
    }
    return checked; // 0
}