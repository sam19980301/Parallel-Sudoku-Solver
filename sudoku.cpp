#include <stdio.h>
#include "sudoku.h"

void show_grid(const Grid *grid){
    printf("Sudoku's Grid\n");
    for (int i = 0; i < N; i++)
    {
        for (int j = 0; j < N; j++)
        {
            printf("%d\t", grid->grid_val[i][j]);
        }
        printf("\n");
    }
    printf("\n");
    printf("Unknown Count:%d\n",grid->unknown);
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
            printf("Row%d\tCol%d\tMarkup in hex %x\t Count%d\n", i, j, markup->markup_val[i][j], markup->count[i][j]);
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
    const GridVal *gridval = &(grid->grid_val);
    // check is filled or not
    if ((*gridval)[row][col] != UNASSIGNED)
        return 0;
    // check row
    // printf("Check Row\n");
    for (int i = 0; i < N; i++)
        if ((*gridval)[row][i] == num)
            return 0;
    // check col
    // printf("Check Column\n");
    for (int i = 0; i < N; i++)
        if ((*gridval)[i][col] == num)
            return 0;
    // check sub-grid
    // printf("Check Sub-Grid\n");
    int start_row = (row / SUB_N) * SUB_N;
    int start_col = (col / SUB_N) * SUB_N;
    for (int i = 0; i < SUB_N; i++)
        for (int j = 0; j < SUB_N; j++)
            if ((*gridval)[start_row + i][start_col + j] == num)
                return 0;
    return 1;
}

// Validate correctness of solution
int validate_solution(const Grid *grid){
    const GridVal *gridval = &(grid->grid_val);
    for (int row = 0; row < N; row++){
        for (int col = 0; col < N; col++){
            // UNFINISHED
            if ((*gridval)[row][col] == UNASSIGNED)
                return 0;
            // Check row
            for (int i = 0; i < N; i++)
                if (((*gridval)[row][col] == (*gridval)[row][i]) && (col != i))
                    return 0;
            // Check column
            for (int i = 0; i < N; i++)
                if (((*gridval)[row][col] == (*gridval)[i][col]) && (row != i))
                    return 0;
            // Check sub-grid
            int start_row = (row / SUB_N) * SUB_N;
            int start_col = (col / SUB_N) * SUB_N;
            for (int i = 0; i < SUB_N; i++)
                for (int j = 0; j < SUB_N; j++)
                    if (((*gridval)[row][col] == (*gridval)[start_row + i][start_col + j]) && (row%SUB_N != i) && (col%SUB_N != j))
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
            grid->grid_val[row][col] = UNASSIGNED;
            markup->markup_val[row][col] = 0;
            markup->count[row][col] = N;
            for (int v = 0; v < N; v++)
            {
                markup->markup_val[row][col] |= (1 << v);
            }
        }
    }
    grid->unknown = N * N;
    heap->count = 0;
    heap->guess = 0;
    heap->depth = 0;
    heap->max_depth = 0;
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
                if ((markup->markup_val[i][j] >> k) & 1){
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
                checked = 1; // return 1;
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
                checked = 1; // return 1;
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
                    checked = 1; // return 1;
                }
            }
        }
    }
    return checked; // 0
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

void read_single_problem(Sudoku *sudoku, FILE *file_ptr){
    char ch;
    if (SUB_N == 3) // different input format according to SUB_N
    {
        for (int i = 0; i < N; i++)
        {
            for (int j = 0; j < N; j++)
            {
                ch = fgetc(file_ptr);
                while ((ch != '.') && (!((ch >= '1') && (ch <= '9'))))
                    ch = fgetc(file_ptr);
                if (ch == '.')
                    sudoku->grid.grid_val[i][j] = UNASSIGNED;
                else
                    set_value(sudoku, i, j, ch-'0');
            }
        }
    }
    else if (SUB_N == 4)
    {
        for (int i = 0; i < N; i++)
        {
            for (int j = 0; j < N; j++)
            {
                ch = fgetc(file_ptr);
                while ((ch != '.') && (!((ch >= '1') && (ch <= '9'))) && (!((ch >= 'A') && (ch <= 'P'))))
                    ch = fgetc(file_ptr);
                if (ch == '.')
                    sudoku->grid.grid_val[i][j] = UNASSIGNED;
                else if ((ch >= '1') && (ch <= '9'))
                    set_value(sudoku, i, j, ch-'0');
                else
                    set_value(sudoku, i, j, ch-'A'+1);
            }
        }
    }
}