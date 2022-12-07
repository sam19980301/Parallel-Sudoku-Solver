#include <iostream>
#include "sudoku.h"
#include "CycleTimer.h"

using namespace std;

void crook_pruning(Sudoku *sudoku);

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

// serialized & non-recursive solver with crooks algorithm
int solve(Sudoku *sudoku)
{
    cout << "[ Serialized Crook Solver] " << endl;
    double start_time = CycleTimer::currentSeconds();
    Grid *grid = &sudoku->grid;
    Heap *heap = &sudoku->heap;
    Markup *markup = &sudoku->markup;
    Cell top_cell;
    int row = 0, col = 0, val;
    int find_safe_insertion;

    // perform elimination & lone ranger first
    crook_pruning(sudoku);
    int unknown = count_unknown(sudoku);
    cout << "[ Crook pruning ]" << endl << " # Unknowns cells in the grid after pruning: " << unknown << endl;
    cout << "---------------------------" << endl << endl;
    if(unknown==0) 
    {
        double end_time = CycleTimer::currentSeconds();
        double elapsed_time = end_time - start_time;
        if(validate_solution(&sudoku->grid)) {
            cout << "Problem has been solved without parallel backtracking! " << endl;
            cout << elapsed_time << " sec " << endl;
            cout << "-----------------" << endl << endl;
        }
        exit(0);
    }
    // set_value
    while (row != N)
    {
        find_safe_insertion = 0;
        if ((*grid)[row][col] == UNASSIGNED)
        {
            if ((*markup)[row][col]) 
            {
                for (int v = 1; v <= N; v++)
                {
                    if ((*markup)[row][col] & (1 << (v-1)))
                    {
                        // find safe insertion, make a guess
                        Cell cell = {row, col, v};
                        heap_push(heap, &cell, &sudoku->markup, &sudoku->grid);
                        set_value(sudoku, row, col, v);
                        crook_pruning(sudoku);
                        find_safe_insertion = 1;
                        break;
                    }
                }
            }
            while (!find_safe_insertion)
            {
                if (heap->count == 0)
                    return 0;
                // fail to find safe insertion, backtrack
                // try other value for the top cell
                heap_pop(heap, &top_cell, &sudoku->markup, &sudoku->grid);
                row = top_cell.row;
                col = top_cell.col;
                val = top_cell.val;
                (*grid)[row][col] = UNASSIGNED;
                for (val =  top_cell.val + 1; val <= N; val++)
                {
                    if ((*markup)[row][col] & (1 << (val-1)))
                    {
                        Cell cell = {row, col, val};
                        heap_push(heap, &cell, &sudoku->markup, &sudoku->grid);
                        set_value(sudoku, row, col, val);
                        crook_pruning(sudoku);
                        find_safe_insertion = 1;
                        break;
                    }
                }
            }
        }
        row = row + (col == N-1);
        col = (col + 1)%N;
    }
    double end_time = CycleTimer::currentSeconds();
    double elapsed_time = end_time - start_time;

    // Verbose result
    if(validate_solution(&sudoku->grid))
    {
        cout << "-- Answer found! --" << endl;
        cout << "Response time (1st answer return): " << elapsed_time << " (sec)" << endl;
        show_grid(&sudoku->grid);
        exit(0);
    }
    else 
    {
        cout << "Answer Not Found!" << endl;
    }

    return 1;
}

void crook_pruning(Sudoku *sudoku)
{
    int changed, e_changed, l_changed;
    do
    {
        changed = 0;
        do
        {
            e_changed = elimination(sudoku);
            changed += e_changed;
        } while (e_changed);
        do
        {
            l_changed = lone_ranger(sudoku);
            changed += l_changed;
        } while (l_changed);
    } while (changed);
}
