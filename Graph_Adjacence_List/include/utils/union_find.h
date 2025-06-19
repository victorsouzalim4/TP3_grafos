#ifndef DISJOINT_SET_H
#define DISJOINT_SET_H

#include <vector>

class UnionFind {
    
private:
    std::vector<int> parent;
    std::vector<int> rank;
    std::vector<int> size;
    std::vector<double> internalDiff;

public:
    UnionFind(int n);

    int find(int u);
    bool join(int u, int v, double weight, double k);
    int getSize(int u);
    double getInternalDiff(int u);
    void forceJoin(int u, int v);
};

#endif
