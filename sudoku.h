#ifndef __SUDOKU_H__
#define __SUDOKU_H__

#include <string.h>
#include <stdio.h>

#define SUB_N 3
#define N ((SUB_N) * (SUB_N))
#define UNASSIGNED 0

typedef int GridVal[N][N];
typedef struct
{
    GridVal grid_val;
    int unknown;
} Grid;
typedef int MarkupVal[N][N];
typedef struct
{
    MarkupVal markup_val;
    MarkupVal count;
} Markup;
// the n th bit of Markup[i][j]: possible / not possible to fill in n-1 into i th row, j th column 
typedef struct
{
    int row;
    int col;
    int val;
} Cell;

typedef struct
{
    Cell cell;
    Grid orig_grid;
    Markup orig_markup;
} Screenshot;

typedef struct
{
    Screenshot cell_arr[N * N];
    int count;
    int guess;
    int depth;
    int max_depth;
} Heap; // a heap containing all the gueeses for a given sudoku puzzle

typedef struct
{
    Grid grid;
    Heap heap;
    Markup markup;
} Sudoku;

// grid function
// check whether an insertion is safe or not without using markup
int is_safe(const Grid *grid, int row, int col, int num);

// markup function
inline int markup_contain(Markup *markup, int row, int col, int val){
    return (markup->markup_val[row][col] >> (val-1)) & 1;
}
inline void remove_from_markup(Markup *markup, int row, int col, int val){
    if (markup->markup_val[row][col] & (1 << (val-1)))
        markup->count[row][col]--;
    markup->markup_val[row][col] &= ~(1 << (val-1));
} // or return whether or not the original bit is set
// inline int markup_count_values(Markup *markup, int row, int col){
//     int cnt = 0;
//     for (int i = 0; i < N; i++)
//         if ((markup->markup_val[row][col] >> i) & 1)
//             cnt++;
//     return cnt;
// }

// heap function
// copy heap
inline void copy_heap(Heap *dest, Heap* src) {
    memcpy(src, dest, sizeof(Heap));
}
// make a single guess: store the guess info (row, col and val wrapped with a cell), current markup and current grid
inline void heap_push(Heap *heap, Cell *cell, Markup *markup, Grid *grid){
    heap->cell_arr[heap->count].cell = *cell;
    memcpy(&heap->cell_arr[heap->count].orig_markup, markup, sizeof(Markup));
    memcpy(&heap->cell_arr[heap->count].orig_grid, grid, sizeof(Grid));
    heap->count++;
    heap->guess++;
    heap->depth++;
    heap->max_depth = (heap->max_depth > heap->depth) ? heap->max_depth : heap->depth;
    // printf("%d %d %d\n", cell->row, cell->col, cell->val);
}
// undo a single guess: pop the top cell and restore originally saved markup and grid
inline void heap_pop(Heap *heap, Cell *cell, Markup *markup, Grid *grid){
    heap->count--;
    heap->depth--;
    *cell = heap->cell_arr[heap->count].cell;
    memcpy(markup, &heap->cell_arr[heap->count].orig_markup, sizeof(Markup));
    memcpy(grid, &heap->cell_arr[heap->count].orig_grid, sizeof(Grid));
}

// sudoku function
void sudoku_reset(Sudoku *sudoku);
inline void set_value(Sudoku *sudoku, int row, int col, int val){
    if (sudoku->grid.grid_val[row][col] == UNASSIGNED)
        sudoku->grid.unknown--;
    int start_row = (row / SUB_N) * SUB_N;
    int start_col = (col / SUB_N) * SUB_N;
    sudoku->grid.grid_val[row][col] = val;
    sudoku->markup.markup_val[row][col] = 0;
    sudoku->markup.count[row][col] = 0;
    for (int i = 0; i < N; i++)
    {
        remove_from_markup(&sudoku->markup, row, i, val);
        remove_from_markup(&sudoku->markup, i, col, val);
        remove_from_markup(&sudoku->markup, start_row + i / SUB_N, start_col + i % SUB_N, val);
    }
}
inline void unset_value(Sudoku *sudoku, int row, int col){
    if (sudoku->grid.grid_val[row][col] != UNASSIGNED)
        sudoku->grid.unknown++;
    sudoku->grid.grid_val[row][col] = UNASSIGNED;
}
// original version: copy entire sudoku excluding its heap, in case of backtracking
// copy entire sudoku "including" its heap
inline void copy_sudoku(Sudoku *dest, Sudoku *src) {
    if (dest == src)
        return;
    memcpy(dest, src, sizeof(Sudoku));
}

// crook's algorithm (seach space pruning): elimination, lone ranger, twins and triplets
int elimination(Sudoku *sudoku);
int lone_ranger(Sudoku *sudoku);
// int twins(Sudoku *twins);    // supported in future
// int triplets(Sudoku *twins); // supported in future
void crook_pruning(Sudoku *sudoku);

// cross hatching algorithm
inline int get_minimum_branch_cell(Markup *markup, Cell *min_branch_cell)
{
    int min_count = N + 1, curr_count;
    for (int i = 0; i < N; i++)
    {
        for (int j = 0; j < N; j++)
        {
            // curr_count = markup_count_values(markup, i, j);
            curr_count = markup->count[i][j];
            if (curr_count)
            {
                if (curr_count < min_count)
                {
                    min_count = curr_count;
                    min_branch_cell->row = i;
                    min_branch_cell->col = j;
                }
                // if (min_count == 2)
                //     break;
            }
            // printf("%d %d %d %d\n", i, j, curr_count, min_count);
        }
        // if (min_count == 2)
        //     break;
    }
    return (min_count != N+1);
}

// parallel utils
inline int get_unfilled_cell(Sudoku *sudoku, Cell *cell){ // get row and col of an unfilled cell
    int row = 0, col = 0;
    while (row != N)
    {
        if (sudoku->markup.count[row][col]){ // grid->grid_val[row][col] == UNASSIGNED
            cell->row = row;
            cell->col = col;
            return 1;
        }
        row = row + (col == N-1);
        col = (col + 1)%N;
    }
    return 0;
}

// debug helper function
void show_grid(const Grid *grid);
void show_heap(const Heap *heap);
void show_markup(const Markup *markup);
void show_sudoku(const Sudoku *sudoku);
int validate_solution(const Grid *grid);

// reading problem
void read_single_problem(Sudoku *sudoku, FILE *file_ptr);

// defined for each solver
// serial_solver
extern int solve(Sudoku *sudoku);
// parallel solver that wrap specific serial solver
extern int parallel_solve(Sudoku *sudoku_head, int max_branched_sudokus, double *end_times);

#endif //__SUDOKU_H__