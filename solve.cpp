#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "CycleTimer.h"
#include "sudoku.h"

void read_single_problem(Grid *grid, FILE *file_ptr){
    char ch;
    for (int i = 0; i < N; i++)
    {
        for (int j = 0; j < N; j++)
        {
            ch = fgetc(file_ptr);
            (*grid)[i][j] = (ch == '.') ? UNASSIGNED : ch-'0';
        }
    }
    ch = fgetc(file_ptr);
}
int main(void)
{
    FILE *file_ptr = fopen("puzzles5_forum_hardest_1905_11+", "r"); // puzzles2_17_clue
    if (NULL == file_ptr) {
        printf("File can't be opened \n");
        exit(1);
    }

    Sudoku sudoku;
    // Sudoku *sudoku = (Sudoku *) malloc(1 * sizeof(Sudoku));
    int n_problems = 100;
    double start_time, end_time, elapsed_time;
    double total_elaspsed_time = 0.0;
    for (int i = 0; i < n_problems; i++)
    {
        sudoku_reset(&sudoku);
        read_single_problem(&sudoku.grid, file_ptr);
        // show_sudoku(&sudoku);
        start_time  = CycleTimer::currentSeconds();
        solve(&sudoku);
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

