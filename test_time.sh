# command line: ./test_time {puzzles2_17_clue, puzzles5_forum_hardest_1905_11+} {1,2,3}

# ./test_time.sh puzzles2_17_clue 2
# ./test_time.sh puzzles2_17_clue 3
# ./test_time.sh puzzles5_forum_hardest_1905_11+ 2
# ./test_time.sh puzzles5_forum_hardest_1905_11+ 3

# Make Folder
DIR="result/"
if [ ! -d "$DIR" ]; then
  mkdir $DIR
fi

# Dataset
echo "Selected Dataset: ${1}"
if [ "$1" = "puzzles2_17_clue" ]
then
    n_prob=49158
else
    n_prob=48766
fi

# Solver
make clean
if [ "$2" = "1" ]
then
    echo "Solver is brute force"
    make PARALLEL=1 BRUTE_FORCE=1
elif [ "$2" = "2" ]
then
    echo "Solver is crooks"
    make PARALLEL=1 CROOKS=1
else
    echo "Solver is cross hatching"
    make PARALLEL=1 CROSS_HATCHING=1
fi

echo "Test Performance"
./sudoku 1 1 $n_prob data/$1 | tee "result/1_$1.txt"
# ./sudoku 2 2 $n_prob data/$1 | tee "result/2_$1.txt"
# ./sudoku 4 4 $n_prob data/$1 | tee "result/4_$1.txt"
# ./sudoku 8 8 $n_prob data/$1 | tee "result/8_$1.txt"
