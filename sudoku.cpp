#include <stdio.h>
#include "sudoku.h"

void show_grid(const Grid *grid){
    printf("Sudoku's Grid\n");
    for (int i = 0; i < N; i++)
    {
        for (int j = 0; j < N; j++)
        {
            printf("%d\t", (*grid)[i][j]);
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
            heap->cell_arr[i].row,
            heap->cell_arr[i].col,
            heap->cell_arr[i].val
        );
    }
    printf("\n");
    printf("\n");
}

void show_sudoku(const Sudoku *sudoku){
    const Grid *grid = &sudoku->grid;
    const Heap *heap = &sudoku->heap;
    show_grid(grid);
    show_heap(heap);
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
    for (int row = 0; row < N; row++){
        for (int col = 0; col < N; col++){
            (*grid)[row][col] = UNASSIGNED;
        }
    }
    heap->count = 0;
}