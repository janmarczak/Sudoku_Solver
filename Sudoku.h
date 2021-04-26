#ifndef SUDOKU_H
#define SUDOKU_H

#include "Searchable.h"
#include "SudokuSquare.h"

#include<set>
using std::set;

#include<vector>
using std::vector;

#include <cmath>
#include<memory>
using std::unique_ptr;

using std::cout;

#include <map>
using std::map;

class Sudoku : public Searchable {

private:
    vector<vector<SudokuSquareSet>> incompleteSolution; // board
    vector<vector<bool>> booleans; // additional 2d vector that keeps track of wether the set in a particular row and columns has been set already.

public:

    Sudoku (int size) {
        incompleteSolution = vector<vector<SudokuSquareSet>>(size, vector<SudokuSquareSet>(size, SudokuSquareSet(size)));
        booleans = vector<vector<bool>> (size, vector<bool>(size, false));
    }

    Sudoku (const Sudoku & other) {
        incompleteSolution = other.incompleteSolution;
        booleans = other.booleans;
    }

    virtual bool isSolution() const override {
        for (int rowIndex = 0; rowIndex < incompleteSolution.size(); ++rowIndex) {
            for (int colIndex = 0; colIndex < incompleteSolution.size(); ++colIndex) {
                if (incompleteSolution[rowIndex][colIndex].size() != 1) {
                    return false;
                }
            }
        }
        return true;
    }


    virtual vector<unique_ptr<Searchable>> successors() const override {
        vector<unique_ptr<Searchable>> successorsToReturn;

        bool found = false;
        int smallestSize = 10000;
        SudokuSquareSet tmp = incompleteSolution[0][0];
        int row = 0;
        int col = 0;
        for (int i = 0; i < incompleteSolution.size() && !found; ++i) {
            for (int j = 0; j < incompleteSolution.size() && !found; ++j) {
                if (incompleteSolution[i][j].size() < smallestSize && !booleans[i][j]) {
                    smallestSize = incompleteSolution[i][j].size();
                    tmp = incompleteSolution[i][j];
                    row = i;
                    col = j;
                    if (incompleteSolution.size() > 9 && smallestSize == 1) {
                        found = true; // for bigger boards we pick the square with the smallest size that hasn't been checked already.
                    }
                    else if(incompleteSolution.size() < 10){
                        found = true; // for smaller boards we pick a fist square that hasn't been checked already.
                    }
                }
            }
        }

        for (auto & s : tmp) {
            Sudoku * copyOfSudoku = new Sudoku(*this);
            if (copyOfSudoku->setSquare(row, col, s)) {
                successorsToReturn.emplace_back(copyOfSudoku);
            }
            else {
                delete copyOfSudoku;
            }
        }
        if(successorsToReturn.size() == 1 && !successorsToReturn[0]->isSolution()) {
            return successorsToReturn[0]->successors();
        }
        else {
            return successorsToReturn;
        }

    }

    virtual int heuristicValue() const override {
        int toReturn = 0;
        for (int row = 0; row < incompleteSolution.size(); ++row) {
            for (int col = 0; col < incompleteSolution.size(); ++col) {
                if (incompleteSolution[row][col].size() > 1) {
                    ++toReturn;
                }
            }
        }
        return toReturn;
    }

    int getSquare(int row, int col) const {
        if (incompleteSolution[row][col].size() == 1) {
            return *(incompleteSolution[row][col].begin());
        }
        else {
            return -1;
        }
    }

    bool setSquare(int row, int col, int value) {

        incompleteSolution[row][col].clear();
        incompleteSolution[row][col].insert(value);

        int boxSize = sqrt(incompleteSolution.size());
        int checkedSize = 0;
        int compareSize = 0;
        bool isDone = false;
        bool returnBool;

        while (!isDone) {
            for (int rowIndex = 0; rowIndex < incompleteSolution.size() && !isDone; ++rowIndex) {
                for (int colIndex = 0; colIndex < incompleteSolution.size() && !isDone; ++colIndex) {
                    if (incompleteSolution[rowIndex][colIndex].size() == 1 && booleans[rowIndex][colIndex] == false && !isDone) {
                        int value = *(incompleteSolution[rowIndex][colIndex].begin()); // value to be deleted from other squares.
                        // deleting from row and column
                        for (int i = 0; i < incompleteSolution.size() && !isDone; ++i) {
                            if (colIndex != i) {
                                incompleteSolution[rowIndex][i].erase(value); // deleting from the same row
                                ++checkedSize;
                                if (incompleteSolution[rowIndex][i].empty()) {
                                    returnBool = false;
                                    isDone = true;
                                }
                            }
                            if (rowIndex != i) {
                                incompleteSolution[i][colIndex].erase(value); // deleting from the same column
                                ++checkedSize;
                                if (incompleteSolution[i][colIndex].empty()) {
                                    returnBool = false;
                                    isDone = true;
                                 }
                            }
                        }
                        // deleting from the same box
                        int boxRowIndex = rowIndex - (rowIndex % boxSize);
                        int boxColIndex = colIndex - (colIndex % boxSize);
                        int rowRange = boxRowIndex + boxSize;
                        int colRange = boxColIndex + boxSize;
                        for (; boxRowIndex < rowRange && !isDone; ++boxRowIndex) {
                            boxColIndex = colIndex - (colIndex % boxSize);
                            for (; boxColIndex < colRange && !isDone; ++boxColIndex) {
                                if (boxRowIndex != rowIndex && boxColIndex != colIndex) {
                                    ++checkedSize;
                                    incompleteSolution[boxRowIndex][boxColIndex].erase(value); // delete from the same box.
                                    if (incompleteSolution[boxRowIndex][boxColIndex].empty()) {
                                        returnBool = false;
                                        isDone = true;
                                    }
                                }
                            }
                        }
                        booleans[rowIndex][colIndex] = true; // The current set has been checked so we mark it as true.
                    }
                }
            }
            if (checkedSize == compareSize && !isDone) {
                returnBool = true;
                isDone = true;
            }
            else {
                compareSize = checkedSize;
            }
        }
        if(!returnBool) {
            return returnBool;
        }
        else {
            if(!duplicatesCheck(row,col)) {
                returnBool = false;
            }
        }
        return returnBool;
    }

    bool duplicatesCheck(int row, int col) {

        // Map of all sets in the row and their number of occurances in that row.
        map<SudokuSquareSet, int> rowMap;
        for (int i = 0; i < incompleteSolution.size(); ++i) {
            if (rowMap.find(incompleteSolution[row][i]) == rowMap.end()) {
                rowMap.emplace(incompleteSolution[row][i], 1);
            }
            else {
                rowMap[incompleteSolution[row][i]]++;
            }
        }

        for (auto const & x : rowMap) {
            if (x.first.size() == x.second) { // when size of the set is equal to amount of occurances in a row we can delete these values from different sets in that row.
                for (auto const & number : x.first) {
                    for (int i = 0; i < incompleteSolution.size(); ++i) {
                        if (incompleteSolution[row][i] != x.first) {
                            incompleteSolution[row][i].erase(number);
                            if (incompleteSolution[row][i].empty()) {
                                return false;
                            }
                        }
                    }
                }
            }
            else if (x.first.size() < x.second) {
                return false;
            }
        }

        // Map of all sets in the column and their number of occurances in that column.
        map<SudokuSquareSet, int> colMap;
        for (int i = 0; i < incompleteSolution.size(); ++i) {
            if (colMap.find(incompleteSolution[i][col]) == colMap.end()) {
                colMap.emplace(incompleteSolution[i][col], 1);
            }
            else {
                colMap[incompleteSolution[i][col]]++;
            }
        }

        for (auto const & x : colMap) {
            if (x.first.size() == x.second) { // when size of the set is equal to amount of occurances in a column we can delete these values from different sets in that column.
                for (auto const & number : x.first) {
                    for (int i = 0; i < incompleteSolution.size(); ++i) {
                        if (incompleteSolution[i][col] != x.first) {
                            incompleteSolution[i][col].erase(number);
                            if (incompleteSolution[i][col].empty()) {
                                return false;
                            }
                        }
                    }
                }
            }
            else if (x.first.size() < x.second) {
                return false;
            }
        }

        // Map of all sets in the box and their number of occurances in that box.
        map<SudokuSquareSet, int> boxMap;
        int boxSize = sqrt(incompleteSolution.size());
        int boxRowIndex = row - (row % boxSize);
        int boxColIndex = col - (col % boxSize);
        int rowRange = boxRowIndex + boxSize;
        int colRange = boxColIndex + boxSize;
        for (; boxRowIndex < rowRange; ++boxRowIndex) {
            boxColIndex = col - (col % boxSize); // reset the column counter
            for (; boxColIndex < colRange; ++boxColIndex) {
                if (boxMap.find(incompleteSolution[boxRowIndex][boxColIndex]) == boxMap.end()) {
                    boxMap.emplace(incompleteSolution[boxRowIndex][boxColIndex], 1);
                }
                else {
                    boxMap[incompleteSolution[boxRowIndex][boxColIndex]]++;
                }
            }
        }

        for (auto const & x : boxMap) {
            if (x.first.size() == x.second && x.first.size() != 1) { // when size of the set is equal to amount of occurances in a box we can delete these values from different sets in that box.
                for (auto const & number : x.first) {
                    for (; boxRowIndex < rowRange; ++boxRowIndex) {
                        boxColIndex = col - (col % boxSize);
                        for (; boxColIndex < colRange; ++boxColIndex) {
                            if (incompleteSolution[boxRowIndex][boxColIndex] != x.first) {
                                incompleteSolution[boxRowIndex][boxColIndex].erase(number);
                                if (incompleteSolution[boxRowIndex][boxColIndex].empty()) {
                                    return false;
                                }
                            }
                        }
                    }
                }
            }
            else if (x.first.size() < x.second) {
                return false;
            }
        }
        return true;
    }
    
    
    void printSudoku() {
        for (int rowIndex = 0; rowIndex < incompleteSolution.size(); ++rowIndex) {
            for (int colIndex = 0; colIndex < incompleteSolution.size(); ++colIndex) {
                cout << "{";
                if (incompleteSolution[rowIndex][colIndex].size() != 1) {
                    cout << " ";
                }
                else {
                    cout << *incompleteSolution[rowIndex][colIndex].begin();
                }
                cout << "} ";
            }
            cout << "\n";
        }
    }
    
};


#endif
