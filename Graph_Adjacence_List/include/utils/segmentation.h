#ifndef SEGMENTATION_H
#define SEGMENTATION_H

#include "undirected_Graph.h"
#include "union_find.h"
#include <vector>
#include <tuple>
#include <string>
#include <unordered_map>

class Segmentation {
public:
    static vector<int> segmentGraph(UndirectedGraph& graph, double k);
};

#endif
