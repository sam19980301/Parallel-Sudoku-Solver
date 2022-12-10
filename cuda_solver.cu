#include <cuda.h>

#include "sudoku.h"

#define NUM_TILES N * N
#define n SUB_N

void load(Grid* grid, int *board) {
    for (int i = 0; i < NUM_TILES; i++)
        board[i] = (*grid)[i / N][i % N];
}

void printBoard(int *board) {
    for (int i = 0; i < N; i++) {
        if (i % n == 0) {
            printf("-----------------------\n");
        }

        for (int j = 0; j < N; j++) {
            if (j % n == 0) {
            printf("| ");
            }
            printf("%d ", board[i * N + j]);
        }

        printf("|\n");
    }
    printf("-----------------------\n");
}

/**
 * This kernel has each thread try to solve a different board in the input array using the
 * backtracking algorithm.
 *
 * boards:      This is an array of size numBoards * N * N. Each board is stored contiguously,
 *              and rows are contiguous within the board. So, to access board x, row r, and col c,
 *              use boards[x * N * N + r * N + c]
 *
 * numBoards:   The total number of boards in the boards array.
 *
 * emptySpaces: This is an array of size numBoards * N * N. board is stored contiguously, and stores
 *              the indices of the empty spaces in that board. Note that this N * N pieces may not
 *              be filled.
 *
 * numEmptySpaces:  This is an array of size numBoards. Each value stores the number of empty spaces
 *                  in the corresponding board.
 *
 * finished:    This is a flag that determines if a solution has been found. This is a stopping
 *              condition for the kernel.
 *
 * solved:      This is an output array of size N * N where the solved board is stored.
 */
__global__
void sudokuBacktrack(int *boards,
        const int numBoards,
        int *emptySpaces,
        int *numEmptySpaces,
        int *finished,
        int *solved) {

    int index = blockDim.x * blockIdx.x + threadIdx.x;

    int *currentBoard;
    int *currentEmptySpaces;
    int currentNumEmptySpaces;


    while ((*finished == 0) && (index < numBoards)) {
    
        int emptyIndex = 0;

        currentBoard = boards + index * 81;
        currentEmptySpaces = emptySpaces + index * 81;
        currentNumEmptySpaces = numEmptySpaces[index];

        while ((emptyIndex >= 0) && (emptyIndex < currentNumEmptySpaces)) {

            currentBoard[currentEmptySpaces[emptyIndex]]++;

            if (!validBoard(currentBoard, currentEmptySpaces[emptyIndex])) {

                // if the board is invalid and we tried all numbers here already, backtrack
                // otherwise continue (it will just try the next number in the next iteration)
                if (currentBoard[currentEmptySpaces[emptyIndex]] >= 9) {
                    currentBoard[currentEmptySpaces[emptyIndex]] = 0;
                    emptyIndex--;
                }
            }
            // if valid board, move forward in algorithm
            else {
                emptyIndex++;
            }

        }

        if (emptyIndex == currentNumEmptySpaces) {
            // solved board found
            *finished = 1;

            // copy board to output
            for (int i = 0; i < N * N; i++) {
                solved[i] = currentBoard[i];
            }
        }

        index += gridDim.x * blockDim.x;
    }
}

void cudaSudokuBacktrack(const unsigned int blocks,
        const unsigned int threadsPerBlock,
        int *boards,
        const int numBoards,
        int *emptySpaces,
        int *numEmptySpaces,
        int *finished,
        int *solved) {

    sudokuBacktrack<<<blocks, threadsPerBlock>>>
        (boards, numBoards, emptySpaces, numEmptySpaces, finished, solved);
}

/**
 * This kernel takes a set of old boards and finds all possible next boards by filling in the next
 * empty space.
 *
 * old_boards:      This is an array of size sk. Each N * N section is another board. The rows
 *                  are contiguous within the board. This array stores the previous set of boards.
 *
 * new_boards:      This is an array of size sk. Each N * N section is another board. The rows
 *                  are contiguous within the board. This array stores the next set of boards.
 *
 * total_boards:    Number of old boards.
 *
 * board_index:     Index specifying the index of the next opening in new_boards.
 *
 * empty_spaces:    This is an array of size sk. Each N * N section is another board, storing the
 *                  indices of empty spaces in new_boards.
 *
 * empty_space_count:   This is an array of size sk / N / N + 1 which stores the number of empty
 *                      spaces in the corresponding board.
 */
__global__
void
cudaBFSKernel(int *old_boards,
        int *new_boards,
        int total_boards,
        int *board_index,
        int *empty_spaces,
        int *empty_space_count) {
    
    unsigned int index = blockIdx.x * blockDim.x + threadIdx.x;
    
    // board_index must start at zero 

    while (index < total_boards) {
        // find the next empty spot
        int found = 0;

        for (int i = (index * N * N); (i < (index * N * N) + N * N) && (found == 0); i++) {
            // found a open spot
            if (old_boards[i] == 0) {
                found = 1;
                // get the correct row and column shits
                int temp = i - N * N * index;
                int row = temp / N;
                int col = temp % N;
                
                // figure out which numbers work here
                for (int attempt = 1; attempt <= N; attempt++) {
                    int works = 1;
                    // row constraint, test various columns
                    for (int c = 0; c < N; c++) {
                        if (old_boards[row * N + c + N * N * index] == attempt) {
                            works = 0;
                        }
                    }
                    // column contraint, test various rows
                    for (int r = 0; r < N; r++) {
                        if (old_boards[r * N + col + N * N * index] == attempt) {
                            works = 0;
                        }
                    }
                    // box constraint
                    for (int r = n * (row / n); r < n; r++) {
                        for (int c = n * (col / n); c < n; c++) {
                            if (old_boards[r * N + c + N * N * index] == attempt) {
                                works = 0;
                            }
                        }
                    }
                    if (works == 1) {
                        // copy the whole board

                        int next_board_index = atomicAdd(board_index, 1);
                        int empty_index = 0;
                        for (int r = 0; r < 9; r++) {
                            for (int c = 0; c < 9; c++) {
                                new_boards[next_board_index * 81 + r * 9 + c] = old_boards[index * 81 + r * 9 + c];
                                if (old_boards[index * 81 + r * 9 + c] == 0 && (r != row || c != col)) {
                                    empty_spaces[empty_index + 81 * next_board_index] = r * 9 + c;

                                    empty_index++;
                                }
                            }
                        }
                        empty_space_count[next_board_index] = empty_index;
                        new_boards[next_board_index * 81 + row * 9 + col] = attempt;
                    }
                }
            }
        }

        index += blockDim.x * gridDim.x;
    }
}


void callBFSKernel(const unsigned int blocks, 
                        const unsigned int threadsPerBlock,
                        int *old_boards,
                        int *new_boards,
                        int total_boards,
                        int *board_index,
                        int *empty_spaces,
                        int *empty_space_count) {
    cudaBFSKernel<<<blocks, threadsPerBlock>>>
        (old_boards, new_boards, total_boards, board_index, empty_spaces, empty_space_count);
}

void hostFE(Grid* grid){
    const unsigned int threadsPerBlock = N;
    const unsigned int maxBlocks = N; 

    // load the board
    int *board = new int[NUM_TILES];
    load(grid, board);

    // the boards after the next iteration of breadth first search
    int *new_boards;
    // the previous boards, which formthe frontier of the breadth first search
    int *old_boards;
    // stores the location of the empty spaces in the boards
    int *empty_spaces;
    // stores the number of empty spaces in each board
    int *empty_space_count;
    // where to store the next new board generated
    int *board_index;

    // maximum number of boards from breadth first search
    const int sk = 1 << 26;

    // allocate memory on the device
    cudaMalloc(&empty_spaces, sk * sizeof(int));
    cudaMalloc(&empty_space_count, (sk / NUM_TILES + 1) * sizeof(int));
    cudaMalloc(&new_boards, sk * sizeof(int));
    cudaMalloc(&old_boards, sk * sizeof(int));
    cudaMalloc(&board_index, sizeof(int));

    // same as board index, except we need to set board_index to zero every time and this can stay
    int total_boards = 1;

    // initialize memory
    cudaMemset(board_index, 0, sizeof(int));
    cudaMemset(new_boards, 0, sk * sizeof(int));
    cudaMemset(old_boards, 0, sk * sizeof(int));

    // copy the initial board to the old boards
    cudaMemcpy(old_boards, board, N * N * sizeof(int), cudaMemcpyHostToDevice);

    // call the kernel to generate boards
    callBFSKernel(maxBlocks, threadsPerBlock, old_boards, new_boards, total_boards, board_index,
        empty_spaces, empty_space_count);

    // number of boards after a call to BFS
    int host_count;
    // number of iterations to run BFS for
    int iterations = 18;

    // loop through BFS iterations to generate more boards deeper in the tree
    for (int i = 0; i < iterations; i++) {
        cudaMemcpy(&host_count, board_index, sizeof(int), cudaMemcpyDeviceToHost);

        //printf("total boards after an iteration %d: %d\n", i, host_count);

        cudaMemset(board_index, 0, sizeof(int));


        if (i % 2 == 0) {
            callBFSKernel(maxBlocks, threadsPerBlock, new_boards, old_boards, host_count, board_index, empty_spaces, empty_space_count);
        }
        else {
            callBFSKernel(maxBlocks, threadsPerBlock, old_boards, new_boards, host_count, board_index, empty_spaces, empty_space_count);
        }
    }

    cudaMemcpy(&host_count, board_index, sizeof(int), cudaMemcpyDeviceToHost);
    //printf("new number of boards retrieved is %d\n", host_count);

    // flag to determine when a solution has been found
    int *dev_finished;
    // output to store solved board in
    int *dev_solved;

    // allocate memory on the device
    cudaMalloc(&dev_finished, sizeof(int));
    cudaMalloc(&dev_solved, N * N * sizeof(int));

    // initialize memory
    cudaMemset(dev_finished, 0, sizeof(int));
    cudaMemcpy(dev_solved, board, N * N * sizeof(int), cudaMemcpyHostToDevice);

    if (iterations % 2 == 1) {
        // if odd number of iterations run, then send it old boards not new boards;
        new_boards = old_boards;
    }

    cudaSudokuBacktrack(maxBlocks, threadsPerBlock, new_boards, host_count, empty_spaces,
        empty_space_count, dev_finished, dev_solved);


    // copy back the solved board
    int *solved = new int[N * N];

    memset(solved, 0, N * N * sizeof(int));

    cudaMemcpy(solved, dev_solved, N * N * sizeof(int), cudaMemcpyDeviceToHost);

    printBoard(solved);


    // free memory
    delete[] board;
    delete[] solved;

    cudaFree(empty_spaces);
    cudaFree(empty_space_count);
    cudaFree(new_boards);
    cudaFree(old_boards);
    cudaFree(board_index);

    cudaFree(dev_finished);
    cudaFree(dev_solved);
}