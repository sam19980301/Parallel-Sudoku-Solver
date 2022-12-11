#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <omp.h>
#include "CycleTimer.h"
#include "sudoku.h"

/* flags for debugging */
// #define GRID_VERBOSE

int main(int argc, char *argv[])
{
    if (argc != 5){
        printf("Invalid arguments command usage: ./sudoku <maximum-num-of-threads> <num-of-branch> <num-of-problems> <filepath-of-problem> \n");
    }
    
    FILE *file_ptr = fopen(argv[4], "r");
    if (NULL == file_ptr) {
        printf("File can't be opened \n");
        exit(1);
    }
    int max_threads = atoi(argv[1]);
    omp_set_num_threads(max_threads);
    int n_branches = atoi(argv[2]);
    int n_problems = atoi(argv[3]);
    Sudoku sudoku_arr[n_branches];
    double start_time, end_time, elapsed_time = -1;
    double branch_end_time[n_branches];
    double total_elapsed_time = 0.0;
    int guesses = -1;
    int total_guesses = 0;
    int max_depth = -1;
    int total_max_depth = 0;
    for (int i = 0; i < n_problems; i++)
    {
        for (int t = 0; t < n_branches; t++)
            sudoku_reset(&sudoku_arr[t]);
        read_single_problem(&sudoku_arr[0], file_ptr);
        for (int t = 1; t < n_branches; t++)
            copy_sudoku(&sudoku_arr[t], &sudoku_arr[0]);

        #ifdef GRID_VERBOSE
        // show_grid(&sudoku.grid);
        show_sudoku(&sudoku_arr[0]);
        #endif

        start_time  = CycleTimer::currentSeconds();
        parallel_solve(sudoku_arr, n_branches, branch_end_time);
        end_time  = CycleTimer::currentSeconds(); // >= branch_end_time[ ]
        
        // // Detailed results for each branch
        // printf("Whole function elapsed time %.4f\n", end_time - start_time);
        // for (int i = 0; i < n_branches; i++)
        // {
        //     int sub_solved = validate_solution(&sudoku_arr[i].grid);
        //     double sub_elapsed_time = branch_end_time[i] - start_time;
        //     printf("Branch %3d Solved %d time %.4f\n", i, sub_solved, sub_elapsed_time);
        //     // show_grid(&sudoku_arr[i].grid);
        // }

        int solved = 0; // fastest_solved_thread_id = -
        for (int t = 0; t < n_branches; t++)
        {
            if (validate_solution(&sudoku_arr[t].grid)){
                solved = 1;
                if (branch_end_time[t] <= end_time)
                {
                    // fastest_solved_thread_id = t;
                    end_time = branch_end_time[t];
                    elapsed_time = end_time - start_time;
                    guesses = sudoku_arr[t].heap.guess;
                    max_depth = sudoku_arr[t].heap.max_depth;
                }
            }
        }        
        if (!solved){
            printf("Problem %d Wrong answer\n", i);
            exit(1);
        }

        #ifdef GRID_VERBOSE
        printf("[Result]\n");
        show_grid(&sudoku_arr[0].grid);
        #endif
        printf("Problem %6d Correct. Elapsed time=%2.8f sec. Guess=%6d times. Max Depth=%6d\n", i, elapsed_time, guesses, max_depth);
        // Skip recording the first sudoku problem (seems that there exists heavy overhead when first running parallel threads sometimes)
        if (i > 0)
        {
            total_elapsed_time += elapsed_time;
            total_guesses += guesses;
            total_max_depth += max_depth;
        }
    }
    double average_elapsed_time = total_elapsed_time / (n_problems-1);
    double average_guesses = (double) total_guesses / (n_problems-1);
    double average_max_depth = (double) total_max_depth / (n_problems-1);
    printf("# Thread Used: %2d Average elapsed time=%2.8f sec. Average guess=%.4f times. Average max depth=%.4f\n", max_threads, average_elapsed_time, average_guesses, average_max_depth);
    return 0;
}

