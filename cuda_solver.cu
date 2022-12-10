#include <cuda.h>

#include "sudoku.h"

// Check whether an insertion is legal
__device__ int cuda_is_safe(const Grid *grid, int row, int col, int num){
    // check is filled or not
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

// make a single guess: store the guess info (row, col and val wrapped with a cell), current markup and current grid
__device__ inline void cuda_heap_push(Heap *heap, Cell *cell, Markup *markup, Grid *grid){
    heap->cell_arr[heap->count].cell = *cell;
    memcpy(&heap->cell_arr[heap->count].orig_markup, markup, sizeof(Markup));
    memcpy(&heap->cell_arr[heap->count].orig_grid, grid, sizeof(Grid));
    heap->count++;
}

// undo a single guess: pop the top cell and restore originally saved markup and grid
__device__ inline void cuda_heap_pop(Heap *heap, Cell *cell, Markup *markup, Grid *grid){
    heap->count--;
    *cell = heap->cell_arr[heap->count].cell;
    memcpy(markup, &heap->cell_arr[heap->count].orig_markup, sizeof(Markup));
    memcpy(grid, &heap->cell_arr[heap->count].orig_grid, sizeof(Grid));
}

// serialized & non-recursive solver
__global__ void non_recursive_solve(Sudoku *sudoku, int *can_solve){
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
                if (cuda_is_safe(grid, row, col, v)){
                    // find safe insertion
                    Cell cell = {row, col, v};
                    cuda_heap_push(heap, &cell, &sudoku->markup, &sudoku->grid);
                    (*grid)[row][col] = v;
                    find_safe_insertion = 1;
                    break;
                }
            }
            while (!find_safe_insertion){
                if (heap->count == 0){
                    *can_solve = 0;
                    return;
                }
                // fail to find safe insertion, backtrack
                // try other value for the top cell
                cuda_heap_pop(heap, &top_cell, &sudoku->markup, &sudoku->grid);
                row = top_cell.row;
                col = top_cell.col;
                (*grid)[row][col] = UNASSIGNED;
                for (val =  top_cell.val + 1; val <= N; val++)
                {
                    if (cuda_is_safe(grid, row, col, val)){
                        (*grid)[row][col] = val;
                        Cell cell = {row, col, val};
                        cuda_heap_push(heap, &cell, &sudoku->markup, &sudoku->grid);
                        find_safe_insertion = 1;
                        break;
                    }
                }
            }
        }
        row = row + (col == N-1);
        col = (col + 1)%N;
    }
    *can_solve = 1;
}

int hostFE(Sudoku *sudoku){
    int *cuda_can_solve;
    int *can_solve = new int;
    *can_solve = 0;
    cudaMalloc(&cuda_can_solve, sizeof(int));
    cudaMemset(cuda_can_solve, 0, sizeof(int));

    Sudoku *cuda_sudoku;
    cudaMalloc(&cuda_sudoku, sizeof(Sudoku));
    cudaMemcpy(cuda_sudoku, sudoku, sizeof(Sudoku), cudaMemcpyHostToDevice);

    non_recursive_solve<<<SUB_N, SUB_N>>>(cuda_sudoku, cuda_can_solve);

    cudaMemcpy(sudoku, cuda_sudoku, sizeof(Sudoku), cudaMemcpyDeviceToHost);
    cudaMemcpy(can_solve, cuda_can_solve, sizeof(int), cudaMemcpyDeviceToHost);
    return *can_solve; 
}