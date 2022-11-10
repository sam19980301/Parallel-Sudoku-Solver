#ifndef __SUDOKU_H__
#define __SUDOKU_H__



using namespace std;

// #define SUB_N 3
// #define N (SUB_N * SUB_N)
constexpr int SUB_N = 3;
constexpr int N = SUB_N * SUB_N;

constexpr int UNASSIGNED = 0;

using Grid = int(*)[N];
using Sudoku = struct {
    Grid grid;
};

#endif //__SUDOKU_H__