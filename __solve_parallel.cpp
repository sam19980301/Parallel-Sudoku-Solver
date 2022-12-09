#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "CycleTimer.h"
#include "sudoku.h"
#include "omp.h"
#include <iostream>
#include <vector>
#include <algorithm>
#include "CycleTimer.h"

/* flags for debugging */
// #define GRID_VERBOSE

int main(int argc, char *argv[])
{
    // printf("%d\n", argc);
    if (argc != 4){
        printf("Invalid arguments command usage: ./sudoku <num-of-threads> <num-of-problems> <filepath-of-problem> \n");
    }
    
    FILE *file_ptr = fopen(argv[3], "r");
    if (NULL == file_ptr) {
        printf("File can't be opened \n");
        exit(1);
    }
    int n_threads = atoi(argv[1]);
    int n_problems = atoi(argv[2]);
    Sudoku sudoku_arr[n_threads + N];
    double elapsed_time;
    double total_elaspsed_time = 0.0;
    int guesses, all_thread_guesses;
    int total_guesses = 0, total_all_thread_guesses = 0;
    int max_depth, all_thread_max_depth;
    int total_max_depth = 0, total_all_thread_max_depth = 0;
    for (int i = 0; i < n_problems; i++)
    {
        for (int t = 0; t < n_threads + N; t++)
            sudoku_reset(&sudoku_arr[t]);
        read_single_problem(&sudoku_arr[0], file_ptr);
        for (int t = 1; t < n_threads + N; t++)
            copy_sudoku(&sudoku_arr[t], &sudoku_arr[0]);

        #ifdef GRID_VERBOSE
        show_grid(&sudoku.grid);
        // show_sudoku(&sudoku);
        #endif

        // start_time  = CycleTimer::currentSeconds();
        solve(sudoku_arr, n_threads, &elapsed_time);
        // end_time  = CycleTimer::currentSeconds();
        
        int solved = 0;
        guesses = 0;
        all_thread_guesses = 0;
        max_depth = 0;
        all_thread_max_depth = 0;
        for (int t = 0; t < n_threads + N; t++)
        {
            if (validate_solution(&sudoku_arr[t].grid)){
                guesses = sudoku_arr[t].heap.guess;
                max_depth = sudoku_arr[t].heap.max_depth;
                solved = 1;
            }
            all_thread_guesses += sudoku_arr[t].heap.guess;
            all_thread_max_depth = (all_thread_max_depth > sudoku_arr[t].heap.max_depth) ? all_thread_max_depth : sudoku_arr[t].heap.max_depth;
        }
        if (!solved){
            printf("Problem %d Wrong answer\n", i);
            exit(1);
        }

        #ifdef GRID_VERBOSE
        printf("[Result]\n");
        show_grid(&sudoku.grid);
        #endif       

        printf("Problem %d Correct. Elapsed time=%.4f sec.\n", 
                i, 
                elapsed_time
        );
        // printf("Problem %d Correct. Elapsed time=%.4f sec. Guess %5d / %5d. Max Depth %5d / %5d\n", 
        //         i, 
        //         elapsed_time,
        //         guesses, all_thread_guesses,
        //         max_depth, all_thread_max_depth
        // );
        total_elaspsed_time += elapsed_time;
        total_guesses += guesses;
        total_all_thread_guesses += all_thread_guesses;
        total_max_depth += max_depth;
        total_all_thread_max_depth += all_thread_max_depth;
    }
    printf("Summary. Elapsed time=%.4f sec.\n", total_elaspsed_time);
    // printf("Summary. Elapsed time=%.4f sec. Guess %5d / %5d. Max Depth %5d / %5d\n", 
    //         total_elaspsed_time / n_problems,
    //         total_guesses, total_all_thread_guesses,
    //         total_max_depth, total_all_thread_max_depth
    // );
    return 0;
}

