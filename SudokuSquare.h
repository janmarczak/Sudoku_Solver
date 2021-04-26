#ifndef SUDOKUSQUARE_H
#define SUDOKUSQUARE_H


class SudokuSquareSet {

private:
    unsigned int valuesInSquare;
    int numberOfElements;

    bool isInSet(int value) {
        if (valuesInSquare == (valuesInSquare | 1 << (value-1))) {
            return true;
        }
        else {
            return false;
        }
    }

public:

    class iterator {

    private:
        unsigned int * elements;
        int * pointTo;
        int index;

    public:

        iterator(unsigned int * elementsIn, int indexIn) : elements(elementsIn), index(indexIn) {
            if (index == (8*sizeof(*elements)+1)) {
                pointTo = nullptr;
            }
            else {
                pointTo = &index;
            }
        }

        iterator(unsigned int * elementsIn) : elements(elementsIn) {
            if (*elements == 0) {
                index = 0;
            }
            else {
                if (*elements == (*elements | 1)) {
                    index = 1;
                }
                else {
                    index = 0;
                    pointTo = &index;
                    operator++();
                }
            }
            pointTo = &index;
        }

        void operator++() {
            if (*elements == (*elements | 1 << (index))) {
                ++index;
                pointTo = &index;
            }
            else {
                while (*elements != (*elements | 1 << (index))) {
                    ++index;
                    if(index == (8*sizeof(*elements))) {
                        pointTo = nullptr;
                        break;
                    }
                }
                if (pointTo != nullptr) {
                    ++index;
                    pointTo = &index;
                }
            }
        }

        int & operator*() {
            return *pointTo;
        }

        bool operator==(const iterator & other) const {
            return (pointTo == other.pointTo);
        }

        bool operator!=(const iterator & other) const {
            return (pointTo != other.pointTo);
        }
    };

    class constIterator {

    private:
        const unsigned int * elements;
        int * pointTo;
        int index;

    public:

        constIterator(const unsigned int * elementsIn, int indexIn) : elements(elementsIn), index(indexIn) {
            if (index == (8*sizeof(*elements)+1)) {
                pointTo = nullptr;
            }
            else {
                pointTo = &index;
            }
        }

        constIterator(const unsigned int * elementsIn) : elements(elementsIn) {
            if (*elements == 0) {
                index = 0;
                pointTo = &index;
            }
            else {
                if (*elements == (*elements | 1)) {
                    index = 1;
                }
                else {
                    index = 0;
                    pointTo = &index;
                    operator++();
                }
            }
            pointTo = &index;
        }

        void operator++() {
            if (*elements == (*elements | 1 << (index))) {
                ++index;
                pointTo = &index;
            }
            else {
                while (*elements != (*elements | 1 << (index))) {
                    ++index;
                    if(index == (8*sizeof(*elements))) {
                        pointTo = nullptr;
                        break;
                    }
                }
                if (pointTo != nullptr) {
                    ++index;
                    pointTo = &index;
                }
            }
        }

        const int & operator*() const {
            return *pointTo;
        }

        bool operator==(const constIterator & other) const {
            return (pointTo == other.pointTo);
        }

        bool operator!=(const constIterator & other) const {
            return (pointTo != other.pointTo);
        }
    };

    SudokuSquareSet() {
        valuesInSquare = 0;
        numberOfElements = 0;
    }

    SudokuSquareSet(int size) {
        valuesInSquare = 0;
        for (int i = 0; i < size; ++i) {
            valuesInSquare |= (1 << i);
        }
        numberOfElements = size;
    }

    SudokuSquareSet(const SudokuSquareSet & other) {
        valuesInSquare = other.valuesInSquare;
        numberOfElements = other.numberOfElements;
    }

    int size() const {
        return numberOfElements;
    }

    bool empty() const {
        return (valuesInSquare == 0);
    }

    void clear() {
        valuesInSquare = 0;
        numberOfElements = 0;
    }

    bool operator==(const SudokuSquareSet & other) const {
        return (valuesInSquare == other.valuesInSquare);
    }

    bool operator!=(const SudokuSquareSet & other) const {
        return (valuesInSquare != other.valuesInSquare);
    }

    bool operator<(const SudokuSquareSet & other) const {
        return (valuesInSquare < other.valuesInSquare);
    }

    iterator begin() {
        return iterator(&valuesInSquare);
    }

    iterator end() {
        return iterator(&valuesInSquare, (8*sizeof(valuesInSquare)+1));
    }

    const constIterator begin() const {
        return constIterator(&valuesInSquare);
    }

    const constIterator end() const {
        return constIterator(&valuesInSquare, (8*sizeof(valuesInSquare)+1));
    }

    iterator find(int value) {
        if (isInSet(value)) {
            return iterator(&valuesInSquare, value);
        }
        else {
            return end();
        }
    }

    iterator insert(int value) {
        if (!isInSet(value)) {
            valuesInSquare = valuesInSquare | (1 << (value-1));
            ++numberOfElements;
        }
        return iterator(&valuesInSquare, value);
    }

    void erase(int value) {
        if (isInSet(value)) {
            valuesInSquare = valuesInSquare ^ (1 << (value-1));
            numberOfElements--;
        }
    }

    void erase(iterator & itr) {
        erase(*itr);
    }

};

#endif
