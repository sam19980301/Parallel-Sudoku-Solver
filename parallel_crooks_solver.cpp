#include "sudoku.h"
#include "omp.h"
#include <iostream>
#include <vector>
#include <algorithm>
#include "CycleTimer.h"

#define DEBUG_COPY_SUDOKU

using namespace std;

void crook_pruning(Sudoku *sudoku);
int crooks_solver(Sudoku *sudoku);

const static int thread_num = 12;
const static int filled_cells_number = 5;

typedef struct
{
    int row;
    int col;
    vector<int> possible_values;
} UnknownCell;


void show_UnknownCell_list(vector<UnknownCell> &unknown_cell_list) {
    cout << "--- show_UnknowCell_list() ---" << endl;
    vector<UnknownCell>::iterator it;
    for(it = unknown_cell_list.begin(); it < unknown_cell_list.end(); it++) {
        cout << "Cell(" << (*it).row << ", " << (*it).col <<") possible value = ";
        for(int i = 0; i < (*it).possible_values.size(); i++) {
            cout << (*it).possible_values[i] << " ";
        }
        cout << endl;
    }
    cout << endl;
}

// get first K cells' cordinate 
void get_UnknownCell_list(vector<UnknownCell> &cell_list, const Markup *markup, const int num) {
    int row = 0, col = 0; 
    while(cell_list.size() < num) {
        // find unknown cell
        if ((*markup)[row][col]) {  
            UnknownCell cell;
            cell.row = row;
            cell.col = col;
            
            // push all possible value into a list
            for (int v = 1; v <= N; v++){
                if (((*markup)[row][col] & (1 << (v-1)))) {
                    cell.possible_values.push_back(v);
                }
            }
            cell_list.push_back(cell);
        }
        row = row + (col == N-1);
        col = (col + 1)%N;
    }
}

void get_perm(vector<Sudoku*> &sudoku_list, Sudoku *sudoku, const vector<UnknownCell> &cell_list, const int depth) {
    UnknownCell cell = cell_list[depth];  
    for(int i = 0; i < cell.possible_values.size(); i++) {
        Cell temp_cell;
        temp_cell.row = cell.row;
        temp_cell.col = cell.col;
        temp_cell.val = cell.possible_values[i];
        heap_push(&sudoku->heap, &temp_cell, &sudoku->markup, &sudoku->grid);
        set_value(sudoku, temp_cell.row, temp_cell.col, temp_cell.val);

        if(depth < cell_list.size()-1) {
            get_perm(sudoku_list, sudoku, cell_list, depth+1);
        } else if (depth == cell_list.size()-1) {
            Sudoku* copy = new Sudoku;
            copy_sudoku(copy, sudoku);
            sudoku_list.push_back(copy);
        }
        heap_pop(&sudoku->heap, &temp_cell, &sudoku->markup, &sudoku->grid);
    }
}

void show_sudoku_list(const vector<Sudoku*> sudoku_list) {
    cout << "--- show sudoku list! ---" << endl;
    for(int i = 0; i < sudoku_list.size(); i++) {
        show_grid(&sudoku_list[i]->grid);
    }
}

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

int solve(Sudoku *sudoku) {
    // const int thread_num = 16, filled_cells_number = 3;
    double start_time, end_time, elapsed_time;
    start_time  = CycleTimer::currentSeconds();

    cout << "-- parallel sudoku solver! --" << endl;
    crook_pruning(sudoku);
    int unknown = count_unknown(sudoku);
    cout << unknown << " [crook pruning] unknowns after pruning ..." << endl;
    cout << "---------------------------" << endl << endl;

    if(unknown==0) {  
        end_time = CycleTimer::currentSeconds();
        elapsed_time = end_time - start_time;
        if(validate_solution(&sudoku->grid)) {
            cout << "Problem has been solved without parallel backtracking! " << endl;
            cout << elapsed_time << "sec " << endl;
            cout << "-----------------" << endl << endl;
        }
        exit(0);
    }

    Grid *grid = &sudoku->grid;
    Heap *heap = &sudoku->heap;
    Markup *markup = &sudoku->markup;
    
    vector<UnknownCell> unknwon_cell_list;
    get_UnknownCell_list(unknwon_cell_list, markup, filled_cells_number);
    show_UnknownCell_list(unknwon_cell_list);
    
    vector<Sudoku*> parallel_sudoku_list;
    get_perm(parallel_sudoku_list, sudoku, unknwon_cell_list, 0);

    // for(int i = 0; i < parallel_sudoku_list.size(); i++) {
    //     cout << i << endl;
    //     show_grid(&parallel_sudoku_list[i]->grid);
    //     show_heap(&parallel_sudoku_list[i]->heap);
    // }

    // show_sudoku_list(parallel_sudoku_list);

    cout << "-- Original Grid --" << endl;
    show_grid(&sudoku->grid);

    end_time  = CycleTimer::currentSeconds();
    elapsed_time = end_time - start_time;
    cout << "Time of preparing parallel sudoku: " << elapsed_time << endl;
    cout << "Running "<< thread_num << " threads in parallel, there are " << parallel_sudoku_list.size() << " threads to be validated ... " << endl;
    #pragma omp parallel for num_threads (thread_num)
    for(int i = 0; i <parallel_sudoku_list.size(); i++) {
        double thread_start_time = CycleTimer::currentSeconds();
        double thread_end_time = 0;
        copy_sudoku(sudoku, parallel_sudoku_list[i]);
        crooks_solver(parallel_sudoku_list[i]);

        // cout << "----------- " << i << " ------------" << endl;
        if(validate_solution(&parallel_sudoku_list[i]->grid)) {
            // cout << i << " : Correct Answer!" << endl;
            // show_grid(&parallel_sudoku_list[i]->grid);
            thread_end_time = CycleTimer::currentSeconds();
            cout << (thread_end_time - thread_start_time) + elapsed_time << " sec" << endl;
            #pragma omp cancel for
        } else {
            // cout << i << " : Wrong Answer!" << endl;
        }
        // cout << "-----------------------" << endl;
    }
}

// serialized & non-recursive solver with crooks algorithm
int crooks_solver(Sudoku *sudoku){
    Grid *grid = &sudoku->grid;
    Heap *heap = &sudoku->heap;
    Markup *markup = &sudoku->markup;
    Cell top_cell;
    int row = 0, col = 0, val;
    int find_safe_insertion;

    // perform elimination & lone ranger first
    crook_pruning(sudoku);

    // set_value
    while (row != N)
    {
        find_safe_insertion = 0;
        if ((*grid)[row][col] == UNASSIGNED){
            if ((*markup)[row][col]){
                for (int v = 1; v <= N; v++){
                    if ((*markup)[row][col] & (1 << (v-1))){
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
            while (!find_safe_insertion){
                if (heap->count == 0)
                    return 0;
                // fail to find safe insertion, backtrack
                // try other value for the top cell
                heap_pop(heap, &top_cell, &sudoku->markup, &sudoku->grid);  // back-tracking
                row = top_cell.row;
                col = top_cell.col;
                val = top_cell.val;
                (*grid)[row][col] = UNASSIGNED;
                for (val =  top_cell.val + 1; val <= N; val++)
                {
                    if ((*markup)[row][col] & (1 << (val-1))){
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
    return 1;
}

void crook_pruning(Sudoku *sudoku){
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


// int solve(Sudoku *sudoku) {

//     // crook_pruning(sudoku);
//     Grid *grid = &sudoku->grid;
//     Heap *heap = &sudoku->heap;
//     Markup *markup = &sudoku->markup;
//     int thread_num = 0, cnt = 0, row = 0, col = 0;

//     // Find the top-left most Cell to parallelize
//     for(int i = 0; i < N; i++) {  
//         for(int j = 0; j < N; j++) {
//             if ((*markup)[i][j]) {
//                 row = i;
//                 col = j;
//                 goto EndOFLoop;
//             }
//         }
//     }

//     EndOFLoop:
//     // cout << "Parallelize possible value of Cell(" << row << ", " << col << ")." << endl;
    
//     // Count number of possible value of this Cell
//     for(int v=1; v<=N; v++) { 
//         if ((*markup)[row][col] & (1 << (v-1)))
//             thread_num++;
//     }


//     #ifdef DEBUG_COPY_SUDOKU
//     cout << "Creating " << thread_num << " threads to run in parallel..." << endl;
//     #endif

//     // Make copies of Sudoku to run in parallel
//     Sudoku **parallel_sudoku = new Sudoku*[thread_num];

//     for(int i=0; i<thread_num; i++) {
//         parallel_sudoku[i] = new Sudoku;
//         sudoku_reset(parallel_sudoku[i]);
//         copy_sudoku(parallel_sudoku[i], sudoku);
//     }

//     // Push possible value to this Cell
//     for (int v = 1; v <= N; v++){
//         if (((*markup)[row][col] & (1 << (v-1))) && cnt < thread_num) {
//             Cell cell = {row, col, v};
//             set_value(parallel_sudoku[cnt], row, col, v);
//             heap_push(&parallel_sudoku[cnt]->heap, &cell, &parallel_sudoku[cnt]->markup, &parallel_sudoku[cnt]->grid);
//             cnt++;
//         }
//     }

//     #ifdef DEBUG_COPY_SUDOKU
//     for(int i=0; i<thread_num; i++) {
//         cout << "----" << i << " thread ----" << endl;
//         show_grid(&parallel_sudoku[i]->grid);
//         // show_markup(&parallel_sudoku[i]->markup);
//         // show_heap(&parallel_sudoku[i]->heap);
//         cout << "-------------------" << endl;
//     }
//     cout << "--- original one ---" << endl;
//     show_grid(&sudoku->grid);
//     cout << "--------------" << endl;
//     #endif

//     // Run crook algo in parallel
//     for(int i = 0; i < thread_num; i++) {
    
//         #ifdef DEBUG_COPY_SUDOKU
//         cout << "----" << i << "----" << endl;
//         show_grid(&parallel_sudoku[i]->grid);
//         #endif

//         crooks_solver(parallel_sudoku[i]);
//         if(validate_solution(&parallel_sudoku[i]->grid)) {
//             cout << "Correct anwser!" << endl << endl;
//             copy_sudoku(sudoku, parallel_sudoku[i]);
//             show_grid(&sudoku->grid);
//         } else {
//             cout << "wrong answer!" << endl << endl;
//         }
//         cout << "----------------------------" <<endl;
//     }
// }
