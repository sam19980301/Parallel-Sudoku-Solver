#include "sudoku.h"
#include "stdlib.h"

void crook_pruning(Sudoku *sudoku);

// solution counter: 0 for no solution, 1 for unique solution, 2 for more than one solution
int count_solution(Sudoku *sudoku){
    Grid *grid = &sudoku->grid;
    Heap *heap = &sudoku->heap;
    Markup *markup = &sudoku->markup;
    Cell top_cell;
    int row = 0, col = 0, val;

    int count = 0;

    // perform elimination & lone ranger first
    crook_pruning(sudoku);
    // set_value
    while (row != N)
    {
        if ((*grid)[row][col] == UNASSIGNED){
            if ((*markup)[row][col]){
                for (int v = 1; v <= N; v++){
                    if ((*markup)[row][col] & (1 << (v-1))){
                        // find safe insertion, make a guess
                        Cell cell = {row, col, v};
                        heap_push(heap, &cell, &sudoku->markup, &sudoku->grid);
                        set_value(sudoku, row, col, v);
                        crook_pruning(sudoku);

                        count += count_solution(sudoku);
                        if (count > 1)
                            return 2;

                        heap_pop(heap, &top_cell, &sudoku->markup, &sudoku->grid);
                    }
                }
            }
            else{
                while (1){
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
                        if ((*markup)[row][col] & (1 << (val-1))){
                            Cell cell = {row, col, val};
                            heap_push(heap, &cell, &sudoku->markup, &sudoku->grid);
                            set_value(sudoku, row, col, val);
                            crook_pruning(sudoku);
                            
                            count += count_solution(sudoku);
                            if (count > 1)
                                return 2;
                            
                            heap_pop(heap, &top_cell, &sudoku->markup, &sudoku->grid);
                        }
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

void shuffle(int *arr, int n)
{
    if (n > 1) 
    {
        int i;
        for (i = 0; i < n - 1; i++) 
        {
          int j = i + rand() / (RAND_MAX / (n - i) + 1);
          int t = arr[j];
          arr[j] = arr[i];
          arr[i] = t;
        }
    }
}

void init_arr(int *arr, int size){
    for (int i = 0; i < size; i++)
        arr[i] = i;    
}

void shuffle_arr(int *arr, int size) {
    if (size > 1) {
        for (int i = 0; i < size - 1; ++i) {
            int j = i + ((int) rand()) / (RAND_MAX / (size - i) + 1);
            int tmp = arr[i];
            arr[i] = arr[j];
            arr[j] = tmp;
        }
    }
}

int *random_index_arr = (int *) malloc(N * N * sizeof(int));
int *random_value_arr = (int *) malloc(N * sizeof(int));

void random_remove(Sudoku *sudoku){
    for (int i = 0; i < N * N; i++)
    {
        int row = random_index_arr[i] / N;
        int col = random_index_arr[i] % N;
        if (sudoku->grid[row][col] != UNASSIGNED){
            sudoku->grid[row][col] = UNASSIGNED;
            return;
        }
    }
    return;   
}
void random_insert(Sudoku *sudoku, Cell *cell){
    for (int i = 0; i < N * N; i++)
    {
        int row = random_index_arr[i] / N;
        int col = random_index_arr[i] % N;
        if (sudoku->grid[row][col] == UNASSIGNED){
            for (int j = 0; j < N; j++)
            {
                int val = random_value_arr[j];
                if (is_safe(&sudoku->grid, row, col, val)){
                    set_value(sudoku, row, col, val);
                    return;
                }
            }
        }
    }
}

int generate_sudoku(Sudoku *sudoku, int max_attempt){
    init_arr(random_index_arr, N * N);
    shuffle_arr(random_index_arr, N * N);
    init_arr(random_value_arr, N);
    shuffle_arr(random_value_arr, N);

    // initiate a new sudoku

    int attempt_count = 0;
    int solution_count;
    Cell last_insert_cell;
    while (attempt_count <= max_attempt)
    {
        solution_count = count_solution(sudoku);
        if (solution_count == 0){
            sudoku->grid[last_insert_cell.row][last_insert_cell.col] = UNASSIGNED;
            random_remove(sudoku);
        }
        else if (solution_count == 1){
            return attempt_count;
        }
        else if (solution_count == 2){
            random_insert(sudoku, &last_insert_cell);
        }

        // cleanup heap
        sudoku->heap.count = 0;
        attempt_count++;
    }
    return -1;
}