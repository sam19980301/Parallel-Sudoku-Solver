#ifndef __SUDOKU_H__
#define __SUDOKU_H__

#include <stdio.h>

using namespace std;

constexpr int SUB_N = 3;
constexpr int N = SUB_N * SUB_N;

constexpr int UNASSIGNED = 0;

using Grid = int(*)[N];
using Location = struct {
    int row;
    int col;
    int val;
};

using Heap = struct
{
    Location *heap_arr;
    int count;
};

using Sudoku = struct {
    Grid grid;
    Heap heap;
};

// TBD use as a structures
inline void heap_push(Heap *heap, Location loc){
    heap->heap_arr[heap->count++] = loc;
}
inline Location heap_pop(Heap *heap){
    return heap->heap_arr[--heap->count];
}
inline Location* heap_top(Heap *heap){
    // peek top element
    return &(heap->heap_arr[heap->count-1]);
}

inline void sudoku_insert(Sudoku *sudoku, Location loc){
    heap_push(&sudoku->heap, loc);
    sudoku->grid[loc.row][loc.col] = loc.val;
}

// General, defined in sudoku.cpp
void print_sudoku(Grid grid);
void print_heap(Heap *heap);
int is_legal(Grid grid, int row, int col, int num);
int validate_solution(Grid grid);

// defined for each solver
void sudoku_init(Sudoku *sudoku);
void sudoku_reset(Sudoku *sudoku);
void sudoku_free(Sudoku *sudoku);
int solve(Sudoku *sudoku);

#endif //__SUDOKU_H__