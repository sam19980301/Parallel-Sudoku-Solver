#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "CycleTimer.h"
#include "sudoku.h"

void read_single_problem(Sudoku *sudoku, FILE *file_ptr){
    char ch;
    for (int i = 0; i < N; i++)
    {
        for (int j = 0; j < N; j++)
        {
            ch = fgetc(file_ptr);
            while ((ch != '.') && ((ch < '1') || (ch > '9')))
                ch = fgetc(file_ptr);
            if (ch == '.')
                sudoku->grid[i][j] = UNASSIGNED;
            else
                set_value(sudoku, i, j, ch-'0');
        }
    }
}
// int main(void)
int main(int argc, char *argv[])
{
    printf("%d\n", argc);
    if (argc != 3){
        printf("Invalid arguments command usage: ./sudoku <num-of-problems> <puzzle_filename> \n");
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
        read_single_problem(&sudoku, file_ptr);
        // show_grid(&sudoku.grid);
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

