#include <cuda.h>

#include "sudoku.h"

const int block_width = SUB_N;
const int block_height = SUB_N;

void copy(int* host_data, Grid* grid){
    for (int i = 0; i < N * N; i++)
        host_data[i] = (*grid)[i / N][i % N];
}

void test(int* host_data, Grid* grid){
    for (int i = 0; i < N * N; i++){
        printf("%d ", host_data[i]);
        if (i+1 % N == 0) printf("\n");
    }
    printf("====================================\n");
    show_grid(grid);
}

__global__ void cuda_bfs(int* new_boards, int* old_boards, int* empty_spaces, int* empty_space_counts, int* board_index, int total_boards){
    int x_index = blockIdx.x * blockDim.x + threadIdx.x; 
    const int num_tiles = N * N;
    const int offset = N * N * x_index;

    while (x_index < total_boards) {
        // find the next empty spot
        bool found = false;

        for (int i = (x_index * num_tiles); i < (x_index + 1) * num_tiles && !found; i++){
            if (old_boards[i] != UNASSIGNED) continue;
            
            // if (old_board[i] == UNASSIGNED):
            found = true;
            const int real_id = i - offset; 
            const int row = real_id / N;
            const int col = real_id % N;

            // find a suitable value to fill in
            for (int value = 1; value <= N; value++){
                bool can_fill_in = true;

                // fix row, change column
                for (int c = 0; c < N; c++){
                    if (old_boards[row * N + c + offset] == value) { can_fill_in = false; break; }
                }
                if (!can_fill_in) continue;

                // fix column, change row
                for (int r = 0; r < N; r++){
                    if (old_boards[r * N + col + offset] == value) { can_fill_in = false; break; }
                }
                if (!can_fill_in) continue;

                // check box
                // little strange
                for (int r = (row /SUB_N) * SUB_N; r < SUB_N; r++){
                    for (int c = (col / SUB_N) * SUB_N; c < SUB_N; c++){
                        if (old_boards[r * N + c + offset] == value) { can_fill_in = false; break; }
                    } 
                }
                if (!can_fill_in) continue;

                int next_board_index = atomicAdd(board_index, 1);
                int empty_index = 0;
                for (int r = 0; r < N; r++){
                    for (int c = 0; c < N; c++){
                        new_boards[r * N + c + next_board_index * num_tiles] = old_boards[r * N + c + offset];
                        if (old_boards[r * N + c + offset] == UNASSIGNED && (r != row || c != col)){
                            empty_spaces[empty_index + next_board_index * num_tiles] = r * N + c;
                            empty_index++;
                        }
                    }
                }
                empty_space_counts[next_board_index] = empty_index;
                new_boards[row * N + col + next_board_index * num_tiles] = value;
            }
        }
        x_index += blockDim.x * gridDim.x;
    }
}

void hostFE(Grid* grid){
    // allocate init_board and copy grid to it
    int *init_board = (int*) malloc(N * N * sizeof(int));
    copy(init_board, grid);

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
    const int num_elements = 1 << 26;    

    // allocate memory
    cudaMalloc(&new_boards, num_elements * sizeof(int));
    cudaMalloc(&old_boards, num_elements * sizeof(int));
    cudaMalloc(&empty_spaces, num_elements * sizeof(int));
    cudaMalloc(&empty_space_count, (num_elements / (N * N) + 1) * sizeof(int));
    cudaMalloc(&board_index, sizeof(int));
    
    int total_boards = 1;

    // initialize memory
    cudaMemset(new_boards, UNASSIGNED, num_elements * sizeof(int));
    cudaMemset(old_boards, UNASSIGNED, num_elements * sizeof(int));
    cudaMemset(board_index, 0, sizeof(int));

    // copy init_board to old_boards
    cudaMemcpy(old_boards, init_board, N * N * sizeof(int), cudaMemcpyHostToDevice);

    return;

    const int size = N * N * sizeof(int);
    int* host_data;
    cudaHostAlloc(&host_data, size, cudaHostAllocMapped);
    copy(host_data, grid);
    //test(host_data, grid);

    size_t pitch;
    int* device_data;
    cudaMallocPitch(&device_data, &pitch, N * sizeof(int), N);

    dim3 thread_per_block(block_width, block_height);
    dim3 num_blocks(N / thread_per_block.x, N / thread_per_block.y);

    cuda_solve<<<num_blocks, thread_per_block>>>();

    cudaFreeHost(host_data);
    cudaFree(device_data);
}