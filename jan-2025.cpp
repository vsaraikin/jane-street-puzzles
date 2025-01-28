#include <iostream>
#include <vector>
#include <numeric>
#include <algorithm>
using namespace std;


// check if placing 'num' at board[row][col] is valid according to Sudoku rules
bool isValid(const vector<vector<int>>& board, int row, int col, int num) {
    // Check row and column
    for(int x = 0; x < 9; x++){
        if(board[row][x] == num) return false;
        if(board[x][col] == num) return false;
    }

    int startRow = row - row % 3;
    int startCol = col - col % 3;
    for(int i = 0; i < 3; i++){
        for(int j = 0; j < 3; j++){
            if(board[startRow + i][startCol + j] == num){
                return false;
            }
        }
    }
    return true;
}

// check if the original grid cell is empty (i.e., -1) or was pre-filled
bool isEmptyOriginal(const vector<vector<int>>& original, int row, int col) {
    return (original[row][col] == -1);
}

// convert a single row to an integer, treating -1 as 0
// e.g., [5, -1, 3, ...] -> "503..." -> integer
long long rowToInt(const vector<int>& row) {
    long long val = 0;
    for(int x : row){
        val = val * 10 + ((x == -1) ? 0 : x);
    }
    return val;
}

// compute the GCD of the rows that are completely filled so far.
// 'index' tells us how many cells have been processed (0..81).
long long computeGCDSoFar(const vector<vector<int>>& grid, int index) {
    int n_rows = index / 9; // how many complete rows are filled
    if(n_rows == 0) return 0; // no complete rows yet

    long long currentGCD = rowToInt(grid[0]);
    for(int i = 1; i < n_rows; i++){
        long long rowVal = rowToInt(grid[i]);
        currentGCD = std::gcd(currentGCD, rowVal);
        if(currentGCD == 1) {
            break;
        }
    }
    return currentGCD;
}

int main(){
    vector<vector<int>> original = {
            {-1, -1, -1, -1,  2, -1, -1, -1,  5},
            {-1, -1, -1, -1, -1, -1, -1,  2, -1},
            {-1,  2, -1, -1, -1, -1, -1, -1, -1},

            {-1, -1,  0, -1, -1, -1, -1, -1, -1},
            {-1, -1, -1, -1, -1, -1, -1, -1, -1},
            {-1, -1, -1,  2, -1, -1, -1, -1, -1},

            {-1, -1, -1, -1,  0, -1, -1, -1, -1},
            {-1, -1, -1, -1, -1,  2, -1, -1, -1},
            {-1, -1, -1, -1, -1, -1,  5, -1, -1}
    };

    vector<vector<int>> board = original;
    vector<vector<int>> bestSolution(9, vector<int>(9, -1));
    long long maxGCD = 0;

    vector<int> missingDigits = {1, 3, 4, 6, 7, 8, 9};
    vector<int> tryBoard(81, 0);

    // We'll run the solver for each possible missing digit
    for(int miss_digit : missingDigits){
        fill(tryBoard.begin(), tryBoard.end(), 0);
        for(int r = 0; r < 9; r++){
            for(int c = 0; c < 9; c++){
                board[r][c] = original[r][c];
            }
        }

        int idx = 0;
        while(idx >= 0 && idx < 81){
            int row = idx / 9;
            int col = idx % 9;

            if(isEmptyOriginal(original, row, col)){
                bool placed = false;

                if(tryBoard[idx] == 10){
                    tryBoard[idx] = 0;
                    board[row][col] = -1;
                    idx--;

                    while(idx >= 0 && !isEmptyOriginal(original, idx/9, idx%9)){
                        idx--;
                    }
                }
                else {
                    // try digits from tryBoard[idx]..9
                    while(tryBoard[idx] < 10){
                        // skip the missing digit
                        if(tryBoard[idx] == miss_digit){
                            tryBoard[idx]++;
                            continue;
                        }
                        // check if this digit is valid
                        if(isValid(board, row, col, tryBoard[idx])){
                            // place the digit
                            board[row][col] = tryBoard[idx];
                            tryBoard[idx]++;

                            // move forward
                            idx++;

                            // skip forward over originally filled cells
                            while(idx < 81 && !isEmptyOriginal(original, idx/9, idx%9)){
                                idx++;
                            }
                            placed = true;
                            break; // break out of the while(tryBoard[idx] < 10)
                        }
                        else {
                            // not valid, try next digit
                            tryBoard[idx]++;
                        }
                    }

                    // if couldn't place any digit (0..9 except miss_digit), backtrack
                    if(!placed){
                        board[row][col] = -1;
                        tryBoard[idx] = 0;
                        idx--;
                        while(idx >= 0 && !isEmptyOriginal(original, idx/9, idx%9)){
                            idx--;
                        }
                    }
                }
            }
            else {
                // if this cell is from the original puzzle, just move forward
                idx++;
                while(idx < 81 && !isEmptyOriginal(original, idx/9, idx%9)){
                    idx++;
                }
            }

            // after finish a row (i.e., idx % 9 == 0) and we have at least 2 rows filled,
            // check if the GCD so far is still promising.
            // (Matches Python code: if(index % 9 == 0 and index >= 18): check GCD)
            if(idx >= 18 && (idx % 9 == 0)){
                long long currentGCD = computeGCDSoFar(board, idx);
                // if the partial GCD is already <= maxGCD, prune this branch
                if(currentGCD <= maxGCD){
                    // force backtrack
                    int backIdx = idx - 1;
                    tryBoard[backIdx] = 10;
                    board[backIdx / 9][backIdx % 9] = -1;
                    idx = backIdx;
                    while(idx >= 0 && !isEmptyOriginal(original, idx/9, idx%9)){
                        idx--;
                    }
                }
            }

            if(idx == 81){
                long long finalGCD = computeGCDSoFar(board, idx);

                if(finalGCD > maxGCD){
                    maxGCD = finalGCD;
                    bestSolution = board;
                }

                idx--;
                while(idx >= 0 && !isEmptyOriginal(original, idx/9, idx%9)){
                    idx--;
                }
            }
        }
    }

    if(maxGCD > 0){
        cout << "Solution found with GCD = " << maxGCD << ":\n";
        for(int r = 0; r < 9; r++){
            for(int c = 0; c < 9; c++){
                if(bestSolution[r][c] == -1)
                    cout << ". ";
                else
                    cout << bestSolution[r][c] << " ";
            }
            cout << "\n";
        }
    } else {
        cout << "No solution found.\n";
    }

    return 0;
}