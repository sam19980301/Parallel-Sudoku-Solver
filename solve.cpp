#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>

#include "CycleTimer.h"
#include "sudoku.h"

#define PBSTR "||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||"
#define PBWIDTH 60

/* flags for debugging */
#define GRID_VERBOSE 1


void read_single_problem(Sudoku *sudoku, FILE *file_ptr){
    int unknown = 0, total = 0;
    if(SUB_N>3) 
    {
        std::cout << "parsing input..." << std::endl;
        size_t len = N*N*3;
        char *problem = NULL;
        getline(&problem, &len, file_ptr);
        char *delim = " ,\n";  // symbols to seperate tokens from a string
        char *token = strtok(problem, delim);

        for (int i = 0; i < N; i++)
        {
            for (int j = 0; j < N; j++)
            {   
                if(strcmp(token, "-1")==0 || strcmp(token, "0")==0) {
                    sudoku->grid[i][j] = UNASSIGNED;
                    unknown++;
                }
                else {
                    set_value(sudoku, i, j, atoi(token));
                }
                token = strtok(NULL, delim);  // iterate to next token
                total++;
            }
        }
    } 
    else if(SUB_N==3) 
    {
        char ch;    
        for (int i = 0; i < N; i++)
        {
            for (int j = 0; j < N; j++)
            {
                ch = fgetc(file_ptr);
                while ((ch != '.') && ((ch < '1') || (ch > '9')))
                    ch = fgetc(file_ptr);
                    total++;
                if (ch == '.') {
                    sudoku->grid[i][j] = UNASSIGNED;
                    unknown++;
                }
                else {
                    set_value(sudoku, i, j, ch-'0');
                }
            }
        }
    }
    printf("%d tokens read from file with %d unknowns.\n", total, unknown);
}

int main(int argc, char *argv[])
{
    // printf("%d\n", argc);0

    if (argc != 3){
        printf("Invalid arguments command usage: ./sudoku <num-of-problems> <filepath-of-problem>\n");
    }
    
    // Current available puzzle datasets: puzzles5_forum_hardest_1905_11+ puzzles2_17_clue
    FILE *file_ptr = fopen(argv[2], "r");
    if (NULL == file_ptr) {
        printf("File can't be opened \n");
        exit(1);
    }

    Sudoku sudoku;
    int n_problems = atoi(argv[1]);
    double start_time, end_time, elapsed_time;
    double total_elaspsed_time = 0.0;

    for (int i = 0; i < n_problems; i++)
    {
        sudoku_reset(&sudoku);
        // set_single_problem(&sudoku);  // read by data-generator
        read_single_problem(&sudoku, file_ptr);

        #ifdef GRID_VERBOSE
        show_grid(&sudoku.grid);
        // show_sudoku(&sudoku);
        #endif
        
        start_time  = CycleTimer::currentSeconds();
        solve(&sudoku);
        // print_progress(double(i)/double(n_problems));
        end_time  = CycleTimer::currentSeconds();

        if (!validate_solution(&sudoku.grid)){
            printf("Wrong answer\n");
            // show_sudoku(&sudoku);
            exit(1);
        }

        #ifdef GRID_VERBOSE
        printf("[Result]\n");
        show_grid(&sudoku.grid);
        #endif
        
        elapsed_time = end_time - start_time;
        printf("Correct. Elapsed time=%.4f sec\n", elapsed_time);
        total_elaspsed_time += elapsed_time;
    }
    printf("Total elapsed time=%.4f sec\n", total_elaspsed_time);    
    return 0;
}

