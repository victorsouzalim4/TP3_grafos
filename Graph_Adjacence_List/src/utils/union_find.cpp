#include "Utils\union_find.h"
#include <algorithm>

UnionFind::UnionFind(int n) {
    parent.resize(n);
    rank.resize(n, 0);
    size.resize(n, 1);
    internalDiff.resize(n, 0.0);
    for (int i = 0; i < n; i++) parent[i] = i;
}

int UnionFind::find(int u) {
    if (parent[u] != u)
        parent[u] = find(parent[u]);
    return parent[u];
}

bool UnionFind::join(int u, int v, double weight, double k) {
    int pu = find(u);
    int pv = find(v);
    if (pu == pv) return false;

    double minInternal = std::min(internalDiff[pu] + k / size[pu], internalDiff[pv] + k / size[pv]);

    if (weight > minInternal) return false;

    if (rank[pu] < rank[pv]) std::swap(pu, pv);
    parent[pv] = pu;
    size[pu] += size[pv];
    internalDiff[pu] = std::max(weight, std::max(internalDiff[pu], internalDiff[pv]));
    if (rank[pu] == rank[pv]) rank[pu]++;

    return true;
}

int UnionFind::getSize(int u) {
    return size[find(u)];
}

double UnionFind::getInternalDiff(int u) {
    return internalDiff[find(u)];
}
