#include <stdio.h>
#include "sudoku.h"

void print_sudoku(Grid grid){
    for (int i = 0; i < N; i++){
        for (int j = 0; j < N; j++)
            printf("%d\t", grid[i][j]);
        printf("\n");
    }
    printf("\n");
}

void print_heap(Heap *heap){
    for (int i = 0; i < heap->count; i++){
        printf("Count %d\tRow %d\tCol %d\tVal %d\n",
            heap->count,
            heap->heap_arr[i].row,
            heap->heap_arr[i].col,
            heap->heap_arr[i].val
        );
    }
    printf("\n");
}

// Check whether the operation is legal
int is_legal(Grid grid, int row, int col, int num){ // Sudoku *sudoku
    // Check if filled
    if (grid[row][col] != UNASSIGNED)
        return (grid[row][col] == num);

    // Check row
    for (int i = 0; i < N; ++i)
        if (grid[row][i] == num)
            return 0;

    // Check column
    for (int i = 0; i < N; ++i)
        if (grid[i][col] == num)
            return 0;

    // Check Subgrid
    int start_row = row / SUB_N, start_col = col / SUB_N;
    for (int i = 0; i < SUB_N; ++i)
        for (int j = 0; j < SUB_N; ++j)
            if (grid[start_row*SUB_N+i][start_col*SUB_N+j] == num)
                return 0;

    // check only, should perform insert explicit elsewhere
    // grid[row][col] = num;
    return 1;
}

// Validate correctness of solution
int validate_solution(Grid grid){
    for (int row = 0; row < N; ++row){
        for (int col = 0; col < N; ++col){
            // printf("%d %d\n",row, col);
            // UNFINISHED
            if (grid[row][col] == UNASSIGNED)
                return 0;
            // Check row
            for (int i = 0; i < N; ++i)
                if ((grid[row][col] == grid[row][i]) && (col != i))
                    return 0;
            // Check column
            for (int i = 0; i < N; ++i)
                if ((grid[row][col] == grid[i][col]) && (row != i))
                    return 0;
            // Check Subgrid
            int start_row = row / SUB_N, start_col = col / SUB_N;
            for (int i = 0; i < SUB_N; ++i)
                for (int j = 0; j < SUB_N; ++j)
                    if ((grid[row][col] == grid[start_row*SUB_N+i][start_col*SUB_N+j]) && (row%SUB_N != i) && (col%SUB_N != j))
                        return 0;
        }
    }
    return 1;
}