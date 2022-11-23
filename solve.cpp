#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "CycleTimer.h"
#include "sudoku.h"

#define PBSTR "||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||"
#define PBWIDTH 60

void print_progress(double percentage) {
    int val = (int) (percentage * 100);
    int lpad = (int) (percentage * PBWIDTH);
    int rpad = PBWIDTH - lpad;
    printf("\r%3d%% [%.*s%*s]", val, lpad, PBSTR, rpad, "");
    fflush(stdout);
}

void read_single_problem(Sudoku *sudoku, FILE *file_ptr){
    size_t len = N*N*3;
    char *problem = NULL;
    getline(&problem, &len, file_ptr);
    char *delim = " ,.\n";  // symbols to seprate tokens from a string
    char *token = strtok(problem, delim);
    int counter = 0;

    for (int i = 0; i < N; i++)
    {
        for (int j = 0; j < N; j++)
        {   
            if(strcmp(token, "-1")==0 || strcmp(token, "0")==0) {
                sudoku->grid[i][j] = UNASSIGNED;
            }
            else {
                set_value(sudoku, i, j, atoi(token));
            }
            token = strtok(NULL, delim);  // iterate to next token
            counter++;
        }
    }
    printf("%d tokens read from file.\n", counter);
}

// void read_single_problem(Sudoku *sudoku, FILE *file_ptr){
//     char ch;
//     for (int i = 0; i < N; i++)
//     {
//         for (int j = 0; j < N; j++)
//         {
//             ch = fgetc(file_ptr);
//             while ((ch != '.') && ((ch < '1') || (ch > '9')))
//                 ch = fgetc(file_ptr);
//             if (ch == '.')
//                 sudoku->grid[i][j] = UNASSIGNED;
//             else
//                 set_value(sudoku, i, j, ch-'0');
//         }
//     }
// }


int main(void)
{
    FILE *file_ptr = fopen("25_25.txt", "r"); // puzzles5_forum_hardest_1905_11+

    if (NULL == file_ptr) {
        printf("File can't be opened \n");
        exit(1);
    }
    Sudoku sudoku;
    int n_problems = 1;
    double start_time, end_time, elapsed_time;
    double total_elaspsed_time = 0.0;

    for (int i = 0; i < n_problems; i++)
    {
        sudoku_reset(&sudoku);
        read_single_problem(&sudoku, file_ptr);
        show_grid(&sudoku.grid);
        // show_sudoku(&sudoku);
        start_time  = CycleTimer::currentSeconds();
        solve(&sudoku);
        print_progress(double(i)/double(n_problems));
        end_time  = CycleTimer::currentSeconds();
        if (!validate_solution(&sudoku.grid)){
            printf("Wrong answer\n");
            show_sudoku(&sudoku);
            exit(1);
        }
        elapsed_time = end_time - start_time;
        printf("Correct. Elapsed time=%.4f sec\n", elapsed_time);
        total_elaspsed_time += elapsed_time;
    }
    printf("Total elapsed time=%.4f sec\n", total_elaspsed_time);    
    return 0;
}

