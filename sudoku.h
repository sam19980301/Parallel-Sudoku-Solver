#ifndef __SUDOKU_H__
#define __SUDOKU_H__

#define SUB_N 3
#define N ((SUB_N) * (SUB_N))
#define UNASSIGNED 0

typedef int Grid[N][N];

typedef struct
{
    int row;
    int col;
    int val;
} Cell;

typedef struct
{
    Cell cell_arr[N * N];
    int count;
} Heap; // a heap containing the inserted cells for a given sudoku puzzle

typedef struct
{
    Grid grid;
    Heap heap;
} Sudoku;

inline void heap_push(Heap *heap, Cell cell){ heap->cell_arr[heap->count++] = cell; }
inline Cell heap_pop(Heap *heap){ return heap->cell_arr[--heap->count]; }

// General, defined in sudoku.cpp
void show_grid(const Grid *grid);
void show_heap(const Heap *heap);
void show_sudoku(const Sudoku *sudoku);
int is_safe(const Grid *grid, int row, int col, int num);
int validate_solution(const Grid *grid);
void sudoku_reset(Sudoku *sudoku);

// defined for each solver
extern int solve(Sudoku *sudoku);

#endif //__SUDOKU_H__