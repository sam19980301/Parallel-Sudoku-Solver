#include <stdlib.h>
#include "sudoku.h"

// void sudoku_free(Sudoku *sudoku);
// int solve(Sudoku *sudoku);

void sudoku_init(Sudoku *sudoku){
    sudoku->grid = (Grid) malloc(N * N * sizeof(int));
    sudoku->heap.heap_arr = (Location *) malloc(N * N * sizeof(Location));
    sudoku->heap.count = 0;
}

void sudoku_reset(Sudoku *sudoku){
    for (int i = 0; i < N; i++)
        for (int j = 0; j < N; j++){
            sudoku->grid[i][j] = UNASSIGNED;
            sudoku->heap.count = 0;
        } 
}

void sudoku_free(Sudoku *sudoku){
    free(sudoku->grid);
    free(sudoku->heap.heap_arr);
}

// serialized & recursive solver
int solve(Sudoku *sudoku){
    Grid grid = sudoku->grid;
    Heap *heap = &sudoku->heap;
    Location top;
    int row = 0, col = 0, val;
    int find_safe_insertion;
    while (row != N)
    {
        find_safe_insertion = 0;
        if (grid[row][col] == UNASSIGNED){
            for (int v = 1; v <= N; v++){
                if (is_legal(grid, row, col, v)){
                    // find safe insertion
                    heap_push(heap, {row, col, v});
                    grid[row][col] = v;
                    find_safe_insertion = 1;
                    break;
                }
            }
            // if (!find_safe_insertion){
            //     // fail to find safe insertion --> backtrack
            //     while (1)
            //     {
            //         // try other value for top element in heap
            //         top = heap_pop(heap);
            //         row = top.row;
            //         col = top.col;
            //         val = top.val;
            //         grid[row][col] = UNASSIGNED;
            //         for (val =  top.val + 1; val <= N; val++)
            //         {
            //             if (is_legal(grid, row, col, val)){
            //                 grid[row][col] = val;
            //                 heap_push(heap, {row, col, val});
            //                 find_safe_insertion = 1;
            //                 break;
            //             }
            //         }
            //         if (find_safe_insertion)
            //             break;
            //         else if (heap->count == 0)
            //             return 0;
            //     }
            // }
            while (!find_safe_insertion){
                // fail to find safe insertion, backtrack
                // try other value for top element in heap
                top = heap_pop(heap);
                row = top.row;
                col = top.col;
                val = top.val;
                grid[row][col] = UNASSIGNED;
                for (val =  top.val + 1; val <= N; val++)
                {
                    if (is_legal(grid, row, col, val)){
                        grid[row][col] = val;
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