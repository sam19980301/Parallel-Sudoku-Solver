#include <queue>
#include <algorithm>
#include "sudoku.h"
#include "omp.h"
#include "CycleTimer.h"

using namespace std;

int get_unfilled_cell(Grid *grid, Cell *cell);
int count_safe_insertions(Grid *grid, int row, int col);
int serial_crooks_solver(Sudoku *sudoku);

// TBD add early stop --> before add early stop, check the sum of # guess is same
int solve(Sudoku *sudoku_head, int num_thread, double *elaspsed_time)
{
    double start_time, end_time;

    start_time  = CycleTimer::currentSeconds();

    queue<Sudoku*> q_sudoku;
    Sudoku *curr_sudoku, *next_sudoku = sudoku_head + 1;
    Markup *curr_markup;
    Cell unfilled_cell;
    int first_safe_value;
    int branched_sudokus = 0;

    q_sudoku.push(sudoku_head);
    while (1)
    {
        first_safe_value = 0;
        curr_sudoku = q_sudoku.front();
        crook_pruning(curr_sudoku);
        get_unfilled_cell(&curr_sudoku->grid, &unfilled_cell);
        for (int v = 1; v <= N; v++)
        {
            curr_markup = &curr_sudoku->markup;
            if ((*curr_markup)[unfilled_cell.row][unfilled_cell.col] & (1 << (v-1))){
                if (!first_safe_value)
                {
                    first_safe_value = v;
                    q_sudoku.push(curr_sudoku);
                    branched_sudokus++;
                }
                else
                {
                    copy_sudoku(next_sudoku, curr_sudoku);
                    set_value(next_sudoku, unfilled_cell.row, unfilled_cell.col, v);
                    show_grid(&next_sudoku->grid);
                    q_sudoku.push(next_sudoku);
                    next_sudoku++;
                    branched_sudokus++;
                }
            }
        }
        if (first_safe_value)
        {
            set_value(curr_sudoku, unfilled_cell.row, unfilled_cell.col, first_safe_value);
            show_grid(&curr_sudoku->grid);

        }
        if (branched_sudokus >= num_thread)
            break;
        branched_sudokus--;
    }

    int global_solved = 0;
    #pragma omp parallel for num_threads (num_thread)
    for(int i = 0; i < branched_sudokus; i++){
        Sudoku *sudoku = &sudoku_head[i];
        int local_solved = serial_crooks_solver(sudoku);
        // printf("Branch %d Solved %d Time %.4f\n", i, local_solved, CycleTimer::currentSeconds()-start_time);
        if (local_solved){
            end_time = CycleTimer::currentSeconds();
            *elaspsed_time = end_time - start_time;
            // printf("Elaspsed_time %.4f sec\n", *elaspsed_time);
            // exit(0);
        }
        global_solved += local_solved;
    } 
    return global_solved;
}

int get_unfilled_cell(Grid *grid, Cell *cell){ // get row and col of an unfilled cell
    int row = 0, col = 0;
    while (row != N)
    {
        if ((*grid)[row][col] == UNASSIGNED){
            cell->row = row;
            cell->col = col;
            return 1;
        }
        row = row + (col == N-1);
        col = (col + 1)%N;
    }
    return 0;
}

int count_safe_insertions(Grid *grid, int row, int col){
    int safe_insertions = 0;
    for (int v = 1; v <= N; v++){
        if (is_safe(grid, row, col, v)){
            safe_insertions++;
        }
    }
    return safe_insertions;
}

// serialized & recursive solver
int serial_crooks_solver(Sudoku *sudoku){
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
            if ((*markup)[row][col]){
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
                // (*grid)[row][col] = UNASSIGNED;
                unset_value(sudoku, row, col);
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