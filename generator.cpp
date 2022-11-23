#include <random>
#include <vector>
#include <algorithm>
#include "sudoku.h"

std::vector<int> nums_9 = {1, 2, 3, 4, 5, 6, 7, 8, 9};
std::vector<int> nums_16 = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16};
std::vector<int> nums_25 = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 
                            17, 18, 19, 20, 21, 22, 23, 24, 25};
std::vector<int> *nums = NULL;

void init_nums(){
    if (N == 9) nums = &nums_9;
    else if (N == 16) nums = &nums_16;
    else if (N == 25) nums = &nums_25;
    else { printf("invalid N (N = %d)\n", N); exit(1); }
}

bool is_full(Grid* grid, int &row, int &col){
    for (int i = 0; i < N; i++)
    {
        for (int j = 0; j < N; j++)
        {
            if((*grid)[i][j] == UNASSIGNED)
            {
                row = i;
                col = j;
                return false;
            }
        }
    }

    return true;
}

bool can_fill_grid(Grid* grid){
    int row, col;
    if (is_full(grid, row, col)) return true;

    std::default_random_engine engine;
    std::shuffle((*nums).begin(), (*nums).end(), engine);

    int count = 18;
    for (int num : (*nums))
    {
        if (is_safe(grid, row, col, num))
        {
            (*grid)[row][col] = num;
            if (can_fill_grid(grid)) return true;
            (*grid)[row][col] = UNASSIGNED;
        }
    }

    return false;
} 

void set_single_problem(Sudoku* sudoku){
    init_nums();
    can_fill_grid(&sudoku->grid);
}