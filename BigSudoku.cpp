#include "Sudoku.h"
#include "BestFirstSearch.h"

#include <vector>
#include <string>

using std::vector;
using std::string;

#include <iostream>

using std::cout;
using std::endl;

bool fillBoard(Sudoku * board, const vector<vector<int> > & fillWith) {

    for (size_t row = 0; row < fillWith.size(); ++row) {
        for (size_t col = 0; col < fillWith.size(); ++col) {
            if (fillWith[row][col] != 0) {
                const int setTo = fillWith[row][col];
                if (!board->setSquare(row, col, setTo)) {
                    cout << "Error: setSquare returned false after setting square[" << row << "][" << col << "] to " << setTo << " - this should happen as the example boards are solvable\n";
                    return false;
                }
            }
        }
    }

    return true;
}



bool checkAnswer(Sudoku * board) {

    for (int row = 0; row < 16; ++row) {
        vector<int> present(17, 0);
        for (int col = 0; col < 16; ++col) {
            const int setTo = board->getSquare(row,col);
            if (setTo < 1 || setTo > 16) {
                cout << "Error: the number " << setTo << " wasn't in the range [1,16]\n";
            }
            if (++present[setTo] != 1) {
                cout << "Error: the number " << setTo << " was present multiple times on row " << row << " (with the top row being row 0)\n";
                return false;
            }
        }
    }

    for (int col = 0; col < 16; ++col) {
        vector<int> present(17, 0);
        for (int row = 0; row < 16; ++row) {
            const int setTo = board->getSquare(row,col);
            if (++present[setTo] != 1) {
                cout << "Error: the number " << setTo << " was present multiple times in column " << col << " (with the leftmost column being column 0)\n";
                return false;
            }
        }
    }

    for (int colA = 0; colA < 16; colA += 4) {
        for (int rowA = 0; rowA < 16; rowA += 4) {
            vector<int> present(17, 0);
            for (int row = rowA; row < rowA + 4; ++row) {
                for (int col = colA; col < colA + 4; ++col) {
                    const int setTo = board->getSquare(row,col);
                    if (++present[setTo] != 1) {
                        cout << "Error: the number " << setTo << " was present multiple times in the 4x4 box whose top-left corner is at row " << rowA << " column "<< colA << " (with the top-left square of the board being at (0,0)\n";
                        return false;
                    }
                }
            }
        }
    }


    return true;
}

int main() {

    {
        vector<vector<int>> bigBoard{

{0,0,0,0,0,0,0,0,7,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,14,0,0,0,2},
{7,0,0,0,11,0,0,0,0,0,0,0,0,0,0,0,10,0,0,0,2,0,13,0,0,0,15,0,0,0,5},
{1,0,0,0,16,0,14,0,3,0,11,0,2,0,0,0,5,0,0,0,0,0,0,0,0,0,0,0,0,0,13},
{0,0,2,0,0,0,5,0,0,0,0,0,0,0,0,0,0,0,0,0,7,0,0,0,0,0,8,0,11,0,0},
{2,0,0,0,0,0,0,0,0,0,0,0,16,0,0,0,0,0,6,0,0,0,0,0,0,0,0,0,0,0,0},
{0,0,11,0,1,0,12,0,14,0,0,0,0,0,0,0,8,0,0,0,5,0,7,0,0,0,10,0,0,0,0},
{15,0,0,0,0,0,16,0,0,0,0,0,0,0,0,0,1,0,0,0,9,0,2,0,0,0,0,0,6,0,4},
{0,0,0,0,5,0,0,0,2,0,0,0,0,0,7,0,0,0,0,0,0,0,4,0,8,0,1,0,0,0,11},
{0,0,0,0,4,0,0,0,0,0,0,0,0,0,8,0,0,0,10,0,0,0,11,0,12,0,0,0,0,0,1},
{0,0,0,0,8,0,6,0,0,0,14,0,0,0,3,0,13,0,0,0,0,0,0,0,0,0,9,0,0,0,0},
{0,0,0,0,13,0,15,0,11,0,16,0,5,0,4,0,0,0,0,0,3,0,0,0,0,0,0,0,0,0,10},
{0,0,16,0,0,0,9,0,12,0,0,0,0,0,0,0,0,0,0,0,0,0,6,0,11,0,0,0,8,0,0},
{0,0,0,0,0,0,13,0,6,0,0,0,0,0,0,0,0,0,1,0,15,0,16,0,0,0,0,0,7,0,0},
{14,0,0,0,0,0,10,0,0,0,12,0,11,0,0,0,9,0,0,0,0,0,0,0,0,0,5,0,0,0,0},
{0,0,0,0,3,0,0,0,5,0,0,0,0,0,0,0,0,0,11,0,0,0,0,0,13,0,0,0,14,0,0},
{0,0,0,0,0,0,0,0,0,0,1,0,0,0,16,0,3,0,14,0,12,0,0,0,10,0,0,0,0,0,0}

        };

        unique_ptr<Sudoku> board(new Sudoku(16));

        if (!fillBoard(board.get(), bigBoard)) {
            cout << "Failed: When filling the board, setSquare() returned false\n";
            return 1;
        }
        cout << "Trying to solve 16x16 board:\n";
        board->printSudoku();

        BestFirstSearch search(std::move(board));

        Searchable * solution = search.solve();

        if (solution == nullptr) {
            cout << "\nFailed: Couldn't be solved\n";
            return 1;
        }

        Sudoku * solvedBoard = static_cast<Sudoku*>(solution);

        if (checkAnswer(solvedBoard)) {
            cout << "\nPassed: Solution was:\n";
            solvedBoard->printSudoku();
            cout << "\nNodes expanded: " << search.getNodesExpanded() << std::endl;
            return 0;
        } else {
            cout << "Failed: Solution was returned, but it was the wrong answer\n";
            return 1;
        }
    }


}
