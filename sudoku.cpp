#include <stdio.h>
#include "sudoku.h"

void show_grid(const Grid *grid){
    printf("Sudoku's Grid\n");
    for (int i = 0; i < N; i++)
    {
        for (int j = 0; j < N; j++)
        {
            printf("%d  ", (*grid)[i][j]);
        }
        printf("\n");
    }
    printf("\n");
}

void show_heap(const Heap *heap){
    printf("Sudoku's Heap contains %d elements\n", heap->count);
    for (int i = 0; i < heap->count; i++)
    {
        printf(
            "Heap[%d]\tRow%d\tCol%d\tVal%d\n",
            i,
            heap->cell_arr[i].cell.row,
            heap->cell_arr[i].cell.col,
            heap->cell_arr[i].cell.val
        );
    }
    printf("\n");
    printf("\n");
}

void show_markup(const Markup *markup){
    printf("Sudoku's Markup\n");
    for (int i = 0; i < N; i++)
    {
        for (int j = 0; j < N; j++)
        {
            printf("Row%d\tCol%d\tMarkup in oct %o\n", i, j, (*markup)[i][j]);
        }
        
    }
    
}

void show_sudoku(const Sudoku *sudoku){
    const Grid *grid = &sudoku->grid;
    const Heap *heap = &sudoku->heap;
    const Markup *markup = &sudoku->markup;
    show_grid(grid);
    show_heap(heap);
    show_markup(markup);
}

// Check whether an insertion is legal
int is_safe(const Grid *grid, int row, int col, int num){
    // check is filled or not
    // printf("Check Filled\n");
    if ((*grid)[row][col] != UNASSIGNED)
        return 0;
    
    // check row
    // printf("Check Row\n");
    for (int i = 0; i < N; i++)
        if ((*grid)[row][i] == num)
            return 0;

    // check col
    // printf("Check Column\n");
    for (int i = 0; i < N; i++)
        if ((*grid)[i][col] == num)
            return 0;
    
    // check sub-grid
    // printf("Check Sub-Grid\n");
    int start_row = (row / SUB_N) * SUB_N;
    int start_col = (col / SUB_N) * SUB_N;
    for (int i = 0; i < SUB_N; i++)
        for (int j = 0; j < SUB_N; j++){
            if ((*grid)[start_row + i][start_col + j] == num){
                return 0;
            }
        }
    return 1;
}

// Validate correctness of solution
int validate_solution(const Grid *grid){
    for (int row = 0; row < N; row++){
        for (int col = 0; col < N; col++){
            // UNFINISHED
            if ((*grid)[row][col] == UNASSIGNED)
                return 0;
            // Check row
            for (int i = 0; i < N; i++)
                if (((*grid)[row][col] == (*grid)[row][i]) && (col != i))
                    return 0;
            // Check column
            for (int i = 0; i < N; i++)
                if (((*grid)[row][col] == (*grid)[i][col]) && (row != i))
                    return 0;
            // Check sub-grid
            int start_row = (row / SUB_N) * SUB_N;
            int start_col = (col / SUB_N) * SUB_N;
            for (int i = 0; i < SUB_N; i++)
                for (int j = 0; j < SUB_N; j++)
                    if (((*grid)[row][col] == (*grid)[start_row + i][start_col + j]) && (row%SUB_N != i) && (col%SUB_N != j))
                        return 0;
        }
    }
    return 1;
}

void sudoku_reset(Sudoku *sudoku){
    Grid *grid = &sudoku->grid;
    Heap *heap = &sudoku->heap;
    Markup *markup = &sudoku->markup;

    for (int row = 0; row < N; row++){
        for (int col = 0; col < N; col++){
            (*grid)[row][col] = UNASSIGNED;
            (*markup)[row][col] = 0;
            for (int v = 0; v < N; v++)
            {
                (*markup)[row][col] |= (1 << v);
            }
        }
    }
    heap->count = 0;
}

void set_value(Sudoku *sudoku, int row, int col, int val){
    int start_row = (row / SUB_N) * SUB_N;
    int start_col = (col / SUB_N) * SUB_N;
    sudoku->grid[row][col] = val;
    sudoku->markup[row][col] = 0;
    for (int i = 0; i < N; i++)
    {
        remove_from_markup(&sudoku->markup, row, i, val);
        remove_from_markup(&sudoku->markup, i, col, val);
        remove_from_markup(&sudoku->markup, start_row + i / SUB_N, start_col + i % SUB_N, val);
    }
}

int elimination(Sudoku *sudoku){
    Grid *grid = &sudoku->grid;
    Markup *markup = &sudoku->markup;
    int changed = 0;
    for (int i = 0; i < N; i++)
    {
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
    // return (getTotalUnfilledCellsNum() == 0);
    return changed;
}

int lone_ranger(Sudoku *sudoku){
    Grid *grid = &sudoku->grid;
    Markup *markup = &sudoku->markup;
    // check row
    for (int i = 0; i < N; i++)
    {
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
                return 1;
            }
        }
    }
    // check column
    for (int j = 0; j < N; j++)
    {
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
                return 1;
            }
        }
    }

    // check sub-grid
    for (int i = 0; i < N; i+=SUB_N)
    {
        for (int j = 0; j < N; j+=SUB_N)
        {
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
                    return 1;
                }
            }
        }
    }
    return 0;    
}
