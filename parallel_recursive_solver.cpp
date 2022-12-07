#include "sudoku.h"
#include "omp.h"
#include <iostream>
#include <vector>
#include <algorithm>
#include "CycleTimer.h"

using namespace std;

void crook_pruning(Sudoku *sudoku);
int serial_recursive_solver(Grid *grid, int row, int col);

static const int threads_in_parallel = 16;  // Specify omp parallel threads
static const int filled_cells_number = 2;   // fill first K unknown cells

typedef struct
{
    int row;
    int col;
    vector<int> possible_values;
} UnknownCell;

// Show possible values in each unknonws Cell (first K unknown cell)
void show_unknownCell_list(vector<UnknownCell> &unknown_cell_list) 
{
    cout << "[ Possible value of unknown cells ]" << endl;
    for (int i = 0; i < unknown_cell_list.size(); i++) 
    {
        int row = unknown_cell_list[i].row; 
        int col = unknown_cell_list[i].col;
        cout << "  Cell(" << row << ", " << col << ") = ";
        for(int j = 0; j <unknown_cell_list[i].possible_values.size(); j++) 
        {
            cout << unknown_cell_list[i].possible_values[j] << " ";
        }
        cout << endl;
    }
    cout << endl << endl;
}

// get first K unknown cells' cordinate 
int get_UnknownCell_list(vector<UnknownCell> &cell_list, const Markup *markup, const int num) 
{
    int row = 0, col = 0, total_threads = 1;
    while(cell_list.size() < num) 
    {
        // find unknown cell
        if ((*markup)[row][col]) 
        {  
            UnknownCell cell;
            cell.row = row;
            cell.col = col;
            
            // push all possible value into a list
            for (int v = 1; v <= N; v++)
            {
                if (((*markup)[row][col] & (1 << (v-1)))) 
                {
                    cell.possible_values.push_back(v);
                }
            }
            cell_list.push_back(cell);
            total_threads *= cell.possible_values.size();
        }
        row = row + (col == N-1);
        col = (col + 1)%N;
    }
    return total_threads;  // number of total threads to be vaildated
}

// Find total threads to be validated 
// Total threads = permutation of possible values in the first K unknown cells.
void get_perm(vector<Sudoku*> &sudoku_list, Sudoku *sudoku, const vector<UnknownCell> &cell_list, const int depth) 
{
    UnknownCell cell = cell_list[depth];  
    for(int i = 0; i < cell.possible_values.size(); i++) 
    {
        Cell temp_cell;
        temp_cell.row = cell.row;
        temp_cell.col = cell.col;
        temp_cell.val = cell.possible_values[i];
        heap_push(&sudoku->heap, &temp_cell, &sudoku->markup, &sudoku->grid);
        set_value(sudoku, temp_cell.row, temp_cell.col, temp_cell.val);

        if(depth < cell_list.size()-1) 
        {
            get_perm(sudoku_list, sudoku, cell_list, depth+1);
        } 
        else if (depth == cell_list.size()-1) 
        {
            Sudoku* copy = new Sudoku;
            copy_sudoku(copy, sudoku);
            sudoku_list.push_back(copy);
        }
        heap_pop(&sudoku->heap, &temp_cell, &sudoku->markup, &sudoku->grid);
    }
}

void show_sudoku_list(const vector<Sudoku*> sudoku_list) 
{
    cout << "[Show parallel sudoku list]" << endl;
    for(int i = 0; i < sudoku_list.size(); i++) 
    {
        show_grid(&sudoku_list[i]->grid);
    }
}

// Return number of unknown cells in a sudoku
int count_unknown(Sudoku *sudoku) 
{
    int count = 0;
    for(int i = 0; i < N; i++) 
    {
        for(int j = 0; j < N; j++)
        {
            if((sudoku->grid)[i][j] == UNASSIGNED)
            {
                count++;
            }
        }
    }
    return count;
}

int solve(Sudoku *sudoku) 
{
    double serial_start_time = 0, serial_end_time = 0, serial_elapsed_time = 0;
    cout << "[ Parallelized Recursive Solver ]" << endl;
    serial_start_time  = CycleTimer::currentSeconds();
/*
    // Crook pruning
    int unknown = count_unknown(sudoku);
    crook_pruning(sudoku);
    cout << "[ Crook pruning ]" << endl << " # Unknowns cells in the grid after pruning: " << unknown << endl;
    cout << "---------------------------" << endl << endl;
    if(unknown==0) {
        serial_end_time = CycleTimer::currentSeconds();
        serial_elapsed_time = serial_end_time - serial_start_time;
        if(validate_solution(&sudoku->grid)) {
            cout << "Problem has been solved without parallel backtracking! " << endl;
            cout << serial_elapsed_time << "sec " << endl;
            cout << "-----------------" << endl << endl;
        }
        exit(0);
    }
*/
    Grid *grid = &sudoku->grid;
    Heap *heap = &sudoku->heap;
    Markup *markup = &sudoku->markup;
    
    // Get first K unknowns cells (including index and possible values)
    vector<UnknownCell> unknown_cell_list;
    unknown_cell_list.reserve(filled_cells_number);
    int total_threads = get_UnknownCell_list(unknown_cell_list, markup, filled_cells_number);
    show_unknownCell_list(unknown_cell_list);
    
    // Get permutations of possible values in the first K unknowm cells
    vector<Sudoku*> parallel_sudoku_list;
    parallel_sudoku_list.reserve(total_threads);   // A simple optimization to avoid memcpy
    get_perm(parallel_sudoku_list, sudoku, unknown_cell_list, 0);  
    serial_end_time  = CycleTimer::currentSeconds();
    serial_elapsed_time = serial_end_time - serial_start_time;
    
    // Verbose
    cout << "-- Original Grid --" << endl;
    show_grid(&sudoku->grid);
    cout << "Number of Unknown cells been filled: "  << unknown_cell_list.size() << endl;
    cout << "              Number of omp threads: " << threads_in_parallel << endl; 
    cout << "  Number of Threads to be validated: " << parallel_sudoku_list.size() << endl;
    cout << "    Time to prepare parallel sudoku: " << serial_elapsed_time << endl << endl;
    
    // Parallelized & recursive solver
    double parallel_start_time = CycleTimer::currentSeconds();
    #pragma omp parallel for num_threads (threads_in_parallel)
    for(int i = 0; i < parallel_sudoku_list.size(); i++) 
    {
        double parallel_end_time = 0;
        // crook_pruning(parallel_sudoku_list[i]);
        serial_recursive_solver(&parallel_sudoku_list[i]->grid, 0, 0);

        #pragma omp critical
        {
            if(validate_solution(&parallel_sudoku_list[i]->grid)) 
            {
                parallel_end_time = CycleTimer::currentSeconds();
                cout << "-- Answer found! --" << endl;
                cout << "Response time (1st answer return): " << (parallel_end_time - parallel_start_time) + serial_elapsed_time << " (sec)" << endl;
                cout << "Thread number (1st answer return): " << i << endl;
                show_grid(&parallel_sudoku_list[i]->grid);
                copy_sudoku(sudoku, parallel_sudoku_list[i]);
                exit(0);
            } 
            else 
            {
                // cout << i << " : Wrong Answer!" << endl;
            }
        // cout << "-----------------------" << endl;
        }
    }
}

// serialized & recursive solver
int serial_recursive_solver(Grid *grid, int row, int col)
{
    // finish iteration
    if (row == N)
        return 1;
    int next_row = row + (col == N-1), next_col = (col + 1)%N;
    // skip if assigned
    if ((*grid)[row][col] != UNASSIGNED)
        return serial_recursive_solver(grid, next_row, next_col);
    // try all possible value
    for (int v = 1; v <= N; ++v)
    {
        // single guess
        if (is_safe(grid, row, col, v))
        {
            #ifdef DEBUG
                show_grid(grid);
            #endif
            (*grid)[row][col] = v;
            if (serial_recursive_solver(grid, next_row, next_col))
                return 1;
        }
        else
        {
            #ifdef DEBUG
                printf("Invalid insertion %d\t%d\t%d\n", row, col, v);
            #endif
        }
        // undo guess
        (*grid)[row][col] = UNASSIGNED;
    }
    return 0;
}