#include <queue>
#include <algorithm>
#include "sudoku.h"
#include "omp.h"
#include "CycleTimer.h"

using namespace std;

extern int solve(Sudoku *sudoku);

int parallel_solve(Sudoku *sudoku_head, int max_branched_sudokus, double *end_times)
{

    queue<Sudoku*> q_sudoku;
    Sudoku *next_sudoku = sudoku_head + 1;
    int branched_sudokus = 1;

    q_sudoku.push(sudoku_head);
    while (branched_sudokus < max_branched_sudokus)
    {
        branched_sudokus--;
        Sudoku *curr_sudoku = q_sudoku.front();
        q_sudoku.pop();
        int safe_value = 0;
        Cell unfilled_cell;
        get_unfilled_cell(curr_sudoku, &unfilled_cell);
        int row = unfilled_cell.row, col = unfilled_cell.col;
        for (int v = 1; v <= N; v++)
        {
            if (is_safe(&curr_sudoku->grid, row, col, v)){
                if (!safe_value) // first time find a safe value
                    q_sudoku.push(curr_sudoku);
                else
                {
                    copy_sudoku(next_sudoku, curr_sudoku);
                    set_value(next_sudoku, row, col, safe_value);
                    q_sudoku.push(next_sudoku);
                    next_sudoku++;
                }
                safe_value = v;
                branched_sudokus++;
            }
            if (branched_sudokus >= max_branched_sudokus)
                break;
        }
        if (safe_value)
        {
            unfilled_cell.val = safe_value; // value filled
            heap_push(&curr_sudoku->heap, &unfilled_cell, &curr_sudoku->markup, &curr_sudoku->grid);
            set_value(curr_sudoku, row, col, safe_value);
        }       
    }

    // // Show result of each branch
    // for (int i = 0; i < max_branched_sudokus; i++)
    // {
    //     printf("Branch %d:\n", i);
    //     // show_grid(&sudoku_head[i].grid);
    //     show_sudoku(sudoku_head + i);
    // }
    

    int global_solved = 0;
    #pragma omp parallel for
    for(int i = 0; i < max_branched_sudokus; i++){
        // int tid = omp_get_thread_num();
        // printf("Thead %d !\n", tid);
        Sudoku *sudoku = sudoku_head + i;
        int local_solved = solve(sudoku);
        end_times[i] = CycleTimer::currentSeconds();
        global_solved += local_solved;
    } 
    return global_solved;
}