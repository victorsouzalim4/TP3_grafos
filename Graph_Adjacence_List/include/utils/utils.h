#pragma once

#include "graph.h"

#include <string>
#include <vector>
#include <utility>


using namespace std;

namespace Utils{
    pair<vector<string>, double> reconstructPath(Graph G, vector<tuple<double, int, int>> d, int from, int to);
}