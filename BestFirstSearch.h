#ifndef BEST_FIRST_SEARCH_H
#define BEST_FIRST_SEARCH_H

#include "Searchable.h"

#include <memory>
using std::unique_ptr;

#include <queue>
using std::priority_queue;

class compByHeuritisic {

public:

    bool operator()(const unique_ptr<Searchable> & a, const unique_ptr<Searchable> & b) const {
        return a->heuristicValue() > b->heuristicValue();
    }

};

class BestFirstSearch {

protected:

    priority_queue<unique_ptr<Searchable>, std::vector<unique_ptr<Searchable>>, compByHeuritisic> Q;
    int nodes = 0;

public:

    BestFirstSearch(std::unique_ptr<Searchable> && startFrom) {
        Q.push(std::move(startFrom));
    }

    int getNodesExpanded() const {
        return nodes;
    }

    Searchable * solve() {

        while (!Q.empty()) {

            if (Q.top()->isSolution()) {
                return Q.top().get();
            }

            ++nodes;

            vector<unique_ptr<Searchable>> successors = Q.top()->successors();
            Q.pop();

            for (auto & successor : successors) {
                Q.push(std::move(successor));
            }
        }
        return nullptr;
    }
};

#endif
