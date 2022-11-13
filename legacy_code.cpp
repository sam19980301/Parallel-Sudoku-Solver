// Solution counter: 0: no solution, 1: single solution, else >1
// int _count_solution(Grid grid, int row, int col){
//     // printf("%d\t%d\n", row, col);
//     // finish iteration
//     if (row == N)
//         return 1;
//     int next_row = row + (col == N-1), next_col = (col + 1)%N;
//     // skip if assigned
//     if (grid[row][col] != UNASSIGNED)
//         return _count_solution(grid, next_row, next_col);
//     // try all possible value
//     int cnt = 0;
//     for (int v = 1; v <= N; ++v){
//         // single guess
//         if (is_legal(grid, row, col, v)){
//             grid[row][col] = v;
//             cnt += _count_solution(grid, next_row, next_col);
//             // early stop
//             if (cnt > 1){
//                 grid[row][col] = UNASSIGNED;
//                 return cnt;
//             }
//         }
//         // undo guess
//         grid[row][col] = UNASSIGNED;
//     }
//     return cnt;
// }
// int count_solution(Grid grid){ return _count_solution(grid, 0, 0); }

// Sudoku genertor
// int generate_sudoku(Grid grid){
//     int row, col, val;
//     int cnt = 0; // attempt counter
//     srand(time(NULL));
//     while (cnt++ <= 100){
//         int num_sol = count_solution(grid);
//         if (num_sol == 0){
//             // no solution, remove 1 value
//             printf("No solution %d\n", cnt);
//             print_sudoku(grid);
//             do{
//             row = rand()%N;
//             col = rand()%N;
//             } while (grid[row][col] == UNASSIGNED);
//             grid[row][col] = UNASSIGNED;
//         }
//         else if (num_sol > 1)
//         {
//             // non-unique grid, add 1 value
//             printf("Non unique solution %d\n", cnt);
//             print_sudoku(grid);
//             do
//             {
//                 row = rand()%N;
//                 col = rand()%N;
//                 val = rand()%N + 1;
//                 printf("%d\t%d\t%d\n", row, col, val);
//             } while (!is_legal(grid, row, col, val));
//             grid[row][col] = val;            
//         }
//         else{
//             // unique solution
//             printf("Attempt Counter %d\n", cnt);
//             return 1;
//         }
//     }
//     return 0;
// }