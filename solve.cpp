#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "CycleTimer.h"
#include "sudoku.h"

/* flags for debugging */
// #define GRID_VERBOSE

int main(int argc, char *argv[])
{
    // printf("%d\n", argc);
    if (argc != 3){
        printf("Invalid arguments command usage: ./sudoku <num-of-problems> <filepath-of-problem> \n");
    }
    
    FILE *file_ptr = fopen(argv[2], "r");
    if (NULL == file_ptr) {
        printf("File can't be opened \n");
        exit(1);
    }
    Sudoku sudoku;
    int n_problems = atoi(argv[1]);
    double start_time, end_time, elapsed_time;
    double total_elaspsed_time = 0.0;
    int guesses;
    int total_guesses = 0;
    int max_depth;
    int total_max_depth = 0;
    for (int i = 0; i < n_problems; i++)
    {
        sudoku_reset(&sudoku);
        read_single_problem(&sudoku, file_ptr);
        // printf("%d\n", sudoku.grid.unknown);

        #ifdef GRID_VERBOSE
        show_grid(&sudoku.grid);
        // show_sudoku(&sudoku);
        #endif

        start_time  = CycleTimer::currentSeconds();
        solve(&sudoku);
        end_time  = CycleTimer::currentSeconds();
        
        if (!validate_solution(&sudoku.grid)){
            printf("Wrong answer\n");
            show_sudoku(&sudoku);
            exit(1);
        }

        #ifdef GRID_VERBOSE
        printf("[Result]\n");
        show_grid(&sudoku.grid);
        #endif

        guesses = sudoku.heap.guess;
        max_depth = sudoku.heap.max_depth;
        elapsed_time = end_time - start_time;
        printf("Correct. Elapsed time=%.4f sec. Guess %5d times. Max Depth %d\n", elapsed_time, guesses, max_depth);
        total_guesses += guesses;
        total_max_depth += max_depth;
        total_elaspsed_time += elapsed_time;
    }
    printf("Total elapsed time=%.4f sec. Total guess %d times. Max Depth %d\n", total_elaspsed_time, total_guesses, total_max_depth);
    return 0;
}

