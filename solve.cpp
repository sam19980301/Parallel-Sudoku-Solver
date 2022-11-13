#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "CycleTimer.h"
#include "sudoku.h"

void read_single_problem(Grid grid, FILE *file_ptr){
    char ch;
    for (int i = 0; i < N; i++)
    {
        for (int j = 0; j < N; j++)
        {
            ch = fgetc(file_ptr);
            grid[i][j] = (ch == '.') ? UNASSIGNED : ch-'0';
        }
    }
    ch = fgetc(file_ptr);
}

int main()
{
    FILE *file_ptr = fopen("puzzles5_forum_hardest_1905_11+", "r"); // puzzles2_17_clue
    if (NULL == file_ptr) {
        printf("file can't be opened \n");
        exit(1);
    }

    Sudoku sudoku;
    sudoku_init(&sudoku);
    // print_sudoku(sudoku.grid);

    int correct;
    int n_problems = 1;
    double elapsed_time;
    double total_elaspsed_time = 0.0;
    for (int i = 0; i < n_problems; i++)
    {
        sudoku_reset(&sudoku);
        read_single_problem(sudoku.grid, file_ptr);
        // print_sudoku(sudoku.grid);
        double start_time = CycleTimer::currentSeconds();
        solve(&sudoku);
        double end_time = CycleTimer::currentSeconds();
        correct = validate_solution(sudoku.grid);
        if (!correct){
            printf("The answer is wrong\n");
            print_sudoku(sudoku.grid);
            exit(1);
        }
        elapsed_time = end_time - start_time;
        printf("Elapsed time=%.4f sec\n", elapsed_time);
        total_elaspsed_time += elapsed_time;
        // print_sudoku(sudoku.grid);
    }
    printf("Total elapsed time=%.4f sec\n", total_elaspsed_time);    
    sudoku_free(&sudoku);
    return 0;
}

