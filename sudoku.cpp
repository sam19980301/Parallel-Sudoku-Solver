#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "sudoku.h"
#include <time.h>

// TBD inline propoerty performance
// TBD backtracing solver
// TBD serialize pruning solver
// TBD validator

// | Date | To Do |
// | ---- | ----- |
// | Oct.31 - Nov.6  | Sudoku basic operation / unittest |
// | Nov.7 - Nov.13  | Sudoku problem generator / data visualization |
// | Nov.14 - Nov.20 | Serialized version solver / answer validator (Done) |
// | Nov.21 - Nov.27 | Parallelized version solver |
// | Nov.28 - Dec.4  | Advanced algorithm optimization (e.g., pruning) and others |
// | Dec.5 - Dec.11  | Presentation preparation |

void sudoku_init(Sudoku *sudoku){
    sudoku->grid = (Grid) malloc(N * N * sizeof(int));
}

void sudoku_free(Sudoku *sudoku){
    free(sudoku->grid);
}

// Print Sudoku grid
void print_sudoku(Grid grid)
{
    for (int i = 0; i < N; i++){
        for (int j = 0; j < N; j++)
            printf("%d\t", grid[i][j]);
        printf("\n");
    }
    printf("\n");
}

// Insert if legal
int legal_insertion(Grid grid, int row, int col, int num){ // Sudoku *sudoku
    // Grid grid = sudoku->grid;
    // Check if filled
    if (grid[row][col] != UNASSIGNED)
        return (grid[row][col] == num);
    // Check row
    for (int i = 0; i < N; ++i)
        if (grid[row][i] == num)
            return 0;
    // Check column
    for (int i = 0; i < N; ++i)
        if (grid[i][col] == num)
            return 0;
    // Check Subgrid
    int start_row = row / SUB_N, start_col = col / SUB_N;
    for (int i = 0; i < SUB_N; ++i)
        for (int j = 0; j < SUB_N; ++j)
            if (grid[start_row*SUB_N+i][start_col*SUB_N+j] == num)
                return 0;
    grid[row][col] = num;
    return 1;
}

// Naive approach
int naive_solver(Grid grid){ return _naive_solver(grid, 0, 0); }
int _naive_solver(Grid grid, int row, int col){
    #ifdef DEBUG
        printf("Solving %d\t%d\n", row, col);
        print_sudoku(grid);
    #endif

    // finish iteration
    if (row == N)
        return 1;
    int next_row = row + (col == N-1), next_col = (col + 1)%N;
    // skip if assigned
    if (grid[row][col] != UNASSIGNED)
        return _naive_solver(grid, next_row, next_col);
    // try all possible value
    for (int v = 1; v <= N; ++v){
        // single guess
        if (legal_insertion(grid, row, col, v)){
            #ifdef DEBUG
                print_sudoku(grid);
            #endif

            if (_naive_solver(grid, next_row, next_col))
                return 1;
        }
        else{
            #ifdef DEBUG
                printf("Invalid insertion %d\t%d\t%d\n", row, col, v);
            #endif
        }
        // undo guess
        grid[row][col] = 0;
    }
    return 0;
}

// Count solution: 0: no solution, 1: single solution, else >1
// TBD no use
int count_solution(Grid grid){ return _count_solution(grid, 0, 0); }
int _count_solution(Grid grid, int row, int col){
    // finish iteration
    if (row == N)
        return 1;
    int next_row = row + (col == N-1), next_col = (col + 1)%N;
    // skip if assigned
    if (grid[row][col] != UNASSIGNED)
        return _count_solution(grid, next_row, next_col);
    // try all possible value
    int cnt = 0;
    for (int v = 1; v <= N; ++v){
        // single guess
        if (legal_insertion(grid, row, col, v)){
            cnt += _count_solution(grid, next_row, next_col);
            // early stop
            if (cnt > 1)
            	return cnt;
        }
        // undo guess
        grid[row][col] = 0;
    }
    return cnt;
}

// Genertor
// TBD
// start with blank
// if not unique then random do legal add
// if no solution then random do remove
// abort if the # attempt exceed threshold
// https://www.geeksforgeeks.org/program-sudoku-generator/
void generate_sudoku(Grid grid, int blank){
    srand(time(NULL));
    // random fill
    for (int i = 0; i < N*N; i++){
        while (legal_insertion(grid, rand()%9, rand()%9, rand()%9+1));
    }
    for (int i = 0; i < blank; i++){
        int row;
        int col;
    	do{
    	    row = rand()%9;
    	    col = rand()%9;
    	} while (grid[row][col] == UNASSIGNED);
    	grid[row][col] = UNASSIGNED;    	
    }    
}

// Validate correctness of solution
int validate_solution(Grid grid){
    for (int row = 0; row < N; ++row){
        for (int col = 0; col < N; ++col){
            printf("%d %d\n",row, col);
            // UNFINISHED
            if (grid[row][col] == UNASSIGNED)
                return 0;
            // Check row
            for (int i = 0; i < N; ++i)
                if ((grid[row][col] == grid[row][i]) && (col != i))
                    return 0;
            // Check column
            for (int i = 0; i < N; ++i)
                if ((grid[row][col] == grid[i][col]) && (row != i))
                    return 0;
            // Check Subgrid
            int start_row = row / SUB_N, start_col = col / SUB_N;
            for (int i = 0; i < SUB_N; ++i)
                for (int j = 0; j < SUB_N; ++j)
                    if ((grid[row][col] == grid[start_row*SUB_N+i][start_col*SUB_N+j]) && (row%SUB_N != i) && (col%SUB_N != j))
                        return 0;
        }
    }
    return 1;
}

int main()
{
    Sudoku sudoku;
    sudoku_init(&sudoku);
    int grid[N][N] = { { 3, 0, 6, 5, 0, 8, 4, 0, 0 },
                       { 5, 2, 0, 0, 0, 0, 0, 0, 0 },
                       { 0, 8, 7, 0, 0, 0, 0, 3, 1 },
                       { 0, 0, 3, 0, 1, 0, 0, 8, 0 },
                       { 9, 0, 0, 8, 6, 3, 0, 0, 5 },
                       { 0, 5, 0, 0, 9, 0, 6, 0, 0 },
                       { 1, 3, 0, 0, 0, 0, 2, 5, 0 },
                       { 0, 0, 0, 0, 0, 0, 0, 7, 4 },
                       { 0, 0, 5, 2, 0, 6, 3, 0, 0 } };
    memcpy(sudoku.grid, grid, N * N * sizeof(int));
    print_sudoku(sudoku.grid);

    if (naive_solver(sudoku.grid)){
        // TBD assert
        printf("Solution exists\n");
        print_sudoku(sudoku.grid);
        // int correct = validate_solution(sudoku.grid);
        // printf("%d ccc", correct);
        // // if ()
        // //     printf("Solution correct\n");
        
    }
    else
        // TBD assert
        printf("No solution exists");
    sudoku_free(&sudoku);
    return 0;
}
