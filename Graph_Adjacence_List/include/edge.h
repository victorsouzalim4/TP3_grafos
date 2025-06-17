#ifndef EDGE_H
#define EDGE_H

#include <iostream>

using namespace std;

class Edge {
    public:
        int to;
        double weight;

    Edge(int to, double weight = 1.0);

    void print() const;
};

#endif 
